/*
 * wepoll - epoll for Windows
 * https://github.com/piscisaureus/wepoll
 *
 * Copyright 2012-2020, Bert Belder <bertbelder@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WEPOLL_H_
#define WEPOLL_H_
#pragma once

#ifndef WEPOLL_EXPORT
#define WEPOLL_EXPORT
#endif

#include <stdint.h>
#include <time.h>

enum EPOLL_EVENTS {
  EPOLLIN          = 0x0001,
#define EPOLLIN          EPOLLIN
  EPOLLPRI         = 0x0002,
#define EPOLLPRI         EPOLLPRI
  EPOLLOUT         = 0x0004,
#define EPOLLOUT         EPOLLOUT
  EPOLLERR         = 0x0008,
#define EPOLLERR         EPOLLERR
  EPOLLHUP         = 0x0010,
#define EPOLLHUP         EPOLLHUP
  EPOLLNVAL        = 0x0020,
#define EPOLLNVAL        EPOLLNVAL
  EPOLLRDNORM      = 0x0040,
#define EPOLLRDNORM      EPOLLRDNORM
  EPOLLRDBAND      = 0x0080,
#define EPOLLRDBAND      EPOLLRDBAND
  EPOLLWRNORM      = 0x0100,
#define EPOLLWRNORM      EPOLLWRNORM
  EPOLLWRBAND      = 0x0200,
#define EPOLLWRBAND      EPOLLWRBAND
  EPOLLMSG         = 0x0400, /* Never reported. */
#define EPOLLMSG         EPOLLMSG
  EPOLLRDHUP       = 0x2000,
#define EPOLLRDHUP       EPOLLRDHUP
  EPOLL_URING_WAKE = (unsigned)(1U << 27),
#define EPOLL_URING_WAKE EPOLL_URING_WAKE
  EPOLLEXCLUSIVE   = (unsigned)(1U << 28),
#define EPOLLEXCLUSIVE   EPOLLEXCLUSIVE
  EPOLLWAKEUP      = (unsigned)(1U << 29),
#define EPOLLWAKEUP      EPOLLWAKEUP
  EPOLLONESHOT     = (unsigned)(1U << 30),
#define EPOLLONESHOT     EPOLLONESHOT
  EPOLLET          = (unsigned)(1U << 31)
#define EPOLLET          EPOLLET
};

#define EPOLL_CTL_ADD    1
#define EPOLL_CTL_DEL    2
#define EPOLL_CTL_MOD    3

enum {
  EPOLL_CLOEXEC = 02000000 /*O_CLOEXEC*/
#define EPOLL_CLOEXEC EPOLL_CLOEXEC
};

typedef void* HANDLE;
typedef uintptr_t SOCKET;

#define _SIGSET_NWORDS (1024 / (8 * sizeof (size_t)))
typedef struct
{
  size_t __val[_SIGSET_NWORDS];
} sigset_t;

typedef union epoll_data {
  void* ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
  SOCKET sock; /* Windows specific */
  HANDLE hnd;  /* Windows specific */
} epoll_data_t;

struct epoll_event {
  uint32_t events;   /* Epoll events and flags */
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
WEPOLL_EXPORT int epoll_pwait(HANDLE ephnd,
                              struct epoll_event* events,
                              int maxevents,
                              int timeout,
                              const sigset_t* sigmask);
WEPOLL_EXPORT int epoll_pwait2(HANDLE ephnd,
                               struct epoll_event* events,
                               int maxevents,
                               const struct timespec* timeout,
                               const sigset_t* sigmask);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WEPOLL_H_ */
