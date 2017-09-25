#ifndef EPOLL_SOCK_H_
#define EPOLL_SOCK_H_

#include <stdint.h>

#include "epoll.h"
#include "internal.h"
#include "queue.h"
#include "rb.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;

typedef struct ep_sock {
  tree_node_t tree_node;
  queue_node_t queue_node;
} ep_sock_t;

EPOLL_INTERNAL ep_sock_t* ep_sock_new(ep_port_t* port_info, SOCKET socket);
EPOLL_INTERNAL void ep_sock_delete(ep_port_t* port_info, ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_sock_force_delete(ep_port_t* port_info,
                                         ep_sock_t* sock_info);

EPOLL_INTERNAL ep_sock_t* ep_sock_find_in_tree(tree_t* tree, SOCKET socket);
EPOLL_INTERNAL ep_sock_t* ep_sock_from_overlapped(OVERLAPPED* overlapped);

EPOLL_INTERNAL int ep_sock_set_event(ep_port_t* port_info,
                                     ep_sock_t* sock_info,
                                     const struct epoll_event* ev);

EPOLL_INTERNAL int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info);
EPOLL_INTERNAL int ep_sock_feed_event(ep_port_t* port_info,
                                      ep_sock_t* sock_info,
                                      struct epoll_event* ev);

#endif /* EPOLL_SOCK_H_ */
