#ifndef WEPOLL_H_
#define WEPOLL_H_

#ifndef WEPOLL_EXPORT
#define WEPOLL_EXPORT
#endif

#include <stdint.h>

/* clang-format off */

enum EPOLL_EVENTS {
  EPOLLIN      = 1 <<  0,
  EPOLLPRI     = 1 <<  1,
  EPOLLOUT     = 1 <<  2,
  EPOLLERR     = 1 <<  3,
  EPOLLHUP     = 1 <<  4,
  EPOLLRDNORM  = 1 <<  6,
  EPOLLRDBAND  = 1 <<  7,
  EPOLLWRNORM  = 1 <<  8,
  EPOLLWRBAND  = 1 <<  9,
  EPOLLMSG     = 1 << 10, /* Never reported. */
  EPOLLRDHUP   = 1 << 13,
  EPOLLONESHOT = 1 << 31
};

#define EPOLLIN      ((uint32_t) EPOLLIN)
#define EPOLLPRI     ((uint32_t) EPOLLPRI)
#define EPOLLOUT     ((uint32_t) EPOLLOUT)
#define EPOLLERR     ((uint32_t) EPOLLERR)
#define EPOLLHUP     ((uint32_t) EPOLLHUP)
#define EPOLLRDNORM  ((uint32_t) EPOLLRDNORM)
#define EPOLLRDBAND  ((uint32_t) EPOLLRDBAND)
#define EPOLLWRNORM  ((uint32_t) EPOLLWRNORM)
#define EPOLLWRBAND  ((uint32_t) EPOLLWRBAND)
#define EPOLLMSG     ((uint32_t) EPOLLMSG)
#define EPOLLRDHUP   ((uint32_t) EPOLLRDHUP)
#define EPOLLONESHOT ((uint32_t) EPOLLONESHOT)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 2
#define EPOLL_CTL_DEL 3

/* clang-format on */

typedef void* HANDLE;
typedef uintptr_t SOCKET;

typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  SOCKET sock; /* Windows specific */
  HANDLE hnd;  /* Windows specific */
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
