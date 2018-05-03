#ifndef WEPOLL_SOCK_H_
#define WEPOLL_SOCK_H_

#include <stdint.h>

#include "internal.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;

WEPOLL_INTERNAL ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket);
WEPOLL_INTERNAL void ep_sock_delete(ep_port_t* port_info,
                                    ep_sock_t* sock_info);
WEPOLL_INTERNAL void ep_sock_force_delete(ep_port_t* port_info,
                                          ep_sock_t* sock_info);

WEPOLL_INTERNAL int ep_sock_set_event(ep_port_t* port_info,
                                      ep_sock_t* sock_info,
                                      const struct epoll_event* ev);

WEPOLL_INTERNAL int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info);
WEPOLL_INTERNAL int ep_sock_feed_event(ep_port_t* port_info,
                                       OVERLAPPED* overlapped,
                                       struct epoll_event* ev);

WEPOLL_INTERNAL ep_sock_t* ep_sock_from_queue_node(queue_node_t* queue_node);
WEPOLL_INTERNAL queue_node_t* ep_sock_to_queue_node(ep_sock_t* sock_info);
WEPOLL_INTERNAL ep_sock_t* ep_sock_from_tree_node(tree_node_t* tree_node);
WEPOLL_INTERNAL tree_node_t* ep_sock_to_tree_node(ep_sock_t* sock_info);

#endif /* WEPOLL_SOCK_H_ */
