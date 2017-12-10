#ifndef WEPOLL_UTIL_H_
#define WEPOLL_UTIL_H_

#include <stddef.h>

#include "internal.h"

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

#define container_of(ptr, type, member) \
  ((type*) ((char*) (ptr) -offsetof(type, member)))

#define safe_container_of(ptr, type, member) \
  ((type*) util_safe_container_of_helper((ptr), offsetof(type, member)))

#define unused_var(v) ((void) (v))

#if defined(__clang__) || defined(__GNUC__)
#define unused_fn __attribute__((__unused__))
#else
#define unused_fn /* nothing */
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
/* Polyfill `inline` for msvc 12 (Visual Studio 2013) */
#define inline __inline
#endif

#if (defined(__clang__) || defined(__GNUC__)) && !defined(static_assert)
/* Polyfill `static_assert` for some versions of clang and gcc. */
#define static_assert(condition, message) typedef __attribute__( \
    (__unused__)) int __static_assert_##__LINE__[(condition) ? 1 : -1];
#endif

WEPOLL_INTERNAL void* util_safe_container_of_helper(void* ptr, size_t offset);

#endif /* WEPOLL_UTIL_H_ */
