#include "epoll.h"
#include "error.h"
#include "init.h"
#include "port.h"
#include "win.h"

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

  return ep_port_delete(port_info);
}
