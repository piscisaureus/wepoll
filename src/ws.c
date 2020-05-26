#include <stdlib.h>

#include "error.h"
#include "util.h"
#include "win.h"
#include "ws.h"

#ifndef SIO_BSP_HANDLE_POLL
#define SIO_BSP_HANDLE_POLL 0x4800001D
#endif

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

static inline SOCKET ws__ioctl_get_bsp_socket(SOCKET socket, DWORD ioctl) {
  SOCKET bsp_socket;
  DWORD bytes;

  if (WSAIoctl(socket,
               ioctl,
               NULL,
               0,
               &bsp_socket,
               sizeof bsp_socket,
               &bytes,
               NULL,
               NULL) != SOCKET_ERROR)
    return bsp_socket;
  else
    return INVALID_SOCKET;
}

SOCKET ws_get_base_socket(SOCKET socket) {
  SOCKET base_socket;
  DWORD error;

  for (;;) {
    base_socket = ws__ioctl_get_bsp_socket(socket, SIO_BASE_HANDLE);
    if (base_socket != INVALID_SOCKET)
      return base_socket;

    error = GetLastError();
    if (error == WSAENOTSOCK)
      return_set_error(INVALID_SOCKET, error);

    /* clang-format off */
    /* Even though Microsoft documentation clearly states that LSPs should
     * never intercept the `SIO_BASE_HANDLE` ioctl [1], Komodia based LSPs do
     * so anyway, breaking it, with the apparent intention of preventing LSP
     * bypass [2]. Fortunately they don't handle `SIO_BSP_HANDLE_POLL`, which
     * will at least let us obtain the socket associated with the next winsock
     * protocol chain entry. If this succeeds, loop around and call
     * `SIO_BASE_HANDLE` again with the returned BSP socket, to make sure that
     * we unwrap all layers and retrieve the actual base socket.
     *  [1] https://docs.microsoft.com/en-us/windows/win32/winsock/winsock-ioctls
     *  [2] https://www.komodia.com/newwiki/index.php?title=Komodia%27s_Redirector_bug_fixes#Version_2.2.2.6
     */
    /* clang-format on */
    base_socket = ws__ioctl_get_bsp_socket(socket, SIO_BSP_HANDLE_POLL);
    if (base_socket != INVALID_SOCKET && base_socket != socket)
      socket = base_socket;
    else
      return_set_error(INVALID_SOCKET, error);
  }
}
