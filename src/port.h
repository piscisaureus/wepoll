#ifndef EPOLL_PORT_DATA_H_
#define EPOLL_PORT_DATA_H_

#include "afd.h"
#include "epoll-socket.h"
#include "internal.h"
#include "poll-group.h"
#include "queue.h"
#include "rb.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;

typedef struct ep_port {
  HANDLE iocp;
  poll_group_allocator_t*
      poll_group_allocators[array_count(AFD_PROVIDER_GUID_LIST)];
  tree_t sock_tree;
  queue_t update_queue;
  size_t poll_req_count;
} ep_port_t;

EPOLL_INTERNAL ep_port_t* ep_port_new(HANDLE iocp);
EPOLL_INTERNAL int ep_port_delete(ep_port_t* port_info);

EPOLL_INTERNAL poll_group_t* ep_port_acquire_poll_group(ep_port_t* port_info,
                                                        SOCKET socket);
EPOLL_INTERNAL void ep_port_release_poll_group(poll_group_t* poll_group);

EPOLL_INTERNAL int ep_port_add_socket(ep_port_t* port_info,
                                      tree_node_t* tree_node,
                                      SOCKET socket);
EPOLL_INTERNAL int ep_port_del_socket(ep_port_t* port_info,
                                      tree_node_t* tree_node);

EPOLL_INTERNAL void ep_port_add_req(ep_port_t* port_info);
EPOLL_INTERNAL void ep_port_del_req(ep_port_t* port_info);

EPOLL_INTERNAL void ep_port_request_socket_update(ep_port_t* port_info,
                                                  ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_port_clear_socket_update(ep_port_t* port_info,
                                                ep_sock_t* sock_info);
EPOLL_INTERNAL bool ep_port_is_socket_update_pending(ep_port_t* port_info,
                                                     ep_sock_t* sock_info);

#endif /* EPOLL_PORT_DATA_H_ */
