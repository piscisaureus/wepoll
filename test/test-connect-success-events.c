/* In this test we check that stream sockets connected with `connect()` do not
 * continuously report EPOLLOUT, but only when they're actually writable.

 * It is a regression test for a bug that was caused by incorrectly mapping the
 `AFD_POLL_CONNECT`
 * event to `EPOLLOUT`.
 */

#include <errno.h>
#include <stdlib.h>

#include "test-util.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"
#include "ws.h"

#define PORT 12345
#define ROUNDS 5
#define SEND_LENGTH (1 << 20)

int main(void) {
  unsigned long one = 1;
  SOCKET listen_sock, server_sock, client_sock;
  struct sockaddr_in addr;
  HANDLE ephnd;
  struct epoll_event ev;
  int round;
  int r;

  /* Initialize winsock. */
  r = ws_global_init();
  check(r == 0);

  /* Set up address that the server listens on and the client connects to. */
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons(PORT);

  /* Create the server socket and start listening for incoming connections. */
  listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  check(listen_sock != INVALID_SOCKET);
  r = bind(listen_sock, (struct sockaddr*) &addr, sizeof addr);
  check(r == 0);
  r = listen(listen_sock, SOMAXCONN);
  check(r == 0);

  /* Create the client-end socket, switch it to non-blocking mode. */
  client_sock = socket(AF_INET, SOCK_STREAM, 0);
  check(client_sock != INVALID_SOCKET);
  r = ioctlsocket(client_sock, (long) FIONBIO, &one);
  check(r == 0);

  /* Create an epoll port for the client end of the connection. */
  ephnd = epoll_create1(0);
  check(ephnd != NULL);

  /* Associate the client socket with the epoll port. */
  ev.events = EPOLLOUT;
  ev.data.sock = client_sock;
  r = epoll_ctl(ephnd, EPOLL_CTL_ADD, client_sock, &ev);
  check(r == 0);

  /* Begin connecting the client socket to the server. */
  r = connect(client_sock, (struct sockaddr*) &addr, sizeof addr);
  check(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK);

  /* Accept the server end of the connection. */
  server_sock = accept(listen_sock, NULL, NULL);
  check(server_sock != INVALID_SOCKET);

  for (round = 0; round < ROUNDS; round++) {
    static char buf[SEND_LENGTH];
    struct epoll_event evs[8];
    int bytes_received, bytes_sent;

    /* Wait for client end to become writable. */
    r = epoll_wait(ephnd, evs, array_count(evs), -1);

    /* Try to send data until the kernel buffer is full. */
    memset(buf, round, sizeof buf);
    bytes_sent = 0;
    do {
      /* We shouldn't get here unless epoll_wait() reported that the client
       * socket is writable. */
      check(r == 1);
      check(evs[0].data.sock == client_sock);
      check(evs[0].events == EPOLLOUT);

      /* The actual send() call should never fail, because epoll_wait() just
       * reported the socket as writable. */
      r = send(client_sock, buf, sizeof buf, 0);
      check(r > 0);
      bytes_sent += r;

      /* Call epoll_wait() without blocking to see if there's more space in the
       * kernel write buffer. */
      r = epoll_wait(ephnd, evs, array_count(evs), 0);
    } while (r > 0);

    /* Verify that epoll_wait() reported no events, but did not fail. */
    check(r == 0);

    /* Read all data incoming on the server end of the connection. */
    bytes_received = 0;
    do {
      r = recv(server_sock, buf, sizeof buf, 0);
      check(r > 0);
      check(buf[0] == round);
      bytes_received += r;
    } while (bytes_received < bytes_sent);
    check(bytes_received == bytes_sent);
  }

  /* Disassociate the client socket from epoll port. */
  r = epoll_ctl(ephnd, EPOLL_CTL_DEL, client_sock, NULL);
  check(r == 0);

  /* Close the epoll port. */
  r = epoll_close(ephnd);
  check(r == 0);

  /* Close all sockets. */
  r = closesocket(listen_sock);
  check(r == 0);
  r = closesocket(server_sock);
  check(r == 0);
  r = closesocket(client_sock);
  check(r == 0);

  return 0;
}
