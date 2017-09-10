#ifndef EPOLL_PORT_DATA_H_
#define EPOLL_PORT_DATA_H_

#include "afd.h"
#include "epoll-socket.h"
#include "handle-tree.h"
#include "internal.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct _ep_port_data _ep_port_data_t;
typedef struct ep_sock ep_sock_t;

typedef struct _ep_port_data {
  HANDLE iocp;
  SOCKET driver_sockets[array_count(AFD_PROVIDER_GUID_LIST)];
  handle_tree_t sock_tree;
  QUEUE update_queue;
  size_t poll_req_count;
} _ep_port_data_t;

SOCKET _ep_get_driver_socket(_ep_port_data_t* port_data, SOCKET socket);

EPOLL_INTERNAL int _ep_port_add_socket(_ep_port_data_t* port_data,
                                       handle_tree_entry_t* tree_entry,
                                       SOCKET socket);
EPOLL_INTERNAL int _ep_port_del_socket(_ep_port_data_t* port_data,
                                       handle_tree_entry_t* tree_entry);

EPOLL_INTERNAL void _ep_port_request_socket_update(_ep_port_data_t* port_data,
                                                   ep_sock_t* sock_info);
EPOLL_INTERNAL void _ep_port_clear_socket_update(_ep_port_data_t* port_data,
                                                 ep_sock_t* sock_info);
EPOLL_INTERNAL bool _ep_port_is_socket_update_pending(
    _ep_port_data_t* port_data, ep_sock_t* sock_info);

#endif /* EPOLL_PORT_DATA_H_ */
