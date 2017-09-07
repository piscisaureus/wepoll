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
#include "port-data.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "win.h"

#define _EP_COMPLETION_LIST_LENGTH 64

typedef struct _ep_port_data _ep_port_data_t;
typedef struct poll_req poll_req_t;
typedef struct ep_sock ep_sock_t;

static int _ep_initialize(void);
static SOCKET _ep_create_driver_socket(HANDLE iocp,
                                       WSAPROTOCOL_INFOW* protocol_info);
static int _ep_submit_poll_req(_ep_port_data_t* port_data,
                               ep_sock_t* sock_info);

static int _ep_initialized = 0;

static int _ep_ctl_add(_ep_port_data_t* port_data,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_data);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_socket(port_data, sock_info, socket) < 0 ||
      ep_sock_set_event(port_data, sock_info, ev) < 0) {
    ep_sock_delete(port_data, sock_info);
    return -1;
  }

  return 0;
}

static int _ep_ctl_mod(_ep_port_data_t* port_data,
                       uintptr_t socket,
                       struct epoll_event* ev) {
  ep_sock_t lookup;
  ep_sock_t* sock_info;

  lookup.socket = socket;
  sock_info = RB_FIND(ep_sock_tree, &port_data->sock_data_tree, &lookup);
  if (sock_info == NULL)
    return_error(-1, ERROR_BAD_NETPATH); /* Socket not in epoll set. */

  if (ep_sock_set_event(port_data, sock_info, ev) < 0)
    return -1;

  return 0;
}

static int _ep_ctl_del(_ep_port_data_t* port_data, uintptr_t socket) {
  ep_sock_t lookup;
  ep_sock_t* sock_info;

  lookup.socket = socket;
  sock_info = RB_FIND(ep_sock_tree, &port_data->sock_data_tree, &lookup);
  if (sock_info == NULL)
    return_error(-1, ERROR_NOT_FOUND); /* Socket not in epoll set. */

  if (ep_sock_delete(port_data, sock_info) < 0)
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

static int _ep_port_update_events(_ep_port_data_t* port_data) {
  QUEUE* update_queue = &port_data->update_queue;

  /* Walk the queue, submitting new poll requests for every socket that needs
   * it. */
  while (!QUEUE_EMPTY(update_queue)) {
    QUEUE* queue_entry = QUEUE_HEAD(update_queue);
    ep_sock_t* sock_info = QUEUE_DATA(queue_entry, ep_sock_t, queue_entry);

    if (ep_sock_update(port_data, sock_info) < 0)
      return -1;

    /* ep_sock_update() removes the socket from the update list if
     * successfull. */
  }

  return 0;
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

    event_count += ep_sock_feed_event(port_data, poll_req, ev);
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
  ep_sock_t* sock_info;

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
  while ((sock_info = RB_ROOT(&port_data->sock_data_tree))) {
    RB_REMOVE(ep_sock_tree, &port_data->sock_data_tree, sock_info);
    free(sock_info);
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

SOCKET _ep_get_driver_socket(_ep_port_data_t* port_data, SOCKET socket) {
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
  driver_socket = port_data->driver_sockets[index];
  if (driver_socket == 0) {
    driver_socket = _ep_create_driver_socket(port_data->iocp, &protocol_info);
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
