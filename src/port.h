#ifndef WEPOLL_PORT_H_
#define WEPOLL_PORT_H_

#include "afd.h"
#include "internal.h"
#include "poll-group.h"
#include "queue.h"
#include "rb.h"
#include "reflock-tree.h"
#include "sock.h"
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
  queue_t sock_update_queue;
  queue_t sock_deleted_queue;
  reflock_tree_node_t handle_tree_node;
  CRITICAL_SECTION lock;
  size_t active_poll_count;
} ep_port_t;

WEPOLL_INTERNAL ep_port_t* ep_port_new(HANDLE* iocp_out);
WEPOLL_INTERNAL int ep_port_close(ep_port_t* port_info);
WEPOLL_INTERNAL int ep_port_delete(ep_port_t* port_info);

WEPOLL_INTERNAL int ep_port_wait(ep_port_t* port_info,
                                 struct epoll_event* events,
                                 int maxevents,
                                 int timeout);

WEPOLL_INTERNAL int ep_port_ctl(ep_port_t* port_info,
                                int op,
                                SOCKET sock,
                                struct epoll_event* ev);

WEPOLL_INTERNAL poll_group_t* ep_port_acquire_poll_group(
    ep_port_t* port_info,
    size_t protocol_id,
    const WSAPROTOCOL_INFOW* protocol_info);
WEPOLL_INTERNAL void ep_port_release_poll_group(ep_port_t* port_info,
                                                poll_group_t* poll_group);

WEPOLL_INTERNAL int ep_port_register_socket_handle(ep_port_t* port_info,
                                                   ep_sock_t* sock_info,
                                                   SOCKET socket);
WEPOLL_INTERNAL int ep_port_unregister_socket_handle(ep_port_t* port_info,
                                                     ep_sock_t* sock_info);
WEPOLL_INTERNAL ep_sock_t* ep_port_find_socket(ep_port_t* port_info,
                                               SOCKET socket);

WEPOLL_INTERNAL void ep_port_request_socket_update(ep_port_t* port_info,
                                                   ep_sock_t* sock_info);
WEPOLL_INTERNAL void ep_port_cancel_socket_update(ep_port_t* port_info,
                                                  ep_sock_t* sock_info);

WEPOLL_INTERNAL void ep_port_add_deleted_socket(ep_port_t* port_info,
                                                ep_sock_t* sock_info);
WEPOLL_INTERNAL void ep_port_remove_deleted_socket(ep_port_t* port_info,
                                                   ep_sock_t* sock_info);

#endif /* WEPOLL_PORT_H_ */
