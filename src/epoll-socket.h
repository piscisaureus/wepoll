#ifndef EPOLL_SOCK_DATA_H_
#define EPOLL_SOCK_DATA_H_

#include <stdint.h>

#include "epoll.h"
#include "handle-tree.h"
#include "internal.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct poll_req poll_req_t;

typedef struct ep_sock {
  handle_tree_entry_t tree_entry;
  QUEUE queue_entry;
} ep_sock_t;

EPOLL_INTERNAL ep_sock_t* ep_sock_new(ep_port_t* port_info);
EPOLL_INTERNAL int ep_sock_delete(ep_port_t* port_info, ep_sock_t* sock_info);

EPOLL_INTERNAL int ep_sock_set_socket(ep_port_t* port_info,
                                      ep_sock_t* sock_info,
                                      SOCKET socket);
EPOLL_INTERNAL int ep_sock_set_event(ep_port_t* port_info,
                                     ep_sock_t* sock_info,
                                     const struct epoll_event* ev);

EPOLL_INTERNAL int ep_sock_update(ep_port_t* port_info, ep_sock_t* sock_info);
EPOLL_INTERNAL int ep_sock_feed_event(ep_port_t* port_info,
                                      poll_req_t* poll_req,
                                      struct epoll_event* ev);

EPOLL_INTERNAL void ep_sock_register_poll_req(ep_port_t* port_info,
                                              ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_sock_unregister_poll_req(ep_port_t* port_info,
                                                ep_sock_t* sock_info);

EPOLL_INTERNAL ep_sock_t* ep_sock_from_tree_entry(
    handle_tree_entry_t* tree_entry);

#endif /* EPOLL_SOCK_DATA_H_ */
