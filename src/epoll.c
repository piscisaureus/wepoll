#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "afd.h"
#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "nt.h"
#include "poll-request.h"
#include "port.h"
#include "queue.h"
#include "rb.h"
#include "util.h"
#include "win.h"

#define _EP_COMPLETION_LIST_LENGTH 64

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;
typedef struct ep_sock ep_sock_t;

static int _ep_initialize(void);
static SOCKET _ep_create_driver_socket(HANDLE iocp,
                                       WSAPROTOCOL_INFOW* protocol_info);
static int _ep_submit_poll_req(ep_port_t* port_info, ep_sock_t* sock_info);

static int _ep_initialized = 0;

static int _ep_ctl_add(ep_port_t* port_info,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_info);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_socket(port_info, sock_info, socket) < 0 ||
      ep_sock_set_event(port_info, sock_info, ev) < 0) {
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

  if (ep_sock_delete(port_info, sock_info) < 0)
    return -1;

  return 0;
}

int epoll_ctl(epoll_t port_handle,
              int op,
              uintptr_t socket,
              struct epoll_event* ev) {
  ep_port_t* port_info = (ep_port_t*) port_handle;

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

epoll_t epoll_create(void) {
  ep_port_t* port_info;
  HANDLE iocp;

  /* If necessary, do global initialization first. This is totally not
   * thread-safe at the moment.
   */
  if (!_ep_initialized) {
    if (_ep_initialize() < 0)
      return NULL;
    _ep_initialized = 1;
  }

  port_info = malloc(sizeof *port_info);
  if (port_info == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE) {
    free(port_info);
    return_error(NULL);
  }

  port_info->iocp = iocp;
  port_info->poll_req_count = 0;

  queue_init(&port_info->update_queue);

  memset(&port_info->driver_sockets, 0, sizeof port_info->driver_sockets);
  tree_init(&port_info->sock_tree);

  return (epoll_t) port_info;
}

int epoll_close(epoll_t port_handle) {
  ep_port_t* port_info;
  tree_node_t* tree_node;

  port_info = (ep_port_t*) port_handle;

  /* Close all peer sockets. This will make all pending io requests return. */
  for (size_t i = 0; i < array_count(port_info->driver_sockets); i++) {
    SOCKET driver_socket = port_info->driver_sockets[i];
    if (driver_socket != 0 && driver_socket != INVALID_SOCKET) {
      if (closesocket(driver_socket) != 0)
        return_error(-1);

      port_info->driver_sockets[i] = 0;
    }
  }

  /* There is no list of io requests to free. And even if there was, just
   * freeing them would be dangerous since the kernel might still alter
   * the overlapped status contained in them. But since we are sure that
   * all requests will soon return, just await them all.
   */
  while (port_info->poll_req_count > 0) {
    OVERLAPPED_ENTRY entries[64];
    DWORD result;
    ULONG count, i;

    result = GetQueuedCompletionStatusEx(port_info->iocp,
                                         entries,
                                         array_count(entries),
                                         &count,
                                         INFINITE,
                                         FALSE);

    if (!result)
      return_error(-1);

    for (i = 0; i < count; i++) {
      poll_req_t* poll_req = overlapped_to_poll_req(entries[i].lpOverlapped);
      poll_req_delete(port_info, poll_req_get_sock_data(poll_req), poll_req);
    }
  }

  /* Remove all entries from the socket_state tree. */
  while ((tree_node = tree_root(&port_info->sock_tree)) != NULL) {
    ep_sock_t* sock_info = container_of(tree_node, ep_sock_t, tree_node);
    ep_sock_delete(port_info, sock_info);
  }

  /* Close the I/O completion port. */
  if (!CloseHandle(port_info->iocp))
    return_error(-1);

  /* Finally, remove the port data. */
  free(port_info);

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

SOCKET ep_port_get_driver_socket(ep_port_t* port_info, SOCKET socket) {
  ssize_t index;
  size_t i;
  SOCKET driver_socket;
  WSAPROTOCOL_INFOW protocol_info;
  int len;

  /* Obtain protocol information about the socket. */
  len = sizeof protocol_info;
  if (getsockopt(socket,
                 SOL_SOCKET,
                 SO_PROTOCOL_INFOW,
                 (char*) &protocol_info,
                 &len) != 0)
    return_error(INVALID_SOCKET);

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
    return_error(INVALID_SOCKET, ERROR_NOT_SUPPORTED);

  /* If we didn't (try) to create a peer socket yet, try to make one. Don't
   * try again if the peer socket creation failed earlier for the same
   * protocol.
   */
  driver_socket = port_info->driver_sockets[index];
  if (driver_socket == 0) {
    driver_socket = _ep_create_driver_socket(port_info->iocp, &protocol_info);
    port_info->driver_sockets[index] = driver_socket;
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
