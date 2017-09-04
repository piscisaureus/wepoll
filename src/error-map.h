#ifndef ERROR_MAP_H_
#define ERROR_MAP_H_

/* clang-format off */

#define WE_ERROR_MAP(X)                                            \
  X(/* STATUS_ABANDONED */ 0x00000080,                             \
    ERROR_WAIT_NO_CHILDREN, ECHILD,                                \
    -1, -1)                                                        \
  X(/* STATUS_PENDING */ 0x00000103,                               \
    ERROR_IO_PENDING, EINPROGRESS,                                 \
    ERROR_IO_PENDING, EINPROGRESS)                                 \
  X(/* STATUS_WORKING_SET_LIMIT_RANGE */ 0x40000002,               \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_BUFFER_OVERFLOW */ 0x80000005,                       \
    ERROR_MORE_DATA, EMSGSIZE,                                     \
    WSAEMSGSIZE, EMSGSIZE)                                         \
  X(/* STATUS_NO_MORE_FILES */ 0x80000006,                         \
    ERROR_NO_MORE_FILES, ENOENT,                                   \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_PAPER_EMPTY */ 0x8000000e,                    \
    ERROR_OUT_OF_PAPER, EACCES,                                    \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_POWERED_OFF */ 0x8000000f,                    \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_OFF_LINE */ 0x80000010,                       \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_UNSUCCESSFUL */ 0xc0000001,                          \
    ERROR_GEN_FAILURE, EACCES,                                     \
    -1, -1)                                                        \
  X(/* STATUS_NOT_IMPLEMENTED */ 0xc0000002,                       \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_INFO_CLASS */ 0xc0000003,                    \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INFO_LENGTH_MISMATCH */ 0xc0000004,                  \
    ERROR_BAD_LENGTH, EACCES,                                      \
    -1, -1)                                                        \
  X(/* STATUS_ACCESS_VIOLATION */ 0xc0000005,                      \
    ERROR_NOACCESS, EFAULT,                                        \
    WSAEFAULT, EFAULT)                                             \
  X(/* STATUS_PAGEFILE_QUOTA */ 0xc0000007,                        \
    ERROR_PAGEFILE_QUOTA, ENOBUFS,                                 \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_INVALID_HANDLE */ 0xc0000008,                        \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    WSAENOTSOCK, ENOTSOCK)                                         \
  X(/* STATUS_BAD_INITIAL_PC */ 0xc000000a,                        \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_CID */ 0xc000000b,                           \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER */ 0xc000000d,                     \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_NO_SUCH_DEVICE */ 0xc000000e,                        \
    ERROR_FILE_NOT_FOUND, ENOENT,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_NO_SUCH_FILE */ 0xc000000f,                          \
    ERROR_FILE_NOT_FOUND, ENOENT,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_INVALID_DEVICE_REQUEST */ 0xc0000010,                \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_WRONG_VOLUME */ 0xc0000012,                          \
    ERROR_WRONG_DISK, EACCES,                                      \
    -1, -1)                                                        \
  X(/* STATUS_NO_MEDIA_IN_DEVICE */ 0xc0000013,                    \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_NONEXISTENT_SECTOR */ 0xc0000015,                    \
    ERROR_SECTOR_NOT_FOUND, EACCES,                                \
    -1, -1)                                                        \
  X(/* STATUS_NO_MEMORY */ 0xc0000017,                             \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_CONFLICTING_ADDRESSES */ 0xc0000018,                 \
    ERROR_INVALID_ADDRESS, ENOBUFS,                                \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_UNABLE_TO_FREE_VM */ 0xc000001a,                     \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_UNABLE_TO_DELETE_SECTION */ 0xc000001b,              \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_SYSTEM_SERVICE */ 0xc000001c,                \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_LOCK_SEQUENCE */ 0xc000001e,                 \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_VIEW_SIZE */ 0xc000001f,                     \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_FILE_FOR_SECTION */ 0xc0000020,              \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_ALREADY_COMMITTED */ 0xc0000021,                     \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_ACCESS_DENIED */ 0xc0000022,                         \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    WSAEACCES, EACCES)                                             \
  X(/* STATUS_BUFFER_TOO_SMALL */ 0xc0000023,                      \
    ERROR_INSUFFICIENT_BUFFER, EFAULT,                             \
    WSAEFAULT, EFAULT)                                             \
  X(/* STATUS_OBJECT_TYPE_MISMATCH */ 0xc0000024,                  \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    WSAENOTSOCK, ENOTSOCK)                                         \
  X(/* STATUS_NOT_LOCKED */ 0xc000002a,                            \
    ERROR_NOT_LOCKED, EACCES,                                      \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_PARAMETER_MIX */ 0xc0000030,                 \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_OBJECT_NAME_NOT_FOUND */ 0xc0000034,                 \
    ERROR_FILE_NOT_FOUND, ENOENT,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_OBJECT_NAME_COLLISION */ 0xc0000035,                 \
    ERROR_ALREADY_EXISTS, EEXIST,                                  \
    -1, -1)                                                        \
  X(/* STATUS_PORT_DISCONNECTED */ 0xc0000037,                     \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_OBJECT_PATH_INVALID */ 0xc0000039,                   \
    ERROR_BAD_PATHNAME, ENOENT,                                    \
    -1, -1)                                                        \
  X(/* STATUS_OBJECT_PATH_NOT_FOUND */ 0xc000003a,                 \
    ERROR_PATH_NOT_FOUND, ENOENT,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_OBJECT_PATH_SYNTAX_BAD */ 0xc000003b,                \
    ERROR_BAD_PATHNAME, ENOENT,                                    \
    -1, -1)                                                        \
  X(/* STATUS_DATA_ERROR */ 0xc000003e,                            \
    ERROR_CRC, EACCES,                                             \
    -1, -1)                                                        \
  X(/* STATUS_CRC_ERROR */ 0xc000003f,                             \
    ERROR_CRC, EACCES,                                             \
    -1, -1)                                                        \
  X(/* STATUS_SECTION_TOO_BIG */ 0xc0000040,                       \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    -1, -1)                                                        \
  X(/* STATUS_PORT_CONNECTION_REFUSED */ 0xc0000041,               \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_PORT_HANDLE */ 0xc0000042,                   \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_SHARING_VIOLATION */ 0xc0000043,                     \
    ERROR_SHARING_VIOLATION, EACCES,                               \
    WSAEADDRINUSE, EADDRINUSE)                                     \
  X(/* STATUS_QUOTA_EXCEEDED */ 0xc0000044,                        \
    ERROR_NOT_ENOUGH_QUOTA, ENOMEM,                                \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_INVALID_PAGE_PROTECTION */ 0xc0000045,               \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_PORT_ALREADY_SET */ 0xc0000048,                      \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_SECTION_NOT_IMAGE */ 0xc0000049,                     \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_THREAD_IS_TERMINATING */ 0xc000004b,                 \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_BAD_WORKING_SET_LIMIT */ 0xc000004c,                 \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INCOMPATIBLE_FILE_MAP */ 0xc000004d,                 \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_SECTION_PROTECTION */ 0xc000004e,                    \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_FILE_LOCK_CONFLICT */ 0xc0000054,                    \
    ERROR_LOCK_VIOLATION, EACCES,                                  \
    -1, -1)                                                        \
  X(/* STATUS_LOCK_NOT_GRANTED */ 0xc0000055,                      \
    ERROR_LOCK_VIOLATION, EACCES,                                  \
    -1, -1)                                                        \
  X(/* STATUS_DELETE_PENDING */ 0xc0000056,                        \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_FORMAT */ 0xc000007b,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_RANGE_NOT_LOCKED */ 0xc000007e,                      \
    ERROR_NOT_LOCKED, EACCES,                                      \
    -1, -1)                                                        \
  X(/* STATUS_DISK_FULL */ 0xc000007f,                             \
    ERROR_DISK_FULL, ENOSPC,                                       \
    -1, -1)                                                        \
  X(/* STATUS_TOO_MANY_PAGING_FILES */ 0xc0000097,                 \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_INSUFFICIENT_RESOURCES */ 0xc000009a,                \
    ERROR_NO_SYSTEM_RESOURCES, ENOBUFS,                            \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_DFS_EXIT_PATH_FOUND */ 0xc000009b,                   \
    ERROR_PATH_NOT_FOUND, ENOENT,                                  \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_DATA_ERROR */ 0xc000009c,                     \
    ERROR_CRC, EACCES,                                             \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_POWER_FAILURE */ 0xc000009e,                  \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_WORKING_SET_QUOTA */ 0xc00000a1,                     \
    ERROR_WORKING_SET_QUOTA, ENOBUFS,                              \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_MEDIA_WRITE_PROTECTED */ 0xc00000a2,                 \
    ERROR_WRITE_PROTECT, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_NOT_READY */ 0xc00000a3,                      \
    ERROR_NOT_READY, EACCES,                                       \
    WSAEWOULDBLOCK, EWOULDBLOCK)                                   \
  X(/* STATUS_BAD_MASTER_BOOT_RECORD */ 0xc00000a9,                \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_ILLEGAL_FUNCTION */ 0xc00000af,                      \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_PIPE_DISCONNECTED */ 0xc00000b0,                     \
    ERROR_PIPE_NOT_CONNECTED, EPIPE,                               \
    WSAESHUTDOWN, EPIPE)                                           \
  X(/* STATUS_IO_TIMEOUT */ 0xc00000b5,                            \
    ERROR_SEM_TIMEOUT, ETIMEDOUT,                                  \
    WSAETIMEDOUT, ETIMEDOUT)                                       \
  X(/* STATUS_FILE_IS_A_DIRECTORY */ 0xc00000ba,                   \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_NOT_SUPPORTED */ 0xc00000bb,                         \
    ERROR_NOT_SUPPORTED, EOPNOTSUPP,                               \
    WSAEOPNOTSUPP, EOPNOTSUPP)                                     \
  X(/* STATUS_REMOTE_NOT_LISTENING */ 0xc00000bc,                  \
    ERROR_REM_NOT_LIST, ECONNREFUSED,                              \
    WSAECONNREFUSED, ECONNREFUSED)                                 \
  X(/* STATUS_BAD_NETWORK_PATH */ 0xc00000be,                      \
    ERROR_BAD_NETPATH, ENOENT,                                     \
    WSAENETUNREACH, ENETUNREACH)                                   \
  X(/* STATUS_NETWORK_BUSY */ 0xc00000bf,                          \
    ERROR_NETWORK_BUSY, ENETDOWN,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_INVALID_NETWORK_RESPONSE */ 0xc00000c3,              \
    ERROR_BAD_NET_RESP, ENETDOWN,                                  \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_UNEXPECTED_NETWORK_ERROR */ 0xc00000c4,              \
    ERROR_UNEXP_NET_ERR, ENETDOWN,                                 \
    WSAENETDOWN, ENETDOWN)                                         \
  X(/* STATUS_NETWORK_ACCESS_DENIED */ 0xc00000ca,                 \
    ERROR_NETWORK_ACCESS_DENIED, EACCES,                           \
    -1, -1)                                                        \
  X(/* STATUS_BAD_NETWORK_NAME */ 0xc00000cc,                      \
    ERROR_BAD_NET_NAME, ENOENT,                                    \
    -1, -1)                                                        \
  X(/* STATUS_REQUEST_NOT_ACCEPTED */ 0xc00000d0,                  \
    ERROR_REQ_NOT_ACCEP, EWOULDBLOCK,                              \
    WSAEWOULDBLOCK, EWOULDBLOCK)                                   \
  X(/* STATUS_NOT_SAME_DEVICE */ 0xc00000d4,                       \
    ERROR_NOT_SAME_DEVICE, EXDEV,                                  \
    -1, -1)                                                        \
  X(/* STATUS_FILE_RENAMED */ 0xc00000d5,                          \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_PARAMETER_1 */ 0xc00000ef,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_2 */ 0xc00000f0,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_3 */ 0xc00000f1,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_4 */ 0xc00000f2,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_5 */ 0xc00000f3,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_6 */ 0xc00000f4,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_7 */ 0xc00000f5,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_8 */ 0xc00000f6,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_9 */ 0xc00000f7,                   \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_10 */ 0xc00000f8,                  \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_11 */ 0xc00000f9,                  \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_PARAMETER_12 */ 0xc00000fa,                  \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_REDIRECTOR_NOT_STARTED */ 0xc00000fb,                \
    ERROR_PATH_NOT_FOUND, ENOENT,                                  \
    -1, -1)                                                        \
  X(/* STATUS_DIRECTORY_NOT_EMPTY */ 0xc0000101,                   \
    ERROR_DIR_NOT_EMPTY, ENOTEMPTY,                                \
    -1, -1)                                                        \
  X(/* STATUS_NAME_TOO_LONG */ 0xc0000106,                         \
    ERROR_FILENAME_EXCED_RANGE, ENOENT,                            \
    -1, -1)                                                        \
  X(/* STATUS_PROCESS_IS_TERMINATING */ 0xc000010a,                \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_NE_FORMAT */ 0xc000011b,               \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_TOO_MANY_OPENED_FILES */ 0xc000011f,                 \
    ERROR_TOO_MANY_OPEN_FILES, EMFILE,                             \
    -1, -1)                                                        \
  X(/* STATUS_CANCELLED */ 0xc0000120,                             \
    ERROR_OPERATION_ABORTED, ECANCELED,                            \
    ERROR_OPERATION_ABORTED, ECANCELED)                            \
  X(/* STATUS_CANNOT_DELETE */ 0xc0000121,                         \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_FILE_DELETED */ 0xc0000123,                          \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_FILE_CLOSED */ 0xc0000128,                           \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_COMMITMENT_LIMIT */ 0xc000012d,                      \
    ERROR_COMMITMENT_LIMIT, ENOBUFS,                               \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_INVALID_IMAGE_LE_FORMAT */ 0xc000012e,               \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_NOT_MZ */ 0xc000012f,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_PROTECT */ 0xc0000130,                 \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_WIN_16 */ 0xc0000131,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_LOCAL_DISCONNECT */ 0xc000013b,                      \
    ERROR_NETNAME_DELETED, ECONNABORTED,                           \
    WSAECONNABORTED, ECONNABORTED)                                 \
  X(/* STATUS_REMOTE_DISCONNECT */ 0xc000013c,                     \
    ERROR_NETNAME_DELETED, ECONNRESET,                             \
    WSAECONNRESET, ECONNRESET)                                     \
  X(/* STATUS_REMOTE_RESOURCES */ 0xc000013d,                      \
    ERROR_REM_NOT_LIST, ENOBUFS,                                   \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_LINK_FAILED */ 0xc000013e,                           \
    ERROR_UNEXP_NET_ERR, ECONNRESET,                               \
    WSAECONNRESET, ECONNRESET)                                     \
  X(/* STATUS_LINK_TIMEOUT */ 0xc000013f,                          \
    ERROR_UNEXP_NET_ERR, ETIMEDOUT,                                \
    WSAETIMEDOUT, ETIMEDOUT)                                       \
  X(/* STATUS_INVALID_CONNECTION */ 0xc0000140,                    \
    ERROR_UNEXP_NET_ERR, ENOTCONN,                                 \
    WSAENOTCONN, ENOTCONN)                                         \
  X(/* STATUS_INVALID_ADDRESS */ 0xc0000141,                       \
    ERROR_UNEXP_NET_ERR, EADDRNOTAVAIL,                            \
    WSAEADDRNOTAVAIL, EADDRNOTAVAIL)                               \
  X(/* STATUS_PIPE_BROKEN */ 0xc000014b,                           \
    ERROR_BROKEN_PIPE, EPIPE,                                      \
    -1, -1)                                                        \
  X(/* STATUS_DEVICE_CONFIGURATION_ERROR */ 0xc0000182,            \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_DEVICE_STATE */ 0xc0000184,                  \
    ERROR_BAD_COMMAND, EACCES,                                     \
    -1, -1)                                                        \
  X(/* STATUS_TOO_LATE */ 0xc0000189,                              \
    ERROR_WRITE_PROTECT, EACCES,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc00001ad,                                      \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    -1, -1)                                                        \
  X(/* STATUS_NETWORK_OPEN_RESTRICTION */ 0xc0000201,              \
    ERROR_NETWORK_ACCESS_DENIED, EACCES,                           \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_BUFFER_SIZE */ 0xc0000206,                   \
    ERROR_INVALID_USER_BUFFER, EMSGSIZE,                           \
    WSAEMSGSIZE, EMSGSIZE)                                         \
  X(/* STATUS_INVALID_ADDRESS_COMPONENT */ 0xc0000207,             \
    ERROR_INVALID_NETNAME, EADDRNOTAVAIL,                          \
    WSAEADDRNOTAVAIL, EADDRNOTAVAIL)                               \
  X(/* STATUS_TOO_MANY_ADDRESSES */ 0xc0000209,                    \
    ERROR_TOO_MANY_NAMES, ENOBUFS,                                 \
    WSAENOBUFS, ENOBUFS)                                           \
  X(/* STATUS_ADDRESS_ALREADY_EXISTS */ 0xc000020a,                \
    ERROR_DUP_NAME, EADDRINUSE,                                    \
    WSAEADDRINUSE, EADDRINUSE)                                     \
  X(/* STATUS_CONNECTION_DISCONNECTED */ 0xc000020c,               \
    ERROR_NETNAME_DELETED, ECONNRESET,                             \
    WSAECONNRESET, ECONNRESET)                                     \
  X(/* STATUS_CONNECTION_RESET */ 0xc000020d,                      \
    ERROR_NETNAME_DELETED, ECONNRESET,                             \
    WSAECONNRESET, ECONNRESET)                                     \
  X(/* STATUS_TRANSACTION_ABORTED */ 0xc000020f,                   \
    ERROR_UNEXP_NET_ERR, ECONNABORTED,                             \
    WSAECONNABORTED, ECONNABORTED)                                 \
  X(/* STATUS_IMAGE_CHECKSUM_MISMATCH */ 0xc0000221,               \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_FAIL_CHECK */ 0xc0000229,                            \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_HANDLE_NOT_CLOSABLE */ 0xc0000235,                   \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_CONNECTION_REFUSED */ 0xc0000236,                    \
    ERROR_CONNECTION_REFUSED, ECONNREFUSED,                        \
    WSAECONNREFUSED, ECONNREFUSED)                                 \
  X(/* STATUS_GRACEFUL_DISCONNECT */ 0xc0000237,                   \
    ERROR_GRACEFUL_DISCONNECT, EPIPE,                              \
    WSAEDISCON, EPIPE)                                             \
  X(/* STATUS_CONNECTION_ACTIVE */ 0xc000023b,                     \
    ERROR_CONNECTION_ACTIVE, EISCONN,                              \
    WSAEISCONN, EISCONN)                                           \
  X(/* STATUS_NETWORK_UNREACHABLE */ 0xc000023c,                   \
    ERROR_NETWORK_UNREACHABLE, ENETUNREACH,                        \
    WSAENETUNREACH, ENETUNREACH)                                   \
  X(/* STATUS_HOST_UNREACHABLE */ 0xc000023d,                      \
    ERROR_HOST_UNREACHABLE, EHOSTUNREACH,                          \
    WSAEHOSTUNREACH, EHOSTUNREACH)                                 \
  X(/* STATUS_PROTOCOL_UNREACHABLE */ 0xc000023e,                  \
    ERROR_PROTOCOL_UNREACHABLE, ENETUNREACH,                       \
    WSAENETUNREACH, ENETUNREACH)                                   \
  X(/* STATUS_PORT_UNREACHABLE */ 0xc000023f,                      \
    ERROR_PORT_UNREACHABLE, ECONNRESET,                            \
    WSAECONNRESET, ECONNRESET)                                     \
  X(/* STATUS_REQUEST_ABORTED */ 0xc0000240,                       \
    ERROR_REQUEST_ABORTED, EINTR,                                  \
    WSAEINTR, EINTR)                                               \
  X(/* STATUS_CONNECTION_ABORTED */ 0xc0000241,                    \
    ERROR_CONNECTION_ABORTED, ECONNABORTED,                        \
    WSAECONNABORTED, ECONNABORTED)                                 \
  X(/* STATUS_IMAGE_MP_UP_MISMATCH */ 0xc0000249,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_VOLUME_DISMOUNTED */ 0xc000026e,                     \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_DIRECTORY_IS_A_REPARSE_POINT */ 0xc0000281,          \
    ERROR_BAD_PATHNAME, ENOENT,                                    \
    -1, -1)                                                        \
  X(/* STATUS_ENCRYPTION_FAILED */ 0xc000028a,                     \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_DECRYPTION_FAILED */ 0xc000028b,                     \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_NO_RECOVERY_POLICY */ 0xc000028d,                    \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_NO_EFS */ 0xc000028e,                                \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_WRONG_EFS */ 0xc000028f,                             \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_NO_USER_KEYS */ 0xc0000290,                          \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)                                                        \
  X(/* STATUS_VOLUME_NOT_UPGRADED */ 0xc000029c,                   \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_DATATYPE_MISALIGNMENT_ERROR */ 0xc00002c5,           \
    ERROR_NOACCESS, EFAULT,                                        \
    WSAEFAULT, EFAULT)                                             \
  X(/* STATUS_POWER_STATE_INVALID */ 0xc00002d3,                   \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_CANNOT_MAKE */ 0xc00002ea,                           \
    ERROR_CANNOT_MAKE, EACCES,                                     \
    -1, -1)                                                        \
  X(/* STATUS_OBJECTID_NOT_FOUND */ 0xc00002f0,                    \
    ERROR_FILE_NOT_FOUND, ENOENT,                                  \
    -1, -1)                                                        \
  X(/* STATUS_HOST_DOWN */ 0xc0000350,                             \
    ERROR_HOST_DOWN, EHOSTUNREACH,                                 \
    WSAEHOSTDOWN, EHOSTUNREACH)                                    \
  X(/* STATUS_INVALID_IMAGE_WIN_32 */ 0xc0000359,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_IMAGE_WIN_64 */ 0xc000035a,                  \
    ERROR_BAD_EXE_FORMAT, ENOEXEC,                                 \
    -1, -1)                                                        \
  X(/* STATUS_FAILED_STACK_SWITCH */ 0xc0000373,                   \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000416,                                      \
    ERROR_NOT_ENOUGH_MEMORY, ENOMEM,                               \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000455,                                      \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000456,                                      \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* unknown */ 0xc0000457,                                      \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* unknown */ 0xc0000458,                                      \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* unknown */ 0xc0000467,                                      \
    ERROR_SHARING_VIOLATION, EACCES,                               \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000487,                                      \
    ERROR_GEN_FAILURE, EACCES,                                     \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000491,                                      \
    ERROR_FILE_NOT_FOUND, ENOENT,                                  \
    -1, -1)                                                        \
  X(/* unknown */ 0xc0000495,                                      \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_CALLBACK_BYPASS */ 0xc0000503,                       \
    ERROR_NOT_READY, EACCES,                                       \
    -1, -1)                                                        \
  X(/* STATUS_INVALID_MESSAGE */ 0xc0000702,                       \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_LPC_RECEIVE_BUFFER_EXPECTED */ 0xc0000705,           \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_LPC_INVALID_CONNECTION_USAGE */ 0xc0000706,          \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_THREADPOOL_HANDLE_EXCEPTION */ 0xc000070a,           \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc000070b,                                      \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc000070c,                                      \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc000070d,                                      \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* unknown */ 0xc000070e,                                      \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_THREADPOOL_RELEASED_DURING_OPERATION */ 0xc000070f,  \
    ERROR_INVALID_HANDLE, EBADF,                                   \
    -1, -1)                                                        \
  X(/* STATUS_CALLBACK_RETURNED_WHILE_IMPERSONATING */ 0xc0000710, \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_APC_RETURNED_WHILE_IMPERSONATING */ 0xc0000711,      \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_THREAD_PRIORITY */ 0xc000071b,     \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_INVALID_THREAD */ 0xc000071c,                        \
    ERROR_INVALID_PARAMETER, EINVAL,                               \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_TRANSACTION */ 0xc000071d,         \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_LDR_LOCK */ 0xc000071e,            \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_LANG */ 0xc000071f,                \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_PRI_BACK */ 0xc0000720,            \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_CALLBACK_RETURNED_THREAD_AFFINITY */ 0xc0000721,     \
    ERROR_INVALID_FUNCTION, EINVAL,                                \
    WSAEINVAL, EINVAL)                                             \
  X(/* STATUS_AUTH_TAG_MISMATCH */ 0xc000a002,                     \
    ERROR_CRC, EACCES,                                             \
    -1, -1)                                                        \
  X(/* STATUS_HOPLIMIT_EXCEEDED */ 0xc000a012,                     \
    ERROR_HOST_UNREACHABLE, EHOSTUNREACH,                          \
    WSAENETRESET, EHOSTUNREACH)                                    \
  X(/* STATUS_PROTOCOL_NOT_SUPPORTED */ 0xc000a013,                \
    ERROR_NOT_SUPPORTED, EAFNOSUPPORT,                             \
    WSAEAFNOSUPPORT, EAFNOSUPPORT)                                 \
  X(/* unknown */ 0xc000a203,                                      \
    ERROR_ACCESS_DENIED, EACCES,                                   \
    -1, -1)

/* clang-format on */

#endif /* ERROR_MAP_H_ */
