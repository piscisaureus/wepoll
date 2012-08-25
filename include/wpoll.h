
#ifndef WPOLL_H_
#define WPOLL_H_

#include <WinSock2.h>
#include <Windows.h>

#include <stdint.h>

#define WPOLL_CTL_ADD 0
#define WPOLL_CTL_MOD 1
#define WPOLL_CTL_DEL 2

typedef void* wpoll_t;

typedef union wpoll_data {
  void    *ptr;
  int      fd;
  uint32_t u32;
  uint64_t u64;
} wpoll_data_t;

struct wpoll_event {
  uint32_t     events;    /* Epoll events */
  wpoll_data_t data;      /* User data variable */
};

wpoll_t wpoll_create();

int wpoll_close();

int wpoll_ctl(wpoll_t wpoll_hnd, int op, SOCKET sock, struct wpoll_event* event); 

int epoll_wait(wpoll_t wpoll_hnd, struct wpoll_event* events, int maxevents, int timeout);

#endif  /* WPOLL_H_ */