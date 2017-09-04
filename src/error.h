#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

#include "ntstatus.h"

DWORD we_map_ntstatus_to_win_error(NTSTATUS ntstatus);
DWORD we_map_ntstatus_to_ws_error(NTSTATUS ntstatus);
errno_t we_map_win_error_to_errno(DWORD error);

void we_set_win_error(DWORD error);
void we_clear_win_error(void);

#define _return_error_helper(error, value) \
  do {                                     \
    we_set_win_error(error);               \
    return (value);                        \
  } while (0)

#define return_error(value, ...) _return_error_helper(__VA_ARGS__ + 0, value)

#define return_success(value) \
  do {                        \
    we_clear_win_error();     \
    return (value);           \
  } while (0)

#endif /* ERROR_H_ */
