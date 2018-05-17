#ifndef WEPOLL_ERROR_H_
#define WEPOLL_ERROR_H_

#include "internal.h"
#include "win.h"

#define return_map_error(value) \
  do {                          \
    err_map_win_error();        \
    return (value);             \
  } while (0)

#define return_set_error(value, error) \
  do {                                 \
    err_set_win_error(error);          \
    return (value);                    \
  } while (0)

WEPOLL_INTERNAL void err_map_win_error(void);
WEPOLL_INTERNAL void err_set_win_error(DWORD error);
WEPOLL_INTERNAL int err_check_handle(HANDLE handle);

#endif /* WEPOLL_ERROR_H_ */
