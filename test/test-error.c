#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "test-util.h"
#include "wepoll.h"
#include "win.h"

static void check_error(bool did_fail,
                        errno_t errno_value,
                        DWORD last_error_value) {
  check(did_fail);
  check(errno == errno_value);
  check(GetLastError() == last_error_value);
}

int main(void) {
  HANDLE event_handle;
  HANDLE ep_null, ep_hinv, ep_badv, ep_type;
  SOCKET sock_null, sock_hinv, sock_badv, sock_type;

  event_handle = CreateEventW(NULL, FALSE, FALSE, NULL);
  check(event_handle != NULL && event_handle != INVALID_HANDLE_VALUE);

  ep_null = NULL;
  ep_hinv = INVALID_HANDLE_VALUE;
  ep_badv = (HANDLE) 0xbadbad;
  ep_type = event_handle;

  sock_null = (SOCKET) NULL;
  sock_hinv = INVALID_SOCKET;
  sock_badv = (SOCKET) 0xbadbad;
  sock_type = (SOCKET) event_handle;

  {
    /* Test epoll_create() errors. */
    HANDLE h;
    h = epoll_create(-1);
    check_error(h == NULL, EINVAL, ERROR_INVALID_PARAMETER);
    h = epoll_create(0);
    check_error(h == NULL, EINVAL, ERROR_INVALID_PARAMETER);
  }

  {
    /* Test epoll_create1() errors. */
    HANDLE h;
    h = epoll_create1(-1);
    check_error(h == NULL, EINVAL, ERROR_INVALID_PARAMETER);
    h = epoll_create1(19);
    check_error(h == NULL, EINVAL, ERROR_INVALID_PARAMETER);
  }

  {
    /* Test epoll_close() errors. */
    int r;
    r = epoll_close(ep_null);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(ep_hinv);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(ep_badv);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(ep_type);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
  }

  {
    /* Test epoll_ctl() errors. */
    HANDLE ep_good;
    SOCKET sock_good;
    struct epoll_event ev;
    int r;

    ep_good = epoll_create1(0);
    check(ep_good != ep_null);

    sock_good = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    check(sock_good != sock_hinv);

    ev.data.u64 = 0;
    ev.events = 0;

    /* Invalid `ephnd` */
    r = epoll_ctl(ep_null, EPOLL_CTL_ADD, sock_good, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_hinv, EPOLL_CTL_ADD, sock_good, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_badv, EPOLL_CTL_ADD, sock_good, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_type, EPOLL_CTL_ADD, sock_good, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);

    /* Invalid `sock` */
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_null, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_hinv, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_badv, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_type, &ev);
    check_error(r < 0, ENOTSOCK, WSAENOTSOCK);

    /* Invalid `op` */
    r = epoll_ctl(ep_good, -1, sock_good, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_ctl(ep_good, 0, sock_good, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_ctl(ep_good, 4, sock_good, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);

    /* Precedence - `EBADF` before `EINVAL`. */
    r = epoll_ctl(ep_hinv, -1, sock_good, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_good, -1, sock_hinv, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(ep_type, -1, sock_hinv, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);

    /* Socket already in epoll set. */
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_good, &ev);
    check(r == 0);
    r = epoll_ctl(ep_good, EPOLL_CTL_ADD, sock_good, &ev);
    check_error(r < 0, EEXIST, ERROR_ALREADY_EXISTS);

    /* Socket not in epoll set. */
    r = epoll_ctl(ep_good, EPOLL_CTL_DEL, sock_good, NULL);
    check(r == 0);
    r = epoll_ctl(ep_good, EPOLL_CTL_MOD, sock_good, &ev);
    check_error(r < 0, ENOENT, ERROR_NOT_FOUND);
    r = epoll_ctl(ep_good, EPOLL_CTL_DEL, sock_good, NULL);
    check_error(r < 0, ENOENT, ERROR_NOT_FOUND);

    r = closesocket(sock_good);
    check(r == 0);
    r = epoll_close(ep_good);
    check(r == 0);
  }

  {
    /* Test epoll_wait() errors. */
    HANDLE ep_good;
    struct epoll_event evs[1];
    int r;

    ep_good = epoll_create1(0);
    check(ep_good != ep_null);

    /* Invalid `ephnd` */
    r = epoll_wait(ep_null, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(ep_hinv, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(ep_badv, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(ep_type, evs, 1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    /* Zero `maxevents` */
    r = epoll_wait(ep_good, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_null, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_hinv, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_badv, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_type, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    /* Negative `maxevents` */
    r = epoll_wait(ep_good, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_null, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_hinv, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_badv, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(ep_type, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    r = epoll_close(ep_good);
    check(r == 0);
  }

  CloseHandle(event_handle);

  return 0;
}
