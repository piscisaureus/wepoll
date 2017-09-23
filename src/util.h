#ifndef EPOLL_UTIL_H_
#define EPOLL_UTIL_H_

#include <stddef.h>

#ifndef _SSIZE_T_DEFINED
#define SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

#define container_of(ptr, type, member) \
  ((type*) ((char*) (ptr) -offsetof(type, member)))

#define unused(v) ((void) (v))

#ifdef __clang__
/* Polyfill static_assert() because clang doesn't support it. */
#define static_assert(condition, message) typedef __attribute__( \
    (unused)) int __static_assert_##__LINE__[(condition) ? 1 : -1];
#endif

#endif /* EPOLL_UTIL_H_ */
