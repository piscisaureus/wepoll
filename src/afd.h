#ifndef WEPOLL_AFD_H_
#define WEPOLL_AFD_H_

#include "internal.h"
#include "nt.h"
#include "util.h"
#include "win.h"

/* clang-format off */

#define AFD_NO_FAST_IO                  0x00000001
#define AFD_OVERLAPPED                  0x00000002
#define AFD_IMMEDIATE                   0x00000004

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

WEPOLL_INTERNAL int afd_poll(SOCKET driver_socket,
                             AFD_POLL_INFO* poll_info,
                             OVERLAPPED* overlapped);

WEPOLL_INTERNAL ssize_t afd_get_protocol(SOCKET socket,
                                         SOCKET* afd_socket_out,
                                         WSAPROTOCOL_INFOW* protocol_info);

/* clang-format off */

static const GUID AFD_PROVIDER_GUID_LIST[] = {
    /* MSAFD Tcpip [TCP+UDP+RAW / IP] */
    {0xe70f1aa0, 0xab8b, 0x11cf,
     {0x8c, 0xa3, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}},
    /* MSAFD Tcpip [TCP+UDP+RAW / IPv6] */
    {0xf9eab0c0, 0x26d4, 0x11d0,
     {0xbb, 0xbf, 0x00, 0xaa, 0x00, 0x6c, 0x34, 0xe4}},
    /* MSAFD RfComm [Bluetooth] */
    {0x9fc48064, 0x7298, 0x43e4,
     {0xb7, 0xbd, 0x18, 0x1f, 0x20, 0x89, 0x79, 0x2a}},
    /* MSAFD Irda [IrDA] */
    {0x3972523d, 0x2af1, 0x11d1,
     {0xb6, 0x55, 0x00, 0x80, 0x5f, 0x36, 0x42, 0xcc}}};

/* clang-format on */

#endif /* WEPOLL_AFD_H_ */
