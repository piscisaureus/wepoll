#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "sock.h"
#include "wepoll.h"

#define _KNOWN_EPOLL_EVENTS                                            \
  (EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | \
   EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLRDHUP)

typedef struct _poll_req {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
} _poll_req_t;

typedef enum _poll_status {
  _POLL_IDLE = 0,
  _POLL_PENDING,
  _POLL_CANCELLED
} _poll_status_t;

typedef struct _ep_sock_private {
  ep_sock_t pub;
  _poll_req_t poll_req;
  poll_group_t* poll_group;
  SOCKET afd_socket;
  epoll_data_t user_data;
  uint32_t user_events;
  uint32_t pending_events;
  _poll_status_t poll_status;
  bool delete_pending;
} _ep_sock_private_t;

static DWORD _epoll_events_to_afd_events(uint32_t epoll_events) {
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

static uint32_t _afd_events_to_epoll_events(DWORD afd_events) {
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

static int _poll_req_submit(_poll_req_t* poll_req,
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

static int _poll_req_cancel(_poll_req_t* poll_req, SOCKET driver_socket) {
  OVERLAPPED* overlapped = &poll_req->overlapped;

  if (!CancelIoEx((HANDLE) driver_socket, overlapped)) {
    DWORD error = GetLastError();
    if (error == ERROR_NOT_FOUND)
      return 0; /* Already completed or canceled. */
    else
      return_error(-1);
  }

  return 0;
}

static void _poll_req_complete(const _poll_req_t* poll_req,
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

static inline _ep_sock_private_t* _ep_sock_private(ep_sock_t* sock_info) {
  return container_of(sock_info, _ep_sock_private_t, pub);
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

static int _ep_sock_cancel_poll(_ep_sock_private_t* sock_private) {
  assert(sock_private->poll_status == _POLL_PENDING);

  if (_poll_req_cancel(&sock_private->poll_req,
                       poll_group_get_socket(sock_private->poll_group)) < 0)
    return -1;

  sock_private->poll_status = _POLL_CANCELLED;
  sock_private->pending_events = 0;

  return 0;
}

ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket) {
  SOCKET afd_socket;
  ssize_t protocol_id;
  WSAPROTOCOL_INFOW protocol_info;
  poll_group_t* poll_group;
  _ep_sock_private_t* sock_private;

  if (socket == 0 || socket == INVALID_SOCKET)
    return_error(NULL, ERROR_INVALID_HANDLE);

  protocol_id = afd_get_protocol(socket, &afd_socket, &protocol_info);
  if (protocol_id < 0)
    return NULL;

  poll_group =
      ep_port_acquire_poll_group(port_info, protocol_id, &protocol_info);
  if (poll_group == NULL)
    return NULL;

  sock_private = _ep_sock_alloc();
  if (sock_private == NULL)
    goto err1;

  memset(sock_private, 0, sizeof *sock_private);

  sock_private->afd_socket = afd_socket;
  sock_private->poll_group = poll_group;

  tree_node_init(&sock_private->pub.tree_node);
  queue_node_init(&sock_private->pub.queue_node);

  if (ep_port_register_socket_handle(port_info, &sock_private->pub, socket) <
      0)
    goto err2;

  return &sock_private->pub;

err2:
  _ep_sock_free(sock_private);
err1:
  ep_port_release_poll_group(port_info, poll_group);

  return NULL;
}

static void _ep_sock_delete(ep_port_t* port_info,
                            ep_sock_t* sock_info,
                            bool force) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  if (!sock_private->delete_pending) {
    if (sock_private->poll_status == _POLL_PENDING)
      _ep_sock_cancel_poll(sock_private);

    ep_port_cancel_socket_update(port_info, sock_info);
    ep_port_unregister_socket_handle(port_info, sock_info);

    sock_private->delete_pending = true;
  }

  /* If the poll request still needs to complete, the ep_sock object can't
   * be free()d yet. `ep_sock_feed_event()` or `ep_port_close()` will take care
   * of this later. */
  if (force || sock_private->poll_status == _POLL_IDLE) {
    /* Free the sock_info now. */
    ep_port_remove_deleted_socket(port_info, sock_info);
    ep_port_release_poll_group(port_info, sock_private->poll_group);
    _ep_sock_free(sock_private);
  } else {
    /* Free the socket later. */
    ep_port_add_deleted_socket(port_info, sock_info);
  }
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
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  /* EPOLLERR and EPOLLHUP are always reported, even when not requested by the
   * caller. However they are disabled after a event has been reported for a
   * socket for which the EPOLLONESHOT flag as set. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_private->user_events = events;
  sock_private->user_data = ev->data;

  if ((events & _KNOWN_EPOLL_EVENTS & ~sock_private->pending_events) != 0)
    ep_port_request_socket_update(port_info, sock_info);

  return 0;
}

int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);
  bool socket_closed = false;

  assert(!sock_private->delete_pending);
  if ((sock_private->poll_status == _POLL_PENDING) &&
      (sock_private->user_events & _KNOWN_EPOLL_EVENTS &
       ~sock_private->pending_events) == 0) {
    /* All the events the user is interested in are already being monitored
     * by the pending poll request. It might spuriously complete because of an
     * event that we're no longer interested in; if that happens we just
     * submit another poll request with the right event mask. */

  } else if (sock_private->poll_status == _POLL_PENDING) {
    /* A poll request is already pending, but it's not monitoring for all the
     * events that the user is interested in. Cancel the pending poll request;
     * when it completes it will be submitted again with the correct event
     * mask. */
    if (_ep_sock_cancel_poll(sock_private) < 0)
      return -1;

  } else if (sock_private->poll_status == _POLL_CANCELLED) {
    /* The poll request has already been cancelled, we're still waiting for it
     * to return. For now, there's nothing that needs to be done. */

  } else if (sock_private->poll_status == _POLL_IDLE) {
    SOCKET driver_socket = poll_group_get_socket(sock_private->poll_group);

    if (_poll_req_submit(&sock_private->poll_req,
                         sock_private->user_events,
                         sock_private->afd_socket,
                         driver_socket) < 0) {
      if (GetLastError() == ERROR_INVALID_HANDLE)
        /* The socket is broken. It will be dropped from the epoll set. */
        socket_closed = true;
      else
        /* Another error occurred, which is propagated to the caller. */
        return -1;

    } else {
      /* The poll request was successfully submitted. */
      sock_private->poll_status = _POLL_PENDING;
      sock_private->pending_events = sock_private->user_events;
    }
  } else {
    /* Unreachable. */
    assert(false);
  }

  ep_port_cancel_socket_update(port_info, sock_info);

  /* If we saw an ERROR_INVALID_HANDLE error, drop the socket. */
  if (socket_closed)
    ep_sock_delete(port_info, sock_info);

  return 0;
}

int ep_sock_feed_event(ep_port_t* port_info,
                       OVERLAPPED* overlapped,
                       struct epoll_event* ev) {
  _ep_sock_private_t* sock_private =
      container_of(overlapped, _ep_sock_private_t, poll_req.overlapped);
  ep_sock_t* sock_info = &sock_private->pub;
  uint32_t epoll_events;
  bool socket_closed;
  int ev_count = 0;

  sock_private->poll_status = _POLL_IDLE;
  sock_private->pending_events = 0;

  if (sock_private->delete_pending) {
    /* Ignore completion for overlapped poll operation if the socket is pending
     * deletion; instead, delete the socket. */
    ep_sock_delete(port_info, sock_info);
    return 0;
  }

  _poll_req_complete(&sock_private->poll_req, &epoll_events, &socket_closed);

  /* Filter events that the user didn't ask for. */
  epoll_events &= sock_private->user_events;

  /* Clear the event mask if EPOLLONESHOT is set and there are any events
   * to report. */
  if (epoll_events != 0 && (sock_private->user_events & EPOLLONESHOT))
    sock_private->user_events = 0;

  /* Fill the ev structure if there are any events to report. */
  if (epoll_events != 0) {
    ev->data = sock_private->user_data;
    ev->events = epoll_events;
    ev_count = 1;
  }

  if (socket_closed)
    /* Drop the socket from the epoll set. */
    ep_sock_delete(port_info, sock_info);
  else
    /* Put the socket back onto the attention list so a new poll request will
     * be submitted. */
    ep_port_request_socket_update(port_info, sock_info);

  return ev_count;
}
