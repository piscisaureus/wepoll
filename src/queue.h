#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>
#include <stdlib.h>

#include "internal.h"

typedef struct queue_node queue_node_t;
typedef struct queue_node {
  queue_node_t* prev;
  queue_node_t* next;
} queue_node_t;
typedef struct queue {
  queue_node_t head;
} queue_t;

EPOLL_INTERNAL inline void queue_node_init(queue_node_t* node) {
  node->prev = node;
  node->next = node;
}

EPOLL_INTERNAL inline void queue_init(queue_t* queue) {
  queue_node_init(&queue->head);
}

EPOLL_INTERNAL inline bool queue_enqueued(const queue_node_t* node) {
  return node->prev != node;
}

EPOLL_INTERNAL inline bool queue_empty(const queue_t* queue) {
  return !queue_enqueued(&queue->head);
}

EPOLL_INTERNAL inline queue_node_t* queue_first(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.next : NULL;
}

EPOLL_INTERNAL inline queue_node_t* queue_last(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.prev : NULL;
}

EPOLL_INTERNAL inline void queue_prepend(queue_t* queue, queue_node_t* node) {
  node->next = queue->head.next;
  node->prev = &queue->head;
  node->next->prev = node;
  queue->head.next = node;
}

EPOLL_INTERNAL inline void queue_append(queue_t* queue, queue_node_t* node) {
  node->next = &queue->head;
  node->prev = queue->head.prev;
  node->prev->next = node;
  queue->head.prev = node;
}

EPOLL_INTERNAL inline void queue_remove(queue_node_t* node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->prev = node;
  node->next = node;
}

#endif /* QUEUE_H_ */
