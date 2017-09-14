#ifndef EPOLL_POLL_REQUEST_H_
#define EPOLL_POLL_REQUEST_H_

#include <stdbool.h>
#include <stdint.h>

#include "afd.h"
#include "internal.h"
#include "win.h"

typedef struct ep_port ep_port_t;
typedef struct ep_sock ep_sock_t;
typedef struct poll_req poll_req_t;

EPOLL_INTERNAL poll_req_t* poll_req_new(ep_sock_t* sock_info);

EPOLL_INTERNAL void poll_req_delete(poll_req_t* poll_req);

EPOLL_INTERNAL poll_req_t* overlapped_to_poll_req(OVERLAPPED* overlapped);

EPOLL_INTERNAL ep_sock_t* poll_req_get_sock_data(const poll_req_t* poll_req);

EPOLL_INTERNAL int poll_req_submit(poll_req_t* poll_req,
                                   uint32_t epoll_events,
                                   SOCKET socket,
                                   SOCKET driver_socket);

EPOLL_INTERNAL int poll_req_cancel(poll_req_t* poll_req, SOCKET group_socket);
EPOLL_INTERNAL void poll_req_complete(const poll_req_t* poll_req,
                                      uint32_t* epoll_events_out,
                                      bool* socket_closed_out);

#endif /* EPOLL_POLL_REQUEST_H_ */
