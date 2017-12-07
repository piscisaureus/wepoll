#include <assert.h>

#include "error.h"
#include "rb.h"
#include "tree.h"
#include "util.h"

static inline int _tree_compare(tree_node_t* a, tree_node_t* b) {
  if (a->key < b->key)
    return -1;
  else if (a->key > b->key)
    return 1;
  else
    return 0;
}

RB_GENERATE_STATIC(tree, tree_node, node, _tree_compare);

void tree_init(tree_t* tree) {
  RB_INIT(tree);
}

void tree_node_init(tree_node_t* node) {
  memset(node, 0, sizeof *node);
}

int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key) {
  tree_node_t* existing_node;

  node->key = key;
  existing_node = RB_INSERT(tree, tree, node);

  if (existing_node != NULL)
    return -1;

  return 0;
}

int tree_del(tree_t* tree, tree_node_t* node) {
  tree_node_t* removed_node;

  removed_node = RB_REMOVE(tree, tree, node);

  if (removed_node == NULL)
    return -1;
  else
    assert(removed_node == node);

  return 0;
}

tree_node_t* tree_find(tree_t* tree, uintptr_t key) {
  tree_node_t lookup;

  memset(&lookup, 0, sizeof lookup);
  lookup.key = key;

  return RB_FIND(tree, tree, &lookup);
}

tree_node_t* tree_root(tree_t* tree) {
  return RB_ROOT(tree);
}
