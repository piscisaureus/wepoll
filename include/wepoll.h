#ifndef WEPOLL_H_
#define WEPOLL_H_

#ifndef WEPOLL_EXPORT
#define WEPOLL_EXPORT
#endif

#include <stdint.h>

enum EPOLL_EVENTS {
  EPOLLIN = 1 << 0,
  EPOLLPRI = 1 << 1,
  EPOLLOUT = 1 << 2,
  EPOLLERR = 1 << 3,
  EPOLLHUP = 1 << 4,
  EPOLLRDNORM = 1 << 6,
  EPOLLRDBAND = 1 << 7,
  EPOLLWRNORM = 1 << 8,
  EPOLLWRBAND = 1 << 9,
  EPOLLMSG = 1 << 10,
  EPOLLRDHUP = 1 << 13,
  EPOLLONESHOT = 1 << 31
};

#define EPOLLIN EPOLLIN
#define EPOLLPRI EPOLLPRI
#define EPOLLOUT EPOLLOUT
#define EPOLLERR EPOLLERR
#define EPOLLHUP EPOLLHUP
#define EPOLLRDNORM EPOLLRDNORM
#define EPOLLRDBAND EPOLLRDBAND
#define EPOLLWRNORM EPOLLWRNORM
#define EPOLLWRBAND EPOLLWRBAND
#define EPOLLMSG EPOLLMSG
#define EPOLLRDHUP EPOLLRDHUP
#define EPOLLONESHOT EPOLLONESHOT

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 2
#define EPOLL_CTL_DEL 3

typedef void* HANDLE;
typedef uintptr_t SOCKET;

typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  SOCKET sock;
  HANDLE hnd;
} epoll_data_t;

struct epoll_event {
  uint32_t events;   /* Epoll events */
  epoll_data_t data; /* User data variable */
};

#ifdef __cplusplus
extern "C" {
#endif

WEPOLL_EXPORT HANDLE epoll_create(int size);
WEPOLL_EXPORT HANDLE epoll_create1(int flags);

WEPOLL_EXPORT int epoll_close(HANDLE ephnd);

WEPOLL_EXPORT int epoll_ctl(HANDLE ephnd,
                           int op,
                           SOCKET sock,
                           struct epoll_event* event);

WEPOLL_EXPORT int epoll_wait(HANDLE ephnd,
                            struct epoll_event* events,
                            int maxevents,
                            int timeout);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WEPOLL_H_ */
