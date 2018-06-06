#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

void tree_init(tree_t* tree) {
  memset(tree, 0, sizeof *tree);
}

void tree_node_init(tree_node_t* node) {
  memset(node, 0, sizeof *node);
}

#define TREE__ROTATE(cis, trans)   \
  tree_node_t* p = node;           \
  tree_node_t* q = node->trans;    \
  tree_node_t* parent = p->parent; \
                                   \
  if (parent) {                    \
    if (parent->left == p)         \
      parent->left = q;            \
    else                           \
      parent->right = q;           \
  } else {                         \
    tree->root = q;                \
  }                                \
                                   \
  q->parent = parent;              \
  p->parent = q;                   \
  p->trans = q->cis;               \
  if (p->trans)                    \
    p->trans->parent = p;          \
  q->cis = p;

static inline void tree__rotate_left(tree_t* tree, tree_node_t* node) {
  TREE__ROTATE(left, right)
}

static inline void tree__rotate_right(tree_t* tree, tree_node_t* node) {
  TREE__ROTATE(right, left)
}

#define TREE__INSERT_OR_DESCEND(side) \
  if (parent->side) {                 \
    parent = parent->side;            \
  } else {                            \
    parent->side = node;              \
    break;                            \
  }

#define TREE__FIXUP_AFTER_INSERT(cis, trans) \
  tree_node_t* grandparent = parent->parent; \
  tree_node_t* uncle = grandparent->trans;   \
                                             \
  if (uncle && uncle->red) {                 \
    parent->red = uncle->red = false;        \
    grandparent->red = true;                 \
    node = grandparent;                      \
  } else {                                   \
    if (node == parent->trans) {             \
      tree__rotate_##cis(tree, parent);      \
      node = parent;                         \
      parent = node->parent;                 \
    }                                        \
    parent->red = false;                     \
    grandparent->red = true;                 \
    tree__rotate_##trans(tree, grandparent); \
  }

int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key) {
  tree_node_t* parent;

  parent = tree->root;
  if (parent) {
    for (;;) {
      if (key < parent->key) {
        TREE__INSERT_OR_DESCEND(left)
      } else if (key > parent->key) {
        TREE__INSERT_OR_DESCEND(right)
      } else {
        return -1;
      }
    }
  } else {
    tree->root = node;
  }

  node->key = key;
  node->left = node->right = NULL;
  node->parent = parent;
  node->red = true;

  for (; parent && parent->red; parent = node->parent) {
    if (parent == parent->parent->left) {
      TREE__FIXUP_AFTER_INSERT(left, right)
    } else {
      TREE__FIXUP_AFTER_INSERT(right, left)
    }
  }
  tree->root->red = false;

  return 0;
}

#define TREE__FIXUP_AFTER_REMOVE(cis, trans)       \
  tree_node_t* sibling = parent->trans;            \
                                                   \
  if (sibling->red) {                              \
    sibling->red = false;                          \
    parent->red = true;                            \
    tree__rotate_##cis(tree, parent);              \
    sibling = parent->trans;                       \
  }                                                \
  if ((sibling->left && sibling->left->red) ||     \
      (sibling->right && sibling->right->red)) {   \
    if (!sibling->trans || !sibling->trans->red) { \
      sibling->cis->red = false;                   \
      sibling->red = true;                         \
      tree__rotate_##trans(tree, sibling);         \
      sibling = parent->trans;                     \
    }                                              \
    sibling->red = parent->red;                    \
    parent->red = sibling->trans->red = false;     \
    tree__rotate_##cis(tree, parent);              \
    node = tree->root;                             \
    break;                                         \
  }                                                \
  sibling->red = true;

void tree_del(tree_t* tree, tree_node_t* node) {
  tree_node_t* parent = node->parent;
  tree_node_t* left = node->left;
  tree_node_t* right = node->right;
  tree_node_t* next;
  bool red;

  if (!left) {
    next = right;
  } else if (!right) {
    next = left;
  } else {
    next = right;
    while (next->left)
      next = next->left;
  }

  if (parent) {
    if (parent->left == node)
      parent->left = next;
    else
      parent->right = next;
  } else {
    tree->root = next;
  }

  if (left && right) {
    red = next->red;
    next->red = node->red;
    next->left = left;
    left->parent = next;
    if (next != right) {
      parent = next->parent;
      next->parent = node->parent;
      node = next->right;
      parent->left = node;
      next->right = right;
      right->parent = next;
    } else {
      next->parent = parent;
      parent = next;
      node = next->right;
    }
  } else {
    red = node->red;
    node = next;
  }

  if (node)
    node->parent = parent;
  if (red)
    return;
  if (node && node->red) {
    node->red = false;
    return;
  }

  do {
    if (node == tree->root)
      break;
    if (node == parent->left) {
      TREE__FIXUP_AFTER_REMOVE(left, right)
    } else {
      TREE__FIXUP_AFTER_REMOVE(right, left)
    }
    node = parent;
    parent = parent->parent;
  } while (!node->red);

  if (node)
    node->red = false;
}

tree_node_t* tree_find(const tree_t* tree, uintptr_t key) {
  tree_node_t* node = tree->root;
  while (node) {
    if (key < node->key)
      node = node->left;
    else if (key > node->key)
      node = node->right;
    else
      return node;
  }
  return NULL;
}

tree_node_t* tree_root(const tree_t* tree) {
  return tree->root;
}
