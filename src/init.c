#include <stdbool.h>

#include "api.h"
#include "error.h"
#include "init.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

static bool _initialized = false;
static INIT_ONCE _once = INIT_ONCE_STATIC_INIT;

static int _winsock_global_init(void) {
  int r;
  WSADATA wsa_data;

  r = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (r != 0)
    return_error(-1);

  return 0;
}

static BOOL CALLBACK _init_once_callback(INIT_ONCE* once,
                                         void* parameter,
                                         void** context) {
  unused_var(once);
  unused_var(parameter);
  unused_var(context);

  if (_winsock_global_init() < 0 || nt_global_init() < 0 ||
      reflock_global_init() < 0 || api_global_init() < 0)
    return FALSE;

  _initialized = true;
  return TRUE;
}

int init(void) {
  if (!_initialized &&
      !InitOnceExecuteOnce(&_once, _init_once_callback, NULL, NULL))
    return -1; /* LastError and errno aren't touched InitOnceExecuteOnce. */

  return 0;
}
