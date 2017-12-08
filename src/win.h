#ifndef WEPOLL_WIN_H_
#define WEPOLL_WIN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0600
#undef _WIN32_WINNT
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#pragma warning(push, 1)

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma warning(pop)

#endif /* WEPOLL_WIN_H_ */
