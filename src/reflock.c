#include <assert.h>
#include <stdlib.h>

#include "error.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

/* clang-format off */
#define REFLOCK__REF          ((long) 0x00000001UL)
#define REFLOCK__REF_MASK     ((long) 0x0fffffffUL)
#define REFLOCK__DESTROY      ((long) 0x10000000UL)
#define REFLOCK__DESTROY_MASK ((long) 0x10000000UL)
#define REFLOCK__SIGNAL       ((long) 0x20000000UL)
#define REFLOCK__SIGNAL_MASK  ((long) 0x20000000UL)
#define REFLOCK__AWAIT        ((long) 0x40000000UL)
#define REFLOCK__AWAIT_MASK   ((long) 0x40000000UL)
#define REFLOCK__POISON       ((long) 0x800dead0UL)
/* clang-format on */

static CRITICAL_SECTION signalMutex;

int reflock_global_init(void) {
  InitializeCriticalSection(&signalMutex);
  return 0;
}

void reflock_init(reflock_t* reflock) {
  reflock->state = 0;
  InitializeConditionVariable(&reflock->cv_signal);
  InitializeConditionVariable(&reflock->cv_await);
}

static void reflock__signal_event(reflock_t* reflock) {
  BOOL status = TRUE;

  EnterCriticalSection(&signalMutex);
  long state = InterlockedOr(&reflock->state, REFLOCK__SIGNAL);
  while ((reflock->state & REFLOCK__AWAIT_MASK) == 0) {
    status = SleepConditionVariableCS(&reflock->cv_signal, &signalMutex, INFINITE);
  }
  LeaveCriticalSection(&signalMutex);

  if (status != TRUE)
    abort();

  /* At most one reflock__await_event call per reflock. */
  WakeConditionVariable(&reflock->cv_await);
  unused_var(state);
}

static void reflock__await_event(reflock_t* reflock) {
  BOOL status = TRUE;

  EnterCriticalSection(&signalMutex);
  long state = InterlockedOr(&reflock->state, REFLOCK__AWAIT);
  while ((reflock->state & REFLOCK__SIGNAL_MASK) == 0) {
    status = SleepConditionVariableCS(&reflock->cv_await, &signalMutex, INFINITE);
  }
  LeaveCriticalSection(&signalMutex);

  if (status != TRUE)
    abort();

  /* Multiple threads could be waiting. */
  WakeAllConditionVariable(&reflock->cv_signal);
  unused_var(state);
}

void reflock_ref(reflock_t* reflock) {
  long state = InterlockedAdd(&reflock->state, REFLOCK__REF);

  /* Verify that the counter didn't overflow and the lock isn't destroyed. */
  assert((state & REFLOCK__DESTROY_MASK) == 0);
  unused_var(state);
}

void reflock_unref(reflock_t* reflock) {
  long state = InterlockedAdd(&reflock->state, -REFLOCK__REF);

  /* Verify that the lock was referenced and not already destroyed. */
  assert((state & REFLOCK__DESTROY_MASK & ~REFLOCK__DESTROY) == 0);

  if ((state & REFLOCK__DESTROY_MASK) == REFLOCK__DESTROY &&
      (state & REFLOCK__REF_MASK) == 0)
    reflock__signal_event(reflock);
}

void reflock_unref_and_destroy(reflock_t* reflock) {
  long state =
      InterlockedAdd(&reflock->state, REFLOCK__DESTROY - REFLOCK__REF);
  long ref_count = state & REFLOCK__REF_MASK;

  /* Verify that the lock was referenced and not already destroyed. */
  assert((state & REFLOCK__DESTROY_MASK) == REFLOCK__DESTROY);

  if (ref_count != 0)
    reflock__await_event(reflock);

  state = InterlockedExchange(&reflock->state, REFLOCK__POISON);
  assert((state & REFLOCK__DESTROY_MASK) == REFLOCK__DESTROY);
  assert((state & REFLOCK__REF_MASK) == 0);
}
