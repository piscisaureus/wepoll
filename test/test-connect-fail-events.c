#include <errno.h>
#include <stdlib.h>

#include "test-util.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

#define FAIL_PORT 1

int main(void) {
  unsigned long one = 1;
  HANDLE ephnd;
  SOCKET sock;
  struct epoll_event ev;
  struct sockaddr_in addr;
  int err, len;
  int r;

  /* Create epoll port. */
  ephnd = epoll_create1(0);
  check(ephnd != NULL);

  /* Create a stream socket, switch it to non-blocking mode. */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  check(sock != INVALID_SOCKET);
  r = ioctlsocket(sock, (long) FIONBIO, &one);
  check(r == 0);

  /* Associate the socket with the epoll port; set it up such that we'll pick
   * up any valid event. */
  ev.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP |
              EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND |
              EPOLLMSG | EPOLLRDHUP;
  ev.data.sock = sock;
  r = epoll_ctl(ephnd, EPOLL_CTL_ADD, sock, &ev);
  check(r == 0);

  /* Attempt to connect to an address that we won't be able to connect to. */
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(FAIL_PORT);
  r = connect(sock, (struct sockaddr*) &addr, sizeof addr);
  check(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);

  /* Wait for the socket to report an event, and verify that the events
   * reported match exactly what the linux kernel would report in case of a
   * connect() failure. */
  r = epoll_wait(ephnd, &ev, 1, -1);
  check(r == 1);
  check(ev.data.sock == sock);
  check(ev.events == (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDNORM |
                      EPOLLWRNORM | EPOLLRDHUP));

  /* Retrieve the error associated with the socket, and verify that connect()
   * failed because the server refused the connection. */
  len = sizeof err;
  r = getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*) &err, &len);
  check(r == 0);
  check(err == WSAECONNREFUSED);

  /* Clean up. */
  r = closesocket(sock);
  check(r == 0);
  r = epoll_close(ephnd);
  check(r == 0);

  return 0;
}
