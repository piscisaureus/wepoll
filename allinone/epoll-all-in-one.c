/*
 * Copyright Bert Belder. All rights reserved.
 *
 * The red-black tree implementation:
 * Copyright 2002 Niels Provos <provos@citi.umich.edu> All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define EPOLL_INTERNAL static
#define EPOLL_INTERNAL_EXTERN static
#ifndef EPOLL_EXTERN
#define EPOLL_EXTERN
#endif

#include <stdint.h>

enum EPOLL_EVENTS {
  EPOLLIN = 1 << 0,
  EPOLLPRI = 1 << 1,
  EPOLLOUT = 1 << 2,
  EPOLLERR = 1 << 3,
  EPOLLHUP = 1 << 4,
  EPOLLRDNORM = 1 << 6,
  EPOLLRDBAND = 1 << 7,
  EPOLLWRNORM = 1 << 8,
  EPOLLWRBAND = 1 << 9,
  EPOLLMSG = 1 << 10,
  EPOLLRDHUP = 1 << 13,
  EPOLLONESHOT = 1 << 31
};

#define EPOLLIN EPOLLIN
#define EPOLLPRI EPOLLPRI
#define EPOLLOUT EPOLLOUT
#define EPOLLERR EPOLLERR
#define EPOLLHUP EPOLLHUP
#define EPOLLRDNORM EPOLLRDNORM
#define EPOLLRDBAND EPOLLRDBAND
#define EPOLLWRNORM EPOLLWRNORM
#define EPOLLWRBAND EPOLLWRBAND
#define EPOLLMSG EPOLLMSG
#define EPOLLRDHUP EPOLLRDHUP
#define EPOLLONESHOT EPOLLONESHOT

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 2
#define EPOLL_CTL_DEL 3

typedef void* epoll_t;

typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  /* SOCKET */ uintptr_t sock;
  /* HANDLE */ void* hnd;
} epoll_data_t;

struct epoll_event {
  uint32_t events;   /* Epoll events */
  epoll_data_t data; /* User data variable */
};

#ifdef __cplusplus
extern "C" {
#endif

EPOLL_EXTERN epoll_t epoll_create(void);

EPOLL_EXTERN int epoll_close(epoll_t epoll_hnd);

EPOLL_EXTERN int epoll_ctl(epoll_t epoll_hnd,
                           int op,
                           /* SOCKET */ uintptr_t sock,
                           struct epoll_event* event);

EPOLL_EXTERN int epoll_wait(epoll_t epoll_hnd,
                            struct epoll_event* events,
                            int maxevents,
                            int timeout);

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifndef EPOLL_INTERNAL
#define EPOLL_INTERNAL
#define EPOLL_INTERNAL_EXTERN extern
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#ifndef _NTDEF_
typedef LONG NTSTATUS;
typedef NTSTATUS* PNTSTATUS;
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(status) (((NTSTATUS)(status)) >= 0)
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_WAIT_0
#define STATUS_WAIT_0 ((NTSTATUS) 0x00000000L)
#endif

#ifndef STATUS_WAIT_1
#define STATUS_WAIT_1 ((NTSTATUS) 0x00000001L)
#endif

#ifndef STATUS_WAIT_2
#define STATUS_WAIT_2 ((NTSTATUS) 0x00000002L)
#endif

#ifndef STATUS_WAIT_3
#define STATUS_WAIT_3 ((NTSTATUS) 0x00000003L)
#endif

#ifndef STATUS_WAIT_63
#define STATUS_WAIT_63 ((NTSTATUS) 0x0000003FL)
#endif

#ifndef STATUS_ABANDONED
#define STATUS_ABANDONED ((NTSTATUS) 0x00000080L)
#endif

#ifndef STATUS_ABANDONED_WAIT_0
#define STATUS_ABANDONED_WAIT_0 ((NTSTATUS) 0x00000080L)
#endif

#ifndef STATUS_ABANDONED_WAIT_63
#define STATUS_ABANDONED_WAIT_63 ((NTSTATUS) 0x000000BFL)
#endif

#ifndef STATUS_USER_APC
#define STATUS_USER_APC ((NTSTATUS) 0x000000C0L)
#endif

#ifndef STATUS_KERNEL_APC
#define STATUS_KERNEL_APC ((NTSTATUS) 0x00000100L)
#endif

#ifndef STATUS_ALERTED
#define STATUS_ALERTED ((NTSTATUS) 0x00000101L)
#endif

#ifndef STATUS_TIMEOUT
#define STATUS_TIMEOUT ((NTSTATUS) 0x00000102L)
#endif

#ifndef STATUS_PENDING
#define STATUS_PENDING ((NTSTATUS) 0x00000103L)
#endif

#ifndef STATUS_CANCELLED
#define STATUS_CANCELLED ((NTSTATUS) 0xC0000120L)
#endif

#ifndef STATUS_SEVERITY_SUCCESS
#define STATUS_SEVERITY_SUCCESS 0x0
#endif

#ifndef STATUS_SEVERITY_INFORMATIONAL
#define STATUS_SEVERITY_INFORMATIONAL 0x1
#endif

#ifndef STATUS_SEVERITY_WARNING
#define STATUS_SEVERITY_WARNING 0x2
#endif

#ifndef STATUS_SEVERITY_ERROR
#define STATUS_SEVERITY_ERROR 0x3
#endif

#ifndef FACILITY_NTWIN32
#define FACILITY_NTWIN32 0x7
#endif

/* This is not the NTSTATUS_FROM_WIN32 that the DDK provides, because the
 * DDK got it wrong!
 */
#ifdef NTSTATUS_FROM_WIN32
#undef NTSTATUS_FROM_WIN32
#endif
#define NTSTATUS_FROM_WIN32(error)                                      \
  ((NTSTATUS)(error) <= 0                                               \
       ? ((NTSTATUS)(error))                                            \
       : ((NTSTATUS)(((error) &0x0000FFFF) | (FACILITY_NTWIN32 << 16) | \
                     ERROR_SEVERITY_WARNING)))

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

EPOLL_INTERNAL int afd_poll(SOCKET driver_socket,
                            AFD_POLL_INFO* poll_info,
                            OVERLAPPED* overlapped);

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

#include <errno.h>

EPOLL_INTERNAL DWORD we_map_ntstatus_to_win_error(NTSTATUS ntstatus);
EPOLL_INTERNAL DWORD we_map_ntstatus_to_ws_error(NTSTATUS ntstatus);
EPOLL_INTERNAL errno_t we_map_win_error_to_errno(DWORD error);

EPOLL_INTERNAL void we_set_win_error(DWORD error);

#define _return_error_helper(error, value)         \
  do {                                             \
    we_set_win_error(error);                       \
    /* { printf("%d\n", error); DebugBreak(); } */ \
    return (value);                                \
  } while (0)

#define return_error(value, ...) _return_error_helper(__VA_ARGS__ + 0, value)

EPOLL_INTERNAL int nt_init(void);

typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef VOID(NTAPI* PIO_APC_ROUTINE)(PVOID ApcContext,
                                     PIO_STATUS_BLOCK IoStatusBlock,
                                     ULONG Reserved);

#define NTDLL_IMPORT_LIST(X)         \
  X(NTSTATUS,                        \
    NTAPI,                           \
    NtDeviceIoControlFile,           \
    (HANDLE FileHandle,              \
     HANDLE Event,                   \
     PIO_APC_ROUTINE ApcRoutine,     \
     PVOID ApcContext,               \
     PIO_STATUS_BLOCK IoStatusBlock, \
     ULONG IoControlCode,            \
     PVOID InputBuffer,              \
     ULONG InputBufferLength,        \
     PVOID OutputBuffer,             \
     ULONG OutputBufferLength))      \
  X(ULONG, WINAPI, RtlNtStatusToDosError, (NTSTATUS Status))

#define X(return_type, declarators, name, parameters) \
  EPOLL_INTERNAL_EXTERN return_type(declarators* name) parameters;
NTDLL_IMPORT_LIST(X)
#undef X

#define FILE_DEVICE_NETWORK 0x00000012
#define METHOD_BUFFERED 0
#define AFD_POLL 9

#define _AFD_CONTROL_CODE(operation, method) \
  ((FILE_DEVICE_NETWORK) << 12 | (operation << 2) | method)

#define IOCTL_AFD_POLL _AFD_CONTROL_CODE(AFD_POLL, METHOD_BUFFERED)

int afd_poll(SOCKET driver_socket,
             AFD_POLL_INFO* poll_info,
             OVERLAPPED* overlapped) {
  IO_STATUS_BLOCK iosb;
  IO_STATUS_BLOCK* iosb_ptr;
  HANDLE event = NULL;
  void* apc_context;
  NTSTATUS status;

  if (overlapped != NULL) {
    /* Overlapped operation. */
    iosb_ptr = (IO_STATUS_BLOCK*) &overlapped->Internal;
    event = overlapped->hEvent;

    /* Do not report iocp completion if hEvent is tagged. */
    if ((uintptr_t) event & 1) {
      event = (HANDLE)((uintptr_t) event & ~(uintptr_t) 1);
      apc_context = NULL;
    } else {
      apc_context = overlapped;
    }

  } else {
    /* Blocking operation. */
    iosb_ptr = &iosb;
    event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (event == NULL)
      return_error(-1);
    apc_context = NULL;
  }

  iosb_ptr->Status = STATUS_PENDING;
  status = NtDeviceIoControlFile((HANDLE) driver_socket,
                                 event,
                                 NULL,
                                 apc_context,
                                 iosb_ptr,
                                 IOCTL_AFD_POLL,
                                 poll_info,
                                 sizeof *poll_info,
                                 poll_info,
                                 sizeof *poll_info);

  if (overlapped == NULL) {
    /* If this is a blocking operation, wait for the event to become
     * signaled, and then grab the real status from the io status block.
     */
    if (status == STATUS_PENDING) {
      DWORD r = WaitForSingleObject(event, INFINITE);

      if (r == WAIT_FAILED) {
        DWORD error = GetLastError();
        CloseHandle(event);
        return_error(-1, error);
      }

      status = iosb_ptr->Status;
    }

    CloseHandle(event);
  }

  if (status == STATUS_SUCCESS)
    return 0;
  else if (status == STATUS_PENDING)
    return_error(-1, ERROR_IO_PENDING);
  else
    return_error(-1, we_map_ntstatus_to_win_error(status));
}

EPOLL_INTERNAL int init(void);

#include <stdbool.h>
#include <stdlib.h>

typedef struct queue_node queue_node_t;
typedef struct queue_node {
  queue_node_t* prev;
  queue_node_t* next;
} queue_node_t;
typedef struct queue {
  queue_node_t head;
} queue_t;

EPOLL_INTERNAL inline void queue_node_init(queue_node_t* node) {
  node->prev = node;
  node->next = node;
}

EPOLL_INTERNAL inline void queue_init(queue_t* queue) {
  queue_node_init(&queue->head);
}

EPOLL_INTERNAL inline bool queue_enqueued(const queue_node_t* node) {
  return node->prev != node;
}

EPOLL_INTERNAL inline bool queue_empty(const queue_t* queue) {
  return !queue_enqueued(&queue->head);
}

EPOLL_INTERNAL inline queue_node_t* queue_first(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.next : NULL;
}

EPOLL_INTERNAL inline queue_node_t* queue_last(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.prev : NULL;
}

EPOLL_INTERNAL inline void queue_prepend(queue_t* queue, queue_node_t* node) {
  node->next = queue->head.next;
  node->prev = &queue->head;
  node->next->prev = node;
  queue->head.next = node;
}

EPOLL_INTERNAL inline void queue_append(queue_t* queue, queue_node_t* node) {
  node->next = &queue->head;
  node->prev = queue->head.prev;
  node->prev->next = node;
  queue->head.prev = node;
}

EPOLL_INTERNAL inline void queue_remove(queue_node_t* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->prev = node;
  node->next = node;
}

#ifdef __clang__
#define RB_UNUSED __attribute__((__unused__))
#else
#define RB_UNUSED /* empty */
#endif

/* clang-format off */

/* Macros that define a red-black tree */
#define RB_HEAD(name, type)                                                   \
struct name {                                                                 \
  struct type *rbh_root; /* root of the tree */                               \
}

#define RB_INITIALIZER(root)                                                  \
  { NULL }

#define RB_INIT(root) do {                                                    \
  (root)->rbh_root = NULL;                                                    \
} while (0)

#define RB_BLACK  0
#define RB_RED    1
#define RB_ENTRY(type)                                                        \
struct {                                                                      \
  struct type *rbe_left;        /* left nodeent */                            \
  struct type *rbe_right;       /* right nodeent */                           \
  struct type *rbe_parent;      /* parent nodeent */                          \
  int rbe_color;                /* node color */                              \
}

#define RB_LEFT(elm, field)     (elm)->field.rbe_left
#define RB_RIGHT(elm, field)    (elm)->field.rbe_right
#define RB_PARENT(elm, field)   (elm)->field.rbe_parent
#define RB_COLOR(elm, field)    (elm)->field.rbe_color
#define RB_ROOT(head)           (head)->rbh_root
#define RB_EMPTY(head)          (RB_ROOT(head) == NULL)

#define RB_SET(elm, parent, field) do {                                       \
  RB_PARENT(elm, field) = parent;                                             \
  RB_LEFT(elm, field) = RB_RIGHT(elm, field) = NULL;                          \
  RB_COLOR(elm, field) = RB_RED;                                              \
} while (0)

#define RB_SET_BLACKRED(black, red, field) do {                               \
  RB_COLOR(black, field) = RB_BLACK;                                          \
  RB_COLOR(red, field) = RB_RED;                                              \
} while (0)

#define RB_ROTATE_LEFT(head, elm, tmp, field) do {                            \
  (tmp) = RB_RIGHT(elm, field);                                               \
  if ((RB_RIGHT(elm, field) = RB_LEFT(tmp, field)) != NULL) {                 \
    RB_PARENT(RB_LEFT(tmp, field), field) = (elm);                            \
  }                                                                           \
  if ((RB_PARENT(tmp, field) = RB_PARENT(elm, field)) != NULL) {              \
    if ((elm) == RB_LEFT(RB_PARENT(elm, field), field))                       \
      RB_LEFT(RB_PARENT(elm, field), field) = (tmp);                          \
    else                                                                      \
      RB_RIGHT(RB_PARENT(elm, field), field) = (tmp);                         \
  } else                                                                      \
    (head)->rbh_root = (tmp);                                                 \
  RB_LEFT(tmp, field) = (elm);                                                \
  RB_PARENT(elm, field) = (tmp);                                              \
} while (0)

#define RB_ROTATE_RIGHT(head, elm, tmp, field) do {                           \
  (tmp) = RB_LEFT(elm, field);                                                \
  if ((RB_LEFT(elm, field) = RB_RIGHT(tmp, field)) != NULL) {                 \
    RB_PARENT(RB_RIGHT(tmp, field), field) = (elm);                           \
  }                                                                           \
  if ((RB_PARENT(tmp, field) = RB_PARENT(elm, field)) != NULL) {              \
    if ((elm) == RB_LEFT(RB_PARENT(elm, field), field))                       \
      RB_LEFT(RB_PARENT(elm, field), field) = (tmp);                          \
    else                                                                      \
      RB_RIGHT(RB_PARENT(elm, field), field) = (tmp);                         \
  } else                                                                      \
    (head)->rbh_root = (tmp);                                                 \
  RB_RIGHT(tmp, field) = (elm);                                               \
  RB_PARENT(elm, field) = (tmp);                                              \
} while (0)

/* Generates prototypes and inline functions */
#define  RB_PROTOTYPE(name, type, field, cmp)                                 \
  RB_PROTOTYPE_INTERNAL(name, type, field, cmp,)
#define  RB_PROTOTYPE_STATIC(name, type, field, cmp)                          \
  RB_PROTOTYPE_INTERNAL(name, type, field, cmp, static RB_UNUSED)
#define RB_PROTOTYPE_INTERNAL(name, type, field, cmp, attr)                   \
attr void name##_RB_INSERT_COLOR(struct name *, struct type *);               \
attr void name##_RB_REMOVE_COLOR(struct name *, struct type *, struct type *);\
attr struct type *name##_RB_REMOVE(struct name *, struct type *);             \
attr struct type *name##_RB_INSERT(struct name *, struct type *);             \
attr struct type *name##_RB_FIND(struct name *, struct type *);               \
attr struct type *name##_RB_NFIND(struct name *, struct type *);              \
attr struct type *name##_RB_NEXT(struct type *);                              \
attr struct type *name##_RB_PREV(struct type *);                              \
attr struct type *name##_RB_MINMAX(struct name *, int);                       \
                                                                              \

/* Main rb operation.
* Moves node close to the key of elm to top
*/
#define  RB_GENERATE(name, type, field, cmp)                                  \
  RB_GENERATE_INTERNAL(name, type, field, cmp,)
#define  RB_GENERATE_STATIC(name, type, field, cmp)                           \
  RB_GENERATE_INTERNAL(name, type, field, cmp, static RB_UNUSED)
#define RB_GENERATE_INTERNAL(name, type, field, cmp, attr)                    \
attr void                                                                     \
name##_RB_INSERT_COLOR(struct name *head, struct type *elm)                   \
{                                                                             \
  struct type *parent, *gparent, *tmp;                                        \
  while ((parent = RB_PARENT(elm, field)) != NULL &&                          \
      RB_COLOR(parent, field) == RB_RED) {                                    \
    gparent = RB_PARENT(parent, field);                                       \
    if (parent == RB_LEFT(gparent, field)) {                                  \
      tmp = RB_RIGHT(gparent, field);                                         \
      if (tmp && RB_COLOR(tmp, field) == RB_RED) {                            \
        RB_COLOR(tmp, field) = RB_BLACK;                                      \
        RB_SET_BLACKRED(parent, gparent, field);                              \
        elm = gparent;                                                        \
        continue;                                                             \
      }                                                                       \
      if (RB_RIGHT(parent, field) == elm) {                                   \
        RB_ROTATE_LEFT(head, parent, tmp, field);                             \
        tmp = parent;                                                         \
        parent = elm;                                                         \
        elm = tmp;                                                            \
      }                                                                       \
      RB_SET_BLACKRED(parent, gparent, field);                                \
      RB_ROTATE_RIGHT(head, gparent, tmp, field);                             \
    } else {                                                                  \
      tmp = RB_LEFT(gparent, field);                                          \
      if (tmp && RB_COLOR(tmp, field) == RB_RED) {                            \
        RB_COLOR(tmp, field) = RB_BLACK;                                      \
        RB_SET_BLACKRED(parent, gparent, field);                              \
        elm = gparent;                                                        \
        continue;                                                             \
      }                                                                       \
      if (RB_LEFT(parent, field) == elm) {                                    \
        RB_ROTATE_RIGHT(head, parent, tmp, field);                            \
        tmp = parent;                                                         \
        parent = elm;                                                         \
        elm = tmp;                                                            \
      }                                                                       \
      RB_SET_BLACKRED(parent, gparent, field);                                \
      RB_ROTATE_LEFT(head, gparent, tmp, field);                              \
    }                                                                         \
  }                                                                           \
  RB_COLOR(head->rbh_root, field) = RB_BLACK;                                 \
}                                                                             \
                                                                              \
attr void                                                                     \
name##_RB_REMOVE_COLOR(struct name *head, struct type *parent,                \
    struct type *elm)                                                         \
{                                                                             \
  struct type *tmp;                                                           \
  while ((elm == NULL || RB_COLOR(elm, field) == RB_BLACK) &&                 \
      elm != RB_ROOT(head)) {                                                 \
    if (RB_LEFT(parent, field) == elm) {                                      \
      tmp = RB_RIGHT(parent, field);                                          \
      if (RB_COLOR(tmp, field) == RB_RED) {                                   \
        RB_SET_BLACKRED(tmp, parent, field);                                  \
        RB_ROTATE_LEFT(head, parent, tmp, field);                             \
        tmp = RB_RIGHT(parent, field);                                        \
      }                                                                       \
      if ((RB_LEFT(tmp, field) == NULL ||                                     \
          RB_COLOR(RB_LEFT(tmp, field), field) == RB_BLACK) &&                \
          (RB_RIGHT(tmp, field) == NULL ||                                    \
          RB_COLOR(RB_RIGHT(tmp, field), field) == RB_BLACK)) {               \
        RB_COLOR(tmp, field) = RB_RED;                                        \
        elm = parent;                                                         \
        parent = RB_PARENT(elm, field);                                       \
      } else {                                                                \
        if (RB_RIGHT(tmp, field) == NULL ||                                   \
            RB_COLOR(RB_RIGHT(tmp, field), field) == RB_BLACK) {              \
          struct type *oleft;                                                 \
          if ((oleft = RB_LEFT(tmp, field))                                   \
              != NULL)                                                        \
            RB_COLOR(oleft, field) = RB_BLACK;                                \
          RB_COLOR(tmp, field) = RB_RED;                                      \
          RB_ROTATE_RIGHT(head, tmp, oleft, field);                           \
          tmp = RB_RIGHT(parent, field);                                      \
        }                                                                     \
        RB_COLOR(tmp, field) = RB_COLOR(parent, field);                       \
        RB_COLOR(parent, field) = RB_BLACK;                                   \
        if (RB_RIGHT(tmp, field))                                             \
          RB_COLOR(RB_RIGHT(tmp, field), field) = RB_BLACK;                   \
        RB_ROTATE_LEFT(head, parent, tmp, field);                             \
        elm = RB_ROOT(head);                                                  \
        break;                                                                \
      }                                                                       \
    } else {                                                                  \
      tmp = RB_LEFT(parent, field);                                           \
      if (RB_COLOR(tmp, field) == RB_RED) {                                   \
        RB_SET_BLACKRED(tmp, parent, field);                                  \
        RB_ROTATE_RIGHT(head, parent, tmp, field);                            \
        tmp = RB_LEFT(parent, field);                                         \
      }                                                                       \
      if ((RB_LEFT(tmp, field) == NULL ||                                     \
          RB_COLOR(RB_LEFT(tmp, field), field) == RB_BLACK) &&                \
          (RB_RIGHT(tmp, field) == NULL ||                                    \
          RB_COLOR(RB_RIGHT(tmp, field), field) == RB_BLACK)) {               \
        RB_COLOR(tmp, field) = RB_RED;                                        \
        elm = parent;                                                         \
        parent = RB_PARENT(elm, field);                                       \
      } else {                                                                \
        if (RB_LEFT(tmp, field) == NULL ||                                    \
            RB_COLOR(RB_LEFT(tmp, field), field) == RB_BLACK) {               \
          struct type *oright;                                                \
          if ((oright = RB_RIGHT(tmp, field))                                 \
              != NULL)                                                        \
            RB_COLOR(oright, field) = RB_BLACK;                               \
          RB_COLOR(tmp, field) = RB_RED;                                      \
          RB_ROTATE_LEFT(head, tmp, oright, field);                           \
          tmp = RB_LEFT(parent, field);                                       \
        }                                                                     \
        RB_COLOR(tmp, field) = RB_COLOR(parent, field);                       \
        RB_COLOR(parent, field) = RB_BLACK;                                   \
        if (RB_LEFT(tmp, field))                                              \
          RB_COLOR(RB_LEFT(tmp, field), field) = RB_BLACK;                    \
        RB_ROTATE_RIGHT(head, parent, tmp, field);                            \
        elm = RB_ROOT(head);                                                  \
        break;                                                                \
      }                                                                       \
    }                                                                         \
  }                                                                           \
  if (elm)                                                                    \
    RB_COLOR(elm, field) = RB_BLACK;                                          \
}                                                                             \
                                                                              \
attr struct type *                                                            \
name##_RB_REMOVE(struct name *head, struct type *elm)                         \
{                                                                             \
  struct type *child, *parent, *old = elm;                                    \
  int color;                                                                  \
  if (RB_LEFT(elm, field) == NULL)                                            \
    child = RB_RIGHT(elm, field);                                             \
  else if (RB_RIGHT(elm, field) == NULL)                                      \
    child = RB_LEFT(elm, field);                                              \
  else {                                                                      \
    struct type *left;                                                        \
    elm = RB_RIGHT(elm, field);                                               \
    while ((left = RB_LEFT(elm, field)) != NULL)                              \
      elm = left;                                                             \
    child = RB_RIGHT(elm, field);                                             \
    parent = RB_PARENT(elm, field);                                           \
    color = RB_COLOR(elm, field);                                             \
    if (child)                                                                \
      RB_PARENT(child, field) = parent;                                       \
    if (parent) {                                                             \
      if (RB_LEFT(parent, field) == elm)                                      \
        RB_LEFT(parent, field) = child;                                       \
      else                                                                    \
        RB_RIGHT(parent, field) = child;                                      \
    } else                                                                    \
      RB_ROOT(head) = child;                                                  \
    if (RB_PARENT(elm, field) == old)                                         \
      parent = elm;                                                           \
    (elm)->field = (old)->field;                                              \
    if (RB_PARENT(old, field)) {                                              \
      if (RB_LEFT(RB_PARENT(old, field), field) == old)                       \
        RB_LEFT(RB_PARENT(old, field), field) = elm;                          \
      else                                                                    \
        RB_RIGHT(RB_PARENT(old, field), field) = elm;                         \
    } else                                                                    \
      RB_ROOT(head) = elm;                                                    \
    RB_PARENT(RB_LEFT(old, field), field) = elm;                              \
    if (RB_RIGHT(old, field))                                                 \
      RB_PARENT(RB_RIGHT(old, field), field) = elm;                           \
    if (parent) {                                                             \
      left = parent;                                                          \
    }                                                                         \
    goto color;                                                               \
  }                                                                           \
  parent = RB_PARENT(elm, field);                                             \
  color = RB_COLOR(elm, field);                                               \
  if (child)                                                                  \
    RB_PARENT(child, field) = parent;                                         \
  if (parent) {                                                               \
    if (RB_LEFT(parent, field) == elm)                                        \
      RB_LEFT(parent, field) = child;                                         \
    else                                                                      \
      RB_RIGHT(parent, field) = child;                                        \
  } else                                                                      \
    RB_ROOT(head) = child;                                                    \
color:                                                                        \
  if (color == RB_BLACK)                                                      \
    name##_RB_REMOVE_COLOR(head, parent, child);                              \
  return (old);                                                               \
}                                                                             \
                                                                              \
/* Inserts a node into the RB tree */                                         \
attr struct type *                                                            \
name##_RB_INSERT(struct name *head, struct type *elm)                         \
{                                                                             \
  struct type *tmp;                                                           \
  struct type *parent = NULL;                                                 \
  int comp = 0;                                                               \
  tmp = RB_ROOT(head);                                                        \
  while (tmp) {                                                               \
    parent = tmp;                                                             \
    comp = (cmp)(elm, parent);                                                \
    if (comp < 0)                                                             \
      tmp = RB_LEFT(tmp, field);                                              \
    else if (comp > 0)                                                        \
      tmp = RB_RIGHT(tmp, field);                                             \
    else                                                                      \
      return (tmp);                                                           \
  }                                                                           \
  RB_SET(elm, parent, field);                                                 \
  if (parent != NULL) {                                                       \
    if (comp < 0)                                                             \
      RB_LEFT(parent, field) = elm;                                           \
    else                                                                      \
      RB_RIGHT(parent, field) = elm;                                          \
  } else                                                                      \
    RB_ROOT(head) = elm;                                                      \
  name##_RB_INSERT_COLOR(head, elm);                                          \
  return (NULL);                                                              \
}                                                                             \
                                                                              \
/* Finds the node with the same key as elm */                                 \
attr struct type *                                                            \
name##_RB_FIND(struct name *head, struct type *elm)                           \
{                                                                             \
  struct type *tmp = RB_ROOT(head);                                           \
  int comp;                                                                   \
  while (tmp) {                                                               \
    comp = cmp(elm, tmp);                                                     \
    if (comp < 0)                                                             \
      tmp = RB_LEFT(tmp, field);                                              \
    else if (comp > 0)                                                        \
      tmp = RB_RIGHT(tmp, field);                                             \
    else                                                                      \
      return (tmp);                                                           \
  }                                                                           \
  return (NULL);                                                              \
}                                                                             \
                                                                              \
/* Finds the first node greater than or equal to the search key */            \
attr struct type *                                                            \
name##_RB_NFIND(struct name *head, struct type *elm)                          \
{                                                                             \
  struct type *tmp = RB_ROOT(head);                                           \
  struct type *res = NULL;                                                    \
  int comp;                                                                   \
  while (tmp) {                                                               \
    comp = cmp(elm, tmp);                                                     \
    if (comp < 0) {                                                           \
      res = tmp;                                                              \
      tmp = RB_LEFT(tmp, field);                                              \
    }                                                                         \
    else if (comp > 0)                                                        \
      tmp = RB_RIGHT(tmp, field);                                             \
    else                                                                      \
      return (tmp);                                                           \
  }                                                                           \
  return (res);                                                               \
}                                                                             \
                                                                              \
attr struct type *                                                            \
name##_RB_NEXT(struct type *elm)                                              \
{                                                                             \
  if (RB_RIGHT(elm, field)) {                                                 \
    elm = RB_RIGHT(elm, field);                                               \
    while (RB_LEFT(elm, field))                                               \
      elm = RB_LEFT(elm, field);                                              \
  } else {                                                                    \
    if (RB_PARENT(elm, field) &&                                              \
        (elm == RB_LEFT(RB_PARENT(elm, field), field)))                       \
      elm = RB_PARENT(elm, field);                                            \
    else {                                                                    \
      while (RB_PARENT(elm, field) &&                                         \
          (elm == RB_RIGHT(RB_PARENT(elm, field), field)))                    \
        elm = RB_PARENT(elm, field);                                          \
      elm = RB_PARENT(elm, field);                                            \
    }                                                                         \
  }                                                                           \
  return (elm);                                                               \
}                                                                             \
                                                                              \
attr struct type *                                                            \
name##_RB_PREV(struct type *elm)                                              \
{                                                                             \
  if (RB_LEFT(elm, field)) {                                                  \
    elm = RB_LEFT(elm, field);                                                \
    while (RB_RIGHT(elm, field))                                              \
      elm = RB_RIGHT(elm, field);                                             \
  } else {                                                                    \
    if (RB_PARENT(elm, field) &&                                              \
        (elm == RB_RIGHT(RB_PARENT(elm, field), field)))                      \
      elm = RB_PARENT(elm, field);                                            \
    else {                                                                    \
      while (RB_PARENT(elm, field) &&                                         \
          (elm == RB_LEFT(RB_PARENT(elm, field), field)))                     \
        elm = RB_PARENT(elm, field);                                          \
      elm = RB_PARENT(elm, field);                                            \
    }                                                                         \
  }                                                                           \
  return (elm);                                                               \
}                                                                             \
                                                                              \
attr struct type *                                                            \
name##_RB_MINMAX(struct name *head, int val)                                  \
{                                                                             \
  struct type *tmp = RB_ROOT(head);                                           \
  struct type *parent = NULL;                                                 \
  while (tmp) {                                                               \
    parent = tmp;                                                             \
    if (val < 0)                                                              \
      tmp = RB_LEFT(tmp, field);                                              \
    else                                                                      \
      tmp = RB_RIGHT(tmp, field);                                             \
  }                                                                           \
  return (parent);                                                            \
}

#define RB_NEGINF   -1
#define RB_INF      1

#define RB_INSERT(name, x, y)   name##_RB_INSERT(x, y)
#define RB_REMOVE(name, x, y)   name##_RB_REMOVE(x, y)
#define RB_FIND(name, x, y)     name##_RB_FIND(x, y)
#define RB_NFIND(name, x, y)    name##_RB_NFIND(x, y)
#define RB_NEXT(name, x, y)     name##_RB_NEXT(y)
#define RB_PREV(name, x, y)     name##_RB_PREV(y)
#define RB_MIN(name, x)         name##_RB_MINMAX(x, RB_NEGINF)
#define RB_MAX(name, x)         name##_RB_MINMAX(x, RB_INF)

#define RB_FOREACH(x, name, head)                                             \
  for ((x) = RB_MIN(name, head);                                              \
       (x) != NULL;                                                           \
       (x) = name##_RB_NEXT(x))

#define RB_FOREACH_FROM(x, name, y)                                           \
  for ((x) = (y);                                                             \
      ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);                \
       (x) = (y))

#define RB_FOREACH_SAFE(x, name, head, y)                                     \
  for ((x) = RB_MIN(name, head);                                              \
      ((x) != NULL) && ((y) = name##_RB_NEXT(x), (x) != NULL);                \
       (x) = (y))

#define RB_FOREACH_REVERSE(x, name, head)                                     \
  for ((x) = RB_MAX(name, head);                                              \
       (x) != NULL;                                                           \
       (x) = name##_RB_PREV(x))

#define RB_FOREACH_REVERSE_FROM(x, name, y)                                   \
  for ((x) = (y);                                                             \
      ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);                \
       (x) = (y))

#define RB_FOREACH_REVERSE_SAFE(x, name, head, y)                             \
  for ((x) = RB_MAX(name, head);                                              \
      ((x) != NULL) && ((y) = name##_RB_PREV(x), (x) != NULL);                \
       (x) = (y))

/* clang-format on */

typedef RB_HEAD(tree, tree_node) tree_t;

typedef struct tree_node {
  RB_ENTRY(tree_node) node;
  uintptr_t key;
} tree_node_t;

EPOLL_INTERNAL void tree_init(tree_t* tree);
EPOLL_INTERNAL void tree_node_init(tree_node_t* node);

EPOLL_INTERNAL int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key);
EPOLL_INTERNAL int tree_del(tree_t* tree, tree_node_t* node);

EPOLL_INTERNAL tree_node_t* tree_find(tree_t* tree, uintptr_t key);
EPOLL_INTERNAL tree_node_t* tree_root(tree_t* tree);

#include <stddef.h>

#ifndef _SSIZE_T_DEFINED
#define SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

#define container_of(ptr, type, member) \
  ((type*) ((char*) (ptr) -offsetof(type, member)))

#define unused(v) ((void) (v))

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;

typedef struct ep_sock {
  tree_node_t tree_node;
  queue_node_t queue_node;
} ep_sock_t;

EPOLL_INTERNAL ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket);
EPOLL_INTERNAL void ep_sock_delete(ep_port_t* port_info, ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_sock_force_delete(ep_port_t* port_info,
                                         ep_sock_t* sock_info);

EPOLL_INTERNAL ep_sock_t* ep_sock_find(tree_t* tree, SOCKET socket);

EPOLL_INTERNAL int ep_sock_set_event(ep_port_t* port_info,
                                     ep_sock_t* sock_info,
                                     const struct epoll_event* ev);

EPOLL_INTERNAL int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info);
EPOLL_INTERNAL int ep_sock_feed_event(ep_port_t* port_info,
                                      poll_req_t* poll_req,
                                      struct epoll_event* ev);

EPOLL_INTERNAL void ep_sock_register_poll_req(ep_port_t* port_info,
                                              ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_sock_unregister_poll_req(ep_port_t* port_info,
                                                ep_sock_t* sock_info);

typedef struct ep_port ep_port_t;
typedef struct poll_group_allocator poll_group_allocator_t;
typedef struct poll_group poll_group_t;

EPOLL_INTERNAL poll_group_allocator_t* poll_group_allocator_new(
    ep_port_t* port_info, const WSAPROTOCOL_INFOW* protocol_info);
EPOLL_INTERNAL void poll_group_allocator_delete(poll_group_allocator_t* pga);

EPOLL_INTERNAL poll_group_t* poll_group_acquire(poll_group_allocator_t* pga);
EPOLL_INTERNAL void poll_group_release(poll_group_t* ds);

EPOLL_INTERNAL SOCKET poll_group_get_socket(poll_group_t* poll_group);

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;

typedef struct ep_port {
  HANDLE iocp;
  poll_group_allocator_t*
      poll_group_allocators[array_count(AFD_PROVIDER_GUID_LIST)];
  tree_t sock_tree;
  queue_t update_queue;
  size_t poll_req_count;
} ep_port_t;

EPOLL_INTERNAL ep_port_t* ep_port_new(HANDLE iocp);
EPOLL_INTERNAL int ep_port_delete(ep_port_t* port_info);

EPOLL_INTERNAL poll_group_t* ep_port_acquire_poll_group(ep_port_t* port_info,
                                                        SOCKET socket);
EPOLL_INTERNAL void ep_port_release_poll_group(poll_group_t* poll_group);

EPOLL_INTERNAL int ep_port_add_socket(ep_port_t* port_info,
                                      tree_node_t* tree_node,
                                      SOCKET socket);
EPOLL_INTERNAL int ep_port_del_socket(ep_port_t* port_info,
                                      tree_node_t* tree_node);

EPOLL_INTERNAL void ep_port_add_req(ep_port_t* port_info);
EPOLL_INTERNAL void ep_port_del_req(ep_port_t* port_info);

EPOLL_INTERNAL void ep_port_request_socket_update(ep_port_t* port_info,
                                                  ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_port_clear_socket_update(ep_port_t* port_info,
                                                ep_sock_t* sock_info);
EPOLL_INTERNAL bool ep_port_is_socket_update_pending(ep_port_t* port_info,
                                                     ep_sock_t* sock_info);

epoll_t epoll_create(void) {
  ep_port_t* port_info;
  HANDLE iocp;

  if (init() < 0)
    return NULL;

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE)
    return_error(NULL);

  port_info = ep_port_new(iocp);
  if (port_info == NULL) {
    CloseHandle(iocp);
    return NULL;
  }

  return (epoll_t) port_info;
}

int epoll_close(epoll_t port_handle) {
  ep_port_t* port_info;

  if (init() < 0)
    return -1;

  port_info = (ep_port_t*) port_handle;

  return ep_port_delete(port_info);
}
#include <assert.h>
#include <malloc.h>

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;
typedef struct poll_req poll_req_t;

EPOLL_INTERNAL poll_req_t* poll_req_new(ep_port_t* port_info,
                                        ep_sock_t* sock_info);

EPOLL_INTERNAL void poll_req_delete(ep_port_t* port_info,
                                    ep_sock_t* sock_info,
                                    poll_req_t* poll_req);

EPOLL_INTERNAL poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped);

EPOLL_INTERNAL ep_sock_t* poll_req_get_sock_data(const poll_req_t* poll_req);

EPOLL_INTERNAL int poll_req_submit(poll_req_t* poll_req,
                                   uint32_t epoll_events,
                                   SOCKET socket,
                                   SOCKET driver_socket);

EPOLL_INTERNAL int poll_req_cancel(poll_req_t* poll_req, SOCKET group_socket);
EPOLL_INTERNAL void poll_req_complete(const poll_req_t* poll_req,
                                      uint32_t* epoll_events_out,
                                      bool* socket_closed_out);

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

#define _EP_EVENT_MASK 0xffff

#define _EP_SOCK_DELETED 0x2

typedef struct _ep_sock_private {
  ep_sock_t pub;
  SOCKET afd_socket;
  poll_group_t* poll_group;
  epoll_data_t user_data;
  poll_req_t* latest_poll_req;
  uint32_t user_events;
  uint32_t latest_poll_req_events;
  uint32_t poll_req_count;
  uint32_t flags;
  bool poll_req_active;
} _ep_sock_private_t;

static inline _ep_sock_private_t* _ep_sock_private(ep_sock_t* sock_info) {
  return container_of(sock_info, _ep_sock_private_t, pub);
}

static inline bool _ep_sock_is_deleted(_ep_sock_private_t* sock_private) {
  return sock_private->flags & _EP_SOCK_DELETED;
}

static inline _ep_sock_private_t* _ep_sock_alloc(void) {
  _ep_sock_private_t* sock_private = malloc(sizeof *sock_private);
  if (sock_private == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return sock_private;
}

static inline void _ep_sock_free(_ep_sock_private_t* sock_private) {
  free(sock_private);
}

static int _get_related_sockets(ep_port_t* port_info,
                                SOCKET socket,
                                SOCKET* afd_socket_out,
                                poll_group_t** poll_group_out) {
  SOCKET afd_socket;
  poll_group_t* poll_group;
  DWORD bytes;

  /* Try to obtain a base handle for the socket, so we can bypass LSPs
   * that get in the way if we want to talk to the kernel directly. If
   * it fails we try if we work with the original socket. Note that on
   * windows XP/2k3 this will always fail since they don't support the
   * SIO_BASE_HANDLE ioctl.
   */
  afd_socket = socket;
  WSAIoctl(socket,
           SIO_BASE_HANDLE,
           NULL,
           0,
           &afd_socket,
           sizeof afd_socket,
           &bytes,
           NULL,
           NULL);

  poll_group = ep_port_acquire_poll_group(port_info, afd_socket);
  if (poll_group == NULL)
    return -1;

  *afd_socket_out = afd_socket;
  *poll_group_out = poll_group;

  return 0;
}

static int _ep_sock_set_socket(ep_port_t* port_info,
                               _ep_sock_private_t* sock_private,
                               SOCKET socket) {
  if (socket == 0 || socket == INVALID_SOCKET)
    return_error(-1, ERROR_INVALID_HANDLE);

  assert(sock_private->afd_socket == 0);

  if (_get_related_sockets(port_info,
                           socket,
                           &sock_private->afd_socket,
                           &sock_private->poll_group) < 0)
    return -1;

  if (ep_port_add_socket(port_info, &sock_private->pub.tree_node, socket) < 0)
    return -1;

  return 0;
}

ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket) {
  _ep_sock_private_t* sock_private = _ep_sock_alloc();
  if (sock_private == NULL)
    return NULL;

  unused(port_info);

  memset(sock_private, 0, sizeof *sock_private);
  tree_node_init(&sock_private->pub.tree_node);
  queue_node_init(&sock_private->pub.queue_node);

  if (_ep_sock_set_socket(port_info, sock_private, socket) < 0) {
    _ep_sock_free(sock_private);
    return NULL;
  }

  return &sock_private->pub;
}

void _ep_sock_maybe_free(_ep_sock_private_t* sock_private) {
  /* The socket may still have pending overlapped requests that have yet to be
   * reported by the completion port. If that's the case the memory can't be
   * released yet. It'll be released later as ep_sock_unregister_poll_req()
   * calls this function.
   */
  if (_ep_sock_is_deleted(sock_private) && sock_private->poll_req_count == 0)
    _ep_sock_free(sock_private);
}

void ep_sock_delete(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  assert(!_ep_sock_is_deleted(sock_private));
  sock_private->flags |= _EP_SOCK_DELETED;

  ep_port_del_socket(port_info, &sock_info->tree_node);
  ep_port_clear_socket_update(port_info, sock_info);
  ep_port_release_poll_group(sock_private->poll_group);
  sock_private->poll_group = NULL;

  _ep_sock_maybe_free(sock_private);
}

void ep_sock_force_delete(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);
  if (sock_private->latest_poll_req != NULL)
    poll_req_delete(port_info, sock_info, sock_private->latest_poll_req);
  assert(sock_private->poll_req_count == 0);
  ep_sock_delete(port_info, sock_info);
}

ep_sock_t* ep_sock_find(tree_t* tree, SOCKET socket) {
  tree_node_t* tree_node = tree_find(tree, socket);
  if (tree_node == NULL)
    return NULL;

  return container_of(tree_node, ep_sock_t, tree_node);
}

void ep_sock_register_poll_req(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  assert(!_ep_sock_is_deleted(sock_private));

  ep_port_add_req(port_info);
  sock_private->poll_req_count++;
  assert(sock_private->poll_req_count == 1);
}

void ep_sock_unregister_poll_req(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  ep_port_del_req(port_info);
  sock_private->poll_req_count--;
  assert(sock_private->poll_req_count == 0);

  _ep_sock_maybe_free(sock_private);
}

int ep_sock_set_event(ep_port_t* port_info,
                      ep_sock_t* sock_info,
                      const struct epoll_event* ev) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  /* EPOLLERR and EPOLLHUP are always reported, even when no sollicited. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_private->user_events = events;
  sock_private->user_data = ev->data;

  if ((events & _EP_EVENT_MASK & ~(sock_private->latest_poll_req_events)) != 0)
    ep_port_request_socket_update(port_info, sock_info);

  return 0;
}

static inline bool _is_latest_poll_req(_ep_sock_private_t* sock_private,
                                       poll_req_t* poll_req) {
  assert(sock_private->latest_poll_req == poll_req ||
         sock_private->latest_poll_req == NULL);
  return poll_req == sock_private->latest_poll_req;
}

static inline void _clear_latest_poll_req(_ep_sock_private_t* sock_private) {
  sock_private->latest_poll_req = NULL;
  sock_private->latest_poll_req_events = 0;
  sock_private->poll_req_active = false;
}

static inline void _set_latest_poll_req(_ep_sock_private_t* sock_private,
                                        poll_req_t* poll_req,
                                        uint32_t epoll_events) {
  sock_private->latest_poll_req = poll_req;
  sock_private->latest_poll_req_events = epoll_events;
  sock_private->poll_req_active = true;
}

int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);
  bool broken = false;
  SOCKET driver_socket;

  assert(ep_port_is_socket_update_pending(port_info, sock_info));

  driver_socket = poll_group_get_socket(sock_private->poll_group);

  /* Check if there are events registered that are not yet submitted. In
   * that case we need to submit another req.
   */
  if ((sock_private->user_events & _EP_EVENT_MASK &
       ~sock_private->latest_poll_req_events) == 0) {
    /* All the events the user is interested in are already being monitored
     * by the latest poll request. It might spuriously complete because of an
     * event that we're no longer interested in; if that happens we just
     * submit another poll request with the right event mask.
     */
    assert(sock_private->latest_poll_req != NULL);

  } else if (sock_private->latest_poll_req != NULL) {
    /* A poll request is already pending. Cancel the old one first; when it
     * completes, we'll submit the new one. */
    if (sock_private->poll_req_active) {
      poll_req_cancel(sock_private->latest_poll_req, driver_socket);
      sock_private->poll_req_active = false;
    }

  } else {
    poll_req_t* poll_req = poll_req_new(port_info, &sock_private->pub);
    if (poll_req == NULL)
      return -1;

    if (poll_req_submit(poll_req,
                        sock_private->user_events,
                        sock_private->afd_socket,
                        driver_socket) < 0) {
      poll_req_delete(port_info, &sock_private->pub, poll_req);

      if (GetLastError() == ERROR_INVALID_HANDLE)
        /* The socket is broken. It will be dropped from the epoll set. */
        broken = true;
      else
        /* Another error occurred, which is propagated to the caller. */
        return -1;
    }

    if (!broken)
      _set_latest_poll_req(sock_private, poll_req, sock_private->user_events);
  }

  ep_port_clear_socket_update(port_info, sock_info);

  /* If we saw an ERROR_INVALID_HANDLE error, drop the socket. */
  if (broken)
    ep_sock_delete(port_info, sock_info);

  return 0;
}

int ep_sock_feed_event(ep_port_t* port_info,
                       poll_req_t* poll_req,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = poll_req_get_sock_data(poll_req);
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  uint32_t epoll_events;
  bool drop_socket;
  int ev_count = 0;

  if (_ep_sock_is_deleted(sock_private) ||
      !_is_latest_poll_req(sock_private, poll_req)) {
    /* Ignore completion for overlapped poll operation if it isn't
     * the the most recently posted one, or if the socket has been
     * deleted. */
    poll_req_delete(port_info, sock_info, poll_req);
    return 0;
  }

  _clear_latest_poll_req(sock_private);

  poll_req_complete(poll_req, &epoll_events, &drop_socket);

  /* Filter events that the user didn't ask for. */
  epoll_events &= sock_private->user_events;

  /* Clear the event mask if EPOLLONESHOT is set and there are any events
   * to report. */
  if (epoll_events != 0 && (sock_private->user_events & EPOLLONESHOT))
    sock_private->user_events = EPOLLERR | EPOLLHUP;

  /* Fill the ev structure if there are any events to report. */
  if (epoll_events != 0) {
    ev->data = sock_private->user_data;
    ev->events = epoll_events;
    ev_count = 1;
  }

  poll_req_delete(port_info, sock_info, poll_req);

  if (drop_socket)
    /* Drop the socket from the epoll set. */
    ep_sock_delete(port_info, sock_info);
  else
    /* Put the socket back onto the attention list so a new poll request will
     * be submitted. */
    ep_port_request_socket_update(port_info, sock_info);

  return ev_count;
}

#define _EP_COMPLETION_LIST_LENGTH 64

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;
typedef struct ep_sock ep_sock_t;

static int _ep_ctl_add(ep_port_t* port_info,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_info, socket);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0) {
    ep_sock_delete(port_info, sock_info);
    return -1;
  }

  return 0;
}

static int _ep_ctl_mod(ep_port_t* port_info,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_find(&port_info->sock_tree, socket);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0)
    return -1;

  return 0;
}

static int _ep_ctl_del(ep_port_t* port_info, uintptr_t socket) {
  ep_sock_t* sock_info = ep_sock_find(&port_info->sock_tree, socket);
  if (sock_info == NULL)
    return -1;

  ep_sock_delete(port_info, sock_info);

  return 0;
}

int epoll_ctl(epoll_t port_handle,
              int op,
              uintptr_t socket,
              struct epoll_event* ev) {
  ep_port_t* port_info = (ep_port_t*) port_handle;

  if (init() < 0)
    return -1;

  switch (op) {
    case EPOLL_CTL_ADD:
      return _ep_ctl_add(port_info, socket, ev);
    case EPOLL_CTL_MOD:
      return _ep_ctl_mod(port_info, socket, ev);
    case EPOLL_CTL_DEL:
      return _ep_ctl_del(port_info, socket);
  }

  return_error(-1, ERROR_INVALID_PARAMETER);
}

static int _ep_port_update_events(ep_port_t* port_info) {
  queue_t* update_queue = &port_info->update_queue;

  /* Walk the queue, submitting new poll requests for every socket that needs
   * it. */
  while (!queue_empty(update_queue)) {
    queue_node_t* queue_node = queue_first(update_queue);
    ep_sock_t* sock_info = container_of(queue_node, ep_sock_t, queue_node);

    if (ep_sock_update(port_info, sock_info) < 0)
      return -1;

    /* ep_sock_update() removes the socket from the update list if
     * successfull. */
  }

  return 0;
}

static size_t _ep_port_feed_events(ep_port_t* port_info,
                                   OVERLAPPED_ENTRY* completion_list,
                                   size_t completion_count,
                                   struct epoll_event* event_list,
                                   size_t max_event_count) {
  if (completion_count > max_event_count)
    abort();

  size_t event_count = 0;

  for (size_t i = 0; i < completion_count; i++) {
    OVERLAPPED* overlapped = completion_list[i].lpOverlapped;
    poll_req_t* poll_req = overlapped_to_poll_req(overlapped);
    struct epoll_event* ev = &event_list[event_count];

    event_count += ep_sock_feed_event(port_info, poll_req, ev);
  }

  return event_count;
}

int epoll_wait(epoll_t port_handle,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  ep_port_t* port_info;
  ULONGLONG due = 0;
  DWORD gqcs_timeout;

  if (init() < 0)
    return -1;

  port_info = (ep_port_t*) port_handle;

  /* Compute the timeout for GetQueuedCompletionStatus, and the wait end
   * time, if the user specified a timeout other than zero or infinite.
   */
  if (timeout > 0) {
    due = GetTickCount64() + timeout;
    gqcs_timeout = (DWORD) timeout;
  } else if (timeout == 0) {
    gqcs_timeout = 0;
  } else {
    gqcs_timeout = INFINITE;
  }

  /* Compute how much overlapped entries can be dequeued at most. */
  if ((size_t) maxevents > _EP_COMPLETION_LIST_LENGTH)
    maxevents = _EP_COMPLETION_LIST_LENGTH;

  /* Dequeue completion packets until either at least one interesting event
   * has been discovered, or the timeout is reached.
   */
  do {
    OVERLAPPED_ENTRY completion_list[_EP_COMPLETION_LIST_LENGTH];
    ULONG completion_count;
    ssize_t event_count;

    if (_ep_port_update_events(port_info) < 0)
      return -1;

    BOOL r = GetQueuedCompletionStatusEx(port_info->iocp,
                                         completion_list,
                                         maxevents,
                                         &completion_count,
                                         gqcs_timeout,
                                         FALSE);
    if (!r) {
      if (GetLastError() == WAIT_TIMEOUT)
        return 0;
      else
        return_error(-1);
    }

    event_count = _ep_port_feed_events(
        port_info, completion_list, completion_count, events, maxevents);
    if (event_count > 0)
      return (int) event_count;

    /* Events were dequeued, but none were relevant. Recompute timeout. */
    if (timeout > 0) {
      ULONGLONG now = GetTickCount64();
      gqcs_timeout = (now < due) ? (DWORD)(due - now) : 0;
    }
  } while (gqcs_timeout > 0);

  return 0;
}

static ep_port_t* _ep_port_alloc(void) {
  ep_port_t* port_info = malloc(sizeof *port_info);
  if (port_info == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  return port_info;
}

static void _ep_port_free(ep_port_t* port) {
  assert(port != NULL);
  free(port);
}

ep_port_t* ep_port_new(HANDLE iocp) {
  ep_port_t* port_info;

  port_info = _ep_port_alloc();
  if (port_info == NULL)
    return NULL;

  memset(port_info, 0, sizeof *port_info);

  port_info->iocp = iocp;
  queue_init(&port_info->update_queue);
  tree_init(&port_info->sock_tree);

  return port_info;
}

int ep_port_delete(ep_port_t* port_info) {
  tree_node_t* tree_node;

  if (!CloseHandle(port_info->iocp))
    return_error(-1);
  port_info->iocp = NULL;

  while ((tree_node = tree_root(&port_info->sock_tree)) != NULL) {
    ep_sock_t* sock_info = container_of(tree_node, ep_sock_t, tree_node);
    ep_sock_force_delete(port_info, sock_info);
  }

  for (size_t i = 0; i < array_count(port_info->poll_group_allocators); i++) {
    poll_group_allocator_t* pga = port_info->poll_group_allocators[i];
    if (pga != NULL)
      poll_group_allocator_delete(pga);
  }

  _ep_port_free(port_info);

  return 0;
}

int ep_port_add_socket(ep_port_t* port_info,
                       tree_node_t* tree_node,
                       SOCKET socket) {
  return tree_add(&port_info->sock_tree, tree_node, socket);
}

int ep_port_del_socket(ep_port_t* port_info, tree_node_t* tree_node) {
  return tree_del(&port_info->sock_tree, tree_node);
}

void ep_port_add_req(ep_port_t* port_info) {
  port_info->poll_req_count++;
}

void ep_port_del_req(ep_port_t* port_info) {
  port_info->poll_req_count--;
}

poll_group_allocator_t* _get_poll_group_allocator(
    ep_port_t* port_info,
    size_t index,
    const WSAPROTOCOL_INFOW* protocol_info) {
  poll_group_allocator_t** pga = &port_info->poll_group_allocators[index];

  if (*pga == NULL)
    *pga = poll_group_allocator_new(port_info, protocol_info);

  return *pga;
}

poll_group_t* ep_port_acquire_poll_group(ep_port_t* port_info, SOCKET socket) {
  ssize_t index;
  size_t i;
  WSAPROTOCOL_INFOW protocol_info;
  int len;
  poll_group_allocator_t* pga;

  /* Obtain protocol information about the socket. */
  len = sizeof protocol_info;
  if (getsockopt(socket,
                 SOL_SOCKET,
                 SO_PROTOCOL_INFOW,
                 (char*) &protocol_info,
                 &len) != 0)
    return_error(NULL);

  index = -1;
  for (i = 0; i < array_count(AFD_PROVIDER_GUID_LIST); i++) {
    if (memcmp((void*) &protocol_info.ProviderId,
               (void*) &AFD_PROVIDER_GUID_LIST[i],
               sizeof protocol_info.ProviderId) == 0) {
      index = i;
      break;
    }
  }

  /* Check if the protocol uses an msafd socket. */
  if (index < 0)
    return_error(NULL, ERROR_NOT_SUPPORTED);

  pga = _get_poll_group_allocator(port_info, index, &protocol_info);

  return poll_group_acquire(pga);
}

void ep_port_release_poll_group(poll_group_t* poll_group) {
  poll_group_release(poll_group);
}

bool ep_port_is_socket_update_pending(ep_port_t* port_info,
                                      ep_sock_t* sock_info) {
  unused(port_info);
  return queue_enqueued(&sock_info->queue_node);
}

void ep_port_request_socket_update(ep_port_t* port_info,
                                   ep_sock_t* sock_info) {
  if (ep_port_is_socket_update_pending(port_info, sock_info))
    return;
  queue_append(&port_info->update_queue, &sock_info->queue_node);
  assert(ep_port_is_socket_update_pending(port_info, sock_info));
}

void ep_port_clear_socket_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  if (!ep_port_is_socket_update_pending(port_info, sock_info))
    return;
  queue_remove(&sock_info->queue_node);
}

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

#pragma warning(push)
#pragma warning(disable : 4127) /* "conditional expression is constant" */

DWORD we_map_ntstatus_to_win_error(NTSTATUS status) {
  if (NT_SUCCESS(status))
    return ERROR_SUCCESS;
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (status == (NTSTATUS) ntstatus && win_error != -1)       \
    return (DWORD) win_error;
  WE_ERROR_MAP(X)
#undef X
  return RtlNtStatusToDosError(status);
}

DWORD we_map_ntstatus_to_ws_error(NTSTATUS status) {
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (status == (NTSTATUS) ntstatus && ws_error != -1)        \
    return (DWORD) ws_error;
  WE_ERROR_MAP(X)
#undef X
  return we_map_ntstatus_to_win_error(status);
}

errno_t we_map_win_error_to_errno(DWORD error) {
#define X(ntstatus, win_error, win_errno, ws_error, ws_errno) \
  if (error == (DWORD) win_error && win_errno != -1)          \
    return win_errno;                                         \
  if (error == (DWORD) ws_error && ws_errno != -1)            \
    return ws_errno;
  WE_ERROR_MAP(X)
#undef X
  return EINVAL;
}

#pragma warning(pop)

void we_set_win_error(DWORD error) {
  if (error == 0)
    error = GetLastError();
  else
    SetLastError(error);
  errno = we_map_win_error_to_errno(error);
}

static bool _initialized = false;

static int _init_winsock(void) {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return_error(-1);

  return 0;
}

static int _init_once(void) {
  if (_init_winsock() < 0 || nt_init() < 0)
    return -1;

  _initialized = true;
  return 0;
}

int init(void) {
  if (_initialized)
    return 0;

  return _init_once();
}

#define X(return_type, declarators, name, parameters) \
  EPOLL_INTERNAL return_type(declarators* name) parameters = NULL;
NTDLL_IMPORT_LIST(X)
#undef X

int nt_init(void) {
  HMODULE ntdll;

  ntdll = GetModuleHandleW(L"ntdll.dll");
  if (ntdll == NULL)
    return -1;

#define X(return_type, declarators, name, parameters)                         \
  name = (return_type(declarators*) parameters) GetProcAddress(ntdll, #name); \
  if (name == NULL)                                                           \
    return -1;
  NTDLL_IMPORT_LIST(X)
#undef X

  return 0;
}

static const size_t _DS_MAX_USERS = 32;

typedef struct poll_group_allocator {
  ep_port_t* port_info;
  queue_t poll_group_queue;
  WSAPROTOCOL_INFOW protocol_info;
} poll_group_allocator_t;

typedef struct poll_group {
  poll_group_allocator_t* allocator;
  queue_node_t queue_node;
  SOCKET socket;
  size_t user_count;
} poll_group_t;

static int _poll_group_create_socket(poll_group_t* poll_group,
                                     WSAPROTOCOL_INFOW* protocol_info,
                                     HANDLE iocp) {
  SOCKET socket;

  socket = WSASocketW(protocol_info->iAddressFamily,
                      protocol_info->iSocketType,
                      protocol_info->iProtocol,
                      protocol_info,
                      0,
                      WSA_FLAG_OVERLAPPED);
  if (socket == INVALID_SOCKET)
    return_error(-1);

  if (!SetHandleInformation((HANDLE) socket, HANDLE_FLAG_INHERIT, 0))
    goto error;

  if (CreateIoCompletionPort((HANDLE) socket, iocp, 0, 0) == NULL)
    goto error;

  poll_group->socket = socket;
  return 0;

error:;
  DWORD error = GetLastError();
  closesocket(socket);
  return_error(-1, error);
}

static poll_group_t* _poll_group_new(poll_group_allocator_t* pga) {
  poll_group_t* poll_group = malloc(sizeof *poll_group);
  if (poll_group == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  memset(poll_group, 0, sizeof *poll_group);

  queue_node_init(&poll_group->queue_node);
  poll_group->allocator = pga;

  if (_poll_group_create_socket(
          poll_group, &pga->protocol_info, pga->port_info->iocp) < 0) {
    free(poll_group);
    return NULL;
  }

  queue_append(&pga->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

static void _poll_group_delete(poll_group_t* poll_group) {
  assert(poll_group->user_count == 0);
  closesocket(poll_group->socket);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

SOCKET poll_group_get_socket(poll_group_t* poll_group) {
  return poll_group->socket;
}

poll_group_allocator_t* poll_group_allocator_new(
    ep_port_t* port_info, const WSAPROTOCOL_INFOW* protocol_info) {
  poll_group_allocator_t* pga = malloc(sizeof *pga);
  if (pga == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  queue_init(&pga->poll_group_queue);
  pga->port_info = port_info;
  pga->protocol_info = *protocol_info;

  return pga;
}

void poll_group_allocator_delete(poll_group_allocator_t* pga) {
  queue_t* poll_group_queue = &pga->poll_group_queue;

  while (!queue_empty(poll_group_queue)) {
    queue_node_t* queue_node = queue_first(poll_group_queue);
    poll_group_t* poll_group =
        container_of(queue_node, poll_group_t, queue_node);
    _poll_group_delete(poll_group);
  }

  free(pga);
}

poll_group_t* poll_group_acquire(poll_group_allocator_t* pga) {
  queue_t* queue = &pga->poll_group_queue;
  poll_group_t* poll_group =
      !queue_empty(queue)
          ? container_of(queue_last(queue), poll_group_t, queue_node)
          : NULL;

  if (poll_group == NULL || poll_group->user_count >= _DS_MAX_USERS)
    poll_group = _poll_group_new(pga);
  if (poll_group == NULL)
    return NULL;

  if (++poll_group->user_count == _DS_MAX_USERS) {
    /* Move to the front of the queue. */
    queue_remove(&poll_group->queue_node);
    queue_prepend(&pga->poll_group_queue, &poll_group->queue_node);
  }

  return poll_group;
}

void poll_group_release(poll_group_t* poll_group) {
  poll_group_allocator_t* pga = poll_group->allocator;

  poll_group->user_count--;
  assert(poll_group->user_count < _DS_MAX_USERS);

  /* Move to the back of the queue. */
  queue_remove(&poll_group->queue_node);
  queue_append(&pga->poll_group_queue, &poll_group->queue_node);

  /* TODO: free the poll_group_t* item at some point. */
}

typedef struct poll_req {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  ep_sock_t* sock_info;
} poll_req_t;

static inline poll_req_t* _poll_req_alloc(void) {
  poll_req_t* poll_req = malloc(sizeof *poll_req);
  if (poll_req == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return poll_req;
}

static inline poll_req_t* _poll_req_free(poll_req_t* poll_req) {
  free(poll_req);
  return NULL;
}

poll_req_t* poll_req_new(ep_port_t* port_info, ep_sock_t* sock_info) {
  poll_req_t* poll_req = _poll_req_alloc();
  if (poll_req == NULL)
    return NULL;

  memset(poll_req, 0, sizeof *poll_req);
  poll_req->sock_info = sock_info;

  ep_sock_register_poll_req(port_info, sock_info);

  return poll_req;
}

void poll_req_delete(ep_port_t* port_info,
                     ep_sock_t* sock_info,
                     poll_req_t* poll_req) {
  assert(poll_req != NULL);

  ep_sock_unregister_poll_req(port_info, sock_info);

  _poll_req_free(poll_req);
}

poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped) {
  return container_of(overlapped, poll_req_t, overlapped);
}

ep_sock_t* poll_req_get_sock_data(const poll_req_t* poll_req) {
  return poll_req->sock_info;
}

static DWORD _epoll_events_to_afd_events(uint32_t epoll_events) {
  DWORD afd_events;

  /* These events should always be monitored. */
  assert(epoll_events & EPOLLERR);
  assert(epoll_events & EPOLLHUP);
  afd_events = AFD_POLL_ABORT | AFD_POLL_CONNECT_FAIL | AFD_POLL_LOCAL_CLOSE;

  if (epoll_events & (EPOLLIN | EPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (epoll_events & (EPOLLPRI | EPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (epoll_events & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT;

  return afd_events;
}

static uint32_t _afd_events_to_epoll_events(DWORD afd_events) {
  uint32_t epoll_events = 0;

  if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
    epoll_events |= EPOLLIN | EPOLLRDNORM;
  if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
    epoll_events |= EPOLLPRI | EPOLLRDBAND;
  if (afd_events & AFD_POLL_SEND)
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if ((afd_events & AFD_POLL_DISCONNECT) && !(afd_events & AFD_POLL_ABORT))
    epoll_events |= EPOLLIN | EPOLLRDHUP;
  if (afd_events & AFD_POLL_ABORT)
    epoll_events |= EPOLLHUP;
  if (afd_events & AFD_POLL_CONNECT)
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if (afd_events & AFD_POLL_CONNECT_FAIL)
    epoll_events |= EPOLLERR;

  return epoll_events;
}

int poll_req_submit(poll_req_t* poll_req,
                    uint32_t epoll_events,
                    SOCKET socket,
                    SOCKET driver_socket) {
  int r;

  memset(&poll_req->overlapped, 0, sizeof poll_req->overlapped);

  poll_req->poll_info.Exclusive = FALSE;
  poll_req->poll_info.NumberOfHandles = 1;
  poll_req->poll_info.Timeout.QuadPart = INT64_MAX;
  poll_req->poll_info.Handles[0].Handle = (HANDLE) socket;
  poll_req->poll_info.Handles[0].Status = 0;
  poll_req->poll_info.Handles[0].Events =
      _epoll_events_to_afd_events(epoll_events);

  r = afd_poll(driver_socket, &poll_req->poll_info, &poll_req->overlapped);
  if (r != 0 && GetLastError() != ERROR_IO_PENDING)
    return_error(-1);

  return 0;
}

int poll_req_cancel(poll_req_t* poll_req, SOCKET driver_socket) {
  OVERLAPPED* overlapped = &poll_req->overlapped;

  if (CancelIoEx((HANDLE) driver_socket, overlapped)) {
    DWORD error = GetLastError();
    if (error == ERROR_NOT_FOUND)
      return 0; /* Already completed or canceled. */
    else
      return_error(-1);
  }

  return 0;
}

void poll_req_complete(const poll_req_t* poll_req,
                       uint32_t* epoll_events_out,
                       bool* socket_closed_out) {
  const OVERLAPPED* overlapped = &poll_req->overlapped;

  uint32_t epoll_events = 0;
  bool socket_closed = false;

  if ((NTSTATUS) overlapped->Internal == STATUS_CANCELLED) {
    /* The poll request was cancelled by CancelIoEx. */
  } else if (!NT_SUCCESS(overlapped->Internal)) {
    /* The overlapped request itself failed in an unexpected way. */
    epoll_events = EPOLLERR;
  } else if (poll_req->poll_info.NumberOfHandles < 1) {
    /* This overlapped request succeeded but didn't report any events. */
  } else {
    /* Events related to our socket were reported. */
    DWORD afd_events = poll_req->poll_info.Handles[0].Events;

    if (afd_events & AFD_POLL_LOCAL_CLOSE)
      socket_closed = true; /* Socket closed locally be silently dropped. */
    else
      epoll_events = _afd_events_to_epoll_events(afd_events);
  }

  *epoll_events_out = epoll_events;
  *socket_closed_out = socket_closed;
}

static inline int _tree_compare(tree_node_t* a, tree_node_t* b) {
  if (a->key < b->key)
    return -1;
  else if (a->key > b->key)
    return 1;
  else
    return 0;
}

RB_GENERATE_STATIC(tree, tree_node, node, _tree_compare);

void tree_init(tree_t* tree) {
  RB_INIT(tree);
}

void tree_node_init(tree_node_t* node) {
  memset(node, 0, sizeof *node);
}

int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key) {
  tree_node_t* existing_node;

  if (key == 0)
    return_error(-1, ERROR_INVALID_PARAMETER);
  if (node->key != 0)
    return_error(-1, ERROR_ALREADY_EXISTS);

  node->key = key;
  existing_node = RB_INSERT(tree, tree, node);

  if (existing_node != NULL) {
    node->key = 0;
    return_error(-1, ERROR_ALREADY_EXISTS);
  }

  return 0;
}

int tree_del(tree_t* tree, tree_node_t* node) {
  tree_node_t* removed_node;

  if (node->key == 0)
    return_error(-1, ERROR_NOT_FOUND);

  removed_node = RB_REMOVE(tree, tree, node);

  if (removed_node == NULL)
    return_error(-1, ERROR_NOT_FOUND);
  else
    assert(removed_node == node);

  node->key = 0;

  return 0;
}

tree_node_t* tree_find(tree_t* tree, uintptr_t key) {
  tree_node_t* node;
  tree_node_t lookup;

  if (key == 0)
    return_error(NULL, ERROR_INVALID_PARAMETER);

  memset(&lookup, 0, sizeof lookup);
  lookup.key = key;

  node = RB_FIND(tree, tree, &lookup);
  if (node == NULL)
    return_error(NULL, ERROR_NOT_FOUND);

  return node;
}

tree_node_t* tree_root(tree_t* tree) {
  return RB_ROOT(tree);
}
