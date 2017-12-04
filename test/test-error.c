#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "test-util.h"
#include "util.h"
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
      /* TODO. */
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
