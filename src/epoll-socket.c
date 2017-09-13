#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "poll-request.h"
#include "port.h"

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

#define _EP_EVENT_MASK 0xffff

#define _EP_SOCK_DELETED 0x2

typedef struct _ep_sock_private {
  ep_sock_t pub;
  SOCKET afd_socket;
  SOCKET driver_socket;
  epoll_data_t user_data;
  poll_req_t* latest_poll_req;
  uint32_t user_events;
  uint32_t latest_poll_req_events;
  uint32_t poll_req_count;
  uint32_t flags;
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
                                SOCKET* driver_socket_out) {
  SOCKET afd_socket, driver_socket;
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

  driver_socket = ep_port_get_driver_socket(port_info, afd_socket);
  if (driver_socket == INVALID_SOCKET)
    return -1;

  *afd_socket_out = afd_socket;
  *driver_socket_out = driver_socket;

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
                           &sock_private->driver_socket) < 0)
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

  _ep_sock_maybe_free(sock_private);
}

ep_sock_t* ep_sock_find(tree_t* tree, SOCKET socket) {
  tree_node_t* tree_node = tree_find(tree, socket);
  if (tree_node == NULL)
    return NULL;

  return container_of(tree, ep_sock_t, tree_node);
}

void ep_sock_register_poll_req(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  assert(!_ep_sock_is_deleted(sock_private));

  ep_port_add_req(port_info);
  sock_private->poll_req_count++;
}

void ep_sock_unregister_poll_req(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);

  ep_port_del_req(port_info);
  sock_private->poll_req_count--;

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
  return poll_req == sock_private->latest_poll_req;
}

static inline void _clear_latest_poll_req(_ep_sock_private_t* sock_private) {
  sock_private->latest_poll_req = NULL;
  sock_private->latest_poll_req_events = 0;
}

static inline void _set_latest_poll_req(_ep_sock_private_t* sock_private,
                                        poll_req_t* poll_req,
                                        uint32_t epoll_events) {
  sock_private->latest_poll_req = poll_req;
  sock_private->latest_poll_req_events = epoll_events;
}

static int _ep_submit_poll_req(ep_port_t* port_info,
                               _ep_sock_private_t* sock_private) {
  poll_req_t* poll_req;
  uint32_t epoll_events = sock_private->user_events;

  poll_req = poll_req_new(port_info, &sock_private->pub);
  if (poll_req == NULL)
    return -1;

  if (poll_req_submit(poll_req,
                      epoll_events,
                      sock_private->afd_socket,
                      sock_private->driver_socket) < 0) {
    poll_req_delete(port_info, &sock_private->pub, poll_req);
    return -1;
  }

  _set_latest_poll_req(sock_private, poll_req, epoll_events);

  return 0;
}

int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  _ep_sock_private_t* sock_private = _ep_sock_private(sock_info);
  bool broken = false;

  assert(ep_port_is_socket_update_pending(port_info, sock_info));

  /* Check if there are events registered that are not yet submitted. In
   * that case we need to submit another req.
   */
  if ((sock_private->user_events & _EP_EVENT_MASK &
       ~sock_private->latest_poll_req_events) == 0)
    /* All the events the user is interested in are already being monitored
     * by the latest poll request. */
    goto done;

  if (_ep_submit_poll_req(port_info, sock_private) < 0) {
    if (GetLastError() == ERROR_INVALID_HANDLE)
      /* The socket is broken. It will be dropped from the epoll set. */
      broken = true;
    else
      /* Another error occurred, which is propagated to the caller. */
      return -1;
  }

done:
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

  /* Drop the socket if the EPOLLONESHOT flag is set and there are any events
   * to report. */
  if (epoll_events != 0 && (sock_private->user_events & EPOLLONESHOT))
    drop_socket = true;

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
