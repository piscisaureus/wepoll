#ifndef EPOLL_SOCK_DATA_H_
#define EPOLL_SOCK_DATA_H_

#include "port-data.h"

typedef struct _ep_sock_data _ep_sock_data_t;

void _ep_sock_data_add_poll_req(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data);
void _ep_sock_data_del_poll_req(_ep_port_data_t* port_data,
                                _ep_sock_data_t* sock_data);

#endif /* EPOLL_SOCK_DATA_H_ */
