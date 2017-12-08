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

#ifndef __GNUC__
#pragma warning(push, 1)
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef __GNUC__
#pragma warning(pop)
#endif

#ifndef ERROR_DEVICE_FEATURE_NOT_SUPPORTED
/* Windows headers distributed with MinGW lack a definition for this. */
#define ERROR_DEVICE_FEATURE_NOT_SUPPORTED 316L
#endif

#endif /* WEPOLL_WIN_H_ */
