#ifndef WEPOLL_WS_H_
#define WEPOLL_WS_H_

#include "internal.h"
#include "util.h"
#include "win.h"

WEPOLL_INTERNAL int ws_global_init(void);

WEPOLL_INTERNAL SOCKET ws_get_base_socket(SOCKET socket);
WEPOLL_INTERNAL int ws_get_protocol_catalog(WSAPROTOCOL_INFOW** infos_out,
                                            size_t* infos_count_out);

#endif /* WEPOLL_WS_H_ */
