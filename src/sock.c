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

static const uint32_t SOCK__KNOWN_EPOLL_EVENTS =
    EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM |
    EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG | EPOLLRDHUP;

typedef enum sock__poll_status {
  SOCK__POLL_IDLE = 0,
  SOCK__POLL_PENDING,
  SOCK__POLL_CANCELLED
} sock__poll_status_t;

typedef struct sock_state {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  queue_node_t queue_node;
  tree_node_t tree_node;
  poll_group_t* poll_group;
  SOCKET base_socket;
  epoll_data_t user_data;
  uint32_t user_events;
  uint32_t pending_events;
  sock__poll_status_t poll_status;
  bool delete_pending;
} sock_state_t;

static inline sock_state_t* sock__alloc(void) {
  sock_state_t* sock_state = malloc(sizeof *sock_state);
  if (sock_state == NULL)
    return_set_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return sock_state;
}

static inline void sock__free(sock_state_t* sock_state) {
  free(sock_state);
}

static int sock__cancel_poll(sock_state_t* sock_state) {
  HANDLE afd_helper_handle =
      poll_group_get_afd_helper_handle(sock_state->poll_group);
  assert(sock_state->poll_status == SOCK__POLL_PENDING);

  /* CancelIoEx() may fail with ERROR_NOT_FOUND if the overlapped operation has
   * already completed. This is not a problem and we proceed normally. */
  if (!HasOverlappedIoCompleted(&sock_state->overlapped) &&
      !CancelIoEx(afd_helper_handle, &sock_state->overlapped) &&
      GetLastError() != ERROR_NOT_FOUND)
    return_map_error(-1);

  sock_state->poll_status = SOCK__POLL_CANCELLED;
  sock_state->pending_events = 0;
  return 0;
}

sock_state_t* sock_new(port_state_t* port_state, SOCKET socket) {
  SOCKET base_socket;
  poll_group_t* poll_group;
  sock_state_t* sock_state;

  if (socket == 0 || socket == INVALID_SOCKET)
    return_set_error(NULL, ERROR_INVALID_HANDLE);

  base_socket = ws_get_base_socket(socket);
  if (base_socket == INVALID_SOCKET)
    return NULL;

  poll_group = poll_group_acquire(port_state);
  if (poll_group == NULL)
    return NULL;

  sock_state = sock__alloc();
  if (sock_state == NULL)
    goto err1;

  memset(sock_state, 0, sizeof *sock_state);

  sock_state->base_socket = base_socket;
  sock_state->poll_group = poll_group;

  tree_node_init(&sock_state->tree_node);
  queue_node_init(&sock_state->queue_node);

  if (port_register_socket_handle(port_state, sock_state, socket) < 0)
    goto err2;

  return sock_state;

err2:
  sock__free(sock_state);
err1:
  poll_group_release(poll_group);

  return NULL;
}

static int sock__delete(port_state_t* port_state,
                        sock_state_t* sock_state,
                        bool force) {
  if (!sock_state->delete_pending) {
    if (sock_state->poll_status == SOCK__POLL_PENDING)
      sock__cancel_poll(sock_state);

    port_cancel_socket_update(port_state, sock_state);
    port_unregister_socket_handle(port_state, sock_state);

    sock_state->delete_pending = true;
  }

  /* If the poll request still needs to complete, the sock_state object can't
   * be free()d yet. `sock_feed_event()` or `port_close()` will take care
   * of this later. */
  if (force || sock_state->poll_status == SOCK__POLL_IDLE) {
    /* Free the sock_state now. */
    port_remove_deleted_socket(port_state, sock_state);
    poll_group_release(sock_state->poll_group);
    sock__free(sock_state);
  } else {
    /* Free the socket later. */
    port_add_deleted_socket(port_state, sock_state);
  }

  return 0;
}

void sock_delete(port_state_t* port_state, sock_state_t* sock_state) {
  sock__delete(port_state, sock_state, false);
}

void sock_force_delete(port_state_t* port_state, sock_state_t* sock_state) {
  sock__delete(port_state, sock_state, true);
}

int sock_set_event(port_state_t* port_state,
                   sock_state_t* sock_state,
                   const struct epoll_event* ev) {
  /* EPOLLERR and EPOLLHUP are always reported, even when not requested by the
   * caller. However they are disabled after a event has been reported for a
   * socket for which the EPOLLONESHOT flag as set. */
  uint32_t events = ev->events | EPOLLERR | EPOLLHUP;

  sock_state->user_events = events;
  sock_state->user_data = ev->data;

  if ((events & SOCK__KNOWN_EPOLL_EVENTS & ~sock_state->pending_events) != 0)
    port_request_socket_update(port_state, sock_state);

  return 0;
}

static inline DWORD sock__epoll_events_to_afd_events(uint32_t epoll_events) {
  /* Always monitor for AFD_POLL_LOCAL_CLOSE, which is triggered when the
   * socket is closed with closesocket() or CloseHandle(). */
  DWORD afd_events = AFD_POLL_LOCAL_CLOSE;

  if (epoll_events & (EPOLLIN | EPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (epoll_events & (EPOLLPRI | EPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (epoll_events & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    afd_events |= AFD_POLL_SEND;
  if (epoll_events & (EPOLLIN | EPOLLRDNORM | EPOLLRDHUP))
    afd_events |= AFD_POLL_DISCONNECT;
  if (epoll_events & EPOLLHUP)
    afd_events |= AFD_POLL_ABORT;
  if (epoll_events & EPOLLERR)
    afd_events |= AFD_POLL_CONNECT_FAIL;

  return afd_events;
}

static inline uint32_t sock__afd_events_to_epoll_events(DWORD afd_events) {
  uint32_t epoll_events = 0;

  if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
    epoll_events |= EPOLLIN | EPOLLRDNORM;
  if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
    epoll_events |= EPOLLPRI | EPOLLRDBAND;
  if (afd_events & AFD_POLL_SEND)
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if (afd_events & AFD_POLL_DISCONNECT)
    epoll_events |= EPOLLIN | EPOLLRDNORM | EPOLLRDHUP;
  if (afd_events & AFD_POLL_ABORT)
    epoll_events |= EPOLLHUP;
  if (afd_events & AFD_POLL_CONNECT_FAIL)
    /* Linux reports all these events after connect() has failed. */
    epoll_events |=
        EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDNORM | EPOLLWRNORM | EPOLLRDHUP;

  return epoll_events;
}

int sock_update(port_state_t* port_state, sock_state_t* sock_state) {
  assert(!sock_state->delete_pending);

  if ((sock_state->poll_status == SOCK__POLL_PENDING) &&
      (sock_state->user_events & SOCK__KNOWN_EPOLL_EVENTS &
       ~sock_state->pending_events) == 0) {
    /* All the events the user is interested in are already being monitored by
     * the pending poll operation. It might spuriously complete because of an
     * event that we're no longer interested in; when that happens we'll submit
     * a new poll operation with the updated event mask. */

  } else if (sock_state->poll_status == SOCK__POLL_PENDING) {
    /* A poll operation is already pending, but it's not monitoring for all the
     * events that the user is interested in. Therefore, cancel the pending
     * poll operation; when we receive it's completion package, a new poll
     * operation will be submitted with the correct event mask. */
    if (sock__cancel_poll(sock_state) < 0)
      return -1;

  } else if (sock_state->poll_status == SOCK__POLL_CANCELLED) {
    /* The poll operation has already been cancelled, we're still waiting for
     * it to return. For now, there's nothing that needs to be done. */

  } else if (sock_state->poll_status == SOCK__POLL_IDLE) {
    /* No poll operation is pending; start one. */
    sock_state->poll_info.Exclusive = FALSE;
    sock_state->poll_info.NumberOfHandles = 1;
    sock_state->poll_info.Timeout.QuadPart = INT64_MAX;
    sock_state->poll_info.Handles[0].Handle = (HANDLE) sock_state->base_socket;
    sock_state->poll_info.Handles[0].Status = 0;
    sock_state->poll_info.Handles[0].Events =
        sock__epoll_events_to_afd_events(sock_state->user_events);

    memset(&sock_state->overlapped, 0, sizeof sock_state->overlapped);

    if (afd_poll(poll_group_get_afd_helper_handle(sock_state->poll_group),
                 &sock_state->poll_info,
                 &sock_state->overlapped) < 0) {
      switch (GetLastError()) {
        case ERROR_IO_PENDING:
          /* Overlapped poll operation in progress; this is expected. */
          break;
        case ERROR_INVALID_HANDLE:
          /* Socket closed; it'll be dropped from the epoll set. */
          return sock__delete(port_state, sock_state, false);
        default:
          /* Other errors are propagated to the caller. */
          return_map_error(-1);
      }
    }

    /* The poll request was successfully submitted. */
    sock_state->poll_status = SOCK__POLL_PENDING;
    sock_state->pending_events = sock_state->user_events;

  } else {
    /* Unreachable. */
    assert(false);
  }

  port_cancel_socket_update(port_state, sock_state);
  return 0;
}

int sock_feed_event(port_state_t* port_state,
                    OVERLAPPED* overlapped,
                    struct epoll_event* ev) {
  sock_state_t* sock_state =
      container_of(overlapped, sock_state_t, overlapped);
  AFD_POLL_INFO* poll_info = &sock_state->poll_info;
  uint32_t epoll_events = 0;

  sock_state->poll_status = SOCK__POLL_IDLE;
  sock_state->pending_events = 0;

  if (sock_state->delete_pending) {
    /* Socket has been deleted earlier and can now be freed. */
    return sock__delete(port_state, sock_state, false);

  } else if ((NTSTATUS) overlapped->Internal == STATUS_CANCELLED) {
    /* The poll request was cancelled by CancelIoEx. */

  } else if (!NT_SUCCESS(overlapped->Internal)) {
    /* The overlapped request itself failed in an unexpected way. */
    epoll_events = EPOLLERR;

  } else if (poll_info->NumberOfHandles < 1) {
    /* This poll operation succeeded but didn't report any socket events. */

  } else if (poll_info->Handles[0].Events & AFD_POLL_LOCAL_CLOSE) {
    /* The poll operation reported that the socket was closed. */
    return sock__delete(port_state, sock_state, false);

  } else {
    /* Events related to our socket were reported. */
    epoll_events =
        sock__afd_events_to_epoll_events(poll_info->Handles[0].Events);
  }

  /* Requeue the socket so a new poll request will be submitted. */
  port_request_socket_update(port_state, sock_state);

  /* Filter out events that the user didn't ask for. */
  epoll_events &= sock_state->user_events;

  /* Return if there are no epoll events to report. */
  if (epoll_events == 0)
    return 0;

  /* If the the socket has the EPOLLONESHOT flag set, unmonitor all events,
   * even EPOLLERR and EPOLLHUP. But always keep looking for closed sockets. */
  if (sock_state->user_events & EPOLLONESHOT)
    sock_state->user_events = 0;

  ev->data = sock_state->user_data;
  ev->events = epoll_events;
  return 1;
}

queue_node_t* sock_state_to_queue_node(sock_state_t* sock_state) {
  return &sock_state->queue_node;
}

sock_state_t* sock_state_from_tree_node(tree_node_t* tree_node) {
  return container_of(tree_node, sock_state_t, tree_node);
}

tree_node_t* sock_state_to_tree_node(sock_state_t* sock_state) {
  return &sock_state->tree_node;
}

sock_state_t* sock_state_from_queue_node(queue_node_t* queue_node) {
  return container_of(queue_node, sock_state_t, queue_node);
}
