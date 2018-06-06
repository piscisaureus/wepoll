#include <stdint.h>
#include <stdlib.h>

#include "api.h"
#include "error.h"
#include "init.h"
#include "port.h"
#include "thread-safe-tree.h"
#include "util.h"
#include "win.h"

static ts_tree_t epoll__handle_tree;

static inline port_state_t* epoll__handle_tree_node_to_port(
    ts_tree_node_t* tree_node) {
  return container_of(tree_node, port_state_t, handle_tree_node);
}

int epoll_global_init(void) {
  ts_tree_init(&epoll__handle_tree);
  return 0;
}

static HANDLE epoll__create(void) {
  port_state_t* port_state;
  HANDLE ephnd;

  if (init() < 0)
    return NULL;

  port_state = port_new(&ephnd);
  if (port_state == NULL)
    return NULL;

  if (ts_tree_add(&epoll__handle_tree,
                  &port_state->handle_tree_node,
                  (uintptr_t) ephnd) < 0) {
    /* This should never happen. */
    port_delete(port_state);
    return_set_error(NULL, ERROR_ALREADY_EXISTS);
  }

  return ephnd;
}

HANDLE epoll_create(int size) {
  if (size <= 0)
    return_set_error(NULL, ERROR_INVALID_PARAMETER);

  return epoll__create();
}

HANDLE epoll_create1(int flags) {
  if (flags != 0)
    return_set_error(NULL, ERROR_INVALID_PARAMETER);

  return epoll__create();
}

int epoll_close(HANDLE ephnd) {
  ts_tree_node_t* tree_node;
  port_state_t* port_state;

  if (init() < 0)
    return -1;

  tree_node = ts_tree_del_and_ref(&epoll__handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_state = epoll__handle_tree_node_to_port(tree_node);
  port_close(port_state);

  ts_tree_node_unref_and_destroy(tree_node);

  return port_delete(port_state);

err:
  err_check_handle(ephnd);
  return -1;
}

int epoll_ctl(HANDLE ephnd, int op, SOCKET sock, struct epoll_event* ev) {
  ts_tree_node_t* tree_node;
  port_state_t* port_state;
  int r;

  if (init() < 0)
    return -1;

  tree_node = ts_tree_find_and_ref(&epoll__handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_state = epoll__handle_tree_node_to_port(tree_node);
  r = port_ctl(port_state, op, sock, ev);

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
  port_state_t* port_state;
  int num_events;

  if (maxevents <= 0)
    return_set_error(-1, ERROR_INVALID_PARAMETER);

  if (init() < 0)
    return -1;

  tree_node = ts_tree_find_and_ref(&epoll__handle_tree, (uintptr_t) ephnd);
  if (tree_node == NULL) {
    err_set_win_error(ERROR_INVALID_PARAMETER);
    goto err;
  }

  port_state = epoll__handle_tree_node_to_port(tree_node);
  num_events = port_wait(port_state, events, maxevents, timeout);

  ts_tree_node_unref(tree_node);

  if (num_events < 0)
    goto err;

  return num_events;

err:
  err_check_handle(ephnd);
  return -1;
}
