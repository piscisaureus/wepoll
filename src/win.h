#ifndef WEPOLL_WIN_H_
#define WEPOLL_WIN_H_

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined(_MSC_VER)
#pragma warning(push, 1)
#endif

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

/* clang-format off */
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
/* clang-format on */

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif /* WEPOLL_WIN_H_ */
