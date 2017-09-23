#include <stdbool.h>

#include "error.h"
#include "init.h"
#include "nt.h"
#include "reflock.h"
#include "win.h"

static bool _initialized = false;

static int _winsock_global_init(void) {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return_error(-1);

  return 0;
}

static int _init_once(void) {
  if (_winsock_global_init() < 0 || nt_global_init() < 0 ||
      reflock_global_init() < 0)
    return -1;

  _initialized = true;
  return 0;
}

int init(void) {
  if (_initialized)
    return 0;

  return _init_once();
}
