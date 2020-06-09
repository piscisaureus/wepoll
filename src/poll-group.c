#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "afd.h"
#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "queue.h"
#include "util.h"
#include "win.h"

#define POLL_GROUP__MAX_GROUP_SIZE 32

typedef struct poll_group {
  port_state_t* port_state;
  queue_node_t queue_node;
  HANDLE afd_device_handle;
  size_t group_size;
} poll_group_t;

static poll_group_t* poll_group__new(port_state_t* port_state) {
  HANDLE iocp_handle = port_get_iocp_handle(port_state);
  queue_t* poll_group_queue = port_get_poll_group_queue(port_state);

  poll_group_t* poll_group = malloc(sizeof *poll_group);
  if (poll_group == NULL)
    return_set_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  memset(poll_group, 0, sizeof *poll_group);

  queue_node_init(&poll_group->queue_node);
  poll_group->port_state = port_state;

  if (afd_create_device_handle(iocp_handle, &poll_group->afd_device_handle) <
      0) {
    free(poll_group);
    return NULL;
  }

  queue_append(poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_delete(poll_group_t* poll_group) {
  assert(poll_group->group_size == 0);
  CloseHandle(poll_group->afd_device_handle);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

poll_group_t* poll_group_from_queue_node(queue_node_t* queue_node) {
  return container_of(queue_node, poll_group_t, queue_node);
}

HANDLE poll_group_get_afd_device_handle(poll_group_t* poll_group) {
  return poll_group->afd_device_handle;
}

poll_group_t* poll_group_acquire(port_state_t* port_state) {
  queue_t* poll_group_queue = port_get_poll_group_queue(port_state);
  poll_group_t* poll_group =
      !queue_is_empty(poll_group_queue)
          ? container_of(
                queue_last(poll_group_queue), poll_group_t, queue_node)
          : NULL;

  if (poll_group == NULL ||
      poll_group->group_size >= POLL_GROUP__MAX_GROUP_SIZE)
    poll_group = poll_group__new(port_state);
  if (poll_group == NULL)
    return NULL;

  if (++poll_group->group_size == POLL_GROUP__MAX_GROUP_SIZE)
    queue_move_to_start(poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_release(poll_group_t* poll_group) {
  port_state_t* port_state = poll_group->port_state;
  queue_t* poll_group_queue = port_get_poll_group_queue(port_state);

  poll_group->group_size--;
  assert(poll_group->group_size < POLL_GROUP__MAX_GROUP_SIZE);

  queue_move_to_end(poll_group_queue, &poll_group->queue_node);

  /* Poll groups are currently only freed when the epoll port is closed. */
}
