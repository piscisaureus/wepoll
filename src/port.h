#ifndef EPOLL_PORT_DATA_H_
#define EPOLL_PORT_DATA_H_

#include "afd.h"
#include "epoll-socket.h"
#include "handle-tree.h"
#include "internal.h"
#include "queue.h"
#include "rb.h"
#include "util.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;

typedef struct ep_port {
  HANDLE iocp;
  SOCKET driver_sockets[array_count(AFD_PROVIDER_GUID_LIST)];
  handle_tree_t sock_tree;
  queue_t update_queue;
  size_t poll_req_count;
} ep_port_t;

EPOLL_INTERNAL SOCKET ep_port_get_driver_socket(ep_port_t* port_info,
                                                SOCKET socket);

EPOLL_INTERNAL int ep_port_add_socket(ep_port_t* port_info,
                                      handle_tree_node_t* tree_node,
                                      SOCKET socket);
EPOLL_INTERNAL int ep_port_del_socket(ep_port_t* port_info,
                                      handle_tree_node_t* tree_node);

EPOLL_INTERNAL void ep_port_add_req(ep_port_t* port_info);
EPOLL_INTERNAL void ep_port_del_req(ep_port_t* port_info);

EPOLL_INTERNAL void ep_port_request_socket_update(ep_port_t* port_info,
                                                  ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_port_clear_socket_update(ep_port_t* port_info,
                                                ep_sock_t* sock_info);
EPOLL_INTERNAL bool ep_port_is_socket_update_pending(ep_port_t* port_info,
                                                     ep_sock_t* sock_info);

#endif /* EPOLL_PORT_DATA_H_ */
