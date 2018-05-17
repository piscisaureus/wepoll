#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "test-util.h"
#include "tree.h"
#include "util.h"

#define NODE_COUNT 1000
static_assert(NODE_COUNT <= RAND_MAX, "NODE_COUNT too high");

typedef void (*test_op_t)(tree_t* tree, uintptr_t key);

static size_t count_subtree(const tree_node_t* node) {
  if (node == NULL)
    return 0;
  else
    return 1 + count_subtree(node->left) + count_subtree(node->right);
}

static size_t count_tree(const tree_t* tree) {
  return count_subtree(tree_root(tree));
}

static size_t check_subtree_structure(const tree_node_t* node) {
  size_t black_height_left;
  size_t black_height_right;

  if (!node)
    return 0;

  black_height_left = check_subtree_structure(node->left);
  black_height_right = check_subtree_structure(node->right);
  check(black_height_left == black_height_right);

  if (node->red) {
    check(!node->left || !node->left->red);
    check(!node->right || !node->right->red);
    return black_height_left;
  } else {
    return black_height_left + 1;
  }
}

static void check_tree_structure(const tree_t* tree) {
  check_subtree_structure(tree_root(tree));
}

static void check_tree_count(const tree_t* tree, size_t expected_count) {
  size_t count = count_tree(tree);
  check(count == expected_count);
}

static void keys_increasing(tree_t* tree, test_op_t op) {
  size_t i;
  for (i = 0; i < NODE_COUNT; i++)
    op(tree, i);
}

static void keys_decreasing(tree_t* tree, test_op_t op) {
  ssize_t i;
  for (i = NODE_COUNT - 1; i >= 0; i--)
    op(tree, (size_t) i);
}

static void keys_random(tree_t* tree, test_op_t op) {
  uintptr_t keys[NODE_COUNT];
  uintptr_t index, key;
  ssize_t left;

  for (index = 0; index < NODE_COUNT; index++)
    keys[index] = index;

  for (left = NODE_COUNT - 1; left >= 0; left--) {
    index = left == 0 ? 0 : (uintptr_t)(rand() % left);
    key = keys[index];
    keys[index] = keys[left];
    op(tree, key);
  };
}

static void add(tree_t* tree, uintptr_t key) {
  tree_node_t* node;
  size_t before_count;
  int r;

  before_count = count_tree(tree);

  node = malloc(sizeof *node);
  check(node != NULL);
  tree_node_init(node);

  r = tree_add(tree, node, key);
  check(r == 0);
  check(node->key == key);

  check_tree_structure(tree);
  check_tree_count(tree, before_count + 1);
}

static void add_error(tree_t* tree, uintptr_t key) {
  tree_node_t node;
  size_t before_count;
  int r;

  before_count = count_tree(tree);

  tree_node_init(&node);
  r = tree_add(tree, &node, key);
  check(r == -1);

  check_tree_structure(tree);
  check_tree_count(tree, before_count);
}

static void find_del(tree_t* tree, uintptr_t key) {
  tree_node_t* node;
  size_t before_count;

  before_count = count_tree(tree);

  node = tree_find(tree, key);
  check(node != NULL);
  check(node->key == key);

  tree_del(tree, node);
  free(node);

  check_tree_structure(tree);
  check_tree_count(tree, before_count - 1);
}

static void find_error(tree_t* tree, uintptr_t key) {
  tree_node_t* node;
  size_t before_count;

  before_count = count_tree(tree);

  node = tree_find(tree, key);
  check(node == NULL);

  check_tree_structure(tree);
  check_tree_count(tree, before_count);
}

int main(void) {
  tree_t tree;
  tree_init(&tree);

  keys_increasing(&tree, add);
  check_tree_count(&tree, NODE_COUNT);
  keys_increasing(&tree, add_error);
  keys_increasing(&tree, find_del);
  check_tree_count(&tree, 0);
  keys_increasing(&tree, find_error);

  keys_decreasing(&tree, add);
  check_tree_count(&tree, NODE_COUNT);
  keys_decreasing(&tree, add_error);
  keys_decreasing(&tree, find_del);
  check_tree_count(&tree, 0);
  keys_decreasing(&tree, find_error);

  keys_random(&tree, add);
  check_tree_count(&tree, NODE_COUNT);
  keys_random(&tree, add_error);
  keys_random(&tree, find_del);
  check_tree_count(&tree, 0);
  keys_random(&tree, find_error);

  keys_random(&tree, add);
  check_tree_count(&tree, NODE_COUNT);
  keys_increasing(&tree, add_error);
  keys_decreasing(&tree, find_del);
  check_tree_count(&tree, 0);
  keys_random(&tree, find_error);
}
