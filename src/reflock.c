#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "error.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

/* clang-format off */
static const long REFLOCK__REF          = (long) 0x00000001;
static const long REFLOCK__REF_MASK     = (long) 0x0fffffff;
static const long REFLOCK__DESTROY      = (long) 0x10000000;
static const long REFLOCK__DESTROY_MASK = (long) 0xf0000000;
static const long REFLOCK__POISON       = (long) 0x300dead0;
/* clang-format on */

static HANDLE reflock__keyed_event = NULL;

int reflock_global_init(void) {
  NTSTATUS status =
      NtCreateKeyedEvent(&reflock__keyed_event, KEYEDEVENT_ALL_ACCESS, NULL, 0);
  if (status != STATUS_SUCCESS)
    return_set_error(-1, RtlNtStatusToDosError(status));
  return 0;
}

void reflock_init(reflock_t* reflock) {
  reflock->state = 0;
}

static void reflock__signal_event(void* address) {
  NTSTATUS status =
      NtReleaseKeyedEvent(reflock__keyed_event, address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
}

static void reflock__await_event(void* address) {
  NTSTATUS status =
      NtWaitForKeyedEvent(reflock__keyed_event, address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
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

  if (state == REFLOCK__DESTROY)
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
  assert(state == REFLOCK__DESTROY);
}
