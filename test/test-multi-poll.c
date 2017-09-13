#include <assert.h>
#include <process.h>
#include <stdint.h>
#include <stdio.h>

#include "epoll.h"
#include "init.h"
#include "util.h"
#include "win.h"

#include <WS2tcpip.h>

#define POLL_THREAD_COUNT 20
#define LISTEN_PORT 12345

static SOCKET create_socket(unsigned short port) {
  SOCKET sock;
  struct sockaddr_in address;
  unsigned long one;
  int r;

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  assert(sock > 0);

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  r = bind(sock, (struct sockaddr*) &address, sizeof address);
  assert(r == 0);

  one = 1;
  r = ioctlsocket(sock, FIONBIO, &one);
  assert(r == 0);

  return sock;
}

static void send_message(SOCKET sock, unsigned short port) {
  char hello[] = "hello";
  struct sockaddr_in address;
  WSABUF buf;
  DWORD bytes_sent;
  int r;

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  buf.buf = hello;
  buf.len = sizeof(hello);

  r = WSASendTo(sock,
                &buf,
                1,
                &bytes_sent,
                0,
                (struct sockaddr*) &address,
                sizeof address,
                NULL,
                NULL);
  assert(r >= 0);
}

static unsigned int __stdcall poll_thread(void* arg) {
  epoll_t epfd;
  SOCKET sock;
  struct epoll_event ev_in;
  struct epoll_event ev_out;
  int r;

  sock = (SOCKET) arg;

  epfd = epoll_create();
  assert(epfd != NULL);

  ev_in.events = EPOLLIN;
  ev_in.data.u64 = 42;
  r = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev_in);
  assert(r == 0);

  memset(&ev_out, 0, sizeof ev_out);
  r = epoll_wait(epfd, &ev_out, 1, -1);
  assert(r == 1);

  assert(ev_out.data.u64 == ev_in.data.u64);
  assert(ev_out.events == ev_in.events);

  puts("Got event.");

  r = epoll_close(epfd);
  assert(r == 0);

  return 0;
}

int main(void) {
  HANDLE threads[POLL_THREAD_COUNT];

  int r = init();
  assert(r == 0);

  SOCKET send_sock = create_socket(0);
  SOCKET recv_sock = create_socket(LISTEN_PORT);

  for (size_t i = 0; i < array_count(threads); i++) {
    HANDLE thread = (HANDLE) _beginthreadex(
        NULL, 0, poll_thread, (void*) recv_sock, 0, NULL);
    assert(thread != INVALID_HANDLE_VALUE);
    threads[i] = thread;
  }

  Sleep(500);

  send_message(send_sock, LISTEN_PORT);

  for (size_t i = 0; i < array_count(threads); i++) {
    HANDLE thread = threads[i];
    DWORD wr = WaitForSingleObject(thread, INFINITE);
    assert(wr == WAIT_OBJECT_0);
    CloseHandle(thread);
  }

  return 0;
}
