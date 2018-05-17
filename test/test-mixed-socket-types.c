#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "test-util.h"
#include "wepoll.h"
#include "win.h"

static void create_and_add_socket(HANDLE epoll_hnd,
                                  int af,
                                  int type,
                                  int protocol) {
  SOCKET sock;
  u_long one = 1;
  int r;
  struct epoll_event ev;

  sock = socket(af, type, protocol);
  check(sock != INVALID_SOCKET);

  r = ioctlsocket(sock, (long) FIONBIO, &one);
  check(r == 0);

  ev.events = EPOLLOUT | EPOLLONESHOT;
  ev.data.sock = sock;
  r = epoll_ctl(epoll_hnd, EPOLL_CTL_ADD, sock, &ev);
  check(r == 0);
}

int main(void) {
  HANDLE epoll_hnd;
  int i, r;
  struct epoll_event ev;

  epoll_hnd = epoll_create1(0);
  check(epoll_hnd != NULL);

  create_and_add_socket(epoll_hnd, AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  create_and_add_socket(epoll_hnd, AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

  for (i = 0; i < 2; i++) {
    r = epoll_wait(epoll_hnd, &ev, 1, -1);
    check(r == 1);

    r = epoll_ctl(epoll_hnd, EPOLL_CTL_DEL, ev.data.sock, NULL);
    check(r == 0);

    r = closesocket(ev.data.sock);
    check(r == 0);
  }

  r = epoll_close(epoll_hnd);
  check(r == 0);

  return 0;
}
