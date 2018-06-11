#ifndef WEPOLL_WIN_H_
#define WEPOLL_WIN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0600

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifndef __GNUC__
#pragma warning(push, 1)
#endif

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#ifndef __GNUC__
#pragma warning(pop)
#endif

#endif /* WEPOLL_WIN_H_ */
