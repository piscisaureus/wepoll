#include <assert.h>

#include "error.h"
#include "rb.h"
#include "tree.h"

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

  if (key == 0)
    return_error(-1, ERROR_INVALID_PARAMETER);
  if (node->key != 0)
    return_error(-1, ERROR_ALREADY_EXISTS);

  node->key = key;
  existing_node = RB_INSERT(tree, tree, node);

  if (existing_node != NULL) {
    node->key = 0;
    return_error(-1, ERROR_ALREADY_EXISTS);
  }

  return 0;
}

int tree_del(tree_t* tree, tree_node_t* node) {
  tree_node_t* removed_node;

  if (node->key == 0)
    return_error(-1, ERROR_NOT_FOUND);

  removed_node = RB_REMOVE(tree, tree, node);

  if (removed_node == NULL)
    return_error(-1, ERROR_NOT_FOUND);
  else
    assert(removed_node == node);

  node->key = 0;

  return 0;
}

tree_node_t* tree_find(tree_t* tree, uintptr_t key) {
  tree_node_t* node;
  tree_node_t lookup;

  if (key == 0)
    return_error(NULL, ERROR_INVALID_PARAMETER);

  memset(&lookup, 0, sizeof lookup);
  lookup.key = key;

  node = RB_FIND(tree, tree, &lookup);
  if (node == NULL)
    return_error(NULL, ERROR_NOT_FOUND);

  return node;
}

tree_node_t* tree_root(tree_t* tree) {
  return RB_ROOT(tree);
}
