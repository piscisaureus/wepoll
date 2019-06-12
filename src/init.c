#include <stdbool.h>
#include <stdlib.h>

#include "api.h"
#include "init.h"
#include "nt.h"
#include "reflock.h"
#include "util.h"
#include "win.h"
#include "ws.h"

static bool init__done = false;
static INIT_ONCE init__once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK init__once_callback(INIT_ONCE* once,
                                         void* parameter,
                                         void** context) {
  unused_var(once);
  unused_var(parameter);
  unused_var(context);

  /* N.b. that initialization order matters here. */
  if (ws_global_init() < 0 || nt_global_init() < 0 ||
      reflock_global_init() < 0 || epoll_global_init() < 0)
    return FALSE;

  init__done = true;
  return TRUE;
}

int init(void) {
  if (!init__done &&
      !InitOnceExecuteOnce(&init__once, init__once_callback, NULL, NULL))
    /* `InitOnceExecuteOnce()` itself is infallible, and it doesn't set any
     * error code when the once-callback returns FALSE. We return -1 here to
     * indicate that global initialization failed; the failing init function is
     * resposible for setting `errno` and calling `SetLastError()`. */
    return -1;

  return 0;
}
