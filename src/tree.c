#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"

static void _tree_rotate_left(tree_t* tree, tree_node_t* node) {
  tree_node_t* p = node;
  tree_node_t* q = node->right;
  tree_node_t* parent = p->parent;

  if (parent) {
    if (parent->left == p)
      parent->left = q;
    else
      parent->right = q;
  } else {
    tree->root = q;
  }

  q->parent = parent;
  p->parent = q;
  p->right = q->left;
  if (p->right)
    p->right->parent = p;
  q->left = p;
}

static void _tree_rotate_right(tree_t* tree, tree_node_t* node) {
  tree_node_t* p = node;
  tree_node_t* q = node->left;
  tree_node_t* parent = p->parent;

  if (parent) {
    if (parent->left == p)
      parent->left = q;
    else
      parent->right = q;
  } else {
    tree->root = q;
  }

  q->parent = parent;
  p->parent = q;
  p->left = q->right;
  if (p->left)
    p->left->parent = p;
  q->right = p;
}

void tree_init(tree_t* tree) {
  memset(tree, 0, sizeof *tree);
}

void tree_node_init(tree_node_t* node) {
  memset(node, 0, sizeof *node);
}

int tree_add(tree_t* tree, tree_node_t* node, uintptr_t key) {
  tree_node_t* parent;
  tree_node_t* grandparent;
  tree_node_t* uncle;

  parent = tree->root;
  if (parent) {
    for (;;) {
      if (key < parent->key) {
        if (parent->left) {
          parent = parent->left;
        } else {
          parent->left = node;
          break;
        }
      } else if (key > parent->key) {
        if (parent->right) {
          parent = parent->right;
        } else {
          parent->right = node;
          break;
        }
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

  while (parent && parent->red) {
    grandparent = parent->parent;
    if (parent == grandparent->left) {
      uncle = grandparent->right;
      if (uncle && uncle->red) {
        parent->red = uncle->red = false;
        grandparent->red = true;
        node = grandparent;
      } else {
        if (node == parent->right) {
          _tree_rotate_left(tree, parent);
          node = parent;
          parent = node->parent;
        }
        parent->red = false;
        grandparent->red = true;
        _tree_rotate_right(tree, grandparent);
      }
    } else {
      uncle = grandparent->left;
      if (uncle && uncle->red) {
        parent->red = uncle->red = false;
        grandparent->red = true;
        node = grandparent;
      } else {
        if (node == parent->left) {
          _tree_rotate_right(tree, parent);
          node = parent;
          parent = node->parent;
        }
        parent->red = false;
        grandparent->red = true;
        _tree_rotate_left(tree, grandparent);
      }
    }
    parent = node->parent;
  }
  tree->root->red = false;

  return 0;
}

void tree_del(tree_t* tree, tree_node_t* node) {
  tree_node_t* parent = node->parent;
  tree_node_t* left = node->left;
  tree_node_t* right = node->right;
  tree_node_t* next;
  tree_node_t* sibling;
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
      sibling = parent->right;
      if (sibling->red) {
        sibling->red = false;
        parent->red = true;
        _tree_rotate_left(tree, parent);
        sibling = parent->right;
      }
      if ((sibling->left && sibling->left->red) ||
          (sibling->right && sibling->right->red)) {
        if (!sibling->right || !sibling->right->red) {
          sibling->left->red = false;
          sibling->red = true;
          _tree_rotate_right(tree, sibling);
          sibling = parent->right;
        }
        sibling->red = parent->red;
        parent->red = sibling->right->red = false;
        _tree_rotate_left(tree, parent);
        node = tree->root;
        break;
      }
    } else {
      sibling = parent->left;
      if (sibling->red) {
        sibling->red = false;
        parent->red = true;
        _tree_rotate_right(tree, parent);
        sibling = parent->left;
      }
      if ((sibling->left && sibling->left->red) ||
          (sibling->right && sibling->right->red)) {
        if (!sibling->left || !sibling->left->red) {
          sibling->right->red = false;
          sibling->red = true;
          _tree_rotate_left(tree, sibling);
          sibling = parent->left;
        }
        sibling->red = parent->red;
        parent->red = sibling->left->red = false;
        _tree_rotate_right(tree, parent);
        node = tree->root;
        break;
      }
    }
    sibling->red = true;
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
