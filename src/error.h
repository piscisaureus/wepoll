#ifndef ERROR_H_
#define ERROR_H_

#include <errno.h>

unsigned long we_map_ntstatus_to_winsock_error(unsigned long ntstatus);
unsigned long we_map_ntstatus_to_win_error(unsigned long ntstatus);
errno_t we_map_ntstatus_to_errno(unsigned long ntstatus);

errno_t we_map_win_error_to_errno(unsigned long code);

const char* we_get_ntstatus_symbol(unsigned long ntstatus);
const char* we_get_win_error_symbol(unsigned long code);
const char* we_get_errno_symbol(errno_t err);

#endif /* ERROR_H_ */
