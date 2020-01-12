#ifndef WEPOLL_POLL_GROUP_H_
#define WEPOLL_POLL_GROUP_H_

#include "config.h"
#include "win.h"

typedef struct poll_group poll_group_t;
typedef struct port_state port_state_t;
typedef struct queue_node queue_node_t;

WEPOLL_INTERNAL poll_group_t* poll_group_acquire(port_state_t* port);
WEPOLL_INTERNAL void poll_group_release(poll_group_t* poll_group);

WEPOLL_INTERNAL void poll_group_delete(poll_group_t* poll_group);

WEPOLL_INTERNAL poll_group_t* poll_group_from_queue_node(
    queue_node_t* queue_node);
WEPOLL_INTERNAL HANDLE
    poll_group_get_afd_helper_handle(poll_group_t* poll_group);

#endif /* WEPOLL_POLL_GROUP_H_ */
