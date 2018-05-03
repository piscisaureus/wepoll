#ifndef WEPOLL_TREE_H_
#define WEPOLL_TREE_H_

#include <stdbool.h>
#include <stdint.h>

#include "internal.h"
#include "util.h"

/* N.b.: the tree functions do not set errno or LastError when they fail. Each
 * of the API functions has at most one failure mode. It is up to the caller to
 * set an appropriate error code when necessary. */

typedef struct tree tree_t;
typedef struct tree_node tree_node_t;

typedef struct tree {
  tree_node_t* root;
} tree_t;

typedef struct tree_node {
  tree_node_t* left;
  tree_node_t* right;
  tree_node_t* parent;
  uintptr_t key;
  bool red;
} tree_node_t;

WEPOLL_INTERNAL void tree_init(tree_t* tree);
WEPOLL_INTERNAL void tree_node_init(tree_node_t* node);

WEPOLL_INTERNAL int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key);
WEPOLL_INTERNAL void tree_del(tree_t* tree, tree_node_t* node);

WEPOLL_INTERNAL tree_node_t* tree_find(const tree_t* tree, uintptr_t key);
WEPOLL_INTERNAL tree_node_t* tree_root(const tree_t* tree);

#endif /* WEPOLL_TREE_H_ */
