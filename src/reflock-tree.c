#include <stdint.h>

#include "reflock-tree.h"
#include "reflock.h"
#include "tree.h"
#include "util.h"
#include "win.h"

void reflock_tree_init(reflock_tree_t* rlt) {
  tree_init(&rlt->tree);
  InitializeSRWLock(&rlt->lock);
}

void reflock_tree_node_init(reflock_tree_node_t* node) {
  tree_node_init(&node->tree_node);
  reflock_init(&node->reflock);
}

int reflock_tree_add(reflock_tree_t* rlt,
                     reflock_tree_node_t* node,
                     uintptr_t key) {
  int r;

  AcquireSRWLockExclusive(&rlt->lock);
  r = tree_add(&rlt->tree, &node->tree_node, key);
  ReleaseSRWLockExclusive(&rlt->lock);

  return r;
}

reflock_tree_node_t* reflock_tree_del_and_ref(reflock_tree_t* rlt,
                                              uintptr_t key) {
  tree_node_t* tree_node;
  reflock_tree_node_t* rlt_node;

  AcquireSRWLockExclusive(&rlt->lock);

  tree_node = tree_find(&rlt->tree, key);
  rlt_node = safe_container_of(tree_node, reflock_tree_node_t, tree_node);

  if (rlt_node != NULL) {
    tree_del(&rlt->tree, tree_node);
    reflock_ref(&rlt_node->reflock);
  }

  ReleaseSRWLockExclusive(&rlt->lock);

  return rlt_node;
}

reflock_tree_node_t* reflock_tree_find_and_ref(reflock_tree_t* rlt,
                                               uintptr_t key) {
  tree_node_t* tree_node;
  reflock_tree_node_t* rlt_node;

  AcquireSRWLockShared(&rlt->lock);

  tree_node = tree_find(&rlt->tree, key);
  rlt_node = safe_container_of(tree_node, reflock_tree_node_t, tree_node);
  if (rlt_node != NULL)
    reflock_ref(&rlt_node->reflock);

  ReleaseSRWLockShared(&rlt->lock);

  return rlt_node;
}

void reflock_tree_node_unref(reflock_tree_node_t* node) {
  reflock_unref(&node->reflock);
}

void reflock_tree_node_unref_and_destroy(reflock_tree_node_t* node) {
  reflock_unref_and_destroy(&node->reflock);
}
