#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>

#include "afd.h"
#include "epoll.h"
#include "error.h"
#include "nt.h"
#include "tree.h"
#include "util.h"
#include "win.h"

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

#define EPOLL__EVENT_MASK 0xffff

#define EPOLL__SOCK_LISTED 0x1
#define EPOLL__SOCK_DELETED 0x2

#define ATTN_LIST_ADD(p, s)                 \
  do {                                      \
    (s)->attn_list_prev = NULL;             \
    (s)->attn_list_next = (p)->attn_list;   \
    if ((p)->attn_list) {                   \
      (p)->attn_list->attn_list_prev = (s); \
    }                                       \
    (p)->attn_list = (s);                   \
    (s)->flags |= EPOLL__SOCK_LISTED;       \
  } while (0)

typedef struct epoll_port_data epoll_port_data_t;
typedef struct epoll_io_req epoll_io_req_t;
typedef struct epoll_sock_data epoll_sock_data_t;

static int epoll__initialize();
static SOCKET epoll__get_peer_socket(epoll_port_data_t* port_data,
                                     WSAPROTOCOL_INFOW* protocol_info);
static SOCKET epoll__create_peer_socket(HANDLE iocp,
                                        WSAPROTOCOL_INFOW* protocol_info);
static int epoll__compare_sock_data(epoll_sock_data_t* a,
                                    epoll_sock_data_t* b);
static int epoll__submit_poll_req(epoll_port_data_t* port_data,
                                  epoll_sock_data_t* sock_data);

static int epoll__initialized = 0;

/* State associated with a epoll handle. */
typedef struct epoll_port_data {
  HANDLE iocp;
  SOCKET peer_sockets[array_count(AFD_PROVIDER_IDS)];
  RB_HEAD(epoll_sock_data_tree, epoll_sock_data) sock_data_tree;
  epoll_sock_data_t* attn_list;
  size_t pending_reqs_count;
} epoll_port_data_t;

/* State associated with a socket that is registered to the epoll port. */
typedef struct epoll_sock_data {
  SOCKET sock;
  SOCKET base_sock;
  SOCKET peer_sock;
  uint32_t registered_events;
  uint32_t submitted_events;
  uint32_t flags;
  uint32_t io_req_generation;
  uint64_t user_data;
  epoll_io_req_t* free_io_req;
  epoll_sock_data_t* attn_list_prev;
  epoll_sock_data_t* attn_list_next;
  RB_ENTRY(epoll_sock_data) tree_entry;
} epoll_sock_data_t;

/* State associated with a AFD_POLL request. */
typedef struct epoll_io_req {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  uint32_t generation;
  epoll_sock_data_t* sock_data;
} epoll_io_req_t;

RB_GENERATE_STATIC(epoll_sock_data_tree,
                   epoll_sock_data,
                   tree_entry,
                   epoll__compare_sock_data)

epoll_t epoll_create() {
  epoll_port_data_t* port_data;
  HANDLE iocp;

  /* If necessary, do global initialization first. This is totally not
   * thread-safe at the moment.
   */
  if (!epoll__initialized) {
    if (epoll__initialize() < 0)
      return NULL;
    epoll__initialized = 1;
  }

  port_data = malloc(sizeof *port_data);
  if (port_data == NULL) {
    SetLastError(ERROR_OUTOFMEMORY);
    return NULL;
  }

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE) {
    free(port_data);
    return NULL;
  }

  port_data->iocp = iocp;
  port_data->attn_list = NULL;
  port_data->pending_reqs_count = 0;

  memset(&port_data->peer_sockets, 0, sizeof port_data->peer_sockets);
  RB_INIT(&port_data->sock_data_tree);

  return (epoll_t) port_data;
}

int epoll_ctl(epoll_t port_handle,
              int op,
              uintptr_t sock,
              struct epoll_event* ev) {
  epoll_port_data_t* port_data;
  SOCKET base_sock;

  port_data = (epoll_port_data_t*) port_handle;

  switch (op) {
    case EPOLL_CTL_ADD: {
      epoll_sock_data_t* sock_data;
      epoll_io_req_t* io_req;
      SOCKET peer_sock;
      WSAPROTOCOL_INFOW protocol_info;
      DWORD bytes;
      int len;

      /* Try to obtain a base handle for the socket, so we can bypass LSPs
       * that get in the way if we want to talk to the kernel directly. If
       * it fails we try if we work with the original socket. Note that on
       * windows XP/2k3 this will always fail since they don't support the
       * SIO_BASE_HANDLE ioctl.
       */
      base_sock = sock;
      WSAIoctl(sock,
               SIO_BASE_HANDLE,
               NULL,
               0,
               &base_sock,
               sizeof base_sock,
               &bytes,
               NULL,
               NULL);

      /* Obtain protocol information about the socket. */
      len = sizeof protocol_info;
      if (getsockopt(base_sock,
                     SOL_SOCKET,
                     SO_PROTOCOL_INFOW,
                     (char*) &protocol_info,
                     &len) != 0) {
        return -1;
      }

      peer_sock = epoll__get_peer_socket(port_data, &protocol_info);
      if (peer_sock == INVALID_SOCKET) {
        return -1;
      }

      sock_data = malloc(sizeof *sock_data);
      if (sock_data == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        return -1;
      }

      io_req = malloc(sizeof *io_req);
      if (io_req == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        free(sock_data);
        return -1;
      }

      sock_data->sock = sock;
      sock_data->base_sock = base_sock;
      sock_data->io_req_generation = 0;
      sock_data->submitted_events = 0;
      sock_data->registered_events = ev->events | EPOLLERR | EPOLLHUP;
      sock_data->user_data = ev->data.u64;
      sock_data->peer_sock = peer_sock;
      sock_data->free_io_req = io_req;
      sock_data->flags = 0;

      if (RB_INSERT(epoll_sock_data_tree,
                    &port_data->sock_data_tree,
                    sock_data) != NULL) {
        /* Socket was already added. */
        free(sock_data);
        free(io_req);
        SetLastError(ERROR_ALREADY_EXISTS);
        return -1;
      }

      /* Add to attention list */
      ATTN_LIST_ADD(port_data, sock_data);

      return 0;
    }

    case EPOLL_CTL_MOD: {
      epoll_sock_data_t lookup;
      epoll_sock_data_t* sock_data;

      lookup.sock = sock;
      sock_data =
          RB_FIND(epoll_sock_data_tree, &port_data->sock_data_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with epoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      if (ev->events & EPOLL__EVENT_MASK & ~sock_data->submitted_events) {
        if (sock_data->free_io_req == NULL) {
          epoll_io_req_t* io_req = malloc(sizeof *io_req);
          if (io_req == NULL) {
            SetLastError(ERROR_OUTOFMEMORY);
            return -1;
          }

          sock_data->free_io_req = NULL;
        }

        /* Add to attention list, if not already added. */
        if (!(sock_data->flags & EPOLL__SOCK_LISTED)) {
          ATTN_LIST_ADD(port_data, sock_data);
        }
      }

      sock_data->registered_events = ev->events | EPOLLERR | EPOLLHUP;
      sock_data->user_data = ev->data.u64;
      return 0;
    }

    case EPOLL_CTL_DEL: {
      epoll_sock_data_t lookup;
      epoll_sock_data_t* sock_data;

      lookup.sock = sock;
      sock_data =
          RB_FIND(epoll_sock_data_tree, &port_data->sock_data_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with epoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      RB_REMOVE(epoll_sock_data_tree, &port_data->sock_data_tree, sock_data);

      free(sock_data->free_io_req);
      sock_data->flags |= EPOLL__SOCK_DELETED;

      /* Remove from attention list. */
      if (sock_data->flags & EPOLL__SOCK_LISTED) {
        if (sock_data->attn_list_prev != NULL)
          sock_data->attn_list_prev->attn_list_next =
              sock_data->attn_list_next;
        if (sock_data->attn_list_next != NULL)
          sock_data->attn_list_next->attn_list_prev =
              sock_data->attn_list_prev;
        if (port_data->attn_list == sock_data)
          port_data->attn_list = sock_data->attn_list_next;
        sock_data->attn_list_prev = NULL;
        sock_data->attn_list_next = NULL;
        sock_data->flags &= ~EPOLL__SOCK_LISTED;
      }

      if (sock_data->submitted_events == 0) {
        assert(sock_data->io_req_generation == 0);
        free(sock_data);
      } else {
        /* There are still one or more io requests pending. Wait for
         * all pending requests to return before freeing.
         */
        assert(sock_data->io_req_generation > 0);
      }

      return 0;
    }

    default:
      WSASetLastError(WSAEINVAL);
      return -1;
  }
}

int epoll_wait(epoll_t port_handle,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  epoll_port_data_t* port_data;
  ULONGLONG due = 0;
  DWORD gqcs_timeout;

  port_data = (epoll_port_data_t*) port_handle;

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

  /* Dequeue completion packets until either at least one interesting event
   * has been discovered, or the timeout is reached.
   */
  do {
    DWORD result, max_entries;
    ULONG count, i;
    OVERLAPPED_ENTRY entries[64];
    int num_events = 0;

    /* Create overlapped poll requests for all sockets on the attention list.
     */
    while (port_data->attn_list != NULL) {
      epoll_sock_data_t* sock_data = port_data->attn_list;
      assert(sock_data->flags & EPOLL__SOCK_LISTED);

      /* Check if there are events registered that are not yet submitted. In
       * that case we need to submit another req.
       */
      if (sock_data->registered_events & EPOLL__EVENT_MASK &
          ~sock_data->submitted_events) {
        int r = epoll__submit_poll_req(port_data, sock_data);

        if (r) {
          /* If submitting the poll request fails then most likely the socket
           * is invalid. In this case we silently remove the socket from the
           * epoll port. Other errors make epoll_wait() fail.
           */
          if (WSAGetLastError() != WSAENOTSOCK)
            return -1;

          /* Skip to the next attention list item already, because we're about
           * to delete the currently selected socket.
           */
          port_data->attn_list = sock_data->attn_list_next;
          sock_data->flags &= ~EPOLL__SOCK_LISTED;

          /* Delete it. */
          r = epoll_ctl(port_handle, EPOLL_CTL_DEL, sock_data->sock, NULL);
          assert(r == 0);

          continue;
        }
      }

      /* Remove from attention list */
      port_data->attn_list = sock_data->attn_list_next;
      sock_data->attn_list_prev = sock_data->attn_list_next = NULL;
      sock_data->flags &= ~EPOLL__SOCK_LISTED;
    }

    /* Compute how much overlapped entries can be dequeued at most. */
    max_entries = array_count(entries);
    if ((int) max_entries > maxevents)
      max_entries = maxevents;

    result = GetQueuedCompletionStatusEx(
        port_data->iocp, entries, max_entries, &count, gqcs_timeout, FALSE);

    if (!result) {
      DWORD error = GetLastError();
      if (error == WAIT_TIMEOUT) {
        return 0;
      } else {
        return -1;
      }
    }

    port_data->pending_reqs_count -= count;

    /* Successfully dequeued overlappeds. */
    for (i = 0; i < count; i++) {
      OVERLAPPED* overlapped;
      epoll_io_req_t* io_req;
      epoll_sock_data_t* sock_data;
      DWORD afd_events;
      int registered_events, reported_events;

      overlapped = entries[i].lpOverlapped;
      io_req = CONTAINING_RECORD(overlapped, epoll_io_req_t, overlapped);
      sock_data = io_req->sock_data;

      if (io_req->generation < sock_data->io_req_generation) {
        /* This io request has been superseded. Free and ignore it. */
        free(io_req);
        continue;
      }

      /* Dequeued the most recent request. Reset generation and
       * submitted_events.
       */
      sock_data->io_req_generation = 0;
      sock_data->submitted_events = 0;
      sock_data->free_io_req = io_req;

      registered_events = sock_data->registered_events;
      reported_events = 0;

      /* Check if this io request was associated with a socket that was removed
       * with EPOLL_CTL_DEL.
       */
      if (sock_data->flags & EPOLL__SOCK_DELETED) {
        free(io_req);
        free(sock_data);
        continue;
      }

      /* Check for error. */
      if (!NT_SUCCESS(overlapped->Internal)) {
        struct epoll_event* ev = events + (num_events++);
        ev->data.u64 = sock_data->user_data;
        ev->events = EPOLLERR;
        continue;
      }

      if (io_req->poll_info.NumberOfHandles == 0) {
        /* NumberOfHandles can be zero if this poll request was canceled
         * due to a more recent exclusive poll request.
         */
        afd_events = 0;
      } else {
        afd_events = io_req->poll_info.Handles[0].Events;
      }

      /* Check for a closed socket. */
      if (afd_events & AFD_POLL_LOCAL_CLOSE) {
        RB_REMOVE(epoll_sock_data_tree, &port_data->sock_data_tree, sock_data);
        free(io_req);
        free(sock_data);
        continue;
      }

      /* Convert afd events to epoll events. */
      if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
        reported_events |= (EPOLLIN | EPOLLRDNORM);
      if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
        reported_events |= (EPOLLIN | EPOLLRDBAND);
      if (afd_events & AFD_POLL_SEND)
        reported_events |= (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND);
      if ((afd_events & AFD_POLL_DISCONNECT) && !(afd_events & AFD_POLL_ABORT))
        reported_events |= (EPOLLRDHUP | EPOLLIN | EPOLLRDNORM | EPOLLRDBAND);
      if (afd_events & AFD_POLL_ABORT)
        reported_events |= EPOLLHUP | EPOLLERR;
      if (afd_events & AFD_POLL_CONNECT)
        reported_events |= (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND);
      if (afd_events & AFD_POLL_CONNECT_FAIL)
        reported_events |= EPOLLERR;

      /* Don't report events that the user didn't specify. */
      reported_events &= registered_events;

      if (reported_events == 0) {
        assert(!(sock_data->flags & EPOLL__SOCK_LISTED));
        ATTN_LIST_ADD(port_data, sock_data);
      } else {
        /* Unless EPOLLONESHOT is used add the socket back to the attention
         * list.
         */
        if (!(registered_events & EPOLLONESHOT)) {
          assert(!(sock_data->flags & EPOLL__SOCK_LISTED));
          ATTN_LIST_ADD(port_data, sock_data);
        }
      }

      if (reported_events) {
        struct epoll_event* ev = events + (num_events++);
        ev->data.u64 = sock_data->user_data;
        ev->events = reported_events;
      }
    }

    if (num_events > 0)
      return num_events;

    /* Events were dequeued, but none were relevant. Recompute timeout. */
    if (timeout > 0) {
      gqcs_timeout = due - GetTickCount64();
    }
  } while (timeout > 0);

  return 0;
}

int epoll_close(epoll_t port_handle) {
  epoll_port_data_t* port_data;
  epoll_sock_data_t* sock_data;

  port_data = (epoll_port_data_t*) port_handle;

  /* Close all peer sockets. This will make all pending io requests return. */
  for (size_t i = 0; i < array_count(port_data->peer_sockets); i++) {
    SOCKET peer_sock = port_data->peer_sockets[i];
    if (peer_sock != 0 && peer_sock != INVALID_SOCKET) {
      if (closesocket(peer_sock) != 0)
        return -1;

      port_data->peer_sockets[i] = 0;
    }
  }

  /* There is no list of io requests to free. And even if there was, just
   * freeing them would be dangerous since the kernel might still alter
   * the overlapped status contained in them. But since we are sure that
   * all requests will soon return, just await them all.
   */
  while (port_data->pending_reqs_count > 0) {
    OVERLAPPED_ENTRY entries[64];
    DWORD result;
    ULONG count, i;

    result = GetQueuedCompletionStatusEx(port_data->iocp,
                                         entries,
                                         array_count(entries),
                                         &count,
                                         INFINITE,
                                         FALSE);

    if (!result)
      return -1;

    port_data->pending_reqs_count -= count;

    for (i = 0; i < count; i++) {
      epoll_io_req_t* io_req = CONTAINING_RECORD(
          entries[i].lpOverlapped, epoll_io_req_t, overlapped);
      free(io_req);
    }
  }

  /* Remove all entries from the socket_state tree. */
  while ((sock_data = RB_ROOT(&port_data->sock_data_tree))) {
    RB_REMOVE(epoll_sock_data_tree, &port_data->sock_data_tree, sock_data);

    if (sock_data->free_io_req != NULL)
      free(sock_data->free_io_req);
    free(sock_data);
  }

  /* Close the I/O completion port. */
  CloseHandle(port_data->iocp);

  /* Finally, remove the port data. */
  free(port_data);

  return 0;
}

int epoll__initialize() {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return -1;

  if (nt_initialize() < 0)
    return -1;

  return 0;
}

SOCKET epoll__get_peer_socket(epoll_port_data_t* port_data,
                              WSAPROTOCOL_INFOW* protocol_info) {
  ssize_t index;
  size_t i;
  SOCKET peer_socket;

  index = -1;
  for (i = 0; i < array_count(AFD_PROVIDER_IDS); i++) {
    if (memcmp((void*) &protocol_info->ProviderId,
               (void*) &AFD_PROVIDER_IDS[i],
               sizeof protocol_info->ProviderId) == 0) {
      index = i;
    }
  }

  /* Check if the protocol uses an msafd socket. */
  if (index < 0) {
    SetLastError(ERROR_NOT_SUPPORTED);
    return INVALID_SOCKET;
  }

  /* If we didn't (try) to create a peer socket yet, try to make one. Don't
   * try again if the peer socket creation failed earlier for the same
   * protocol.
   */
  peer_socket = port_data->peer_sockets[index];
  if (peer_socket == 0) {
    peer_socket = epoll__create_peer_socket(port_data->iocp, protocol_info);
    port_data->peer_sockets[index] = peer_socket;
  }

  return peer_socket;
}

SOCKET epoll__create_peer_socket(HANDLE iocp,
                                 WSAPROTOCOL_INFOW* protocol_info) {
  SOCKET sock = 0;

  sock = WSASocketW(protocol_info->iAddressFamily,
                    protocol_info->iSocketType,
                    protocol_info->iProtocol,
                    protocol_info,
                    0,
                    WSA_FLAG_OVERLAPPED);
  if (sock == INVALID_SOCKET) {
    return INVALID_SOCKET;
  }

  if (!SetHandleInformation((HANDLE) sock, HANDLE_FLAG_INHERIT, 0)) {
    goto error;
  };

  if (CreateIoCompletionPort((HANDLE) sock, iocp, 0, 0) == NULL) {
    goto error;
  }

  return sock;

error:
  closesocket(sock);
  return INVALID_SOCKET;
}

int epoll__compare_sock_data(epoll_sock_data_t* a, epoll_sock_data_t* b) {
  return a->sock - b->sock;
}

int epoll__submit_poll_req(epoll_port_data_t* port_data,
                           epoll_sock_data_t* sock_data) {
  epoll_io_req_t* io_req;
  int registered_events;
  DWORD result, afd_events;

  io_req = sock_data->free_io_req;
  registered_events = sock_data->registered_events;

  /* epoll_ctl should ensure that there is a free io_req struct. */
  assert(io_req != NULL);

  /* These events should always be registered. */
  assert(registered_events & EPOLLERR);
  assert(registered_events & EPOLLHUP);
  afd_events = AFD_POLL_ABORT | AFD_POLL_CONNECT_FAIL | AFD_POLL_LOCAL_CLOSE;

  if (registered_events & (EPOLLIN | EPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (registered_events & (EPOLLIN | EPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (registered_events & (EPOLLOUT | EPOLLWRNORM | EPOLLRDBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT;

  io_req->generation = sock_data->io_req_generation + 1;
  io_req->sock_data = sock_data;

  memset(&io_req->overlapped, 0, sizeof io_req->overlapped);

  io_req->poll_info.Exclusive = TRUE;
  io_req->poll_info.NumberOfHandles = 1;
  io_req->poll_info.Timeout.QuadPart = INT64_MAX;
  io_req->poll_info.Handles[0].Handle = (HANDLE) sock_data->base_sock;
  io_req->poll_info.Handles[0].Status = 0;
  io_req->poll_info.Handles[0].Events = afd_events;

  result =
      afd_poll(sock_data->peer_sock, &io_req->poll_info, &io_req->overlapped);
  if (result != 0) {
    DWORD error = WSAGetLastError();
    if (error != WSA_IO_PENDING) {
      /* If this happens an error happened and no overlapped operation was
       * started.
       */
      return -1;
    }
  }

  sock_data->submitted_events = registered_events;
  sock_data->io_req_generation = io_req->generation;
  sock_data->free_io_req = NULL;
  port_data->pending_reqs_count++;

  return 0;
}
