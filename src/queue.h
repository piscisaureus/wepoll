#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>

#include "internal.h"

typedef struct queue_node queue_node_t;

typedef struct queue_node {
  queue_node_t* prev;
  queue_node_t* next;
} queue_node_t;

typedef struct queue {
  queue_node_t head;
} queue_t;

EPOLL_INTERNAL void queue_init(queue_t* queue);
EPOLL_INTERNAL void queue_node_init(queue_node_t* node);

EPOLL_INTERNAL queue_node_t* queue_first(const queue_t* queue);
EPOLL_INTERNAL queue_node_t* queue_last(const queue_t* queue);

EPOLL_INTERNAL void queue_prepend(queue_t* queue, queue_node_t* node);
EPOLL_INTERNAL void queue_append(queue_t* queue, queue_node_t* node);
EPOLL_INTERNAL void queue_move_first(queue_t* queue, queue_node_t* node);
EPOLL_INTERNAL void queue_move_last(queue_t* queue, queue_node_t* node);
EPOLL_INTERNAL void queue_remove(queue_node_t* node);

EPOLL_INTERNAL bool queue_empty(const queue_t* queue);
EPOLL_INTERNAL bool queue_enqueued(const queue_node_t* node);

#endif /* QUEUE_H_ */
