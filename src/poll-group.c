#include <assert.h>
#include <malloc.h>

#include "error.h"
#include "poll-group.h"
#include "port.h"
#include "util.h"
#include "win.h"

static const size_t _POLL_GROUP_MAX_SIZE = 32;

typedef struct poll_group_allocator {
  ep_port_t* port_info;
  queue_t poll_group_queue;
  WSAPROTOCOL_INFOW protocol_info;
} poll_group_allocator_t;

typedef struct poll_group {
  poll_group_allocator_t* allocator;
  queue_node_t queue_node;
  SOCKET socket;
  size_t group_size;
} poll_group_t;

static int _poll_group_create_socket(poll_group_t* poll_group,
                                     WSAPROTOCOL_INFOW* protocol_info,
                                     HANDLE iocp) {
  SOCKET socket;

  socket = WSASocketW(protocol_info->iAddressFamily,
                      protocol_info->iSocketType,
                      protocol_info->iProtocol,
                      protocol_info,
                      0,
                      WSA_FLAG_OVERLAPPED);
  if (socket == INVALID_SOCKET)
    return_error(-1);

  if (!SetHandleInformation((HANDLE) socket, HANDLE_FLAG_INHERIT, 0))
    goto error;

  if (CreateIoCompletionPort((HANDLE) socket, iocp, 0, 0) == NULL)
    goto error;

  poll_group->socket = socket;
  return 0;

error:;
  DWORD error = GetLastError();
  closesocket(socket);
  return_error(-1, error);
}

static poll_group_t* _poll_group_new(poll_group_allocator_t* pga) {
  poll_group_t* poll_group = malloc(sizeof *poll_group);
  if (poll_group == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  memset(poll_group, 0, sizeof *poll_group);

  queue_node_init(&poll_group->queue_node);
  poll_group->allocator = pga;

  if (_poll_group_create_socket(
          poll_group, &pga->protocol_info, pga->port_info->iocp) < 0) {
    free(poll_group);
    return NULL;
  }

  queue_append(&pga->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

static void _poll_group_delete(poll_group_t* poll_group) {
  assert(poll_group->group_size == 0);
  closesocket(poll_group->socket);
  queue_remove(&poll_group->queue_node);
  free(poll_group);
}

SOCKET poll_group_get_socket(poll_group_t* poll_group) {
  return poll_group->socket;
}

poll_group_allocator_t* poll_group_allocator_new(
    ep_port_t* port_info, const WSAPROTOCOL_INFOW* protocol_info) {
  poll_group_allocator_t* pga = malloc(sizeof *pga);
  if (pga == NULL)
    return_error(NULL, ERROR_NOT_ENOUGH_MEMORY);

  queue_init(&pga->poll_group_queue);
  pga->port_info = port_info;
  pga->protocol_info = *protocol_info;

  return pga;
}

void poll_group_allocator_delete(poll_group_allocator_t* pga) {
  queue_t* poll_group_queue = &pga->poll_group_queue;

  while (!queue_empty(poll_group_queue)) {
    queue_node_t* queue_node = queue_first(poll_group_queue);
    poll_group_t* poll_group =
        container_of(queue_node, poll_group_t, queue_node);
    _poll_group_delete(poll_group);
  }

  free(pga);
}

poll_group_t* poll_group_acquire(poll_group_allocator_t* pga) {
  queue_t* queue = &pga->poll_group_queue;
  poll_group_t* poll_group =
      !queue_empty(queue)
          ? container_of(queue_last(queue), poll_group_t, queue_node)
          : NULL;

  if (poll_group == NULL || poll_group->group_size >= _POLL_GROUP_MAX_SIZE)
    poll_group = _poll_group_new(pga);
  if (poll_group == NULL)
    return NULL;

  if (++poll_group->group_size == _POLL_GROUP_MAX_SIZE)
    queue_move_first(&pga->poll_group_queue, &poll_group->queue_node);

  return poll_group;
}

void poll_group_release(poll_group_t* poll_group) {
  poll_group_allocator_t* pga = poll_group->allocator;

  poll_group->group_size--;
  assert(poll_group->group_size < _POLL_GROUP_MAX_SIZE);

  queue_move_last(&pga->poll_group_queue, &poll_group->queue_node);

  /* TODO: free the poll_group_t* item at some point. */
}
