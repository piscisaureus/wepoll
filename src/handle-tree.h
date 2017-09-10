#ifndef EPOLL_HANDLE_TREE_H_
#define EPOLL_HANDLE_TREE_H_

#include "internal.h"
#include "rb.h"

typedef RB_HEAD(handle_tree, handle_tree_node) handle_tree_t;

typedef struct handle_tree_node {
  RB_ENTRY(handle_tree_node) node;
  uintptr_t key;
} handle_tree_node_t;

EPOLL_INTERNAL void handle_tree_init(handle_tree_t* tree);
EPOLL_INTERNAL void handle_tree_node_init(handle_tree_node_t* node);

EPOLL_INTERNAL int handle_tree_add(handle_tree_t* tree,
                                   handle_tree_node_t* node,
                                   uintptr_t key);
EPOLL_INTERNAL int handle_tree_del(handle_tree_t* tree,
                                   handle_tree_node_t* node);

EPOLL_INTERNAL handle_tree_node_t* handle_tree_find(handle_tree_t* tree,
                                                    uintptr_t key);
EPOLL_INTERNAL handle_tree_node_t* handle_tree_root(handle_tree_t* tree);

#endif /* EPOLL_HANDLE_TREE_H_ */
