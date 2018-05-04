#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "afd.h"
#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "sock.h"
#include "ws.h"

#define _KNOWN_EPOLL_EVENTS                                            \
  (EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | \
   EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLRDHUP)

typedef enum _poll_status {
  _POLL_IDLE = 0,
  _POLL_PENDING,
  _POLL_CANCELLED
} _poll_status_t;

typedef struct ep_sock {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  queue_node_t queue_node;
  tree_node_t tree_node;
  poll_group_t* poll_group;
  SOCKET base_socket;
  epoll_data_t user_data;
  uint32_t user_events;
  uint32_t pending_events;
  _poll_status_t poll_status;
  bool delete_pending;
} ep_sock_t;

static inline ep_sock_t* _ep_sock_alloc(void) {
  ep_sock_t* sock_info = malloc(sizeof *sock_info);
  if (sock_info == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return sock_info;
}

static inline void _ep_sock_free(ep_sock_t* sock_info) {
  free(sock_info);
}

static int _ep_sock_cancel_poll(ep_sock_t* sock_info) {
  HANDLE driver_handle = (HANDLE) poll_group_get_socket(sock_info->poll_group);
  assert(sock_info->poll_status == _POLL_PENDING);

  /* CancelIoEx() may fail with ERROR_NOT_FOUND if the overlapped operation has
   * already completed. This is not a problem and we proceed normally. */
  if (!CancelIoEx(driver_handle, &sock_info->overlapped) &&
      GetLastError() != ERROR_NOT_FOUND)
    return_error(-1);

  sock_info->poll_status = _POLL_CANCELLED;
  sock_info->pending_events = 0;
  return 0;
}

ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket) {
  SOCKET base_socket;
  poll_group_t* poll_group;
  ep_sock_t* sock_info;

  if (socket == 0 || socket == INVALID_SOCKET)
    return_error(NULL, ERROR_INVALID_HANDLE);

  base_socket = ws_get_base_socket(socket);
  if (base_socket == INVALID_SOCKET)
    return NULL;

  poll_group = poll_group_acquire(port_info);
  if (poll_group == NULL)
    return NULL;

  sock_info = _ep_sock_alloc();
  if (sock_info == NULL)
    goto err1;

  memset(sock_info, 0, sizeof *sock_info);

  sock_info->base_socket = base_socket;
  sock_info->poll_group = poll_group;

  tree_node_init(&sock_info->tree_node);
  queue_node_init(&sock_info->queue_node);

  if (ep_port_register_socket_handle(port_info, sock_info, socket) < 0)
    goto err2;

  return sock_info;

err2:
  _ep_sock_free(sock_info);
err1:
  poll_group_release(poll_group);

  return NULL;
}

static int _ep_sock_delete(ep_port_t* port_info,
                           ep_sock_t* sock_info,
                           bool force) {
  if (!sock_info->delete_pending) {
    if (sock_info->poll_status == _POLL_PENDING)
      _ep_sock_cancel_poll(sock_info);

    ep_port_cancel_socket_update(port_info, sock_info);
    ep_port_unregister_socket_handle(port_info, sock_info);

    sock_info->delete_pending = true;
  }

  /* If the poll request still needs to complete, the ep_sock object can't
   * be free()d yet. `ep_sock_feed_event()` or `ep_port_close()` will take care
   * of this later. */
  if (force || sock_info->poll_status == _POLL_IDLE) {
    /* Free the sock_info now. */
    ep_port_remove_deleted_socket(port_info, sock_info);
    poll_group_release(sock_info->poll_group);
    _ep_sock_free(sock_info);
  } else {
    /* Free the socket later. */
    ep_port_add_deleted_socket(port_info, sock_info);
  }

  return 0;
}

void ep_sock_delete(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_delete(port_info, sock_info, false);
}

void ep_sock_force_delete(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_delete(port_info, sock_info, true);
}

int ep_sock_set_event(ep_port_t* port_info,
                      ep_sock_t* sock_info,
                      const struct epoll_event* ev) {
  /* EPOLLERR and EPOLLHUP are always reported, even when not requested by the
   * caller. However they are disabled after a event has been reported for a
   * socket for which the EPOLLONESHOT flag as set. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_info->user_events = events;
  sock_info->user_data = ev->data;

  if ((events & _KNOWN_EPOLL_EVENTS & ~sock_info->pending_events) != 0)
    ep_port_request_socket_update(port_info, sock_info);

  return 0;
}

static inline ULONG _epoll_events_to_afd_events(uint32_t epoll_events) {
  /* Always monitor for AFD_POLL_LOCAL_CLOSE, which is triggered when the
   * socket is closed with closesocket() or CloseHandle(). */
  DWORD afd_events = AFD_POLL_LOCAL_CLOSE;

  if (epoll_events & (EPOLLIN | EPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (epoll_events & (EPOLLPRI | EPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (epoll_events & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT;
  if (epoll_events & (EPOLLIN | EPOLLRDNORM | EPOLLRDHUP))
    afd_events |= AFD_POLL_DISCONNECT;
  if (epoll_events & EPOLLHUP)
    afd_events |= AFD_POLL_ABORT;
  if (epoll_events & EPOLLERR)
    afd_events |= AFD_POLL_CONNECT_FAIL;

  return afd_events;
}

static inline uint32_t _afd_events_to_epoll_events(ULONG afd_events) {
  uint32_t epoll_events = 0;

  if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
    epoll_events |= EPOLLIN | EPOLLRDNORM;
  if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
    epoll_events |= EPOLLPRI | EPOLLRDBAND;
  if (afd_events & (AFD_POLL_SEND | AFD_POLL_CONNECT))
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if (afd_events & AFD_POLL_DISCONNECT)
    epoll_events |= EPOLLIN | EPOLLRDNORM | EPOLLRDHUP;
  if (afd_events & AFD_POLL_ABORT)
    epoll_events |= EPOLLHUP;
  if (afd_events & AFD_POLL_CONNECT_FAIL)
    epoll_events |= EPOLLERR;

  return epoll_events;
}

int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  assert(!sock_info->delete_pending);

  if ((sock_info->poll_status == _POLL_PENDING) &&
      (sock_info->user_events & _KNOWN_EPOLL_EVENTS &
       ~sock_info->pending_events) == 0) {
    /* All the events the user is interested in are already being monitored by
     * the pending poll operation. It might spuriously complete because of an
     * event that we're no longer interested in; when that happens we'll submit
     * a new poll operation with the updated event mask. */

  } else if (sock_info->poll_status == _POLL_PENDING) {
    /* A poll operation is already pending, but it's not monitoring for all the
     * events that the user is interested in. Therefore, cancel the pending
     * poll operation; when we receive it's completion package, a new poll
     * operation will be submitted with the correct event mask. */
    if (_ep_sock_cancel_poll(sock_info) < 0)
      return -1;

  } else if (sock_info->poll_status == _POLL_CANCELLED) {
    /* The poll operation has already been cancelled, we're still waiting for
     * it to return. For now, there's nothing that needs to be done. */

  } else if (sock_info->poll_status == _POLL_IDLE) {
    /* No poll operation is pending; start one. */
    sock_info->poll_info.Exclusive = FALSE;
    sock_info->poll_info.NumberOfHandles = 1;
    sock_info->poll_info.Timeout.QuadPart = INT64_MAX;
    sock_info->poll_info.Handles[0].Handle = (HANDLE) sock_info->base_socket;
    sock_info->poll_info.Handles[0].Status = 0;
    sock_info->poll_info.Handles[0].Events =
        _epoll_events_to_afd_events(sock_info->user_events);

    memset(&sock_info->overlapped, 0, sizeof sock_info->overlapped);

    if (afd_poll(poll_group_get_socket(sock_info->poll_group),
                 &sock_info->poll_info,
                 &sock_info->overlapped) < 0) {
      switch (GetLastError()) {
        case ERROR_IO_PENDING:
          /* Overlapped poll operation in progress; this is expected. */
          break;
        case ERROR_INVALID_HANDLE:
          /* Socket closed; it'll be dropped from the epoll set. */
          return _ep_sock_delete(port_info, sock_info, false);
        default:
          /* Other errors are propagated to the caller. */
          return_error(-1);
      }
    }

    /* The poll request was successfully submitted. */
    sock_info->poll_status = _POLL_PENDING;
    sock_info->pending_events = sock_info->user_events;

  } else {
    /* Unreachable. */
    assert(false);
  }

  ep_port_cancel_socket_update(port_info, sock_info);
  return 0;
}

int ep_sock_feed_event(ep_port_t* port_info,
                       OVERLAPPED* overlapped,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = container_of(overlapped, ep_sock_t, overlapped);
  AFD_POLL_INFO* poll_info = &sock_info->poll_info;
  uint32_t epoll_events = 0;

  sock_info->poll_status = _POLL_IDLE;
  sock_info->pending_events = 0;

  if (sock_info->delete_pending) {
    /* Socket has been deleted earlier and can now be freed. */
    return _ep_sock_delete(port_info, sock_info, false);

  } else if ((NTSTATUS) overlapped->Internal == STATUS_CANCELLED) {
    /* The poll request was cancelled by CancelIoEx. */

  } else if (!NT_SUCCESS(overlapped->Internal)) {
    /* The overlapped request itself failed in an unexpected way. */
    epoll_events = EPOLLERR;

  } else if (poll_info->NumberOfHandles < 1) {
    /* This poll operation succeeded but didn't report any socket events. */

  } else if (poll_info->Handles[0].Events & AFD_POLL_LOCAL_CLOSE) {
    /* The poll operation reported that the socket was closed. */
    return _ep_sock_delete(port_info, sock_info, false);

  } else {
    /* Events related to our socket were reported. */
    epoll_events = _afd_events_to_epoll_events(poll_info->Handles[0].Events);
  }

  /* Requeue the socket so a new poll request will be submitted. */
  ep_port_request_socket_update(port_info, sock_info);

  /* Filter out events that the user didn't ask for. */
  epoll_events &= sock_info->user_events;

  /* Return if there are no epoll events to report. */
  if (epoll_events == 0)
    return 0;

  /* If the the socket has the EPOLLONESHOT flag set, unmonitor all events,
   * even EPOLLERR and EPOLLHUP. But always keep looking for closed sockets. */
  if (sock_info->user_events & EPOLLONESHOT)
    sock_info->user_events = 0;

  ev->data = sock_info->user_data;
  ev->events = epoll_events;
  return 1;
}

queue_node_t* ep_sock_to_queue_node(ep_sock_t* sock_info) {
  return &sock_info->queue_node;
}

ep_sock_t* ep_sock_from_tree_node(tree_node_t* tree_node) {
  return container_of(tree_node, ep_sock_t, tree_node);
}

tree_node_t* ep_sock_to_tree_node(ep_sock_t* sock_info) {
  return &sock_info->tree_node;
}

ep_sock_t* ep_sock_from_queue_node(queue_node_t* queue_node) {
  return container_of(queue_node, ep_sock_t, queue_node);
}
