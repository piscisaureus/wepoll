#ifndef EPOLL_HANDLE_TREE_H_
#define EPOLL_HANDLE_TREE_H_

#include "internal.h"
#include "rb.h"

typedef RB_HEAD(tree, tree_node) tree_t;

typedef struct tree_node {
  RB_ENTRY(tree_node) node;
  uintptr_t key;
} tree_node_t;

EPOLL_INTERNAL void tree_init(tree_t* tree);
EPOLL_INTERNAL void tree_node_init(tree_node_t* node);

EPOLL_INTERNAL int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key);
EPOLL_INTERNAL int tree_del(tree_t* tree, tree_node_t* node);

EPOLL_INTERNAL tree_node_t* tree_find(tree_t* tree, uintptr_t key);
EPOLL_INTERNAL tree_node_t* tree_root(tree_t* tree);

#endif /* EPOLL_HANDLE_TREE_H_ */
