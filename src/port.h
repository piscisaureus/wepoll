#ifndef WEPOLL_PORT_H_
#define WEPOLL_PORT_H_

#include "afd.h"
#include "internal.h"
#include "poll-group.h"
#include "queue.h"
#include "sock.h"
#include "thread-safe-tree.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct port_state port_state_t;
typedef struct sock_state sock_state_t;

typedef struct port_state {
  HANDLE iocp;
  tree_t sock_tree;
  queue_t sock_update_queue;
  queue_t sock_deleted_queue;
  queue_t poll_group_queue;
  ts_tree_node_t handle_tree_node;
  CRITICAL_SECTION lock;
  size_t active_poll_count;
} port_state_t;

WEPOLL_INTERNAL port_state_t* port_new(HANDLE* iocp_out);
WEPOLL_INTERNAL int port_close(port_state_t* port_state);
WEPOLL_INTERNAL int port_delete(port_state_t* port_state);

WEPOLL_INTERNAL int port_wait(port_state_t* port_state,
                              struct epoll_event* events,
                              int maxevents,
                              int timeout);

WEPOLL_INTERNAL int port_ctl(port_state_t* port_state,
                             int op,
                             SOCKET sock,
                             struct epoll_event* ev);

WEPOLL_INTERNAL int port_register_socket_handle(port_state_t* port_state,
                                                sock_state_t* sock_state,
                                                SOCKET socket);
WEPOLL_INTERNAL void port_unregister_socket_handle(port_state_t* port_state,
                                                   sock_state_t* sock_state);
WEPOLL_INTERNAL sock_state_t* port_find_socket(port_state_t* port_state,
                                               SOCKET socket);

WEPOLL_INTERNAL void port_request_socket_update(port_state_t* port_state,
                                                sock_state_t* sock_state);
WEPOLL_INTERNAL void port_cancel_socket_update(port_state_t* port_state,
                                               sock_state_t* sock_state);

WEPOLL_INTERNAL void port_add_deleted_socket(port_state_t* port_state,
                                             sock_state_t* sock_state);
WEPOLL_INTERNAL void port_remove_deleted_socket(port_state_t* port_state,
                                                sock_state_t* sock_state);

#endif /* WEPOLL_PORT_H_ */
