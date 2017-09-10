#include <assert.h>

#include "error.h"
#include "handle-tree.h"
#include "rb.h"

static inline int _handle_tree_compare(handle_tree_node_t* a,
                                       handle_tree_node_t* b) {
  if (a->key < b->key)
    return -1;
  else if (a->key > b->key)
    return 1;
  else
    return 0;
}

RB_GENERATE_STATIC(handle_tree, handle_tree_node, node, _handle_tree_compare);

void handle_tree_init(handle_tree_t* tree) {
  RB_INIT(tree);
}

void handle_tree_node_init(handle_tree_node_t* node) {
  memset(node, 0, sizeof *node);
}

int handle_tree_add(handle_tree_t* tree,
                    handle_tree_node_t* node,
                    uintptr_t key) {
  handle_tree_node_t* existing_node;

  if (key == 0)
    return_error(-1, ERROR_INVALID_PARAMETER);
  if (node->key != 0)
    return_error(-1, ERROR_ALREADY_EXISTS);

  node->key = key;
  existing_node = RB_INSERT(handle_tree, tree, node);

  if (existing_node != NULL) {
    node->key = 0;
    return_error(-1, ERROR_ALREADY_EXISTS);
  }

  return 0;
}

int handle_tree_del(handle_tree_t* tree, handle_tree_node_t* node) {
  handle_tree_node_t* removed_node;

  if (node->key == 0)
    return_error(-1, ERROR_NOT_FOUND);

  removed_node = RB_REMOVE(handle_tree, tree, node);

  if (removed_node == NULL)
    return_error(-1, ERROR_NOT_FOUND);
  else
    assert(removed_node == node);

  node->key = 0;

  return 0;
}

handle_tree_node_t* handle_tree_find(handle_tree_t* tree, uintptr_t key) {
  handle_tree_node_t* node;
  handle_tree_node_t lookup;

  if (key == 0)
    return_error(NULL, ERROR_INVALID_PARAMETER);

  memset(&lookup, 0, sizeof lookup);
  lookup.key = key;

  node = RB_FIND(handle_tree, tree, &lookup);
  if (node == NULL)
    return_error(NULL, ERROR_NOT_FOUND);

  return node;
}

handle_tree_node_t* handle_tree_root(handle_tree_t* tree) {
  return RB_ROOT(tree);
}
