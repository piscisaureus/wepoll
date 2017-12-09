#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "queue.h"
#include "sock.h"
#include "tree.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

#define _PORT_MAX_ON_STACK_COMPLETIONS 256

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

static HANDLE _ep_port_create_iocp(void) {
  HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == NULL)
    return_error(NULL);

  return iocp;
}

ep_port_t* ep_port_new(HANDLE* iocp_out) {
  ep_port_t* port_info;
  HANDLE iocp;

  port_info = _ep_port_alloc();
  if (port_info == NULL)
    goto err1;

  iocp = _ep_port_create_iocp();
  if (iocp == NULL)
    goto err2;

  memset(port_info, 0, sizeof *port_info);

  port_info->iocp = iocp;
  queue_init(&port_info->sock_update_queue);
  queue_init(&port_info->sock_deleted_queue);
  tree_init(&port_info->sock_tree);
  reflock_tree_node_init(&port_info->handle_tree_node);
  InitializeCriticalSection(&port_info->lock);

  *iocp_out = iocp;
  return port_info;

err2:
  _ep_port_free(port_info);
err1:
  return NULL;
}

static int _ep_port_close_iocp(ep_port_t* port_info) {
  HANDLE iocp = port_info->iocp;
  port_info->iocp = NULL;

  if (!CloseHandle(iocp))
    return_error(-1);

  return 0;
}

int ep_port_close(ep_port_t* port_info) {
  int result;

  EnterCriticalSection(&port_info->lock);
  result = _ep_port_close_iocp(port_info);
  LeaveCriticalSection(&port_info->lock);

  return result;
}

int ep_port_delete(ep_port_t* port_info) {
  tree_node_t* tree_node;
  queue_node_t* queue_node;
  size_t i;

  EnterCriticalSection(&port_info->lock);

  if (port_info->iocp != NULL)
    _ep_port_close_iocp(port_info);

  while ((tree_node = tree_root(&port_info->sock_tree)) != NULL) {
    ep_sock_t* sock_info = container_of(tree_node, ep_sock_t, tree_node);
    ep_sock_force_delete(port_info, sock_info);
  }

  while ((queue_node = queue_first(&port_info->sock_deleted_queue)) != NULL) {
    ep_sock_t* sock_info = container_of(queue_node, ep_sock_t, queue_node);
    ep_sock_force_delete(port_info, sock_info);
  }

  for (i = 0; i < array_count(port_info->poll_group_allocators); i++) {
    poll_group_allocator_t* pga = port_info->poll_group_allocators[i];
    if (pga != NULL)
      poll_group_allocator_delete(pga);
  }

  LeaveCriticalSection(&port_info->lock);

  DeleteCriticalSection(&port_info->lock);

  _ep_port_free(port_info);

  return 0;
}

static int _ep_port_update_events(ep_port_t* port_info) {
  queue_t* sock_update_queue = &port_info->sock_update_queue;

  /* Walk the queue, submitting new poll requests for every socket that needs
   * it. */
  while (!queue_empty(sock_update_queue)) {
    queue_node_t* queue_node = queue_first(sock_update_queue);
    ep_sock_t* sock_info = container_of(queue_node, ep_sock_t, queue_node);

    if (ep_sock_update(port_info, sock_info) < 0)
      return -1;

    /* ep_sock_update() removes the socket from the update list if
     * successfull. */
  }

  return 0;
}

static void _ep_port_update_events_if_polling(ep_port_t* port_info) {
  if (port_info->active_poll_count > 0)
    _ep_port_update_events(port_info);
}

static int _ep_port_feed_events(ep_port_t* port_info,
                                struct epoll_event* epoll_events,
                                OVERLAPPED_ENTRY* iocp_events,
                                int iocp_event_count) {
  int epoll_event_count = 0;
  int i;

  for (i = 0; i < iocp_event_count; i++) {
    OVERLAPPED* overlapped = iocp_events[i].lpOverlapped;
    struct epoll_event* ev = &epoll_events[epoll_event_count];

    epoll_event_count += ep_sock_feed_event(port_info, overlapped, ev);
  }

  return epoll_event_count;
}

static int _ep_port_poll(ep_port_t* port_info,
                         struct epoll_event* epoll_events,
                         OVERLAPPED_ENTRY* iocp_events,
                         int maxevents,
                         DWORD timeout) {
  ULONG completion_count;

  if (_ep_port_update_events(port_info) < 0)
    return -1;

  port_info->active_poll_count++;

  LeaveCriticalSection(&port_info->lock);

  BOOL r = GetQueuedCompletionStatusEx(port_info->iocp,
                                       iocp_events,
                                       maxevents,
                                       &completion_count,
                                       timeout,
                                       FALSE);

  EnterCriticalSection(&port_info->lock);

  port_info->active_poll_count--;

  if (!r)
    return_error(-1);

  return _ep_port_feed_events(
      port_info, epoll_events, iocp_events, completion_count);
}

int ep_port_wait(ep_port_t* port_info,
                 struct epoll_event* events,
                 int maxevents,
                 int timeout) {
  OVERLAPPED_ENTRY stack_iocp_events[_PORT_MAX_ON_STACK_COMPLETIONS];
  OVERLAPPED_ENTRY* iocp_events;
  ULONGLONG due = 0;
  DWORD gqcs_timeout;
  int result;

  /* Check whether `maxevents` is in range. */
  if (maxevents <= 0)
    return_error(-1, ERROR_INVALID_PARAMETER);

  /* Decide whether the IOCP completion list can live on the stack, or allocate
   * memory for it on the heap. */
  if ((size_t) maxevents <= array_count(stack_iocp_events)) {
    iocp_events = stack_iocp_events;
  } else if ((iocp_events = malloc(maxevents * sizeof *iocp_events)) == NULL) {
    iocp_events = stack_iocp_events;
    maxevents = array_count(stack_iocp_events);
  }

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

  EnterCriticalSection(&port_info->lock);

  /* Dequeue completion packets until either at least one interesting event
   * has been discovered, or the timeout is reached.
   */
  do {
    ULONGLONG now;

    result =
        _ep_port_poll(port_info, events, iocp_events, maxevents, gqcs_timeout);
    if (result < 0 || result > 0)
      break; /* Result, error, or time-out. */

    if (timeout < 0)
      continue; /* _ep_port_wait() never times out. */

    /* Check for time-out. */
    now = GetTickCount64();
    if (now >= due)
      break;

    /* Recompute timeout. */
    gqcs_timeout = (DWORD)(due - now);
  } while (gqcs_timeout > 0);

  _ep_port_update_events_if_polling(port_info);

  LeaveCriticalSection(&port_info->lock);

  if (iocp_events != stack_iocp_events)
    free(iocp_events);

  if (result >= 0)
    return result;
  else if (GetLastError() == WAIT_TIMEOUT)
    return 0;
  else
    return -1;
}

static int _ep_port_ctl_add(ep_port_t* port_info,
                            SOCKET sock,
                            struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0) {
    ep_sock_delete(port_info, sock_info);
    return -1;
  }

  _ep_port_update_events_if_polling(port_info);

  return 0;
}

static int _ep_port_ctl_mod(ep_port_t* port_info,
                            SOCKET sock,
                            struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0)
    return -1;

  _ep_port_update_events_if_polling(port_info);

  return 0;
}

static int _ep_port_ctl_del(ep_port_t* port_info, SOCKET sock) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  ep_sock_delete(port_info, sock_info);

  return 0;
}

static int _ep_port_ctl_op(ep_port_t* port_info,
                           int op,
                           SOCKET sock,
                           struct epoll_event* ev) {
  switch (op) {
    case EPOLL_CTL_ADD:
      return _ep_port_ctl_add(port_info, sock, ev);
    case EPOLL_CTL_MOD:
      return _ep_port_ctl_mod(port_info, sock, ev);
    case EPOLL_CTL_DEL:
      return _ep_port_ctl_del(port_info, sock);
    default:
      return_error(-1, ERROR_INVALID_PARAMETER);
  }
}

int ep_port_ctl(ep_port_t* port_info,
                int op,
                SOCKET sock,
                struct epoll_event* ev) {
  int result;

  EnterCriticalSection(&port_info->lock);
  result = _ep_port_ctl_op(port_info, op, sock, ev);
  LeaveCriticalSection(&port_info->lock);

  return result;
}

int ep_port_register_socket_handle(ep_port_t* port_info,
                                   ep_sock_t* sock_info,
                                   SOCKET socket) {
  if (tree_add(&port_info->sock_tree, &sock_info->tree_node, socket) < 0)
    return_error(-1, ERROR_ALREADY_EXISTS);
  return 0;
}

int ep_port_unregister_socket_handle(ep_port_t* port_info,
                                     ep_sock_t* sock_info) {
  if (tree_del(&port_info->sock_tree, &sock_info->tree_node) < 0)
    return_error(-1, ERROR_NOT_FOUND);
  return 0;
}

ep_sock_t* ep_port_find_socket(ep_port_t* port_info, SOCKET socket) {
  ep_sock_t* sock_info = safe_container_of(
      tree_find(&port_info->sock_tree, socket), ep_sock_t, tree_node);
  if (sock_info == NULL)
    return_error(NULL, ERROR_NOT_FOUND);
  return sock_info;
}

static poll_group_allocator_t* _ep_port_get_poll_group_allocator(
    ep_port_t* port_info,
    size_t protocol_id,
    const WSAPROTOCOL_INFOW* protocol_info) {
  poll_group_allocator_t** pga;

  assert(protocol_id < array_count(port_info->poll_group_allocators));

  pga = &port_info->poll_group_allocators[protocol_id];
  if (*pga == NULL)
    *pga = poll_group_allocator_new(port_info, protocol_info);

  return *pga;
}

poll_group_t* ep_port_acquire_poll_group(
    ep_port_t* port_info,
    size_t protocol_id,
    const WSAPROTOCOL_INFOW* protocol_info) {
  poll_group_allocator_t* pga =
      _ep_port_get_poll_group_allocator(port_info, protocol_id, protocol_info);
  return poll_group_acquire(pga);
}

void ep_port_release_poll_group(ep_port_t* port_info,
                                poll_group_t* poll_group) {
  unused_var(port_info);
  poll_group_release(poll_group);
}

void ep_port_request_socket_update(ep_port_t* port_info,
                                   ep_sock_t* sock_info) {
  if (queue_enqueued(&sock_info->queue_node))
    return;
  queue_append(&port_info->sock_update_queue, &sock_info->queue_node);
}

void ep_port_cancel_socket_update(ep_port_t* port_info, ep_sock_t* sock_info) {
  unused_var(port_info);
  if (!queue_enqueued(&sock_info->queue_node))
    return;
  queue_remove(&sock_info->queue_node);
}

void ep_port_add_deleted_socket(ep_port_t* port_info, ep_sock_t* sock_info) {
  if (queue_enqueued(&sock_info->queue_node))
    return;
  queue_append(&port_info->sock_deleted_queue, &sock_info->queue_node);
}

void ep_port_remove_deleted_socket(ep_port_t* port_info,
                                   ep_sock_t* sock_info) {
  unused_var(port_info);
  if (!queue_enqueued(&sock_info->queue_node))
    return;
  queue_remove(&sock_info->queue_node);
}
