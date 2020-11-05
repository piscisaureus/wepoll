#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "api.h"
#include "error.h"
#include "nt.h"
#include "port.h"
#include "synthetic-event.h"
#include "wepoll.h"
#include "win.h"

/*
 * Completion packets that carry a synthetic event are distinguished from
 * regular I/O completions by a special `NTSTATUS` value. This is a 32-bit
 * integer, which is constructed as follows:
 *
 *       Bit(s)  Len  Value   Meaning
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  MSB  31-30    2   0 0     Is success (not: error/warning/informational)
 *       29       1   1       Is application-defined (not: Windows-defined)
 *       28       1   0       Reserved for `HRESULT`, must be 0
 *       27       1   0       Reserved, must be 0
 *       26-16   11   0x3bb   Facility number
 *  LSB  15-00   16   varies  Epoll events (copied from `struct epoll_event`)
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define SYNTHETIC_EVENT__NTSTATUS 0x23bb0000UL

int synthetic_event_post_completion(port_state_t* port_state,
                                    const struct epoll_event* ev) {
  HANDLE iocp_handle;
  NTSTATUS status;

  /* These are the "fields" of the completion packet that will be posted to
   * the I/O completion port. Windows will not interpret or modify them when
   * `NtSetIoCompletion` is used. A fourth field, typically called
   * `IoCompletionKey` or `KeyContext`, is deliberatly not used; we might need
   * it in the future to add support for sharing an I/O completion port with
   * other libraries. */
  uint32_t status_field;
  uintptr_t data_fields[2] = {0, 0};

  /* Validate the events and flags specified by `ev.events`:
   *  - Only bits corresponding with actual epoll events may be set.
   *  - It is explicitly allowed to not specify any events at all.
   *  - The `EPOLLONESHOT` flag must be set.
   *  - No other flags may be specified.
   */
  if ((ev->events & ~API_VALID_EPOLL_EVENTS) != EPOLLONESHOT)
    return_set_error(-1, ERROR_INVALID_PARAMETER);
  status_field =
      SYNTHETIC_EVENT__NTSTATUS | (ev->events & API_VALID_EPOLL_EVENTS);

  /* On 64-bit platforms, `epoll_data_t` is the size of a pointer, so only
   * `data_fields[0]` is assigned a value here. On 32-bit platforms, both
   * array elements are used. */
  assert(sizeof ev->data <= sizeof data_fields);
  memcpy(data_fields, &ev->data, sizeof ev->data);

  iocp_handle = port_get_iocp_handle(port_state);

  // TODO: check
  // https://bugs.chromium.org/p/project-zero/issues/detail?id=1269.
  status = NtSetIoCompletion(iocp_handle,
                             NULL,
                             (VOID*) data_fields[0],
                             (NTSTATUS) status_field,
                             (ULONG_PTR) data_fields[1]);
  if (status != STATUS_SUCCESS)
    return_set_error(-1, RtlNtStatusToDosError(status));

  return 0;
}

int synthetic_event_feed_completion(const OVERLAPPED_ENTRY* completion,
                                    struct epoll_event* ev) {
  uint32_t status_field;
  uintptr_t data_fields[2];

  status_field = (uint32_t) completion->Internal;
  if ((status_field & ~API_VALID_EPOLL_EVENTS) != SYNTHETIC_EVENT__NTSTATUS)
    return 0; /* This completion packet does not contain a synthetic event. */

  ev->events = status_field & API_VALID_EPOLL_EVENTS;

  data_fields[0] = (uintptr_t) completion->lpOverlapped;
  data_fields[1] = completion->dwNumberOfBytesTransferred;
  memcpy(&ev->data, data_fields, sizeof ev->data);

  return 1;
}
