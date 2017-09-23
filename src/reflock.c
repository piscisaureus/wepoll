#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "error.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

typedef union _reflock_private {
  volatile uint32_t counter;
  uint64_t dummy;
  struct {
    uint32_t lock;
    uint32_t delete;
  } events;
} _reflock_private_t;

static const uint32_t _CTR_REF = (uint32_t) 1;
static const uint32_t _CTR_LOCK = (uint32_t) 1 << 16;
static const uint32_t _CTR_DELETE = (uint32_t) 1 << 31;

static const uint32_t _CTR_REF_MASK = ((uint32_t) 1 << 16) - 1;
static const uint32_t _CTR_LOCK_MASK = (((uint32_t) 1 << 15) - 1) << 16;

/* clang-format off */

#define _TOK_REFERENCED       0x01
#define _TOK_LOCK_ANNOUNCED   0x02
#define _TOK_DELETE_ANNOUNCED 0x04
#define _TOK_MUST_WAIT        0x08
#define _TOK_NO_WAIT          0x10
#define _TOK_LOCKED           0x20
#define _TOK_DELETED          0x40

/* clang-format on */

static HANDLE _global_event_handle = NULL;

uint32_t _sync_add(volatile uint32_t* target, uint32_t addend) {
  static_assert(sizeof(*target) == sizeof(long), "");
  return InterlockedAdd((volatile long*) target, addend) - addend;
}

int reflock_global_init(void) {
  NTSTATUS status =
      NtCreateKeyedEvent(&_global_event_handle, ~(ACCESS_MASK) 0, NULL, 0);
  if (status != STATUS_SUCCESS)
    return_error(-1, RtlNtStatusToDosError(status));
  return 0;
}

void reflock_init(reflock_t* reflock) {
  memset(reflock, 0, sizeof *reflock);
}

static _reflock_private_t* _reflock_private(reflock_t* reflock) {
  static_assert(sizeof(reflock_t) == sizeof(_reflock_private_t), "");
  return (_reflock_private_t*) reflock;
}

static void _wait_for_event(const uint32_t* address) {
  NTSTATUS status =
      NtWaitForKeyedEvent(_global_event_handle, (PVOID) address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
}

static void _signal_event(const uint32_t* address) {
  NTSTATUS status =
      NtReleaseKeyedEvent(_global_event_handle, (PVOID) address, FALSE, NULL);
  if (status != STATUS_SUCCESS)
    abort();
}

reflock_token_t reflock_ref_and_announce_lock(reflock_t* reflock) {
  _reflock_private_t* priv = _reflock_private(reflock);

  uint32_t prev = _sync_add(&priv->counter, _CTR_REF | _CTR_LOCK);
  uint32_t prev_lock_count = prev & _CTR_LOCK_MASK;

  if (prev & _CTR_DELETE)
    abort();

  if (prev_lock_count == 0)
    return _TOK_REFERENCED | _TOK_LOCK_ANNOUNCED | _TOK_NO_WAIT;
  else
    return _TOK_REFERENCED | _TOK_LOCK_ANNOUNCED | _TOK_MUST_WAIT;
}

reflock_token_t reflock_lock(reflock_t* reflock, reflock_token_t token) {
  _reflock_private_t* priv = _reflock_private(reflock);

  switch (token) {
    case _TOK_REFERENCED: {
      /* Lock hasn't been announced; increase lock count and wait if necessary.
       */
      uint32_t prev_lock_count =
          _sync_add(&priv->counter, _CTR_LOCK) & _CTR_LOCK_MASK;
      if (prev_lock_count > 0)
        _wait_for_event(&priv->events.lock);
      break;
    }

    case _TOK_REFERENCED | _TOK_LOCK_ANNOUNCED | _TOK_MUST_WAIT:
      /* Lock has been announced; wait for the thread currently owning the lock
       * to signal the event. */
      _wait_for_event(&priv->events.lock);
      break;

    case _TOK_REFERENCED | _TOK_LOCK_ANNOUNCED | _TOK_NO_WAIT:
      /* There were no waiters when the lock was announced; no need to wait for
       * an event. */
      break;

    default:
      abort();
  }

  return _TOK_REFERENCED | _TOK_LOCKED;
}

void _unlock_signal_event(_reflock_private_t* priv, uint32_t counter) {
  if (counter & _CTR_LOCK_MASK) {
    /* Wake a thread that is waiting to acquire the lock. */
    _signal_event(&priv->events.lock);
  } else if ((counter & _CTR_DELETE) && !(counter & _CTR_REF_MASK)) {
    /* Wake the thread that is waiting to delete the lock. */
    _signal_event(&priv->events.delete);
  }
}

reflock_token_t reflock_unlock(reflock_t* reflock, reflock_token_t token) {
  _reflock_private_t* priv = _reflock_private(reflock);

  uint32_t add = -(int32_t) _CTR_LOCK;
  uint32_t prev_counter = _sync_add(&priv->counter, add);

  _unlock_signal_event(priv, prev_counter + add);

  unused(token);
  assert(token == (_TOK_REFERENCED | _TOK_LOCKED));
  return _TOK_REFERENCED;
}

void reflock_unref_and_unlock(reflock_t* reflock, reflock_token_t token) {
  _reflock_private_t* priv = _reflock_private(reflock);

  uint32_t add = -(int32_t)(_CTR_REF | _CTR_LOCK);
  uint32_t prev_counter = _sync_add(&priv->counter, add);

  _unlock_signal_event(priv, prev_counter + add);

  unused(token);
  assert(token == (_TOK_REFERENCED | _TOK_LOCKED));
}

reflock_token_t reflock_announce_delete(reflock_t* reflock) {
  _reflock_private_t* priv = _reflock_private(reflock);

  uint32_t prev = _sync_add(&priv->counter, _CTR_DELETE);
  uint32_t ref_count = prev & _CTR_REF_MASK;

  if (prev & _CTR_DELETE)
    abort();

  if (ref_count == 0)
    return _TOK_DELETE_ANNOUNCED | _TOK_NO_WAIT;
  else
    return _TOK_DELETE_ANNOUNCED | _TOK_MUST_WAIT;
}

void reflock_delete(reflock_t* reflock, reflock_token_t token) {
  _reflock_private_t* priv = _reflock_private(reflock);

  switch (token) {
    case _TOK_DELETE_ANNOUNCED | _TOK_MUST_WAIT:
      /* The lock was owned when deletion was announced; wait for the last lock
       * owner to signal the delete event. */
      _wait_for_event(&priv->events.delete);
      break;

    case _TOK_DELETE_ANNOUNCED | _TOK_NO_WAIT:
      /* There were no other waiters when deletion was announced; there's no
       * need to wait for an event. */
      break;

    default:
      abort();
  }

  priv->counter = 0;
}
