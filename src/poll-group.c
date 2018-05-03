#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "util.h"
#include "win.h"

static const size_t _POLL_GROUP_MAX_SIZE = 32;

typedef struct poll_group {
  ep_port_t* port_info;
  queue_node_t queue_node;
  SOCKET socket;
  size_t group_size;
} poll_group_t;

static poll_group_t* _poll_group_new(ep_port_t* port_info) {
  poll_group_t* poll_group = malloc(sizeof *poll_group);
  if (poll_group == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  memset(poll_group, 0, sizeof *poll_group);

  queue_node_init(&poll_group->queue_node);
  poll_group->port_info = port_info;

  if (afd_create_driver_socket(port_info->iocp, &poll_group->socket) < 0) {
    free(poll_group);
    return NULL;
  }

  queue_append(&port_info->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_delete(poll_group_t* poll_group) {
  assert(poll_group->group_size == 0);
  closesocket(poll_group->socket);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

poll_group_t* poll_group_from_queue_node(queue_node_t* queue_node) {
  return container_of(queue_node, poll_group_t, queue_node);
}

SOCKET poll_group_get_socket(poll_group_t* poll_group) {
  return poll_group->socket;
}

poll_group_t* poll_group_acquire(ep_port_t* port_info) {
  queue_t* queue = &port_info->poll_group_queue;
  poll_group_t* poll_group =
      !queue_empty(queue)
          ? container_of(queue_last(queue), poll_group_t, queue_node)
          : NULL;

  if (poll_group == NULL || poll_group->group_size >= _POLL_GROUP_MAX_SIZE)
    poll_group = _poll_group_new(port_info);
  if (poll_group == NULL)
    return NULL;

  if (++poll_group->group_size == _POLL_GROUP_MAX_SIZE)
    queue_move_first(&port_info->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_release(poll_group_t* poll_group) {
  ep_port_t* port_info = poll_group->port_info;

  poll_group->group_size--;
  assert(poll_group->group_size < _POLL_GROUP_MAX_SIZE);

  queue_move_last(&port_info->poll_group_queue, &poll_group->queue_node);

  /* Poll groups are currently only freed when the epoll port is closed. */
}
