#ifndef WEPOLL_UTIL_H_
#define WEPOLL_UTIL_H_

#include <stddef.h>
#include <stdint.h>

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

/* clang-format off */
#define container_of(ptr, type, member) \
  ((type*) ((uintptr_t) (ptr) - offsetof(type, member)))
/* clang-format on */

#define unused_var(v) ((void) (v))

/* Polyfill `inline` for older versions of msvc (up to Visual Studio 2013) */
#if defined(_MSC_VER) && _MSC_VER < 1900
#define inline __inline
#endif

#endif /* WEPOLL_UTIL_H_ */
