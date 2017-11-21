#ifndef WEPOLL_SOCK_H_
#define WEPOLL_SOCK_H_

#include <stdint.h>

#include "internal.h"
#include "queue.h"
#include "rb.h"
#include "tree.h"
#include "util.h"
#include "wepoll.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;

typedef struct ep_sock {
  tree_node_t tree_node;
  queue_node_t queue_node;
} ep_sock_t;

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

#endif /* WEPOLL_SOCK_H_ */
