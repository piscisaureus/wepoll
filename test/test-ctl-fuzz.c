#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "test-util.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

#define NUM_SOCKETS 10000
#define RUN_TIME 5000
#define PRINT_INTERVAL 500

static SOCKET create_and_add_socket(HANDLE epfd) {
  SOCKET sock;
  unsigned long one = 1;
  int r;
  struct epoll_event ev;

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  check(sock > 0);

  r = ioctlsocket(sock, (long) FIONBIO, &one);
  check(r == 0);

  ev.events = 0;
  ev.data.u64 = 42;
  r = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
  check(r == 0);

  return sock;
}

int main(void) {
  uint64_t total_events = 0;
  uint64_t start_time, last_print_time, now, total_time;
  SOCKET sockets[NUM_SOCKETS];
  HANDLE epfd;
  size_t i;
  int r;

  r = init();
  check(r == 0);

  epfd = epoll_create1(0);
  check(epfd != NULL);

  for (i = 0; i < NUM_SOCKETS; i++)
    sockets[i] = create_and_add_socket(epfd);

  start_time = GetTickCount64();
  last_print_time = 0;

  do {
    struct epoll_event ev_out[64];
    uint64_t count;

    for (i = 0; i < NUM_SOCKETS; i++) {
      SOCKET sock = sockets[i];
      struct epoll_event ev_in;

      ev_in.events = (rand() & 0xff) | EPOLLONESHOT;
      ev_in.data.u64 = 42;

      r = epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &ev_in);
      check(r == 0);
    }

    count = 0;
    do {
      r = epoll_wait(epfd, ev_out, array_count(ev_out), count > 0 ? 0 : -1);
      check(r >= 0);
      count += (uint64_t) r;
    } while (r > 0);

    total_events += count;

    now = GetTickCount64();
    total_time = now - start_time;

    if (now - last_print_time > PRINT_INTERVAL) {
      last_print_time = now;
      printf("%f events (%f events/sec)\n",
             (double) total_events,
             (double) total_events / total_time * 1000);
    }
  } while (total_time < RUN_TIME);

  epoll_close(epfd);

  return 0;
}
