#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "util.h"
#include "win.h"

static const size_t POLL_GROUP__MAX_GROUP_SIZE = 32;

typedef struct poll_group {
  port_state_t* port_state;
  queue_node_t queue_node;
  HANDLE afd_helper_handle;
  size_t group_size;
} poll_group_t;

static poll_group_t* poll_group__new(port_state_t* port_state) {
  poll_group_t* poll_group = malloc(sizeof *poll_group);
  if (poll_group == NULL)
    return_set_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  memset(poll_group, 0, sizeof *poll_group);

  queue_node_init(&poll_group->queue_node);
  poll_group->port_state = port_state;

  if (afd_create_helper_handle(port_state->iocp,
                               &poll_group->afd_helper_handle) < 0) {
    free(poll_group);
    return NULL;
  }

  queue_append(&port_state->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_delete(poll_group_t* poll_group) {
  assert(poll_group->group_size == 0);
  CloseHandle(poll_group->afd_helper_handle);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

poll_group_t* poll_group_from_queue_node(queue_node_t* queue_node) {
  return container_of(queue_node, poll_group_t, queue_node);
}

HANDLE poll_group_get_afd_helper_handle(poll_group_t* poll_group) {
  return poll_group->afd_helper_handle;
}

poll_group_t* poll_group_acquire(port_state_t* port_state) {
  queue_t* queue = &port_state->poll_group_queue;
  poll_group_t* poll_group =
      !queue_empty(queue)
          ? container_of(queue_last(queue), poll_group_t, queue_node)
          : NULL;

  if (poll_group == NULL ||
      poll_group->group_size >= POLL_GROUP__MAX_GROUP_SIZE)
    poll_group = poll_group__new(port_state);
  if (poll_group == NULL)
    return NULL;

  if (++poll_group->group_size == POLL_GROUP__MAX_GROUP_SIZE)
    queue_move_first(&port_state->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_release(poll_group_t* poll_group) {
  port_state_t* port_state = poll_group->port_state;

  poll_group->group_size--;
  assert(poll_group->group_size < POLL_GROUP__MAX_GROUP_SIZE);

  queue_move_last(&port_state->poll_group_queue, &poll_group->queue_node);

  /* Poll groups are currently only freed when the epoll port is closed. */
}
