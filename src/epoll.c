#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "afd.h"
#include "epoll.h"
#include "error.h"
#include "nt.h"
#include "poll-request.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "win.h"

#define _EP_COMPLETION_LIST_LENGTH 64

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

#define _EP_EVENT_MASK 0xffff

#define _EP_SOCK_LISTED 0x1
#define _EP_SOCK_DELETED 0x2

#define ATTN_LIST_ADD(p, s)                                   \
  do {                                                        \
    QUEUE_INSERT_TAIL(&(p)->update_queue, &(s)->queue_entry); \
    (s)->flags |= _EP_SOCK_LISTED;                            \
  } while (0)

typedef struct _ep_port_data _ep_port_data_t;
typedef struct poll_req poll_req_t;
typedef struct _ep_sock_data _ep_sock_data_t;

static int _ep_initialize(void);
static SOCKET _ep_get_driver_socket(_ep_port_data_t* port_data,
                                    WSAPROTOCOL_INFOW* protocol_info);
static SOCKET _ep_create_driver_socket(HANDLE iocp,
                                       WSAPROTOCOL_INFOW* protocol_info);
static int _ep_compare_sock_data(_ep_sock_data_t* a, _ep_sock_data_t* b);
static int _ep_submit_poll_req(_ep_port_data_t* port_data,
                               _ep_sock_data_t* sock_data);

static int _ep_initialized = 0;

/* State associated with a epoll handle. */
typedef struct _ep_port_data {
  HANDLE iocp;
  SOCKET driver_sockets[array_count(AFD_PROVIDER_GUID_LIST)];
  RB_HEAD(_ep_sock_data_tree, _ep_sock_data) sock_data_tree;
  QUEUE update_queue;
  size_t poll_req_count;
} _ep_port_data_t;

/* State associated with a socket that is registered to the epoll port. */
typedef struct _ep_sock_data {
  SOCKET socket;
  SOCKET afd_socket;
  SOCKET driver_socket;
  RB_ENTRY(_ep_sock_data) tree_entry;
  QUEUE queue_entry;
  epoll_data_t user_data;
  poll_req_t* latest_poll_req;
  uint32_t user_events;
  uint32_t latest_poll_req_events;
  uint32_t poll_req_count;
  uint32_t flags;
} _ep_sock_data_t;

static int _ep_sock_data_delete(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data);

RB_GENERATE_STATIC(_ep_sock_data_tree,
                   _ep_sock_data,
                   tree_entry,
                   _ep_compare_sock_data)

static int _ep_get_related_sockets(_ep_port_data_t* port_data,
                                   SOCKET socket,
                                   SOCKET* afd_socket_out,
                                   SOCKET* driver_socket_out) {
  SOCKET afd_socket, driver_socket;
  WSAPROTOCOL_INFOW protocol_info;
  DWORD bytes;
  int len;

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

  /* Obtain protocol information about the socket. */
  len = sizeof protocol_info;
  if (getsockopt(afd_socket,
                 SOL_SOCKET,
                 SO_PROTOCOL_INFOW,
                 (char*) &protocol_info,
                 &len) != 0)
    return_error(-1);

  driver_socket = _ep_get_driver_socket(port_data, &protocol_info);
  if (driver_socket == INVALID_SOCKET)
    return -1;

  *afd_socket_out = afd_socket;
  *driver_socket_out = driver_socket;

  return 0;
}

static inline _ep_sock_data_t* _ep_sock_data_alloc(void) {
  _ep_sock_data_t* sock_data = malloc(sizeof *sock_data);
  if (sock_data == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return sock_data;
}

static inline void _ep_sock_data_free(_ep_sock_data_t* sock_data) {
  free(sock_data);
}

static _ep_sock_data_t* _ep_sock_data_new(_ep_port_data_t* port_data) {
  _ep_sock_data_t* sock_data = _ep_sock_data_alloc();
  if (sock_data == NULL)
    return NULL;

  (void) port_data;

  memset(sock_data, 0, sizeof *sock_data);
  QUEUE_INIT(&sock_data->queue_entry);

  return sock_data;
}

void _ep_sock_data_add_poll_req(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data) {
  assert(!(sock_data->flags & _EP_SOCK_DELETED));
  sock_data->poll_req_count++;
  port_data->poll_req_count++;
}

void _ep_sock_data_del_poll_req(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data) {
  sock_data->poll_req_count--;
  port_data->poll_req_count--;

  if ((sock_data->flags & _EP_SOCK_DELETED) && sock_data->poll_req_count == 0)
    _ep_sock_data_delete(port_data, sock_data);
}

static int _ep_sock_data_set_socket(_ep_port_data_t* port_data,
                                    _ep_sock_data_t* sock_data,
                                    SOCKET socket) {
  if (socket == 0 || socket == INVALID_SOCKET)
    return_error(-1, ERROR_INVALID_HANDLE);

  if (sock_data->socket != 0)
    return_error(-1, ERROR_INVALID_PARAMETER);

  if (_ep_get_related_sockets(port_data,
                              socket,
                              &sock_data->afd_socket,
                              &sock_data->driver_socket) < 0)
    return -1;

  sock_data->socket = socket;

  if (RB_INSERT(_ep_sock_data_tree, &port_data->sock_data_tree, sock_data) !=
      NULL)
    return_error(-1, ERROR_ALREADY_EXISTS); /* Socket already in epoll set. */

  return 0;
}

static int _ep_sock_data_delete(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data) {
  /* Remove from lookup tree. */
  if (sock_data->socket != 0 && !(sock_data->flags & _EP_SOCK_DELETED)) {
    RB_REMOVE(_ep_sock_data_tree, &port_data->sock_data_tree, sock_data);
    sock_data->flags |= _EP_SOCK_DELETED;
  }

  /* Remove from attention list. */
  if (sock_data->flags & _EP_SOCK_LISTED) {
    QUEUE_REMOVE(&sock_data->queue_entry);
    sock_data->flags &= ~_EP_SOCK_LISTED;
  }

  /* The socket may still have pending overlapped requests that have yet to be
   * reported by the completion port. If that's the case the structure can't be
   * free'd yet; epoll_wait() will do it as it dequeues those requests.
   */
  if (sock_data->poll_req_count == 0)
    _ep_sock_data_free(sock_data);

  return 0;
}

static bool _ep_sock_data_marked_for_deletion(_ep_sock_data_t* sock_data) {
  return sock_data->flags & _EP_SOCK_DELETED;
}

static int _ep_sock_data_set_event(_ep_port_data_t* port_data,
                                   _ep_sock_data_t* sock_data,
                                   const struct epoll_event* ev) {
  /* EPOLLERR and EPOLLHUP are always reported, even when no sollicited. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_data->user_events = events;
  sock_data->user_data = ev->data;

  if (events & _EP_EVENT_MASK & ~(sock_data->latest_poll_req_events)) {
    /* Add to attention list, if not already added. */
    if (!(sock_data->flags & _EP_SOCK_LISTED)) {
      QUEUE_INSERT_TAIL(&port_data->update_queue, &sock_data->queue_entry);
      sock_data->flags |= _EP_SOCK_LISTED;
    }
  }

  return 0;
}

static int _ep_ctl_add(_ep_port_data_t* port_data,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  _ep_sock_data_t* sock_data = _ep_sock_data_new(port_data);
  if (sock_data == NULL)
    return -1;

  if (_ep_sock_data_set_socket(port_data, sock_data, socket) < 0 ||
      _ep_sock_data_set_event(port_data, sock_data, ev) < 0) {
    _ep_sock_data_delete(port_data, sock_data);
    return -1;
  }

  return 0;
}

static int _ep_ctl_mod(_ep_port_data_t* port_data,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  _ep_sock_data_t lookup;
  _ep_sock_data_t* sock_data;

  lookup.socket = socket;
  sock_data = RB_FIND(_ep_sock_data_tree, &port_data->sock_data_tree, &lookup);
  if (sock_data == NULL)
    return_error(-1, ERROR_BAD_NETPATH); /* Socket not in epoll set. */

  if (_ep_sock_data_set_event(port_data, sock_data, ev) < 0)
    return -1;

  return 0;
}

static int _ep_ctl_del(_ep_port_data_t* port_data, uintptr_t socket) {
  _ep_sock_data_t lookup;
  _ep_sock_data_t* sock_data;

  lookup.socket = socket;
  sock_data = RB_FIND(_ep_sock_data_tree, &port_data->sock_data_tree, &lookup);
  if (sock_data == NULL)
    return_error(-1, ERROR_NOT_FOUND); /* Socket not in epoll set. */

  if (_ep_sock_data_delete(port_data, sock_data) < 0)
    return -1;

  return 0;
}

int epoll_ctl(epoll_t port_handle,
              int op,
              uintptr_t socket,
              struct epoll_event* ev) {
  _ep_port_data_t* port_data = (_ep_port_data_t*) port_handle;

  switch (op) {
    case EPOLL_CTL_ADD:
      return _ep_ctl_add(port_data, socket, ev);
    case EPOLL_CTL_MOD:
      return _ep_ctl_mod(port_data, socket, ev);
    case EPOLL_CTL_DEL:
      return _ep_ctl_del(port_data, socket);
  }

  return_error(-1, ERROR_INVALID_PARAMETER);
}

static int _ep_sock_update(_ep_port_data_t* port_data,
                           _ep_sock_data_t* sock_data) {
  bool broken = false;

  assert(sock_data->flags & _EP_SOCK_LISTED);

  /* Check if there are events registered that are not yet submitted. In
   * that case we need to submit another req.
   */
  if ((sock_data->user_events & _EP_EVENT_MASK &
       ~sock_data->latest_poll_req_events) == 0)
    /* All the events the user is interested in are already being monitored
     * by the latest poll request. */
    goto done;

  if (_ep_submit_poll_req(port_data, sock_data) < 0) {
    if (GetLastError() == ERROR_INVALID_HANDLE)
      /* The socket is broken. It will be dropped from the epoll set. */
      broken = true;
    else
      /* Another error occurred, which is propagated to the caller. */
      return -1;
  }

done:
  /* Remove the socket from the update queue. */
  QUEUE_REMOVE(&sock_data->queue_entry);
  sock_data->flags &= ~_EP_SOCK_LISTED;

  /* If we saw an ERROR_INVALID_HANDLE error, drop the socket.. */
  if (broken)
    _ep_sock_data_delete(port_data, sock_data);

  return 0;
}

static int _ep_port_update_events(_ep_port_data_t* port_data) {
  QUEUE* update_queue = &port_data->update_queue;

  /* Walk the queue, submitting new poll requests for every socket that needs
   * it. */
  while (!QUEUE_EMPTY(update_queue)) {
    QUEUE* queue_entry = QUEUE_HEAD(update_queue);
    _ep_sock_data_t* sock_data =
        QUEUE_DATA(queue_entry, _ep_sock_data_t, queue_entry);

    if (_ep_sock_update(port_data, sock_data) < 0)
      return -1;

    /* _ep_sock_update() removes the socket from the update list if
     * successfull. */
  }

  return 0;
}

static inline bool _poll_req_is_most_recent(_ep_sock_data_t* sock_data,
                                            poll_req_t* poll_req) {
  return poll_req == sock_data->latest_poll_req;
}

static inline void _poll_req_clear_recent(_ep_sock_data_t* sock_data) {
  sock_data->latest_poll_req = NULL;
  sock_data->latest_poll_req_events = 0;
}

static inline void _poll_req_set_recent(_ep_sock_data_t* sock_data,
                                        poll_req_t* poll_req,
                                        uint32_t epoll_events) {
  sock_data->latest_poll_req = poll_req;
  sock_data->latest_poll_req_events = epoll_events;
}

static size_t _ep_port_feed_event(_ep_port_data_t* port_data,
                                  poll_req_t* poll_req,
                                  struct epoll_event* ev) {
  _ep_sock_data_t* sock_data = poll_req_get_sock_data(poll_req);

  uint32_t epoll_events;
  bool drop_socket;
  size_t ev_count = 0;

  if (_ep_sock_data_marked_for_deletion(sock_data) ||
      !_poll_req_is_most_recent(sock_data, poll_req)) {
    /* Ignore completion for overlapped poll operation if it isn't
     * the the most recently posted one, or if the socket has been
     * deleted. */
    poll_req_delete(port_data, sock_data, poll_req);
    return 0;
  }

  _poll_req_clear_recent(sock_data);

  poll_req_complete(poll_req, &epoll_events, &drop_socket);

  /* Filter events that the user didn't ask for. */
  epoll_events &= sock_data->user_events;

  /* Drop the socket if the EPOLLONESHOT flag is set and there are any events
   * to report. */
  if (epoll_events != 0 && (sock_data->user_events & EPOLLONESHOT))
    drop_socket = true;

  /* Fill the ev structure if there are any events to report. */
  if (epoll_events != 0) {
    ev->data = sock_data->user_data;
    ev->events = epoll_events;
    ev_count = 1;
  }

  poll_req_delete(port_data, sock_data, poll_req);

  if (drop_socket)
    /* Drop the socket from the epoll set. */
    _ep_sock_data_delete(port_data, sock_data);
  else
    /* Put the socket back onto the attention list so a new poll request will
     * be submitted. */
    ATTN_LIST_ADD(port_data, sock_data);

  return ev_count;
}

static size_t _ep_port_feed_events(_ep_port_data_t* port_data,
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

    event_count += _ep_port_feed_event(port_data, poll_req, ev);
  }

  return event_count;
}

int epoll_wait(epoll_t port_handle,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  _ep_port_data_t* port_data;
  ULONGLONG due = 0;
  DWORD gqcs_timeout;

  port_data = (_ep_port_data_t*) port_handle;

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

    if (_ep_port_update_events(port_data) < 0)
      return -1;

    BOOL r = GetQueuedCompletionStatusEx(port_data->iocp,
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
        port_data, completion_list, completion_count, events, maxevents);
    if (event_count > 0)
      return (int) event_count;

    /* Events were dequeued, but none were relevant. Recompute timeout. */
    if (timeout > 0) {
      gqcs_timeout = due - GetTickCount64();
    }
  } while (timeout > 0);

  return 0;
}

epoll_t epoll_create(void) {
  _ep_port_data_t* port_data;
  HANDLE iocp;

  /* If necessary, do global initialization first. This is totally not
   * thread-safe at the moment.
   */
  if (!_ep_initialized) {
    if (_ep_initialize() < 0)
      return NULL;
    _ep_initialized = 1;
  }

  port_data = malloc(sizeof *port_data);
  if (port_data == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE) {
    free(port_data);
    return_error(NULL);
  }

  port_data->iocp = iocp;
  port_data->poll_req_count = 0;

  QUEUE_INIT(&port_data->update_queue);

  memset(&port_data->driver_sockets, 0, sizeof port_data->driver_sockets);
  RB_INIT(&port_data->sock_data_tree);

  return (epoll_t) port_data;
}

int epoll_close(epoll_t port_handle) {
  _ep_port_data_t* port_data;
  _ep_sock_data_t* sock_data;

  port_data = (_ep_port_data_t*) port_handle;

  /* Close all peer sockets. This will make all pending io requests return. */
  for (size_t i = 0; i < array_count(port_data->driver_sockets); i++) {
    SOCKET driver_socket = port_data->driver_sockets[i];
    if (driver_socket != 0 && driver_socket != INVALID_SOCKET) {
      if (closesocket(driver_socket) != 0)
        return_error(-1);

      port_data->driver_sockets[i] = 0;
    }
  }

  /* There is no list of io requests to free. And even if there was, just
   * freeing them would be dangerous since the kernel might still alter
   * the overlapped status contained in them. But since we are sure that
   * all requests will soon return, just await them all.
   */
  while (port_data->poll_req_count > 0) {
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
      return_error(-1);

    for (i = 0; i < count; i++) {
      poll_req_t* poll_req = overlapped_to_poll_req(entries[i].lpOverlapped);
      poll_req_delete(port_data, poll_req_get_sock_data(poll_req), poll_req);
    }
  }

  /* Remove all entries from the socket_state tree. */
  while ((sock_data = RB_ROOT(&port_data->sock_data_tree))) {
    RB_REMOVE(_ep_sock_data_tree, &port_data->sock_data_tree, sock_data);
    free(sock_data);
  }

  /* Close the I/O completion port. */
  if (!CloseHandle(port_data->iocp))
    return_error(-1);

  /* Finally, remove the port data. */
  free(port_data);

  return 0;
}

static int _ep_initialize(void) {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return_error(-1);

  if (nt_initialize() < 0)
    return -1;

  return 0;
}

static SOCKET _ep_get_driver_socket(_ep_port_data_t* port_data,
                                    WSAPROTOCOL_INFOW* protocol_info) {
  ssize_t index;
  size_t i;
  SOCKET driver_socket;

  index = -1;
  for (i = 0; i < array_count(AFD_PROVIDER_GUID_LIST); i++) {
    if (memcmp((void*) &protocol_info->ProviderId,
               (void*) &AFD_PROVIDER_GUID_LIST[i],
               sizeof protocol_info->ProviderId) == 0) {
      index = i;
    }
  }

  /* Check if the protocol uses an msafd socket. */
  if (index < 0)
    return_error(INVALID_SOCKET, ERROR_NOT_SUPPORTED);

  /* If we didn't (try) to create a peer socket yet, try to make one. Don't
   * try again if the peer socket creation failed earlier for the same
   * protocol.
   */
  driver_socket = port_data->driver_sockets[index];
  if (driver_socket == 0) {
    driver_socket = _ep_create_driver_socket(port_data->iocp, protocol_info);
    port_data->driver_sockets[index] = driver_socket;
  }

  return driver_socket;
}

static SOCKET _ep_create_driver_socket(HANDLE iocp,
                                       WSAPROTOCOL_INFOW* protocol_info) {
  SOCKET socket = 0;

  socket = WSASocketW(protocol_info->iAddressFamily,
                      protocol_info->iSocketType,
                      protocol_info->iProtocol,
                      protocol_info,
                      0,
                      WSA_FLAG_OVERLAPPED);
  if (socket == INVALID_SOCKET)
    return_error(INVALID_SOCKET);

  if (!SetHandleInformation((HANDLE) socket, HANDLE_FLAG_INHERIT, 0))
    goto error;

  if (CreateIoCompletionPort((HANDLE) socket, iocp, 0, 0) == NULL)
    goto error;

  return socket;

error:;
  DWORD error = GetLastError();
  closesocket(socket);
  return_error(INVALID_SOCKET, error);
}

int _ep_compare_sock_data(_ep_sock_data_t* a, _ep_sock_data_t* b) {
  return a->socket - b->socket;
}

static int _ep_submit_poll_req(_ep_port_data_t* port_data,
                               _ep_sock_data_t* sock_data) {
  poll_req_t* poll_req;
  uint32_t epoll_events = sock_data->user_events;

  poll_req = poll_req_new(port_data, sock_data);
  if (poll_req == NULL)
    return -1;

  if (poll_req_submit(poll_req,
                      epoll_events,
                      sock_data->afd_socket,
                      sock_data->driver_socket) < 0) {
    poll_req_delete(port_data, sock_data, poll_req);
    return -1;
  }

  _poll_req_set_recent(sock_data, poll_req, epoll_events);

  return 0;
}
