#ifndef EPOLL_UTIL_H_
#define EPOLL_UTIL_H_

#ifndef _SSIZE_T_DEFINED
#define SSIZE_T_DEFINED
typedef intptr_t ssize_t;
#endif

#define array_count(a) (sizeof(a) / (sizeof((a)[0])))

#endif /* EPOLL_UTIL_H_ */
