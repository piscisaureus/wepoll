#include <stdint.h>
#include <stdlib.h>

#include "api.h"
#include "error.h"
#include "init.h"
#include "port.h"
#include "thread-safe-tree.h"
#include "util.h"
#include "win.h"

static ts_tree_t _epoll_handle_tree;

static inline ep_port_t* _handle_tree_node_to_port(ts_tree_node_t* tree_node) {
  return container_of(tree_node, ep_port_t, handle_tree_node);
}

int api_global_init(void) {
  ts_tree_init(&_epoll_handle_tree);
  return 0;
}

static HANDLE _epoll_create(void) {
  ep_port_t* port_info;
  HANDLE ephnd;

  if (init() < 0)
    return NULL;

  port_info = ep_port_new(&ephnd);
  if (port_info == NULL)
    return NULL;

  if (ts_tree_add(&_epoll_handle_tree,
                  &port_info->handle_tree_node,
                  (uintptr_t) ephnd) < 0) {
    /* This should never happen. */
    ep_port_delete(port_info);
    return_set_error(NULL, ERROR_ALREADY_EXISTS);
  }

  return ephnd;
}

HANDLE epoll_create(int size) {
  if (size <= 0)
    return_set_error(NULL, ERROR_INVALID_PARAMETER);

  return _epoll_create();
}

HANDLE epoll_create1(int flags) {
  if (flags != 0)
    return_set_error(NULL, ERROR_INVALID_PARAMETER);

  return _epoll_create();
}

int epoll_close(HANDLE ephnd) {
  ts_tree_node_t* tree_node;
  ep_port_t* port_info;

  if (init() < 0)
    return -1;

  tree_node = ts_tree_del_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_info = _handle_tree_node_to_port(tree_node);
  ep_port_close(port_info);

  ts_tree_node_unref_and_destroy(tree_node);

  return ep_port_delete(port_info);

err:
  err_check_handle(ephnd);
  return -1;
}

int epoll_ctl(HANDLE ephnd, int op, SOCKET sock, struct epoll_event* ev) {
  ts_tree_node_t* tree_node;
  ep_port_t* port_info;
  int r;

  if (init() < 0)
    return -1;

  tree_node = ts_tree_find_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_info = _handle_tree_node_to_port(tree_node);
  r = ep_port_ctl(port_info, op, sock, ev);

  ts_tree_node_unref(tree_node);

  if (r < 0)
    goto err;

  return 0;

err:
  /* On Linux, in the case of epoll_ctl_mod(), EBADF takes priority over other
   * errors. Wepoll mimics this behavior. */
  err_check_handle(ephnd);
  err_check_handle((HANDLE) sock);
  return -1;
}

int epoll_wait(HANDLE ephnd,
               struct epoll_event* events,
               int maxevents,
               int timeout) {
  ts_tree_node_t* tree_node;
  ep_port_t* port_info;
  int num_events;

  if (maxevents <= 0)
    return_set_error(-1, ERROR_INVALID_PARAMETER);

  if (init() < 0)
    return -1;

  tree_node = ts_tree_find_and_ref(&_epoll_handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_info = _handle_tree_node_to_port(tree_node);
  num_events = ep_port_wait(port_info, events, maxevents, timeout);

  ts_tree_node_unref(tree_node);

  if (num_events < 0)
    goto err;

  return num_events;

err:
  err_check_handle(ephnd);
  return -1;
}
