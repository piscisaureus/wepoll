#include <errno.h>

#include "error-map.h"
#include "error.h"
#include "nt.h"
#include "ntstatus.h"
#include "win.h"

#pragma warning(push)
#pragma warning(disable : 4127) /* "conditional expression is constant" */

DWORD we_map_ntstatus_to_win_error(NTSTATUS status) {
  if (NT_SUCCESS(status))
    return ERROR_SUCCESS;
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (status == ntstatus && win_error != -1)                  \
    return (DWORD) win_error;
  WE_ERROR_MAP(X)
#undef X
  return RtlNtStatusToDosError(status);
}

DWORD we_map_ntstatus_to_ws_error(NTSTATUS status) {
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (status == ntstatus && ws_error != -1)                   \
    return (DWORD) ws_error;
  WE_ERROR_MAP(X)
#undef X
  return we_map_ntstatus_to_win_error(status);
}

errno_t we_map_win_error_to_errno(DWORD error) {
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (error == win_error && win_errno != -1)                  \
    return win_errno;                                         \
  if (error == ws_error && ws_errno != -1)                    \
    return ws_errno;
  WE_ERROR_MAP(X)
#undef X
  return EINVAL;
}

#pragma warning(pop)

void we_set_win_error(DWORD error) {
  if (error == 0)
    error = GetLastError();
  else
    SetLastError(error);
  errno = we_map_win_error_to_errno(error);
}

void we_clear_win_error(void) {
  SetLastError(ERROR_SUCCESS);
}
