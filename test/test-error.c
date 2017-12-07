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
  const HANDLE bad_value = (HANDLE) 0xbadbad;
  const HANDLE bad_type = CreateEventW(NULL, FALSE, FALSE, NULL);
  check(bad_type != NULL && bad_type != INVALID_HANDLE_VALUE);

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
    r = epoll_close(NULL);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(INVALID_HANDLE_VALUE);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(bad_value);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_close(bad_type);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
  }

  {
    /* Test epoll_ctl() errors. */
    HANDLE valid_ephnd;
    SOCKET sock_valid;
    SOCKET sock_bad = (SOCKET) 0xbadbad;
    SOCKET sock_nonsock = (SOCKET) bad_type;
    struct epoll_event ev;
    int r;

    valid_ephnd = epoll_create1(0);
    check(valid_ephnd != NULL);

    sock_valid = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    check(sock_valid != INVALID_SOCKET);

    ev.data.u64 = 0;
    ev.events = 0;

    /* Invalid `ephnd` */
    r = epoll_ctl(NULL, EPOLL_CTL_ADD, sock_valid, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(INVALID_HANDLE_VALUE, EPOLL_CTL_ADD, sock_valid, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(bad_value, EPOLL_CTL_ADD, sock_valid, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(bad_type, EPOLL_CTL_ADD, sock_valid, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);

    /* Invalid `sock` */
    r = epoll_ctl(valid_ephnd, EPOLL_CTL_ADD, 0, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(valid_ephnd, EPOLL_CTL_ADD, INVALID_SOCKET, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(valid_ephnd, EPOLL_CTL_ADD, sock_bad, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(valid_ephnd, EPOLL_CTL_ADD, sock_nonsock, &ev);
    check_error(r < 0, ENOTSOCK, WSAENOTSOCK);

    /* Invalid `op` */
    r = epoll_ctl(valid_ephnd, -1, sock_valid, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_ctl(valid_ephnd, 0, sock_valid, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_ctl(valid_ephnd, 4, sock_valid, &ev);
    check_error(r < 0, EINVAL, ERROR_INVALID_PARAMETER);

    /* Precedence - `EBADF` before `EINVAL`. */
    r = epoll_ctl(INVALID_HANDLE_VALUE, -1, sock_valid, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_ctl(valid_ephnd, -1, INVALID_SOCKET, &ev);
    check_error(r < 0, EBADF, ERROR_INVALID_HANDLE);
    /* TODO: bad `ephnd` type with invalid `sock`. */

    r = closesocket(sock_valid);
    check(r == 0);
    r = epoll_close(valid_ephnd);
    check(r == 0);
  }

  {
    /* Test epoll_wait() errors. */
    HANDLE valid_ephnd;
    struct epoll_event evs[1];
    int r;

    valid_ephnd = epoll_create1(0);
    check(valid_ephnd != NULL);

    /* Invalid `ephnd` */
    r = epoll_wait(NULL, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(INVALID_HANDLE_VALUE, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(bad_value, evs, 1, 0);
    check_error(r == -1, EBADF, ERROR_INVALID_HANDLE);
    r = epoll_wait(bad_type, evs, 1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    /* Zero `maxevents` */
    r = epoll_wait(valid_ephnd, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(NULL, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(INVALID_HANDLE_VALUE, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(bad_value, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(bad_type, evs, 0, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    /* Negative `maxevents` */
    r = epoll_wait(valid_ephnd, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(NULL, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(INVALID_HANDLE_VALUE, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(bad_value, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);
    r = epoll_wait(bad_type, evs, -1, 0);
    check_error(r == -1, EINVAL, ERROR_INVALID_PARAMETER);

    r = epoll_close(valid_ephnd);
    check(r == 0);
  }

  CloseHandle(bad_type);

  return 0;
}
