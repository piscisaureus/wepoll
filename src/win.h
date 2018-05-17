#ifndef WEPOLL_WIN_H_
#define WEPOLL_WIN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0600
#undef _WIN32_WINNT
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifndef __GNUC__
#pragma warning(push, 1)
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef __GNUC__
#pragma warning(pop)
#endif

#endif /* WEPOLL_WIN_H_ */
