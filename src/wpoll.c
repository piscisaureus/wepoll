 
#include <assert.h>

#include <wpoll.h>
#include "msafd.h"
#include "tree.h"

#define ARRAY_COUNT(a) (sizeof(a) / (sizeof((a)[0])))
#define WPOLL_KEY 0xE9011


typedef struct wpoll_op_s wpoll_op_t;
typedef struct wpoll_socket_state_s wpoll_socket_state_t;

/* State associated with a wpoll handle. */
typedef struct wpoll_handle_state_s {
  HANDLE iocp;
  SOCKET peer_sockets[ARRAY_COUNT(AFD_PROVIDER_IDS)];
  size_t pending_ops_count;
  RB_HEAD(wpoll_socket_state_tree, wpoll_socket_state_s) socket_state_tree; 
  wpoll_socket_state_t* attn;
} wpoll_handle_state_t;

/* State associated with a socket that is registered to the wpoll port. */
typedef struct wpoll_socket_state_s {
  SOCKET sock;
  SOCKET base_sock;
  SOCKET peer_sock;
  int op_generation;
  int submitted_events;
  int events;
  uint64_t user_data;
  wpoll_op_t* free_op;
  wpoll_socket_state_t* attn_prev;
  wpoll_socket_state_t* attn_next;
  RB_ENTRY(wpoll_socket_state_s) tree_entry;
};

/* State associated with a AFD_POLL request. */
struct wpoll_op_s {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  int generation;
  wpoll_socket_state_t* socket_state;
};


int wpoll_socket_compare(wpoll_socket_state_t* a, wpoll_socket_state_t* b) {
  return a->sock - b->sock;
}


RB_GENERATE_STATIC(wpoll_socket_state_tree, wpoll_socket_state_s, tree_entry, wpoll_socket_compare)


wpoll_t wpoll_create() {
  HANDLE iocp;

  wpoll_handle_state_t* state = malloc(sizeof *state);
  if (state == NULL) {
    SetLastError(ERROR_OUTOFMEMORY);
    return NULL;
  }

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                NULL,
                                0,
                                0);
  if (iocp == INVALID_HANDLE_VALUE) {
    free(state);
    return NULL;
  }

  state->iocp = iocp;
  state->attn = NULL;
  memset(&state->peer_sockets, 0, sizeof state->peer_sockets);
  state->pending_ops_count  = 0;
  RB_INIT(&state->socket_state_tree);

  return (wpoll_t*) state;
}

static SOCKET wpoll__create_peer_socket(HANDLE iocp,
    WSAPROTOCOL_INFOW* protocol_info) {
  SOCKET sock = 0;

  sock = WSASocketW(protocol_info->iAddressFamily,
                    protocol_info->iSocketType,
                    protocol_info->iProtocol,
                    protocol_info,
                    0,
                    WSA_FLAG_OVERLAPPED);
  if (sock == INVALID_SOCKET) {
    return INVALID_SOCKET;
  }

  if (!SetHandleInformation((HANDLE) sock, HANDLE_FLAG_INHERIT, 0)) {
    goto error;
  };

  if (CreateIoCompletionPort((HANDLE) sock,
                             iocp,
                             WPOLL_KEY,
                             0) == NULL) {
    goto error;
  }

  return sock;

 error:
  closesocket(sock);
  return INVALID_SOCKET;
}


static SOCKET wpoll__get_peer_socket(wpoll_handle_state_t* port_data,
    WSAPROTOCOL_INFOW* protocol_info) {
  int index, i;
  SOCKET peer_socket;

  index = -1;
  for (i = 0; i < ARRAY_COUNT(AFD_PROVIDER_IDS); i++) {
    if (memcmp((void*) &protocol_info->ProviderId,
               (void*) &AFD_PROVIDER_IDS[i],
               sizeof protocol_info->ProviderId) == 0) {
      index = i;
    }
  }

  /* Check if the protocol uses an msafd socket. */
  if (index < 0) {
    SetLastError(ERROR_NOT_SUPPORTED);
    return INVALID_SOCKET;
  }

  /* If we didn't (try) to create a peer socket yet, try to make one. Don't */
  /* try again if the peer socket creation failed earlier for the same */
  /* protocol. */
  peer_socket = port_data->peer_sockets[index];
  if (peer_socket == 0) {
    peer_socket = wpoll__create_peer_socket(port_data->iocp, protocol_info);
    port_data->peer_sockets[index] = peer_socket;
  }

  return peer_socket;
}


int wpoll__submit_poll_op(wpoll_handle_state_t* port_data, wpoll_socket_state_t* sock_data) {
  wpoll_op_t* op = sock_data->free_op;
  int events;
  DWORD result, afd_events;

  assert(op != NULL);

  events = sock_data->events;

  /* Always observe these events. */
  afd_events = AFD_POLL_ABORT | AFD_POLL_CONNECT_FAIL | AFD_POLL_LOCAL_CLOSE;

  if (events & (WPOLLIN | WPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (events & (WPOLLIN | WPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (events & (WPOLLOUT | WPOLLWRNORM | WPOLLRDBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT; 

  op->generation = ++sock_data->op_generation;
  op->socket_state = sock_data;
  
  memset(&op->overlapped, 0, sizeof op->overlapped);

  op->poll_info.Exclusive = TRUE;
  op->poll_info.NumberOfHandles = 1;
  op->poll_info.Timeout.QuadPart = INT64_MAX;
  op->poll_info.Handles[0].Handle = (HANDLE) sock_data->base_sock;
  op->poll_info.Handles[0].Status = 0;
  op->poll_info.Handles[0].Events = afd_events;

  result = afd_poll(sock_data->peer_sock,
                    &op->poll_info,
                    &op->overlapped);
  if (result != 0) {
    DWORD error = WSAGetLastError();
    if (error != WSA_IO_PENDING) {
      /* If this happens an error happened and no overlapped operation was */
      /* started. */
      return -1;
    }
  }

  sock_data->free_op = NULL;

  return 0;
}


int wpoll_ctl(wpoll_t wpoll_hnd, int op, SOCKET sock,
    struct wpoll_event* event) {
  wpoll_handle_state_t* port_data;

  port_data = (wpoll_handle_state_t*) wpoll_hnd;

  switch (op) {
    case WPOLL_CTL_ADD: {
      wpoll_socket_state_t* sock_data;
      wpoll_op_t* op;
      SOCKET peer_sock;
      WSAPROTOCOL_INFOW protocol_info;
      int len;

      /* Obtain protocol information about the socket. */
      len = sizeof protocol_info;
      if (getsockopt(sock,
                     SOL_SOCKET,
                     SO_PROTOCOL_INFOW,
                     (char*) &protocol_info,
                     &len) != 0) {
        return -1;
      }
      
      peer_sock = wpoll__get_peer_socket(port_data, &protocol_info);
      if (peer_sock == INVALID_SOCKET) {
        return -1;
      }
      
      sock_data = malloc(sizeof *sock_data);
      if (sock_data == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        return -1;
      }

      op = malloc(sizeof *op);
      if (op == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        free(sock_data);
        return -1;
      }

      sock_data->sock = sock;
      /* TODO: actually get base socket. */
      sock_data->base_sock = sock;
      sock_data->op_generation = 0;
      sock_data->submitted_events = 0;
      sock_data->events = event->events;
      sock_data->user_data = event->data.u64;
      sock_data->peer_sock = peer_sock;
      sock_data->free_op = op;

      if (RB_INSERT(wpoll_socket_state_tree, &port_data->socket_state_tree, sock_data) != NULL) {
        /* Poll entry was already there. */
        free(sock_data);
        free(op);
        SetLastError(ERROR_ALREADY_EXISTS);
        return -1;
      }
     
      // Add to attention list
      sock_data->attn_prev = NULL;
      sock_data->attn_next = port_data->attn;
      port_data->attn->attn_next = sock_data;
      port_data->attn = sock_data;

      return 0;
    }

    case WPOLL_CTL_MOD: {
      wpoll_socket_state_t lookup;
      wpoll_socket_state_t* sock_data;

      lookup.sock = sock;
      sock_data = RB_FIND(wpoll_socket_state_tree, &port_data->socket_state_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with wpoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      if (event->events & ~sock_data->events) {
        if (sock_data->free_op == NULL) {
          wpoll_op_t* op = malloc(sizeof *op);
          if (op == NULL) {
            SetLastError(ERROR_OUTOFMEMORY);
            return -1;
          }

          sock_data->free_op = NULL;
        }

        // Add to attention list
        sock_data->attn_prev = NULL;
        sock_data->attn_next = port_data->attn;
        port_data->attn->attn_next = sock_data;
        port_data->attn = sock_data;
      }

      sock_data->user_data = event->data.u64;
      return 0;
    }

    case WPOLL_CTL_DEL: {
      wpoll_socket_state_t lookup;
      wpoll_socket_state_t* sock_data;

      lookup.sock = sock;
      sock_data = RB_REMOVE(wpoll_socket_state_tree, &port_data->socket_state_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with wpoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      free(sock_data->free_op);
      sock_data->events = -1;

      /* Remove from attention list. */
      if (sock_data->attn_prev != NULL)
        sock_data->attn_prev->attn_next = sock_data->attn_next;
      if (sock_data->attn_next != NULL)
        sock_data->attn_next->attn_prev = sock_data->attn_prev;
      if (port_data->attn == sock_data)
        port_data->attn = sock_data->attn_next;
      sock_data->attn_prev = sock_data->attn_next = NULL;

      if (sock_data->submitted_events == 0) {
        assert(sock_data->op_generation == 0);
        free(sock_data);
      } else {
        /* There are still one or more ops pending. */
        /* Wait for all pending ops to return before freeing. */
        assert(sock_data->op_generation > 0);
      }

      return 0;
    }

    default:
      WSASetLastError(WSAEINVAL);
      return -1;
  }
}


int epoll_wait(wpoll_t wpoll_hnd, struct wpoll_event* events, int maxevents, int timeout) {
  wpoll_handle_state_t* port_data = (wpoll_handle_state_t*) wpoll_hnd;

  while (port_data->attn != NULL) {
    wpoll_socket_state_t* sock_data = port_data->attn;
    /* Check if we need to submit another req. */
    if (sock_data->events & WPOLL_EVENT_MASK & ~sock_data->submitted_events) {
      int r = wpoll__submit_poll_op(port_data, sock_data);
      assert(r == 0);
    }

    /* Remove from attention list */
    port_data->attn = sock_data->attn_next;
    sock_data->attn_prev = sock_data->attn_next = NULL;
  }
  port_data->attn = NULL;

  {
    DWORD due = GetTickCount() + timeout;
    do {
      DWORD result;
      ULONG count;
      OVERLAPPED_ENTRY entries[64];
      DWORD max_entries;
      DWORD i;
      int num_events = 0;
      
      max_entries = ARRAY_COUNT(entries);
      if (max_entries > maxevents)
        max_entries = maxevents;

      result = GetQueuedCompletionStatusEx(port_data->iocp,
                                           entries,
                                           max_entries,
                                           &count,
                                           timeout,
                                           FALSE);

      if (!result) {
        DWORD error = GetLastError();
        if (error == WAIT_TIMEOUT) {
          return 0;
        } else {
          return -1;
        }
      }

      /* Successfully dequeued overlappeds. */
      for (i = 0; i < count; i++) {
        OVERLAPPED* overlapped = entries[i].lpOverlapped;
        wpoll_op_t* op = CONTAINING_RECORD(overlapped, wpoll_op_t, overlapped);
        wpoll_socket_state_t* sock_data = op->socket_state;
        DWORD afd_events;
        int reg_events, reported_events;

        if (op->generation < sock_data->op_generation) {
          /* This op has been superseded. Free and ignore it. */
          free(op);
          continue;
        }

        /* Dequeued the most recent op. Reset generation and submitted_events. */
        sock_data->op_generation = 0;
        sock_data->submitted_events = 0;
        sock_data->free_op = op;

        /* Check for error. */
        if (!NT_SUCCESS(overlapped->Internal)) {
          struct wpoll_event* ev = events + (num_events++);
          ev->data.u64 = sock_data->user_data;
          ev->events = WPOLLERR;
          continue;
        }

        reg_events = sock_data->events;
        afd_events = op->poll_info.Handles[0].Events;
        reported_events = 0;

        /* Check for a closed socket, or a socket that has been removed */
        /* with EPOLL_CTL_DEL. */
        if ((afd_events & AFD_POLL_LOCAL_CLOSE) || reg_events == -1) {
          free(op);
          free(sock_data);
          continue;
        }

        /* Unless WPOLLONESHOT is used, add the socket back to the attention list. */
        if (!reg_events & WPOLLONESHOT) {
          if (port_data->attn == NULL) {
            sock_data->attn_next = sock_data->attn_prev = NULL;
            port_data->attn = sock_data;
          } else {
            sock_data->attn_prev = NULL;
            sock_data->attn_next = port_data->attn;
            port_data->attn->attn_next = sock_data;
            port_data->attn = sock_data;
          }
        }

        /* Convert afd events to epoll events. */
        if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
          reported_events |= (WPOLLIN | WPOLLRDNORM) & reg_events;
        if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
          reported_events |= (WPOLLIN | WPOLLRDBAND) & reg_events;
        if (afd_events & AFD_POLL_SEND)
          reported_events |= (WPOLLOUT | WPOLLWRNORM | WPOLLWRBAND) & reg_events;
        if ((afd_events & AFD_POLL_DISCONNECT) && !(afd_events & AFD_POLL_ABORT))
          reported_events |= (WPOLLRDHUP | WPOLLIN | WPOLLRDNORM | WPOLLRDBAND) & reg_events;
        if (afd_events & AFD_POLL_ABORT)
          reported_events |= WPOLLHUP | WPOLLERR;
        if (afd_events & AFD_POLL_CONNECT)
          reported_events |= (WPOLLOUT | WPOLLWRNORM | WPOLLWRBAND) & reg_events;
        if (afd_events & AFD_POLL_CONNECT_FAIL)
          reported_events |= WPOLLERR;

        if (afd_events) {
          struct wpoll_event* ev = events + (num_events++);
          ev->data.u64 = sock_data->user_data;
          ev->events = reported_events;
        }
      }

      if (num_events > 0)
        return num_events;

      /* Events were dequeued, but none were relevant. Recompute timeout. */
      timeout = due - GetTickCount();
    } while (timeout > 0);

    return 0;
  }
}
