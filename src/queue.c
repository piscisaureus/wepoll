#include <stdbool.h>
#include <stdlib.h>

#include "queue.h"
#include "util.h"

void queue_init(queue_t* queue) {
  queue_node_init(&queue->head);
}

void queue_node_init(queue_node_t* node) {
  node->prev = node;
  node->next = node;
}

static inline void queue__detach_node(queue_node_t* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
}

queue_node_t* queue_first(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.next : NULL;
}

queue_node_t* queue_last(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.prev : NULL;
}

void queue_prepend(queue_t* queue, queue_node_t* node) {
  node->next = queue->head.next;
  node->prev = &queue->head;
  node->next->prev = node;
  queue->head.next = node;
}

void queue_append(queue_t* queue, queue_node_t* node) {
  node->next = &queue->head;
  node->prev = queue->head.prev;
  node->prev->next = node;
  queue->head.prev = node;
}

void queue_move_first(queue_t* queue, queue_node_t* node) {
  queue__detach_node(node);
  queue_prepend(queue, node);
}

void queue_move_last(queue_t* queue, queue_node_t* node) {
  queue__detach_node(node);
  queue_append(queue, node);
}

void queue_remove(queue_node_t* node) {
  queue__detach_node(node);
  queue_node_init(node);
}

bool queue_empty(const queue_t* queue) {
  return !queue_enqueued(&queue->head);
}

bool queue_enqueued(const queue_node_t* node) {
  return node->prev != node;
}
