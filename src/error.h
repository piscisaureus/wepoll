#ifndef WEPOLL_ERROR_H_
#define WEPOLL_ERROR_H_

#include <errno.h>

#include "internal.h"
#include "win.h"

#define _return_error_helper(error, value) \
  do {                                     \
    err_set_win_error(error);              \
    return (value);                        \
  } while (0)

#define return_error(value, ...) _return_error_helper(__VA_ARGS__ + 0, value)

WEPOLL_INTERNAL errno_t err_map_win_error_to_errno(DWORD error);
WEPOLL_INTERNAL void err_set_win_error(DWORD error);

#endif /* WEPOLL_ERROR_H_ */
