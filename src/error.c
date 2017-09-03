#include <errno.h>

#include "error-map.h"
#include "win.h"

#pragma warning(push)
#pragma warning(disable : 4127) /* "conditional expression is constant" */

unsigned long we_map_ntstatus_to_winsock_error(unsigned long ntstatus) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  if (winsock_error != -1 && ntstatus == ntstatus_code)                    \
    return (unsigned long) winsock_error;
  WE_ERROR_MAP(X)
#undef X
  return WSAEINVAL;
}

unsigned long we_map_ntstatus_to_win_error(unsigned long ntstatus) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  if (win_error != -1 && ntstatus == ntstatus_code)                        \
    return win_error;
  WE_ERROR_MAP(X)
#undef X
  return ERROR_MR_MID_NOT_FOUND;
}

errno_t we_map_ntstatus_to_errno(unsigned long ntstatus) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  if (c_err != -1 && ntstatus == ntstatus_code)                            \
    return c_err;
  WE_ERROR_MAP(X)
#undef X
  return EINVAL;
}

errno_t we_map_win_error_to_errno(unsigned long code) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  if (c_err != -1 && ((win_error != -1 && code == win_error) ||            \
                      (winsock_error != -1 && code == winsock_error)))     \
    return c_err;
  WE_ERROR_MAP(X)
#undef X
  return EINVAL;
}

const char* we_get_ntstatus_symbol(unsigned long ntstatus) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  {                                                                        \
    static const char SYMBOL[] = #ntstatus_symbol;                         \
    if (sizeof SYMBOL > 0 && ntstatus == ntstatus_code)                    \
      return SYMBOL;                                                       \
  }
  WE_ERROR_MAP(X)
#undef X
  return "";
}

const char* we_get_win_error_symbol(unsigned long code) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  {                                                                        \
    static const char SYMBOL[] = #win_error;                               \
    if (sizeof SYMBOL > 0 && win_error != -1 && code == win_error)         \
      return SYMBOL;                                                       \
  }
  WE_ERROR_MAP(X)
#undef X
  return "";
}

const char* we_get_errno_symbol(errno_t err) {
#define X(ntstatus_code, ntstatus_symbol, win_error, winsock_error, c_err) \
  {                                                                        \
    static const char SYMBOL[] = #c_err;                                   \
    if (sizeof SYMBOL > 0 && c_err != -1 && err == c_err)                  \
      return SYMBOL;                                                       \
  }
  WE_ERROR_MAP(X)
#undef X
  return "";
}

#pragma warning(pop)
