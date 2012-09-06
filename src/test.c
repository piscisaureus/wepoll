
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <assert.h>
#include <stdio.h>
#include <epoll.h>

static const char PING[] = "PING";
static const int NUM_PINGERS = 10000;
static const int RUN_TIME = 10000;

int x = 0;

int main(int argc, char* argv[]) {
  epoll_t epoll_hnd;
  WSADATA wsa_data;
  int r;
  u_long one = 1;
  struct addrinfo hints;
  struct addrinfo* addrinfo;
  struct sockaddr_in addr, bind_addr;
  DWORD ticks_start, ticks_last;
  long long pings = 0, pings_sent = 0;
  int i;

  epoll_hnd = epoll_create();
  assert(epoll_hnd && epoll_hnd != INVALID_HANDLE_VALUE);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_protocol = IPPROTO_IP;

  r = getaddrinfo("localhost", NULL, &hints, &addrinfo);
  assert(r == 0);
  assert(addrinfo->ai_addrlen <= sizeof addr);

  memset(&addr, 0, sizeof addr);
  memcpy(&addr, addrinfo->ai_addr, addrinfo->ai_addrlen);
  addr.sin_port = htons(9123);

  freeaddrinfo(addrinfo);

  printf("resolved\n");

  /*
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port = htons(0);
  bind_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
  memset(bind_addr.sin_zero, 0, sizeof bind_addr.sin_zero);
  */

  for (i = 0; i < NUM_PINGERS; i++) {
    SOCKET sock;
    struct epoll_event ev;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);

    r = ioctlsocket(sock, FIONBIO, &one);
    assert(r == 0);

    r = connect(sock, (struct sockaddr*) &addr, sizeof addr);
    /* Unlike unix, windows sets the error to EWOULDBLOCK when the connection */
    /* is being established in the background. */
    assert(r == 0 || WSAGetLastError() == WSAEWOULDBLOCK);

    ev.events = EPOLLOUT | EPOLLERR;
    ev.data.sock = sock;
    r = epoll_ctl(epoll_hnd, EPOLL_CTL_ADD, sock, &ev);
    assert(r == 0);
  }

  {
    SOCKET sock;
    struct epoll_event ev;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);

    r = ioctlsocket(sock, FIONBIO, &one);
    assert(r == 0);
   
    ev.events = EPOLLOUT | EPOLLERR;
    ev.data.sock = sock;
    r = epoll_ctl(epoll_hnd, EPOLL_CTL_ADD, sock, &ev);
    assert(r == 0);
  }


  ticks_start = GetTickCount();
  ticks_last = ticks_start;

  for (;;) {
    int i, count;
    struct epoll_event events[16];
    DWORD ticks;

    ticks = GetTickCount();
    if (ticks >= ticks_last + 1000) {
      printf("%lld pings (%f per sec), %lld sent\n", pings, (double) pings / (ticks - ticks_start) * 1000, pings_sent);
      ticks_last = ticks;

     // if (ticks - ticks_start > RUN_TIME) 
     //   break;
    }

    count = epoll_wait(epoll_hnd, events, 15, 1000);
    assert(count >= 0);

    for (i = 0; i < count; i++) {
      SOCKET sock = events[i].data.sock;
      int revents = events[i].events;

      if (revents & EPOLLERR) {
        int r;
        int err = -1;
        int err_len = sizeof err;

        r = getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &err_len);
        assert(r == 0);
        fprintf(stderr, "Socket error: %d\n", err);

        r = epoll_ctl(epoll_hnd, EPOLL_CTL_DEL, sock, NULL);
        assert(r == 0);
        continue;
      }
      
      if (revents & EPOLLIN) {
        char buf[1024];
        WSABUF wsa_buf;
        DWORD flags, bytes;
        struct epoll_event ev;
        int r;

        wsa_buf.buf = buf;
        wsa_buf.len = sizeof buf;
        
        flags = 0;
        r = WSARecv(sock, &wsa_buf, 1, &bytes, &flags, NULL, NULL);
        assert(r >= 0);

        assert(bytes == sizeof PING);

        ev.data.sock = sock;
        ev.events = EPOLLOUT;

        //r = epoll_ctl(epoll_hnd, EPOLL_CTL_DEL, sock, &ev);
        //assert(r == 0);
        r = epoll_ctl(epoll_hnd, EPOLL_CTL_MOD, sock, &ev);
        assert(r == 0);
        
        pings++;

        continue;
      }

      if (revents & EPOLLOUT) {
        WSABUF wsa_buf;
        DWORD bytes;
        int r;
        struct epoll_event ev;

        wsa_buf.buf = PING;
        wsa_buf.len = sizeof PING;
        r = WSASend(sock, &wsa_buf, 1, &bytes, 0, NULL, NULL);
        assert(r >= 0);
        assert(bytes == sizeof PING);
                
        ev.data.sock = sock;
        ev.events = EPOLLIN;

        r = epoll_ctl(epoll_hnd, EPOLL_CTL_MOD, sock, &ev);
        assert(r == 0);

        pings_sent++;

        continue;
      }

      assert(0);
    }
  }

  r = epoll_close(epoll_hnd);
  assert(r == 0);
}


