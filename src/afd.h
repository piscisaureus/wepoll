#ifndef WEPOLL_AFD_H_
#define WEPOLL_AFD_H_

#include "internal.h"
#include "nt.h"
#include "util.h"
#include "win.h"

/* clang-format off */
#define AFD_POLL_RECEIVE           0x0001
#define AFD_POLL_RECEIVE_EXPEDITED 0x0002
#define AFD_POLL_SEND              0x0004
#define AFD_POLL_DISCONNECT        0x0008
#define AFD_POLL_ABORT             0x0010
#define AFD_POLL_LOCAL_CLOSE       0x0020
#define AFD_POLL_ACCEPT            0x0080
#define AFD_POLL_CONNECT_FAIL      0x0100
/* clang-format on */

typedef struct _AFD_POLL_HANDLE_INFO {
  HANDLE Handle;
  ULONG Events;
  NTSTATUS Status;
} AFD_POLL_HANDLE_INFO, *PAFD_POLL_HANDLE_INFO;

typedef struct _AFD_POLL_INFO {
  LARGE_INTEGER Timeout;
  ULONG NumberOfHandles;
  ULONG Exclusive;
  AFD_POLL_HANDLE_INFO Handles[1];
} AFD_POLL_INFO, *PAFD_POLL_INFO;

WEPOLL_INTERNAL int afd_create_helper_handle(HANDLE iocp,
                                             HANDLE* afd_helper_handle_out);

WEPOLL_INTERNAL int afd_poll(HANDLE afd_helper_handle,
                             AFD_POLL_INFO* poll_info,
                             OVERLAPPED* overlapped);

#endif /* WEPOLL_AFD_H_ */
