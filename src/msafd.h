 
#ifndef EPOLL_MSAFD_H_
#define EPOLL_MSAFD_H_

#include <windows.h>
#include "ntapi.h"

#ifndef SIO_BASE_HANDLE
# define SIO_BASE_HANDLE 0x48000022
#endif

#define AFD_NO_FAST_IO   0x00000001
#define AFD_OVERLAPPED   0x00000002
#define AFD_IMMEDIATE    0x00000004

#define AFD_POLL_RECEIVE_BIT            0
#define AFD_POLL_RECEIVE                (1 << AFD_POLL_RECEIVE_BIT)
#define AFD_POLL_RECEIVE_EXPEDITED_BIT  1
#define AFD_POLL_RECEIVE_EXPEDITED      (1 << AFD_POLL_RECEIVE_EXPEDITED_BIT)
#define AFD_POLL_SEND_BIT               2
#define AFD_POLL_SEND                   (1 << AFD_POLL_SEND_BIT)
#define AFD_POLL_DISCONNECT_BIT         3
#define AFD_POLL_DISCONNECT             (1 << AFD_POLL_DISCONNECT_BIT)
#define AFD_POLL_ABORT_BIT              4
#define AFD_POLL_ABORT                  (1 << AFD_POLL_ABORT_BIT)
#define AFD_POLL_LOCAL_CLOSE_BIT        5
#define AFD_POLL_LOCAL_CLOSE            (1 << AFD_POLL_LOCAL_CLOSE_BIT)
#define AFD_POLL_CONNECT_BIT            6
#define AFD_POLL_CONNECT                (1 << AFD_POLL_CONNECT_BIT)
#define AFD_POLL_ACCEPT_BIT             7
#define AFD_POLL_ACCEPT                 (1 << AFD_POLL_ACCEPT_BIT)
#define AFD_POLL_CONNECT_FAIL_BIT       8
#define AFD_POLL_CONNECT_FAIL           (1 << AFD_POLL_CONNECT_FAIL_BIT)
#define AFD_POLL_QOS_BIT                9
#define AFD_POLL_QOS                    (1 << AFD_POLL_QOS_BIT)
#define AFD_POLL_GROUP_QOS_BIT          10
#define AFD_POLL_GROUP_QOS              (1 << AFD_POLL_GROUP_QOS_BIT)

#define AFD_NUM_POLL_EVENTS             11
#define AFD_POLL_ALL                    ((1 << AFD_NUM_POLL_EVENTS) - 1)

#define FSCTL_AFD_BASE FILE_DEVICE_NETWORK

#define _AFD_CONTROL_CODE(operation, method) \
    ((FSCTL_AFD_BASE) << 12 | (operation << 2) | method)

#define AFD_POLL                        9

#define IOCTL_AFD_POLL \
    _AFD_CONTROL_CODE(AFD_POLL, METHOD_BUFFERED)

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

static const GUID AFD_PROVIDER_IDS[] = {
  {0xe70f1aa0, 0xab8b, 0x11cf,
      {0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}},
  {0xf9eab0c0, 0x26d4, 0x11d0,
      {0xbb, 0xbf, 0x00, 0xaa, 0x00, 0x6c, 0x34, 0xe4}},
  {0x9fc48064, 0x7298, 0x43e4,
      {0xb7, 0xbd, 0x18, 0x1f, 0x20, 0x89, 0x79, 0x2a}}
};

#endif  /* EPOLL_MSAFD_H_ */