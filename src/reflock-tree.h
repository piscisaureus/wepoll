#ifndef REFLOCK_TREE_H_
#define REFLOCK_TREE_H_

#include <stdint.h>

#include "internal.h"
#include "reflock.h"
#include "tree.h"
#include "win.h"

typedef struct reflock_tree {
  tree_t tree;
  SRWLOCK lock;
} reflock_tree_t;

typedef struct reflock_tree_node {
  tree_node_t tree_node;
  reflock_t reflock;
} reflock_tree_node_t;

EPOLL_INTERNAL void reflock_tree_init(reflock_tree_t* rtl);
EPOLL_INTERNAL void reflock_tree_node_init(reflock_tree_node_t* node);

EPOLL_INTERNAL int reflock_tree_add(reflock_tree_t* rlt,
                                    reflock_tree_node_t* node,
                                    uintptr_t key);

EPOLL_INTERNAL reflock_tree_node_t* reflock_tree_del_and_ref(
    reflock_tree_t* rlt, uintptr_t key);
EPOLL_INTERNAL reflock_tree_node_t* reflock_tree_find_and_ref(
    reflock_tree_t* rlt, uintptr_t key);

EPOLL_INTERNAL void reflock_tree_node_unref(reflock_tree_node_t* node);
EPOLL_INTERNAL void reflock_tree_node_unref_and_destroy(
    reflock_tree_node_t* node);

#endif /* REFLOCK_TREE_H_ */
