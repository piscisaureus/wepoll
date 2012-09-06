
#ifndef WPOLL_H_
#define WPOLL_H_

#include <WinSock2.h>
#include <Windows.h>

#include <stdint.h>


#define WPOLLIN 0x001
#define WPOLLPRI 0x002
#define WPOLLOUT 0x004
#define WPOLLERR 0x008
#define WPOLLHUP 0x010
#define WPOLLRDNORM 0x040
#define WPOLLRDBAND 0x080
#define WPOLLWRNORM 0x100
#define WPOLLMSG 0x400
#define WPOLLWRBAND 0x200
#define WPOLLRDHUP 0x2000

#define WPOLL_EVENT_MASK 0xffff

/* #define WPOLLET (1 << 30) Not supported */
#define WPOLLONESHOT (1 << 31)


#define WPOLL_CTL_ADD 1
#define WPOLL_CTL_MOD 2
#define WPOLL_CTL_DEL 3

typedef void* wpoll_t;

typedef union wpoll_data {
  void    *ptr;
  int      fd;
  uint32_t u32;
  uint64_t u64;
  /* Windows-specific extensions. */
  SOCKET sock;
  HANDLE hnd;
} wpoll_data_t;

struct wpoll_event {
  uint32_t     events;    /* Epoll events */
  wpoll_data_t data;      /* User data variable */
};

wpoll_t wpoll_create();

int wpoll_close(wpoll_t wpoll_hnd);

int wpoll_ctl(wpoll_t wpoll_hnd, int op, SOCKET sock, struct wpoll_event* event); 

int wpoll_wait(wpoll_t wpoll_hnd, struct wpoll_event* events, int maxevents, int timeout);

int afd_init();


#endif  /* WPOLL_H_ */