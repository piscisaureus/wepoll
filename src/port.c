#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "queue.h"
#include "sock.h"
#include "thread-safe-tree.h"
#include "tree.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

#define PORT__MAX_ON_STACK_COMPLETIONS 256

static port_state_t* port__alloc(void) {
  port_state_t* port_state = malloc(sizeof *port_state);
  if (port_state == NULL)
    return_set_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  return port_state;
}

static void port__free(port_state_t* port) {
  assert(port != NULL);
  free(port);
}

static HANDLE port__create_iocp(void) {
  HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == NULL)
    return_map_error(NULL);

  return iocp;
}

port_state_t* port_new(HANDLE* iocp_out) {
  port_state_t* port_state;
  HANDLE iocp;

  port_state = port__alloc();
  if (port_state == NULL)
    goto err1;

  iocp = port__create_iocp();
  if (iocp == NULL)
    goto err2;

  memset(port_state, 0, sizeof *port_state);

  port_state->iocp = iocp;
  tree_init(&port_state->sock_tree);
  queue_init(&port_state->sock_update_queue);
  queue_init(&port_state->sock_deleted_queue);
  queue_init(&port_state->poll_group_queue);
  ts_tree_node_init(&port_state->handle_tree_node);
  InitializeCriticalSection(&port_state->lock);

  *iocp_out = iocp;
  return port_state;

err2:
  port__free(port_state);
err1:
  return NULL;
}

static int port__close_iocp(port_state_t* port_state) {
  HANDLE iocp = port_state->iocp;
  port_state->iocp = NULL;

  if (!CloseHandle(iocp))
    return_map_error(-1);

  return 0;
}

int port_close(port_state_t* port_state) {
  int result;

  EnterCriticalSection(&port_state->lock);
  result = port__close_iocp(port_state);
  LeaveCriticalSection(&port_state->lock);

  return result;
}

int port_delete(port_state_t* port_state) {
  tree_node_t* tree_node;
  queue_node_t* queue_node;

  /* At this point the IOCP port should have been closed. */
  assert(port_state->iocp == NULL);

  while ((tree_node = tree_root(&port_state->sock_tree)) != NULL) {
    sock_state_t* sock_state = sock_state_from_tree_node(tree_node);
    sock_force_delete(port_state, sock_state);
  }

  while ((queue_node = queue_first(&port_state->sock_deleted_queue)) != NULL) {
    sock_state_t* sock_state = sock_state_from_queue_node(queue_node);
    sock_force_delete(port_state, sock_state);
  }

  while ((queue_node = queue_first(&port_state->poll_group_queue)) != NULL) {
    poll_group_t* poll_group = poll_group_from_queue_node(queue_node);
    poll_group_delete(poll_group);
  }

  assert(queue_empty(&port_state->sock_update_queue));

  DeleteCriticalSection(&port_state->lock);

  port__free(port_state);

  return 0;
}

static int port__update_events(port_state_t* port_state) {
  queue_t* sock_update_queue = &port_state->sock_update_queue;

  /* Walk the queue, submitting new poll requests for every socket that needs
   * it. */
  while (!queue_empty(sock_update_queue)) {
    queue_node_t* queue_node = queue_first(sock_update_queue);
    sock_state_t* sock_state = sock_state_from_queue_node(queue_node);

    if (sock_update(port_state, sock_state) < 0)
      return -1;

    /* sock_update() removes the socket from the update queue. */
  }

  return 0;
}

static void port__update_events_if_polling(port_state_t* port_state) {
  if (port_state->active_poll_count > 0)
    port__update_events(port_state);
}

static int port__feed_events(port_state_t* port_state,
                             struct epoll_event* epoll_events,
                             OVERLAPPED_ENTRY* iocp_events,
                             DWORD iocp_event_count) {
  int epoll_event_count = 0;
  DWORD i;

  for (i = 0; i < iocp_event_count; i++) {
    OVERLAPPED* overlapped = iocp_events[i].lpOverlapped;
    struct epoll_event* ev = &epoll_events[epoll_event_count];

    epoll_event_count += sock_feed_event(port_state, overlapped, ev);
  }

  return epoll_event_count;
}

static int port__poll(port_state_t* port_state,
                      struct epoll_event* epoll_events,
                      OVERLAPPED_ENTRY* iocp_events,
                      DWORD maxevents,
                      DWORD timeout) {
  DWORD completion_count;

  if (port__update_events(port_state) < 0)
    return -1;

  port_state->active_poll_count++;

  LeaveCriticalSection(&port_state->lock);

  BOOL r = GetQueuedCompletionStatusEx(port_state->iocp,
                                       iocp_events,
                                       maxevents,
                                       &completion_count,
                                       timeout,
                                       FALSE);

  EnterCriticalSection(&port_state->lock);

  port_state->active_poll_count--;

  if (!r)
    return_map_error(-1);

  return port__feed_events(
      port_state, epoll_events, iocp_events, completion_count);
}

int port_wait(port_state_t* port_state,
              struct epoll_event* events,
              int maxevents,
              int timeout) {
  OVERLAPPED_ENTRY stack_iocp_events[PORT__MAX_ON_STACK_COMPLETIONS];
  OVERLAPPED_ENTRY* iocp_events;
  uint64_t due = 0;
  DWORD gqcs_timeout;
  int result;

  /* Check whether `maxevents` is in range. */
  if (maxevents <= 0)
    return_set_error(-1, ERROR_INVALID_PARAMETER);

  /* Decide whether the IOCP completion list can live on the stack, or allocate
   * memory for it on the heap. */
  if ((size_t) maxevents <= array_count(stack_iocp_events)) {
    iocp_events = stack_iocp_events;
  } else if ((iocp_events =
                  malloc((size_t) maxevents * sizeof *iocp_events)) == NULL) {
    iocp_events = stack_iocp_events;
    maxevents = array_count(stack_iocp_events);
  }

  /* Compute the timeout for GetQueuedCompletionStatus, and the wait end
   * time, if the user specified a timeout other than zero or infinite. */
  if (timeout > 0) {
    due = GetTickCount64() + (uint64_t) timeout;
    gqcs_timeout = (DWORD) timeout;
  } else if (timeout == 0) {
    gqcs_timeout = 0;
  } else {
    gqcs_timeout = INFINITE;
  }

  EnterCriticalSection(&port_state->lock);

  /* Dequeue completion packets until either at least one interesting event
   * has been discovered, or the timeout is reached. */
  for (;;) {
    uint64_t now;

    result = port__poll(
        port_state, events, iocp_events, (DWORD) maxevents, gqcs_timeout);
    if (result < 0 || result > 0)
      break; /* Result, error, or time-out. */

    if (timeout < 0)
      continue; /* When timeout is negative, never time out. */

    /* Update time. */
    now = GetTickCount64();

    /* Do not allow the due time to be in the past. */
    if (now >= due) {
      SetLastError(WAIT_TIMEOUT);
      break;
    }

    /* Recompute time-out argument for GetQueuedCompletionStatus. */
    gqcs_timeout = (DWORD)(due - now);
  }

  port__update_events_if_polling(port_state);

  LeaveCriticalSection(&port_state->lock);

  if (iocp_events != stack_iocp_events)
    free(iocp_events);

  if (result >= 0)
    return result;
  else if (GetLastError() == WAIT_TIMEOUT)
    return 0;
  else
    return -1;
}

static int port__ctl_add(port_state_t* port_state,
                         SOCKET sock,
                         struct epoll_event* ev) {
  sock_state_t* sock_state = sock_new(port_state, sock);
  if (sock_state == NULL)
    return -1;

  if (sock_set_event(port_state, sock_state, ev) < 0) {
    sock_delete(port_state, sock_state);
    return -1;
  }

  port__update_events_if_polling(port_state);

  return 0;
}

static int port__ctl_mod(port_state_t* port_state,
                         SOCKET sock,
                         struct epoll_event* ev) {
  sock_state_t* sock_state = port_find_socket(port_state, sock);
  if (sock_state == NULL)
    return -1;

  if (sock_set_event(port_state, sock_state, ev) < 0)
    return -1;

  port__update_events_if_polling(port_state);

  return 0;
}

static int port__ctl_del(port_state_t* port_state, SOCKET sock) {
  sock_state_t* sock_state = port_find_socket(port_state, sock);
  if (sock_state == NULL)
    return -1;

  sock_delete(port_state, sock_state);

  return 0;
}

static int port__ctl_op(port_state_t* port_state,
                        int op,
                        SOCKET sock,
                        struct epoll_event* ev) {
  switch (op) {
    case EPOLL_CTL_ADD:
      return port__ctl_add(port_state, sock, ev);
    case EPOLL_CTL_MOD:
      return port__ctl_mod(port_state, sock, ev);
    case EPOLL_CTL_DEL:
      return port__ctl_del(port_state, sock);
    default:
      return_set_error(-1, ERROR_INVALID_PARAMETER);
  }
}

int port_ctl(port_state_t* port_state,
             int op,
             SOCKET sock,
             struct epoll_event* ev) {
  int result;

  EnterCriticalSection(&port_state->lock);
  result = port__ctl_op(port_state, op, sock, ev);
  LeaveCriticalSection(&port_state->lock);

  return result;
}

int port_register_socket_handle(port_state_t* port_state,
                                sock_state_t* sock_state,
                                SOCKET socket) {
  if (tree_add(&port_state->sock_tree,
               sock_state_to_tree_node(sock_state),
               socket) < 0)
    return_set_error(-1, ERROR_ALREADY_EXISTS);
  return 0;
}

void port_unregister_socket_handle(port_state_t* port_state,
                                   sock_state_t* sock_state) {
  tree_del(&port_state->sock_tree, sock_state_to_tree_node(sock_state));
}

sock_state_t* port_find_socket(port_state_t* port_state, SOCKET socket) {
  tree_node_t* tree_node = tree_find(&port_state->sock_tree, socket);
  if (tree_node == NULL)
    return_set_error(NULL, ERROR_NOT_FOUND);
  return sock_state_from_tree_node(tree_node);
}

void port_request_socket_update(port_state_t* port_state,
                                sock_state_t* sock_state) {
  if (queue_enqueued(sock_state_to_queue_node(sock_state)))
    return;
  queue_append(&port_state->sock_update_queue,
               sock_state_to_queue_node(sock_state));
}

void port_cancel_socket_update(port_state_t* port_state,
                               sock_state_t* sock_state) {
  unused_var(port_state);
  if (!queue_enqueued(sock_state_to_queue_node(sock_state)))
    return;
  queue_remove(sock_state_to_queue_node(sock_state));
}

void port_add_deleted_socket(port_state_t* port_state,
                             sock_state_t* sock_state) {
  if (queue_enqueued(sock_state_to_queue_node(sock_state)))
    return;
  queue_append(&port_state->sock_deleted_queue,
               sock_state_to_queue_node(sock_state));
}

void port_remove_deleted_socket(port_state_t* port_state,
                                sock_state_t* sock_state) {
  unused_var(port_state);
  if (!queue_enqueued(sock_state_to_queue_node(sock_state)))
    return;
  queue_remove(sock_state_to_queue_node(sock_state));
}
