#include <errno.h>

#include "error.h"
#include "win.h"

#define ERR__ERRNO_MAPPINGS(X)               \
  X(ERROR_ACCESS_DENIED, EACCES)             \
  X(ERROR_ALREADY_EXISTS, EEXIST)            \
  X(ERROR_BAD_COMMAND, EACCES)               \
  X(ERROR_BAD_EXE_FORMAT, ENOEXEC)           \
  X(ERROR_BAD_LENGTH, EACCES)                \
  X(ERROR_BAD_NETPATH, ENOENT)               \
  X(ERROR_BAD_NET_NAME, ENOENT)              \
  X(ERROR_BAD_NET_RESP, ENETDOWN)            \
  X(ERROR_BAD_PATHNAME, ENOENT)              \
  X(ERROR_BROKEN_PIPE, EPIPE)                \
  X(ERROR_CANNOT_MAKE, EACCES)               \
  X(ERROR_COMMITMENT_LIMIT, ENOMEM)          \
  X(ERROR_CONNECTION_ABORTED, ECONNABORTED)  \
  X(ERROR_CONNECTION_ACTIVE, EISCONN)        \
  X(ERROR_CONNECTION_REFUSED, ECONNREFUSED)  \
  X(ERROR_CRC, EACCES)                       \
  X(ERROR_DIR_NOT_EMPTY, ENOTEMPTY)          \
  X(ERROR_DISK_FULL, ENOSPC)                 \
  X(ERROR_DUP_NAME, EADDRINUSE)              \
  X(ERROR_FILENAME_EXCED_RANGE, ENOENT)      \
  X(ERROR_FILE_NOT_FOUND, ENOENT)            \
  X(ERROR_GEN_FAILURE, EACCES)               \
  X(ERROR_GRACEFUL_DISCONNECT, EPIPE)        \
  X(ERROR_HOST_DOWN, EHOSTUNREACH)           \
  X(ERROR_HOST_UNREACHABLE, EHOSTUNREACH)    \
  X(ERROR_INSUFFICIENT_BUFFER, EFAULT)       \
  X(ERROR_INVALID_ADDRESS, EADDRNOTAVAIL)    \
  X(ERROR_INVALID_FUNCTION, EINVAL)          \
  X(ERROR_INVALID_HANDLE, EBADF)             \
  X(ERROR_INVALID_NETNAME, EADDRNOTAVAIL)    \
  X(ERROR_INVALID_PARAMETER, EINVAL)         \
  X(ERROR_INVALID_USER_BUFFER, EMSGSIZE)     \
  X(ERROR_IO_PENDING, EINPROGRESS)           \
  X(ERROR_LOCK_VIOLATION, EACCES)            \
  X(ERROR_MORE_DATA, EMSGSIZE)               \
  X(ERROR_NETNAME_DELETED, ECONNABORTED)     \
  X(ERROR_NETWORK_ACCESS_DENIED, EACCES)     \
  X(ERROR_NETWORK_BUSY, ENETDOWN)            \
  X(ERROR_NETWORK_UNREACHABLE, ENETUNREACH)  \
  X(ERROR_NOACCESS, EFAULT)                  \
  X(ERROR_NONPAGED_SYSTEM_RESOURCES, ENOMEM) \
  X(ERROR_NOT_ENOUGH_MEMORY, ENOMEM)         \
  X(ERROR_NOT_ENOUGH_QUOTA, ENOMEM)          \
  X(ERROR_NOT_FOUND, ENOENT)                 \
  X(ERROR_NOT_LOCKED, EACCES)                \
  X(ERROR_NOT_READY, EACCES)                 \
  X(ERROR_NOT_SAME_DEVICE, EXDEV)            \
  X(ERROR_NOT_SUPPORTED, ENOTSUP)            \
  X(ERROR_NO_MORE_FILES, ENOENT)             \
  X(ERROR_NO_SYSTEM_RESOURCES, ENOMEM)       \
  X(ERROR_OPERATION_ABORTED, EINTR)          \
  X(ERROR_OUT_OF_PAPER, EACCES)              \
  X(ERROR_PAGED_SYSTEM_RESOURCES, ENOMEM)    \
  X(ERROR_PAGEFILE_QUOTA, ENOMEM)            \
  X(ERROR_PATH_NOT_FOUND, ENOENT)            \
  X(ERROR_PIPE_NOT_CONNECTED, EPIPE)         \
  X(ERROR_PORT_UNREACHABLE, ECONNRESET)      \
  X(ERROR_PROTOCOL_UNREACHABLE, ENETUNREACH) \
  X(ERROR_REM_NOT_LIST, ECONNREFUSED)        \
  X(ERROR_REQUEST_ABORTED, EINTR)            \
  X(ERROR_REQ_NOT_ACCEP, EWOULDBLOCK)        \
  X(ERROR_SECTOR_NOT_FOUND, EACCES)          \
  X(ERROR_SEM_TIMEOUT, ETIMEDOUT)            \
  X(ERROR_SHARING_VIOLATION, EACCES)         \
  X(ERROR_TOO_MANY_NAMES, ENOMEM)            \
  X(ERROR_TOO_MANY_OPEN_FILES, EMFILE)       \
  X(ERROR_UNEXP_NET_ERR, ECONNABORTED)       \
  X(ERROR_WAIT_NO_CHILDREN, ECHILD)          \
  X(ERROR_WORKING_SET_QUOTA, ENOMEM)         \
  X(ERROR_WRITE_PROTECT, EACCES)             \
  X(ERROR_WRONG_DISK, EACCES)                \
  X(WSAEACCES, EACCES)                       \
  X(WSAEADDRINUSE, EADDRINUSE)               \
  X(WSAEADDRNOTAVAIL, EADDRNOTAVAIL)         \
  X(WSAEAFNOSUPPORT, EAFNOSUPPORT)           \
  X(WSAECONNABORTED, ECONNABORTED)           \
  X(WSAECONNREFUSED, ECONNREFUSED)           \
  X(WSAECONNRESET, ECONNRESET)               \
  X(WSAEDISCON, EPIPE)                       \
  X(WSAEFAULT, EFAULT)                       \
  X(WSAEHOSTDOWN, EHOSTUNREACH)              \
  X(WSAEHOSTUNREACH, EHOSTUNREACH)           \
  X(WSAEINPROGRESS, EBUSY)                   \
  X(WSAEINTR, EINTR)                         \
  X(WSAEINVAL, EINVAL)                       \
  X(WSAEISCONN, EISCONN)                     \
  X(WSAEMSGSIZE, EMSGSIZE)                   \
  X(WSAENETDOWN, ENETDOWN)                   \
  X(WSAENETRESET, EHOSTUNREACH)              \
  X(WSAENETUNREACH, ENETUNREACH)             \
  X(WSAENOBUFS, ENOMEM)                      \
  X(WSAENOTCONN, ENOTCONN)                   \
  X(WSAENOTSOCK, ENOTSOCK)                   \
  X(WSAEOPNOTSUPP, EOPNOTSUPP)               \
  X(WSAEPROCLIM, ENOMEM)                     \
  X(WSAESHUTDOWN, EPIPE)                     \
  X(WSAETIMEDOUT, ETIMEDOUT)                 \
  X(WSAEWOULDBLOCK, EWOULDBLOCK)             \
  X(WSANOTINITIALISED, ENETDOWN)             \
  X(WSASYSNOTREADY, ENETDOWN)                \
  X(WSAVERNOTSUPPORTED, ENOSYS)

static errno_t err__map_win_error_to_errno(DWORD error) {
  switch (error) {
#define X(error_sym, errno_sym) \
  case error_sym:               \
    return errno_sym;
    ERR__ERRNO_MAPPINGS(X)
#undef X
  }
  return EINVAL;
}

void err_map_win_error(void) {
  errno = err__map_win_error_to_errno(GetLastError());
}

void err_set_win_error(DWORD error) {
  SetLastError(error);
  errno = err__map_win_error_to_errno(error);
}

int err_check_handle(HANDLE handle) {
  DWORD flags;

  /* GetHandleInformation() succeeds when passed INVALID_HANDLE_VALUE, so check
   * for this condition explicitly. */
  if (handle == INVALID_HANDLE_VALUE)
    return_set_error(-1, ERROR_INVALID_HANDLE);

  if (!GetHandleInformation(handle, &flags))
    return_map_error(-1);

  return 0;
}
