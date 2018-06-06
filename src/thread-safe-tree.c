#include <stdint.h>

#include "reflock.h"
#include "thread-safe-tree.h"
#include "tree.h"
#include "util.h"
#include "win.h"

void ts_tree_init(ts_tree_t* ts_tree) {
  tree_init(&ts_tree->tree);
  InitializeSRWLock(&ts_tree->lock);
}

void ts_tree_node_init(ts_tree_node_t* node) {
  tree_node_init(&node->tree_node);
  reflock_init(&node->reflock);
}

int ts_tree_add(ts_tree_t* ts_tree, ts_tree_node_t* node, uintptr_t key) {
  int r;

  AcquireSRWLockExclusive(&ts_tree->lock);
  r = tree_add(&ts_tree->tree, &node->tree_node, key);
  ReleaseSRWLockExclusive(&ts_tree->lock);

  return r;
}

static inline ts_tree_node_t* ts_tree__find_node(ts_tree_t* ts_tree,
                                                 uintptr_t key) {
  tree_node_t* tree_node = tree_find(&ts_tree->tree, key);
  if (tree_node == NULL)
    return NULL;

  return container_of(tree_node, ts_tree_node_t, tree_node);
}

ts_tree_node_t* ts_tree_del_and_ref(ts_tree_t* ts_tree, uintptr_t key) {
  ts_tree_node_t* ts_tree_node;

  AcquireSRWLockExclusive(&ts_tree->lock);

  ts_tree_node = ts_tree__find_node(ts_tree, key);
  if (ts_tree_node != NULL) {
    tree_del(&ts_tree->tree, &ts_tree_node->tree_node);
    reflock_ref(&ts_tree_node->reflock);
  }

  ReleaseSRWLockExclusive(&ts_tree->lock);

  return ts_tree_node;
}

ts_tree_node_t* ts_tree_find_and_ref(ts_tree_t* ts_tree, uintptr_t key) {
  ts_tree_node_t* ts_tree_node;

  AcquireSRWLockShared(&ts_tree->lock);

  ts_tree_node = ts_tree__find_node(ts_tree, key);
  if (ts_tree_node != NULL)
    reflock_ref(&ts_tree_node->reflock);

  ReleaseSRWLockShared(&ts_tree->lock);

  return ts_tree_node;
}

void ts_tree_node_unref(ts_tree_node_t* node) {
  reflock_unref(&node->reflock);
}

void ts_tree_node_unref_and_destroy(ts_tree_node_t* node) {
  reflock_unref_and_destroy(&node->reflock);
}
