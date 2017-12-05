#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "test-util.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

int main(void) {
  HANDLE ephnd;
  SOCKET sock;
  struct epoll_event ev;
  int r;

  ephnd = epoll_create1(0);
  check(ephnd != NULL);

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  check(sock != INVALID_SOCKET);

  ev.events = 0;
  ev.data.u64 = 0;
  r = epoll_ctl(ephnd, EPOLL_CTL_ADD, sock, &ev);
  check(r == 0);

  r = epoll_wait(ephnd, &ev, 1, 0);
  check(r == 0);

  r = epoll_ctl(ephnd, EPOLL_CTL_DEL, sock, NULL);
  check(r == 0);

  r = epoll_close(ephnd);
  check(r == 0);

  return 0;
}
