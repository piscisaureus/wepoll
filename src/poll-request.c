#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "ntstatus.h"
#include "poll-request.h"
#include "util.h"
#include "win.h"

typedef struct poll_req {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  ep_sock_t* sock_info;
} poll_req_t;

static inline poll_req_t* _poll_req_alloc(void) {
  poll_req_t* poll_req = malloc(sizeof *poll_req);
  if (poll_req == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);
  return poll_req;
}

static inline poll_req_t* _poll_req_free(poll_req_t* poll_req) {
  free(poll_req);
  return NULL;
}

poll_req_t* poll_req_new(ep_port_t* port_info, ep_sock_t* sock_info) {
  poll_req_t* poll_req = _poll_req_alloc();
  if (poll_req == NULL)
    return NULL;

  memset(poll_req, 0, sizeof *poll_req);
  poll_req->sock_info = sock_info;

  ep_sock_register_poll_req(port_info, sock_info);

  return poll_req;
}

void poll_req_delete(ep_port_t* port_info,
                     ep_sock_t* sock_info,
                     poll_req_t* poll_req) {
  assert(poll_req != NULL);

  ep_sock_unregister_poll_req(port_info, sock_info);

  _poll_req_free(poll_req);
}

poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped) {
  return container_of(overlapped, poll_req_t, overlapped);
}

ep_sock_t* poll_req_get_sock_data(const poll_req_t* poll_req) {
  return poll_req->sock_info;
}

static DWORD _epoll_events_to_afd_events(uint32_t epoll_events) {
  DWORD afd_events;

  /* These events should always be monitored. */
  assert(epoll_events & EPOLLERR);
  assert(epoll_events & EPOLLHUP);
  afd_events = AFD_POLL_ABORT | AFD_POLL_CONNECT_FAIL | AFD_POLL_LOCAL_CLOSE;

  if (epoll_events & (EPOLLIN | EPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (epoll_events & (EPOLLPRI | EPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (epoll_events & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT;

  return afd_events;
}

static uint32_t _afd_events_to_epoll_events(DWORD afd_events) {
  uint32_t epoll_events = 0;

  if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
    epoll_events |= EPOLLIN | EPOLLRDNORM;
  if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
    epoll_events |= EPOLLPRI | EPOLLRDBAND;
  if (afd_events & AFD_POLL_SEND)
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if ((afd_events & AFD_POLL_DISCONNECT) && !(afd_events & AFD_POLL_ABORT))
    epoll_events |= EPOLLIN | EPOLLRDHUP;
  if (afd_events & AFD_POLL_ABORT)
    epoll_events |= EPOLLHUP;
  if (afd_events & AFD_POLL_CONNECT)
    epoll_events |= EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND;
  if (afd_events & AFD_POLL_CONNECT_FAIL)
    epoll_events |= EPOLLERR;

  return epoll_events;
}

int poll_req_submit(poll_req_t* poll_req,
                    uint32_t epoll_events,
                    SOCKET socket,
                    SOCKET driver_socket) {
  int r;

  memset(&poll_req->overlapped, 0, sizeof poll_req->overlapped);

  poll_req->poll_info.Exclusive = TRUE;
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

int poll_req_cancel(poll_req_t* poll_req, SOCKET driver_socket) {
  OVERLAPPED* overlapped = &poll_req->overlapped;

  if (CancelIoEx((HANDLE) driver_socket, overlapped)) {
    DWORD error = GetLastError();
    if (error == ERROR_NOT_FOUND)
      return 0; /* Already completed or canceled. */
    else
      return_error(-1);
  }

  return 0;
}

void poll_req_complete(const poll_req_t* poll_req,
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
