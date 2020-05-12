#ifndef WEPOLL_QUEUE_H_
#define WEPOLL_QUEUE_H_

#include <stdbool.h>

#include "config.h"

typedef struct queue_node queue_node_t;

typedef struct queue_node {
  queue_node_t* prev;
  queue_node_t* next;
} queue_node_t;

typedef struct queue {
  queue_node_t head;
} queue_t;

WEPOLL_INTERNAL void queue_init(queue_t* queue);
WEPOLL_INTERNAL void queue_node_init(queue_node_t* node);

WEPOLL_INTERNAL queue_node_t* queue_first(const queue_t* queue);
WEPOLL_INTERNAL queue_node_t* queue_last(const queue_t* queue);

WEPOLL_INTERNAL void queue_prepend(queue_t* queue, queue_node_t* node);
WEPOLL_INTERNAL void queue_append(queue_t* queue, queue_node_t* node);
WEPOLL_INTERNAL void queue_move_to_start(queue_t* queue, queue_node_t* node);
WEPOLL_INTERNAL void queue_move_to_end(queue_t* queue, queue_node_t* node);
WEPOLL_INTERNAL void queue_remove(queue_node_t* node);

WEPOLL_INTERNAL bool queue_is_empty(const queue_t* queue);
WEPOLL_INTERNAL bool queue_is_enqueued(const queue_node_t* node);

#endif /* WEPOLL_QUEUE_H_ */
