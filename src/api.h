#ifndef WEPOLL_API_H_
#define WEPOLL_API_H_

#include "config.h"
#include "wepoll.h"

#define API_VALID_EPOLL_EVENTS                                         \
  (EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDNORM | \
   EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG | EPOLLRDHUP)

WEPOLL_INTERNAL int api_global_init(void);

#endif /* WEPOLL_API_H_ */
