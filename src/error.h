#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

#include "internal.h"
#include "ntstatus.h"

EPOLL_INTERNAL DWORD we_map_ntstatus_to_win_error(NTSTATUS ntstatus);
EPOLL_INTERNAL DWORD we_map_ntstatus_to_ws_error(NTSTATUS ntstatus);
EPOLL_INTERNAL errno_t we_map_win_error_to_errno(DWORD error);

EPOLL_INTERNAL void we_set_win_error(DWORD error);

#define _return_error_helper(error, value)         \
  do {                                             \
    we_set_win_error(error);                       \
    /* { printf("%d\n", error); DebugBreak(); } */ \
    return (value);                                \
  } while (0)

#define return_error(value, ...) _return_error_helper(__VA_ARGS__ + 0, value)

#endif /* ERROR_H_ */
