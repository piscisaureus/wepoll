#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "error.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

/* clang-format off */
static const long _REF          = (long) 0x00000001;
static const long _REF_MASK     = (long) 0x0fffffff;
static const long _DESTROY      = (long) 0x10000000;
static const long _DESTROY_MASK = (long) 0xf0000000;
static const long _POISON       = (long) 0x300DEAD0;
/* clang-format on */

static HANDLE _keyed_event = NULL;

int reflock_global_init(void) {
  NTSTATUS status =
      NtCreateKeyedEvent(&_keyed_event, ~(ACCESS_MASK) 0, NULL, 0);
  if (status != STATUS_SUCCESS)
    return_set_error(-1, RtlNtStatusToDosError(status));
  return 0;
}

void reflock_init(reflock_t* reflock) {
  reflock->state = 0;
}

static void _signal_event(void* address) {
  NTSTATUS status = NtReleaseKeyedEvent(_keyed_event, address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
}

static void _await_event(void* address) {
  NTSTATUS status = NtWaitForKeyedEvent(_keyed_event, address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
}

void reflock_ref(reflock_t* reflock) {
  long state = InterlockedAdd(&reflock->state, _REF);
  unused_var(state);
  assert((state & _DESTROY_MASK) == 0); /* Overflow or destroyed. */
}

void reflock_unref(reflock_t* reflock) {
  long state = InterlockedAdd(&reflock->state, -_REF);
  long ref_count = state & _REF_MASK;
  long destroy = state & _DESTROY_MASK;

  unused_var(ref_count);
  unused_var(destroy);

  if (state == _DESTROY)
    _signal_event(reflock);
  else
    assert(destroy == 0 || ref_count > 0);
}

void reflock_unref_and_destroy(reflock_t* reflock) {
  long state = InterlockedAdd(&reflock->state, _DESTROY - _REF);
  long ref_count = state & _REF_MASK;

  assert((state & _DESTROY_MASK) ==
         _DESTROY); /* Underflow or already destroyed. */

  if (ref_count != 0)
    _await_event(reflock);

  state = InterlockedExchange(&reflock->state, _POISON);
  assert(state == _DESTROY);
}
