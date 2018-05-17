#include <malloc.h>
#include <stdlib.h>

#include "error.h"
#include "util.h"
#include "win.h"
#include "ws.h"

#ifndef SIO_BASE_HANDLE
#define SIO_BASE_HANDLE 0x48000022
#endif

#define _WS_INITIAL_CATALOG_BUFFER_SIZE 0x4000 /* 16kb. */

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

/* Retrieves a copy of the winsock catalog.
 * The infos pointer must be released by the caller with free(). */
int ws_get_protocol_catalog(WSAPROTOCOL_INFOW** infos_out,
                            size_t* infos_count_out) {
  DWORD buffer_size = _WS_INITIAL_CATALOG_BUFFER_SIZE;
  int count;
  WSAPROTOCOL_INFOW* infos;

  for (;;) {
    infos = malloc(buffer_size);
    if (infos == NULL)
      return_set_error(-1, ERROR_NOT_ENOUGH_MEMORY);

    count = WSAEnumProtocolsW(NULL, infos, &buffer_size);
    if (count == SOCKET_ERROR) {
      free(infos);
      if (WSAGetLastError() == WSAENOBUFS)
        continue; /* Try again with bigger buffer size. */
      else
        return_map_error(-1);
    }

    *infos_out = infos;
    *infos_count_out = (size_t) count;
    return 0;
  }
}
