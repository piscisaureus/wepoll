#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdbool.h>
#include <stdlib.h>

#include "internal.h"

typedef struct queue_elem queue_elem_t;
typedef struct queue_elem {
  queue_elem_t* prev;
  queue_elem_t* next;
} queue_elem_t;
typedef struct queue {
  queue_elem_t head;
} queue_t;

EPOLL_INTERNAL inline void queue_elem_init(queue_elem_t* elem) {
  elem->prev = elem;
  elem->next = elem;
}

EPOLL_INTERNAL inline void queue_init(queue_t* queue) {
  queue_elem_init(&queue->head);
}

EPOLL_INTERNAL inline bool queue_enqueued(const queue_elem_t* elem) {
  return elem->prev != elem;
}

EPOLL_INTERNAL inline bool queue_empty(const queue_t* queue) {
  return !queue_enqueued(&queue->head);
}

EPOLL_INTERNAL inline queue_elem_t* queue_first(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.next : NULL;
}

EPOLL_INTERNAL inline queue_elem_t* queue_last(const queue_t* queue) {
  return !queue_empty(queue) ? queue->head.prev : NULL;
}

EPOLL_INTERNAL inline void queue_prepend(queue_t* queue, queue_elem_t* elem) {
  elem->next = queue->head.next;
  elem->prev = &queue->head;
  elem->next->prev = elem;
  queue->head.next = elem;
}

EPOLL_INTERNAL inline void queue_append(queue_t* queue, queue_elem_t* elem) {
  elem->next = &queue->head;
  elem->prev = queue->head.prev;
  elem->prev->next = elem;
  queue->head.prev = elem;
}

EPOLL_INTERNAL inline void queue_remove(queue_elem_t* elem) {
  elem->prev->next = elem->next;
  elem->next->prev = elem->prev;
  elem->prev = elem;
  elem->next = elem;
}

#endif /* QUEUE_H_ */
