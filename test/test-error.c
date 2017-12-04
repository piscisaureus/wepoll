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
    /* TODO. */
  }

  CloseHandle(bad_type);

  return 0;
}
