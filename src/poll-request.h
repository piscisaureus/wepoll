#ifndef EPOLL_POLL_REQUEST_H_
#define EPOLL_POLL_REQUEST_H_

#include <stdint.h>
#include <stdbool.h>

#include "afd.h"
#include "port-data.h"
#include "sock-data.h"
#include "win.h"

typedef struct poll_req poll_req_t;

poll_req_t* poll_req_new(_ep_port_data_t* port_data,
                         _ep_sock_data_t* sock_data);

void poll_req_delete(_ep_port_data_t* port_data,
                     _ep_sock_data_t* sock_data,
                     poll_req_t* poll_req);

poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped);

_ep_sock_data_t* poll_req_get_sock_data(const poll_req_t* poll_req);

int poll_req_submit(poll_req_t* poll_req,
                    uint32_t epoll_events,
                    SOCKET socket,
                    SOCKET driver_socket);

void poll_req_complete(const poll_req_t* poll_req,
                       uint32_t* epoll_events_out,
                       bool* socket_closed_out);

#endif /* EPOLL_POLL_REQUEST_H_ */
