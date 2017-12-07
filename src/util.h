#ifndef WEPOLL_UTIL_H_
#define WEPOLL_UTIL_H_

#include <stddef.h>

#include "internal.h"

#ifndef _SSIZE_T_DEFINED
#define SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

#define container_of(ptr, type, member) \
  ((type*) ((char*) (ptr) -offsetof(type, member)))

#define safe_container_of(ptr, type, member) \
  ((type*) util_safe_container_of_helper((ptr), offsetof(type, member)))

#define unused(v) ((void) (v))

#if defined(_MSC_VER) && _MSC_VER < 1900
/* Polyfill `inline` for msvc 12 (Visual Studio 2013) */
#define inline __inline
#endif

#ifdef __clang__
/* Polyfill static_assert() because clang doesn't support it. */
#define static_assert(condition, message) typedef __attribute__( \
    (unused)) int __static_assert_##__LINE__[(condition) ? 1 : -1];
#endif

WEPOLL_INTERNAL void* util_safe_container_of_helper(void* ptr, size_t offset);

#endif /* WEPOLL_UTIL_H_ */
