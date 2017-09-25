#include <stdint.h>
#include <stdlib.h>

#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "init.h"
#include "port.h"
#include "util.h"
#include "win.h"

#define _EPOLL_MAX_COMPLETION_COUNT 64

int epoll_global_init(void) {
  return 0;
}

epoll_t epoll_create(void) {
  ep_port_t* port_info;
  HANDLE iocp;

  if (init() < 0)
    return NULL;

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE)
    return_error(NULL);

  port_info = ep_port_new(iocp);
  if (port_info == NULL) {
    CloseHandle(iocp);
    return NULL;
  }

  return (epoll_t) port_info;
}

int epoll_close(epoll_t port_handle) {
  ep_port_t* port_info;

  if (init() < 0)
    return -1;

  port_info = (ep_port_t*) port_handle;

  ep_port_close(port_info);

  return ep_port_delete(port_info);
}

static int _ep_ctl_add(ep_port_t* port_info,
                       SOCKET sock,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0) {
    ep_sock_delete(port_info, sock_info);
    return -1;
  }

  return 0;
}

static int _ep_ctl_mod(ep_port_t* port_info,
                       SOCKET sock,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0)
    return -1;

  return 0;
}

static int _ep_ctl_del(ep_port_t* port_info, SOCKET sock) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  ep_sock_delete(port_info, sock_info);

  return 0;
}

int epoll_ctl(epoll_t port_handle,
              int op,
              SOCKET sock,
              struct epoll_event* ev) {
  ep_port_t* port_info = (ep_port_t*) port_handle;

  if (init() < 0)
    return -1;

  switch (op) {
    case EPOLL_CTL_ADD:
      return _ep_ctl_add(port_info, sock, ev);
    case EPOLL_CTL_MOD:
      return _ep_ctl_mod(port_info, sock, ev);
    case EPOLL_CTL_DEL:
      return _ep_ctl_del(port_info, sock);
  }

  return_error(-1, ERROR_INVALID_PARAMETER);
}

int epoll_wait(epoll_t port_handle,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  ep_port_t* port_info;
  ULONGLONG due = 0;
  DWORD gqcs_timeout;

  if (init() < 0)
    return -1;

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
  if ((size_t) maxevents > _EPOLL_MAX_COMPLETION_COUNT)
    maxevents = _EPOLL_MAX_COMPLETION_COUNT;

  /* Dequeue completion packets until either at least one interesting event
   * has been discovered, or the timeout is reached.
   */
  do {
    OVERLAPPED_ENTRY completion_list[_EPOLL_MAX_COMPLETION_COUNT];
    ULONG completion_count;
    ssize_t event_count;

    if (ep_port_update_events(port_info) < 0)
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

    event_count = ep_port_feed_events(
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
