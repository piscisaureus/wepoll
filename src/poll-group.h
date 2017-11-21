#ifndef WEPOLL_POLL_GROUP_H_
#define WEPOLL_POLL_GROUP_H_

#include "error.h"
#include "internal.h"
#include "queue.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct poll_group_allocator poll_group_allocator_t;
typedef struct poll_group poll_group_t;

WEPOLL_INTERNAL poll_group_allocator_t* poll_group_allocator_new(
    ep_port_t* port_info, const WSAPROTOCOL_INFOW* protocol_info);
WEPOLL_INTERNAL void poll_group_allocator_delete(poll_group_allocator_t* pga);

WEPOLL_INTERNAL poll_group_t* poll_group_acquire(poll_group_allocator_t* pga);
WEPOLL_INTERNAL void poll_group_release(poll_group_t* ds);

WEPOLL_INTERNAL SOCKET poll_group_get_socket(poll_group_t* poll_group);

#endif /* WEPOLL_POLL_GROUP_H_ */
