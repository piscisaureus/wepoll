#ifndef WEPOLL_WS_H_
#define WEPOLL_WS_H_

#include "internal.h"
#include "util.h"
#include "win.h"

WEPOLL_INTERNAL int ws_global_init(void);
WEPOLL_INTERNAL SOCKET ws_get_base_socket(SOCKET socket);

#endif /* WEPOLL_WS_H_ */
