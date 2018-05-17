/* This test verifies that sockets are automatically dropped from an epoll set
 * when they are closed.
 *
 * On Linux, a socket would be dropped from the epoll set immediately after the
 * call to close(). However on Windows, this isn't possible - wepoll may not
 * detect that a socket has been closed until a call to epoll_wait() is made.
 *
 * There are multiple code paths that detect a closed socket in the wepoll
 * implementation; this test attempts to stress all of them.
 */

#include <errno.h>
#include <stdlib.h>

#include "test-util.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

static SOCKET create_and_add_socket(HANDLE ephnd, uint32_t events) {
  SOCKET sock;
  struct epoll_event ev;
  int r;

  /* Create UDP socket. */
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  check(sock != INVALID_SOCKET);

  /* Associate with epoll port. */
  ev.events = events;
  ev.data.sock = sock;
  r = epoll_ctl(ephnd, EPOLL_CTL_ADD, sock, &ev);
  check(r == 0);

  return sock;
}

static void check_dropped(HANDLE ephnd, SOCKET sock) {
  struct epoll_event ev;
  int r;

  ev.events = EPOLLERR | EPOLLHUP;
  ev.data.u64 = 0;

  /* Check that EPOLL_CTL_MOD fails. */
  r = epoll_ctl(ephnd, EPOLL_CTL_MOD, sock, &ev);
  check(r < 0);
  check(errno == EBADF);

  /* Check that EPOLL_CTL_DEL fails. */
  r = epoll_ctl(ephnd, EPOLL_CTL_DEL, sock, &ev);
  check(r < 0);
  check(errno == EBADF);
}

int main(void) {
  HANDLE ephnd;
  SOCKET sock1, sock2, sock3, sock4, sock5;
  struct epoll_event evs[8];
  int r;

  ephnd = epoll_create1(0);
  check(ephnd != NULL);

  sock1 = create_and_add_socket(ephnd, EPOLLIN);
  sock2 = create_and_add_socket(ephnd, EPOLLIN);
  sock3 = create_and_add_socket(ephnd, EPOLLOUT);
  sock4 = create_and_add_socket(ephnd, EPOLLOUT | EPOLLONESHOT);
  sock5 = create_and_add_socket(ephnd, 0);

  r = closesocket(sock1);
  check(r == 0);

  r = epoll_wait(ephnd, evs, array_count(evs), -1);
  check(r == 2); /* sock3 and sock4 should report events. */

  check_dropped(ephnd, sock1);

  r = closesocket(sock2);
  check(r == 0);
  r = closesocket(sock3);
  check(r == 0);

  r = epoll_wait(ephnd, evs, array_count(evs), 0);
  check(r == 0); /* No events should be reported. */

  check_dropped(ephnd, sock2);
  check_dropped(ephnd, sock3);

  r = closesocket(sock4);
  check(r == 0);
  r = closesocket(sock5);
  check(r == 0);

  r = epoll_wait(ephnd, evs, array_count(evs), 0);
  check(r == 0); /* No events should be reported. */

  check_dropped(ephnd, sock4);
  check_dropped(ephnd, sock5);

  r = epoll_close(ephnd);
  check(r == 0);

  return 0;
}
