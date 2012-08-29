
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#include <assert.h>s
#include <wpoll.h>

int main(int argc, char* argv[]) {
  wpoll_t wpoll_hnd;
  SOCKET sock;
  WSADATA wsa_data;
  int r;
  u_long one = 1;
  struct addrinfo hints;
  struct addrinfo* addrinfo;
  struct sockaddr_in addr;
  struct wpoll_event event;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  assert(r == 0);

  wpoll_hnd = wpoll_create();
  assert(wpoll_hnd);

  sock = socket(AF_INET, SOCK_STREAM, 0);

  r = ioctlsocket(sock, FIONBIO, &one);
  assert(r == 0);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_protocol = IPPROTO_IP;

  r = getaddrinfo("www.google.com", NULL, &hints, &addrinfo);
  assert(r == 0);

  memset(&addr, 0, sizeof addr);
  addr.sin_addr = *(IN_ADDR*) &addrinfo->ai_addr;
  addr.sin_family = addrinfo->ai_family;  
  addr.sin_port = htons(80);

  freeaddrinfo(addrinfo);

  r = connect(sock, (sockaddr*) &addr, sizeof addr);\
  assert(r == 0 || WSAGetLastError() == WSAEINPROGRESS);

  event.events = WPOLLOUT | WPOLLERR;
  event.data.sock = sock;
  r = wpoll_ctl(wpoll_hnd, WPOLL_CTL_ADD, sock, &event);

  
}
