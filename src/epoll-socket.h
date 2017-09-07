#ifndef EPOLL_SOCK_DATA_H_
#define EPOLL_SOCK_DATA_H_

#include <stdint.h>

#include "epoll.h"
#include "queue.h"
#include "tree.h"
#include "util.h"
#include "win.h"

typedef struct _ep_port_data _ep_port_data_t;
typedef struct poll_req poll_req_t;

typedef RB_HEAD(ep_sock_tree, ep_sock) ep_sock_tree_head_t;
typedef RB_ENTRY(ep_sock) ep_sock_tree_entry_t;

typedef struct ep_sock {
  SOCKET socket;
  SOCKET afd_socket;
  SOCKET driver_socket;
  ep_sock_tree_entry_t tree_entry;
  QUEUE queue_entry;
  epoll_data_t user_data;
  poll_req_t* latest_poll_req;
  uint32_t user_events;
  uint32_t latest_poll_req_events;
  uint32_t poll_req_count;
  uint32_t flags;
} ep_sock_t;

ep_sock_t* ep_sock_new(_ep_port_data_t* port_data);
int ep_sock_delete(_ep_port_data_t* port_data, ep_sock_t* sock_info);

int ep_sock_set_socket(_ep_port_data_t* port_data,
                       ep_sock_t* sock_info,
                       SOCKET socket);
int ep_sock_set_event(_ep_port_data_t* port_data,
                      ep_sock_t* sock_info,
                      const struct epoll_event* ev);

int ep_sock_update(_ep_port_data_t* port_data, ep_sock_t* sock_info);
int ep_sock_feed_event(_ep_port_data_t* port_data,
                       poll_req_t* poll_req,
                       struct epoll_event* ev);

void ep_sock_register_poll_req(_ep_port_data_t* port_data,
                               ep_sock_t* sock_info);
void ep_sock_unregister_poll_req(_ep_port_data_t* port_data,
                                 ep_sock_t* sock_info);

int ep_sock_compare(ep_sock_t* a, ep_sock_t* b);

RB_GENERATE_STATIC(ep_sock_tree, ep_sock, tree_entry, ep_sock_compare)

#endif /* EPOLL_SOCK_DATA_H_ */
