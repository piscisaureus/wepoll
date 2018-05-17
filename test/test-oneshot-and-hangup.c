#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "test-util.h"
#include "wepoll.h"
#include "win.h"

static int tcp_socketpair(SOCKET socks[2]) {
  SOCKET listen_sock;
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof addr;

  listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock == INVALID_SOCKET)
    return SOCKET_ERROR;

  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  if (bind(listen_sock, (struct sockaddr*) &addr, sizeof addr) == SOCKET_ERROR)
    goto err1;

  if (getsockname(listen_sock, (struct sockaddr*) &addr, &addrlen) ==
      SOCKET_ERROR)
    goto err1;

  if (listen(listen_sock, 1) == SOCKET_ERROR)
    goto err1;

  socks[0] = socket(AF_INET, SOCK_STREAM, 0);
  if (socks[0] == INVALID_SOCKET)
    goto err1;

  if (connect(socks[0], (struct sockaddr*) &addr, sizeof addr) == SOCKET_ERROR)
    goto err2;

  socks[1] = accept(listen_sock, NULL, NULL);
  if (socks[1] == INVALID_SOCKET)
    goto err2;

  closesocket(listen_sock);

  return 0;

err2:
  closesocket(socks[0]);
err1:
  closesocket(listen_sock);
  return -1;
}

static int sock_set_nonblock(SOCKET sock, bool enable) {
  u_long arg = enable;
  return ioctlsocket(sock, (long) FIONBIO, &arg);
}

int main(void) {
  static const char HELLO[] = "hello, world!";
  HANDLE epoll_port;
  SOCKET send_sock, recv_sock;

  {
    /* Create an epoll instance. */
    epoll_port = epoll_create(1);
    check(epoll_port != NULL);
  }

  {
    /* Create a TCP socket pair. */
    SOCKET socks[2];
    int r = tcp_socketpair(socks);
    check(r == 0);

    send_sock = socks[0];
    recv_sock = socks[1];
  }

  {
    /* Enable non-blocking mode on the receiving end. */
    int r = sock_set_nonblock(recv_sock, true);
    check(r == 0);
  }

  {
    /* Send some data in order to trigger an event on the receiving socket. */
    int r = send(send_sock, HELLO, sizeof HELLO, 0);
    check(r == sizeof HELLO);
  }

  {
    /* Add the receiving socket to the epoll set. */
    struct epoll_event ev;
    int r;

    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.sock = recv_sock;

    r = epoll_ctl(epoll_port, EPOLL_CTL_ADD, recv_sock, &ev);
    check(r >= 0);
  }

  {
    /* Receive the EPOLLIN event for recv_sock. */
    struct epoll_event ev;
    int r;

    memset(&ev, 0, sizeof ev);

    r = epoll_wait(epoll_port, &ev, 1, -1);
    check(r == 1);
    check(ev.events == EPOLLIN);
    check(ev.data.sock == recv_sock);
  }

  {
    /* Read the data from the socket. */
    char buffer[16];
    int r = recv(recv_sock, buffer, sizeof buffer, 0);
    check(r > 0);
  }

  {
    /* Since the last epoll_ctl() call specified the EPOLLONESOT flag,
     * no events should be reported here -- neither EPOLLIN nor EPOLLHUP. */
    static const int timeout = 250; /* Quarter second. */
    struct epoll_event ev;
    int r;

    memset(&ev, 0, sizeof ev);

    r = epoll_wait(epoll_port, &ev, 1, timeout);
    check(r == 0); /* Time-out. */
  }

  {
    /* Attempt to EPOLL_CTL_ADD the socket to the port. This should fail,
     * because EPOLLONESHOT causes events to be disabled after one is reported,
     * but the socket should not be dropped from the epoll set. */
    struct epoll_event ev;
    int r;

    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.sock = recv_sock;

    r = epoll_ctl(epoll_port, EPOLL_CTL_ADD, recv_sock, &ev);

    check(r == -1);
    check(errno == EEXIST);
    check(GetLastError() == ERROR_ALREADY_EXISTS);
  }

  {
    /* Modify the read socket event mask to EPOLLRDHUP. */
    struct epoll_event ev;
    int r;

    ev.events = EPOLLRDHUP | EPOLLONESHOT;
    ev.data.sock = recv_sock;

    r = epoll_ctl(epoll_port, EPOLL_CTL_MOD, recv_sock, &ev);
    check(r == 0);
  }

  {
    /* Send some data, which will never be read by the receiving end, otherwise
     * Windows won't detect that the connection is closed. */
    int r = send(send_sock, HELLO, sizeof HELLO, 0);
    check(r == sizeof HELLO);
  }

  {
    /* Send FIN packet. */
    int r = shutdown(send_sock, SD_SEND);
    check(r == 0);
  }

  {
    /* Receive the EPOLLRDHUP event for recv_sock. */
    struct epoll_event ev;
    int r;

    memset(&ev, 0, sizeof ev);

    r = epoll_wait(epoll_port, &ev, 1, -1);
    check(r == 1);
    check(ev.events == EPOLLRDHUP);
    check(ev.data.sock == recv_sock);
  }

  {
    /* Close to receiving socket, so the sending socket should detect a
     * connection hang-up */
    int r = closesocket(recv_sock);
    check(r == 0);
  }

  {
    /* Add the *write* socket to the epoll set. The event mask is empty, but
     * since EPOLLHUP and EPOLLERR are always reportable, the next call to
     * epoll_wait() should detect that the connection has been closed. */
    struct epoll_event ev;
    int r;

    ev.events = 0;
    ev.data.sock = send_sock;

    r = epoll_ctl(epoll_port, EPOLL_CTL_ADD, send_sock, &ev);
    check(r == 0);
  }

  {
    /* Receive the EPOLLHUP event for write end of the connection. */
    struct epoll_event ev;
    int r;

    memset(&ev, 0, sizeof ev);

    r = epoll_wait(epoll_port, &ev, 1, -1);
    check(r == 1);
    check(ev.events == EPOLLHUP);
    check(ev.data.sock == send_sock);
  }

  {
    /* Close the send socket. */
    int r = closesocket(send_sock);
    check(r == 0);
  }

  {
    /* Close the epoll port. */
    int r = epoll_close(epoll_port);
    check(r == 0);
  }

  return 0;
}
