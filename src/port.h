#ifndef WEPOLL_PORT_H_
#define WEPOLL_PORT_H_

#include "config.h"
#include "wepoll.h"
#include "win.h"

typedef struct port_state port_state_t;
typedef struct queue queue_t;
typedef struct sock_state sock_state_t;
typedef struct ts_tree_node ts_tree_node_t;

WEPOLL_INTERNAL port_state_t* port_new(HANDLE* iocp_handle_out);
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

WEPOLL_INTERNAL HANDLE port_get_iocp_handle(port_state_t* port_state);
WEPOLL_INTERNAL queue_t* port_get_poll_group_queue(port_state_t* port_state);

WEPOLL_INTERNAL port_state_t* port_state_from_handle_tree_node(
    ts_tree_node_t* tree_node);
WEPOLL_INTERNAL ts_tree_node_t* port_state_to_handle_tree_node(
    port_state_t* port_state);

#endif /* WEPOLL_PORT_H_ */
