#include <assert.h>

#include "error.h"
#include "handle-tree.h"
#include "tree.h"

static inline int _handle_tree_compare(handle_tree_entry_t* a,
                                       handle_tree_entry_t* b) {
  return a->key - b->key;
}

RB_GENERATE_STATIC(handle_tree,
                   handle_tree_entry,
                   entry,
                   _handle_tree_compare);

void handle_tree_init(handle_tree_t* tree) {
  RB_INIT(tree);
}

void handle_tree_entry_init(handle_tree_entry_t* entry) {
  memset(entry, 0, sizeof *entry);
}

int handle_tree_add(handle_tree_t* tree,
                    handle_tree_entry_t* entry,
                    uintptr_t key) {
  handle_tree_entry_t* existing_entry;

  if (key == 0)
    return_error(-1, ERROR_INVALID_PARAMETER);
  if (entry->key != 0)
    return_error(-1, ERROR_ALREADY_EXISTS);

  entry->key = key;
  existing_entry = RB_INSERT(handle_tree, tree, entry);

  if (existing_entry != NULL) {
    entry->key = 0;
    return_error(-1, ERROR_ALREADY_EXISTS);
  }

  return 0;
}

int handle_tree_del(handle_tree_t* tree, handle_tree_entry_t* entry) {
  handle_tree_entry_t* removed_entry;

  if (entry->key == 0)
    return_error(-1, ERROR_NOT_FOUND);

  removed_entry = RB_REMOVE(handle_tree, tree, entry);

  if (removed_entry == NULL)
    return_error(-1, ERROR_NOT_FOUND);
  else
    assert(removed_entry == entry);

  entry->key = 0;

  return 0;
}

handle_tree_entry_t* handle_tree_find(handle_tree_t* tree, uintptr_t key) {
  handle_tree_entry_t* entry;
  handle_tree_entry_t lookup;

  if (key == 0)
    return_error(NULL, ERROR_INVALID_PARAMETER);

  memset(&lookup, 0, sizeof lookup);
  lookup.key = key;

  entry = RB_FIND(handle_tree, tree, &lookup);
  if (entry == NULL)
    return_error(NULL, ERROR_NOT_FOUND);

  return entry;
}

handle_tree_entry_t* handle_tree_root(handle_tree_t* tree) {
  return RB_ROOT(tree);
}
