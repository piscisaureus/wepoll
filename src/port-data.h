#ifndef EPOLL_PORT_DATA_H_
#define EPOLL_PORT_DATA_H_

#include "afd.h"
#include "epoll-socket.h"
#include "handle-tree.h"
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

int _ep_port_add_socket(_ep_port_data_t* port_data,
                        handle_tree_entry_t* tree_entry,
                        SOCKET socket);
int _ep_port_del_socket(_ep_port_data_t* port_data,
                        handle_tree_entry_t* tree_entry);

#endif /* EPOLL_PORT_DATA_H_ */
