#ifndef EPOLL_HANDLE_TREE_H_
#define EPOLL_HANDLE_TREE_H_

#include "tree.h"

typedef RB_HEAD(handle_tree, handle_tree_entry) handle_tree_t;

typedef struct handle_tree_entry {
  RB_ENTRY(handle_tree_entry) entry;
  uintptr_t key;
} handle_tree_entry_t;

void handle_tree_init(handle_tree_t* tree);
void handle_tree_entry_init(handle_tree_entry_t* entry);

int handle_tree_add(handle_tree_t* tree,
                    handle_tree_entry_t* entry,
                    uintptr_t key);
int handle_tree_del(handle_tree_t* tree, handle_tree_entry_t* entry);

handle_tree_entry_t* handle_tree_find(handle_tree_t* tree, uintptr_t key);
handle_tree_entry_t* handle_tree_root(handle_tree_t* tree);

#endif /* EPOLL_HANDLE_TREE_H_ */
