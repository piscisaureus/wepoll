#ifndef WEPOLL_SOCK_H_
#define WEPOLL_SOCK_H_

#include <stdint.h>

#include "internal.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

typedef struct port_state port_state_t;
typedef struct sock_state sock_state_t;

WEPOLL_INTERNAL sock_state_t* sock_new(port_state_t* port_state,
                                       SOCKET socket);
WEPOLL_INTERNAL void sock_delete(port_state_t* port_state,
                                 sock_state_t* sock_state);
WEPOLL_INTERNAL void sock_force_delete(port_state_t* port_state,
                                       sock_state_t* sock_state);

WEPOLL_INTERNAL int sock_set_event(port_state_t* port_state,
                                   sock_state_t* sock_state,
                                   const struct epoll_event* ev);

WEPOLL_INTERNAL int sock_update(port_state_t* port_state,
                                sock_state_t* sock_state);
WEPOLL_INTERNAL int sock_feed_event(port_state_t* port_state,
                                    OVERLAPPED* overlapped,
                                    struct epoll_event* ev);

WEPOLL_INTERNAL sock_state_t* sock_state_from_queue_node(
    queue_node_t* queue_node);
WEPOLL_INTERNAL queue_node_t* sock_state_to_queue_node(
    sock_state_t* sock_state);
WEPOLL_INTERNAL sock_state_t* sock_state_from_tree_node(
    tree_node_t* tree_node);
WEPOLL_INTERNAL tree_node_t* sock_state_to_tree_node(sock_state_t* sock_state);

#endif /* WEPOLL_SOCK_H_ */
