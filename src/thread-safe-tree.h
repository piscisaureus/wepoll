#ifndef WEPOLL_THREAD_SAFE_TREE_H_
#define WEPOLL_THREAD_SAFE_TREE_H_

#include <stdint.h>

#include "internal.h"
#include "reflock.h"
#include "tree.h"
#include "win.h"

typedef struct ts_tree {
  tree_t tree;
  SRWLOCK lock;
} ts_tree_t;

typedef struct ts_tree_node {
  tree_node_t tree_node;
  reflock_t reflock;
} ts_tree_node_t;

WEPOLL_INTERNAL void ts_tree_init(ts_tree_t* rtl);
WEPOLL_INTERNAL void ts_tree_node_init(ts_tree_node_t* node);

WEPOLL_INTERNAL int ts_tree_add(ts_tree_t* ts_tree,
                                ts_tree_node_t* node,
                                uintptr_t key);

WEPOLL_INTERNAL ts_tree_node_t* ts_tree_del_and_ref(ts_tree_t* ts_tree,
                                                    uintptr_t key);
WEPOLL_INTERNAL ts_tree_node_t* ts_tree_find_and_ref(ts_tree_t* ts_tree,
                                                     uintptr_t key);

WEPOLL_INTERNAL void ts_tree_node_unref(ts_tree_node_t* node);
WEPOLL_INTERNAL void ts_tree_node_unref_and_destroy(ts_tree_node_t* node);

#endif /* WEPOLL_THREAD_SAFE_TREE_H_ */
