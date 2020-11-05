#ifndef WEPOLL_SYNTHETIC_EVENT_H_
#define WEPOLL_SYNTHETIC_EVENT_H_

#include "config.h"
#include "wepoll.h"
#include "win.h"

typedef struct port_state port_state_t;

WEPOLL_INTERNAL int synthetic_event_post_completion(
    port_state_t* port_state, const struct epoll_event* ev);
WEPOLL_INTERNAL int synthetic_event_feed_completion(
    const OVERLAPPED_ENTRY* completion, struct epoll_event* ev);

#endif /* WEPOLL_SYNTHETIC_EVENT_H_ */
