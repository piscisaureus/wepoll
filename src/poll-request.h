#ifndef EPOLL_POLL_REQUEST_H_
#define EPOLL_POLL_REQUEST_H_

#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "win.h"

typedef struct _ep_port_data _ep_port_data_t;
typedef struct ep_sock ep_sock_t;
typedef struct poll_req poll_req_t;

poll_req_t* poll_req_new(_ep_port_data_t* port_data, ep_sock_t* sock_info);

void poll_req_delete(_ep_port_data_t* port_data,
                     ep_sock_t* sock_info,
                     poll_req_t* poll_req);

poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped);

ep_sock_t* poll_req_get_sock_data(const poll_req_t* poll_req);

int poll_req_submit(poll_req_t* poll_req,
                    uint32_t epoll_events,
                    SOCKET socket,
                    SOCKET driver_socket);

void poll_req_complete(const poll_req_t* poll_req,
                       uint32_t* epoll_events_out,
                       bool* socket_closed_out);

#endif /* EPOLL_POLL_REQUEST_H_ */
