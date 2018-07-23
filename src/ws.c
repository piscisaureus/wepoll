#include <malloc.h>
#include <stdlib.h>

#include "error.h"
#include "util.h"
#include "win.h"
#include "ws.h"

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

int ws_global_init(void) {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return_set_error(-1, (DWORD) r);

  return 0;
}

SOCKET ws_get_base_socket(SOCKET socket) {
  SOCKET base_socket;
  DWORD bytes;

  if (WSAIoctl(socket,
               SIO_BASE_HANDLE,
               NULL,
               0,
               &base_socket,
               sizeof base_socket,
               &bytes,
               NULL,
               NULL) == SOCKET_ERROR)
    return_map_error(INVALID_SOCKET);

  return base_socket;
}
