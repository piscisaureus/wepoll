#ifndef EPOLL_PORT_DATA_H_
#define EPOLL_PORT_DATA_H_

#include "afd.h"
#include "epoll-socket.h"
#include "epoll.h"
#include "internal.h"
#include "poll-group.h"
#include "queue.h"
#include "rb.h"
#include "reflock-tree.h"
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
  reflock_tree_node_t handle_tree_node;
  CRITICAL_SECTION lock;
} ep_port_t;

EPOLL_INTERNAL ep_port_t* ep_port_new(HANDLE iocp);
EPOLL_INTERNAL int ep_port_close(ep_port_t* port_info);
EPOLL_INTERNAL int ep_port_delete(ep_port_t* port_info);

EPOLL_INTERNAL int ep_port_update_events(ep_port_t* port_info);
EPOLL_INTERNAL size_t ep_port_feed_events(ep_port_t* port_info,
                                          OVERLAPPED_ENTRY* completion_list,
                                          size_t completion_count,
                                          struct epoll_event* event_list,
                                          size_t max_event_count);

EPOLL_INTERNAL poll_group_t* ep_port_acquire_poll_group(
    ep_port_t* port_info,
    size_t protocol_id,
    const WSAPROTOCOL_INFOW* protocol_info);
EPOLL_INTERNAL void ep_port_release_poll_group(poll_group_t* poll_group);

EPOLL_INTERNAL int ep_port_add_socket(ep_port_t* port_info,
                                      ep_sock_t* sock_info,
                                      SOCKET socket);
EPOLL_INTERNAL int ep_port_del_socket(ep_port_t* port_info,
                                      ep_sock_t* sock_info);
EPOLL_INTERNAL ep_sock_t* ep_port_find_socket(ep_port_t* port_info,
                                              SOCKET socket);

EPOLL_INTERNAL void ep_port_request_socket_update(ep_port_t* port_info,
                                                  ep_sock_t* sock_info);
EPOLL_INTERNAL void ep_port_clear_socket_update(ep_port_t* port_info,
                                                ep_sock_t* sock_info);
EPOLL_INTERNAL bool ep_port_is_socket_update_pending(ep_port_t* port_info,
                                                     ep_sock_t* sock_info);

#endif /* EPOLL_PORT_DATA_H_ */
