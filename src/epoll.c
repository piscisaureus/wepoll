#include <stdint.h>
#include <stdlib.h>

#include "epoll-socket.h"
#include "epoll.h"
#include "error.h"
#include "init.h"
#include "port.h"
#include "reflock-tree.h"
#include "util.h"
#include "win.h"

static reflock_tree_t _epoll_handle_tree;

static inline ep_port_t* _handle_tree_node_to_port(
    reflock_tree_node_t* tree_node) {
  return container_of(tree_node, ep_port_t, handle_tree_node);
}

int epoll_global_init(void) {
  reflock_tree_init(&_epoll_handle_tree);
  return 0;
}

HANDLE epoll_create(void) {
  ep_port_t* port_info;
  HANDLE iocp;

  if (init() < 0)
    return NULL;

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (iocp == INVALID_HANDLE_VALUE)
    return_error(NULL);

  port_info = ep_port_new(iocp);
  if (port_info == NULL) {
    CloseHandle(iocp);
    return NULL;
  }

  if (reflock_tree_add(&_epoll_handle_tree,
                       &port_info->handle_tree_node,
                       (uintptr_t) iocp) < 0) {
    ep_port_delete(port_info);
    return_error(INVALID_HANDLE_VALUE, ERROR_ALREADY_EXISTS);
  }

  return iocp;
}

int epoll_close(HANDLE ephnd) {
  reflock_tree_node_t* tree_node;
  ep_port_t* port_info;

  if (init() < 0)
    return -1;

  tree_node = reflock_tree_del_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL)
    return_error(-1, ERROR_INVALID_HANDLE);
  port_info = _handle_tree_node_to_port(tree_node);

  ep_port_close(port_info);

  reflock_tree_node_unref_and_destroy(tree_node);

  return ep_port_delete(port_info);
}

static int _ep_ctl_add(ep_port_t* port_info,
                       SOCKET sock,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_sock_new(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0) {
    ep_sock_delete(port_info, sock_info);
    return -1;
  }

  return 0;
}

static int _ep_ctl_mod(ep_port_t* port_info,
                       SOCKET sock,
                       struct epoll_event* ev) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  if (ep_sock_set_event(port_info, sock_info, ev) < 0)
    return -1;

  return 0;
}

static int _ep_ctl_del(ep_port_t* port_info, SOCKET sock) {
  ep_sock_t* sock_info = ep_port_find_socket(port_info, sock);
  if (sock_info == NULL)
    return -1;

  ep_sock_delete(port_info, sock_info);

  return 0;
}

static int _ep_ctl(ep_port_t* port_info,
                   int op,
                   SOCKET sock,
                   struct epoll_event* ev) {
  switch (op) {
    case EPOLL_CTL_ADD:
      return _ep_ctl_add(port_info, sock, ev);
    case EPOLL_CTL_MOD:
      return _ep_ctl_mod(port_info, sock, ev);
    case EPOLL_CTL_DEL:
      return _ep_ctl_del(port_info, sock);
    default:
      return_error(-1, ERROR_INVALID_PARAMETER);
  }
}

int epoll_ctl(HANDLE ephnd, int op, SOCKET sock, struct epoll_event* ev) {
  reflock_tree_node_t* tree_node;
  ep_port_t* port_info;
  int result;

  if (init() < 0)
    return -1;

  tree_node =
      reflock_tree_find_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL)
    return_error(-1, ERROR_INVALID_HANDLE);
  port_info = _handle_tree_node_to_port(tree_node);

  result = _ep_ctl(port_info, op, sock, ev);

  reflock_tree_node_unref(tree_node);

  return result;
}

int epoll_wait(HANDLE ephnd,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  reflock_tree_node_t* tree_node;
  ep_port_t* port_info;
  int result;

  if (init() < 0)
    return -1;

  tree_node =
      reflock_tree_find_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL)
    return_error(-1, ERROR_INVALID_HANDLE);
  port_info = _handle_tree_node_to_port(tree_node);

  result = ep_port_wait(port_info, events, maxevents, timeout);

  reflock_tree_node_unref(tree_node);

  return result;
}
