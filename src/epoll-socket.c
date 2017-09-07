#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "poll-request.h"
#include "port-data.h"

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

static inline ep_sock_t* _ep_sock_alloc(void) {
  ep_sock_t* sock_info = malloc(sizeof *sock_info);
  if (sock_info == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return sock_info;
}

static inline void _ep_sock_free(ep_sock_t* sock_info) {
  free(sock_info);
}

ep_sock_t* ep_sock_new(_ep_port_data_t* port_data) {
  ep_sock_t* sock_info = _ep_sock_alloc();
  if (sock_info == NULL)
    return NULL;

  (void) port_data;

  memset(sock_info, 0, sizeof *sock_info);
  handle_tree_entry_init(&sock_info->tree_entry);
  QUEUE_INIT(&sock_info->queue_entry);

  return sock_info;
}

int ep_sock_delete(_ep_port_data_t* port_data, ep_sock_t* sock_info) {
  /* Remove SOCKET -> ep_sock mapping from port. */
  if (!(sock_info->flags & _EP_SOCK_DELETED))
    _ep_port_del_socket(port_data, &sock_info->tree_entry);

  sock_info->flags |= _EP_SOCK_DELETED;

  /* Remove from attention list. */
  if (sock_info->flags & _EP_SOCK_LISTED) {
    QUEUE_REMOVE(&sock_info->queue_entry);
    sock_info->flags &= ~_EP_SOCK_LISTED;
  }

  /* The socket may still have pending overlapped requests that have yet to be
   * reported by the completion port. If that's the case the structure can't be
   * free'd yet; epoll_wait() will do it as it dequeues those requests.
   */
  if (sock_info->poll_req_count == 0)
    _ep_sock_free(sock_info);

  return 0;
}

static inline bool _ep_sock_delete_pending(ep_sock_t* sock_info) {
  return sock_info->flags & _EP_SOCK_DELETED;
}

void ep_sock_register_poll_req(_ep_port_data_t* port_data,
                               ep_sock_t* sock_info) {
  assert(!_ep_sock_delete_pending(sock_info));
  sock_info->poll_req_count++;
  port_data->poll_req_count++;
}

void ep_sock_unregister_poll_req(_ep_port_data_t* port_data,
                                 ep_sock_t* sock_info) {
  sock_info->poll_req_count--;
  port_data->poll_req_count--;

  if (_ep_sock_delete_pending(sock_info) && sock_info->poll_req_count == 0)
    ep_sock_delete(port_data, sock_info);
}

static int _get_related_sockets(_ep_port_data_t* port_data,
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

  driver_socket = _ep_get_driver_socket(port_data, afd_socket);
  if (driver_socket == INVALID_SOCKET)
    return -1;

  *afd_socket_out = afd_socket;
  *driver_socket_out = driver_socket;

  return 0;
}

int ep_sock_set_socket(_ep_port_data_t* port_data,
                       ep_sock_t* sock_info,
                       SOCKET socket) {
  if (socket == 0 || socket == INVALID_SOCKET)
    return_error(-1, ERROR_INVALID_HANDLE);
  if (sock_info->afd_socket != 0)
    return_error(-1, ERROR_ALREADY_ASSIGNED);

  if (_get_related_sockets(port_data,
                           socket,
                           &sock_info->afd_socket,
                           &sock_info->driver_socket) < 0)
    return -1;

  if (_ep_port_add_socket(port_data, &sock_info->tree_entry, socket) < 0)
    return -1;

  return 0;
}

int ep_sock_set_event(_ep_port_data_t* port_data,
                      ep_sock_t* sock_info,
                      const struct epoll_event* ev) {
  /* EPOLLERR and EPOLLHUP are always reported, even when no sollicited. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_info->user_events = events;
  sock_info->user_data = ev->data;

  if (events & _EP_EVENT_MASK & ~(sock_info->latest_poll_req_events)) {
    /* Add to attention list, if not already added. */
    if (!(sock_info->flags & _EP_SOCK_LISTED)) {
      QUEUE_INSERT_TAIL(&port_data->update_queue, &sock_info->queue_entry);
      sock_info->flags |= _EP_SOCK_LISTED;
    }
  }

  return 0;
}

static inline bool _is_latest_poll_req(ep_sock_t* sock_info,
                                       poll_req_t* poll_req) {
  return poll_req == sock_info->latest_poll_req;
}

static inline void _clear_latest_poll_req(ep_sock_t* sock_info) {
  sock_info->latest_poll_req = NULL;
  sock_info->latest_poll_req_events = 0;
}

static inline void _set_latest_poll_req(ep_sock_t* sock_info,
                                        poll_req_t* poll_req,
                                        uint32_t epoll_events) {
  sock_info->latest_poll_req = poll_req;
  sock_info->latest_poll_req_events = epoll_events;
}

static int _ep_submit_poll_req(_ep_port_data_t* port_data,
                               ep_sock_t* sock_info) {
  poll_req_t* poll_req;
  uint32_t epoll_events = sock_info->user_events;

  poll_req = poll_req_new(port_data, sock_info);
  if (poll_req == NULL)
    return -1;

  if (poll_req_submit(poll_req,
                      epoll_events,
                      sock_info->afd_socket,
                      sock_info->driver_socket) < 0) {
    poll_req_delete(port_data, sock_info, poll_req);
    return -1;
  }

  _set_latest_poll_req(sock_info, poll_req, epoll_events);

  return 0;
}

int ep_sock_update(_ep_port_data_t* port_data, ep_sock_t* sock_info) {
  bool broken = false;

  assert(sock_info->flags & _EP_SOCK_LISTED);

  /* Check if there are events registered that are not yet submitted. In
   * that case we need to submit another req.
   */
  if ((sock_info->user_events & _EP_EVENT_MASK &
       ~sock_info->latest_poll_req_events) == 0)
    /* All the events the user is interested in are already being monitored
     * by the latest poll request. */
    goto done;

  if (_ep_submit_poll_req(port_data, sock_info) < 0) {
    if (GetLastError() == ERROR_INVALID_HANDLE)
      /* The socket is broken. It will be dropped from the epoll set. */
      broken = true;
    else
      /* Another error occurred, which is propagated to the caller. */
      return -1;
  }

done:
  /* Remove the socket from the update queue. */
  QUEUE_REMOVE(&sock_info->queue_entry);
  sock_info->flags &= ~_EP_SOCK_LISTED;

  /* If we saw an ERROR_INVALID_HANDLE error, drop the socket. */
  if (broken)
    ep_sock_delete(port_data, sock_info);

  return 0;
}

int ep_sock_feed_event(_ep_port_data_t* port_data,
                       poll_req_t* poll_req,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = poll_req_get_sock_data(poll_req);

  uint32_t epoll_events;
  bool drop_socket;
  int ev_count = 0;

  if (_ep_sock_delete_pending(sock_info) ||
      !_is_latest_poll_req(sock_info, poll_req)) {
    /* Ignore completion for overlapped poll operation if it isn't
     * the the most recently posted one, or if the socket has been
     * deleted. */
    poll_req_delete(port_data, sock_info, poll_req);
    return 0;
  }

  _clear_latest_poll_req(sock_info);

  poll_req_complete(poll_req, &epoll_events, &drop_socket);

  /* Filter events that the user didn't ask for. */
  epoll_events &= sock_info->user_events;

  /* Drop the socket if the EPOLLONESHOT flag is set and there are any events
   * to report. */
  if (epoll_events != 0 && (sock_info->user_events & EPOLLONESHOT))
    drop_socket = true;

  /* Fill the ev structure if there are any events to report. */
  if (epoll_events != 0) {
    ev->data = sock_info->user_data;
    ev->events = epoll_events;
    ev_count = 1;
  }

  poll_req_delete(port_data, sock_info, poll_req);

  if (drop_socket)
    /* Drop the socket from the epoll set. */
    ep_sock_delete(port_data, sock_info);
  else
    /* Put the socket back onto the attention list so a new poll request will
     * be submitted. */
    ATTN_LIST_ADD(port_data, sock_info);

  return ev_count;
}

ep_sock_t* ep_sock_from_tree_entry(handle_tree_entry_t* tree_entry) {
  assert(tree_entry != NULL);
  return container_of(tree_entry, ep_sock_t, tree_entry);
}
