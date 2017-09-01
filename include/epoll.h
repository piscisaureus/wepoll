#ifndef EPOLL_H_
#define EPOLL_H_

#ifndef EPOLL_EXTERN
#define EPOLL_EXTERN
#endif

#include <stdint.h>

#define EPOLLIN 0x001
#define EPOLLPRI 0x002
#define EPOLLOUT 0x004
#define EPOLLERR 0x008
#define EPOLLHUP 0x010
#define EPOLLRDNORM 0x040
#define EPOLLRDBAND 0x080
#define EPOLLWRNORM 0x100
#define EPOLLMSG 0x400
#define EPOLLWRBAND 0x200
#define EPOLLRDHUP 0x2000

/* #define EPOLLET (1 << 30) Not supported */
#define EPOLLONESHOT (1 << 31)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_MOD 2
#define EPOLL_CTL_DEL 3

typedef void* epoll_t;

typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  /* SOCKET */ uintptr_t sock;
  /* HANDLE */ void* hnd;
} epoll_data_t;

struct epoll_event {
  uint32_t events;   /* Epoll events */
  epoll_data_t data; /* User data variable */
};

#ifdef __cplusplus
extern "C" {
#endif

EPOLL_EXTERN epoll_t epoll_create();

EPOLL_EXTERN int epoll_close(epoll_t epoll_hnd);

EPOLL_EXTERN int epoll_ctl(epoll_t epoll_hnd,
                           int op,
                           /* SOCKET */ uintptr_t sock,
                           struct epoll_event* event);

EPOLL_EXTERN int epoll_wait(epoll_t epoll_hnd,
                            struct epoll_event* events,
                            int maxevents,
                            int timeout);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EPOLL_H_ */
