#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

#include "ntstatus.h"

DWORD we_map_ntstatus_to_win_error(NTSTATUS ntstatus);
DWORD we_map_ntstatus_to_ws_error(NTSTATUS ntstatus);
errno_t we_map_win_error_to_errno(DWORD error);

void we_set_win_error(DWORD error);
void we_clear_win_error(void);

#endif /* ERROR_H_ */
