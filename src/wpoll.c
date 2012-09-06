
#include <stdio.h>
#include <assert.h>

#include <wpoll.h>
#include "msafd.h"
#include "tree.h"

#define ARRAY_COUNT(a) (sizeof(a) / (sizeof((a)[0])))
#define WPOLL_KEY 0xE9011


typedef struct wpoll_port_data_s wpoll_port_data_t;
typedef struct wpoll_op_s wpoll_op_t;
typedef struct wpoll_sock_data_s wpoll_sock_data_t;


/* State associated with a wpoll handle. */
struct wpoll_port_data_s {
  HANDLE iocp;
  SOCKET peer_sockets[ARRAY_COUNT(AFD_PROVIDER_IDS)];
  RB_HEAD(wpoll_sock_data_tree, wpoll_sock_data_s) sock_data_tree;
  wpoll_sock_data_t* attn;
  size_t pending_ops_count;
};

/* State associated with a socket that is registered to the wpoll port. */
typedef struct wpoll_sock_data_s {
  SOCKET sock;
  SOCKET base_sock;
  SOCKET peer_sock;
  int op_generation;
  int submitted_events;
  int events;
  int attn;
  uint64_t user_data;
  wpoll_op_t* free_op;
  wpoll_sock_data_t* attn_prev;
  wpoll_sock_data_t* attn_next;
  RB_ENTRY(wpoll_sock_data_s) tree_entry;
};

/* State associated with a AFD_POLL request. */
struct wpoll_op_s {
  OVERLAPPED overlapped;
  AFD_POLL_INFO poll_info;
  int generation;
  wpoll_sock_data_t* sock_data;
};


int wpoll_socket_compare(wpoll_sock_data_t* a, wpoll_sock_data_t* b) {
  return a->sock - b->sock;
}


RB_GENERATE_STATIC(wpoll_sock_data_tree, wpoll_sock_data_s, tree_entry, wpoll_socket_compare)


wpoll_t wpoll_create() {
  HANDLE iocp;

  wpoll_port_data_t* port_data = malloc(sizeof *port_data);
  if (port_data == NULL) {
    SetLastError(ERROR_OUTOFMEMORY);
    return NULL;
  }

  iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                NULL,
                                0,
                                0);
  if (iocp == INVALID_HANDLE_VALUE) {
    free(port_data);
    return NULL;
  }

  port_data->iocp = iocp;
  port_data->attn = NULL;
  port_data->pending_ops_count = 0;

  memset(&port_data->peer_sockets, 0, sizeof port_data->peer_sockets);
  RB_INIT(&port_data->sock_data_tree);

  return (wpoll_t) port_data;
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


static SOCKET wpoll__get_peer_socket(wpoll_port_data_t* port_data,
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


int wpoll__submit_poll_op(wpoll_port_data_t* port_data, wpoll_sock_data_t* sock_data) {
  wpoll_op_t* op;
  int events;
  DWORD result, afd_events;

  op = sock_data->free_op;
  events = sock_data->events;

  /* wpoll_ctl should ensure that there is a free op struct. */
  assert(op != NULL);

  /* These events should always be registered. */
  assert(events & WPOLLERR);
  assert(events & WPOLLHUP);
  afd_events = AFD_POLL_ABORT | AFD_POLL_CONNECT_FAIL | AFD_POLL_LOCAL_CLOSE;

  if (events & (WPOLLIN | WPOLLRDNORM))
    afd_events |= AFD_POLL_RECEIVE | AFD_POLL_ACCEPT;
  if (events & (WPOLLIN | WPOLLRDBAND))
    afd_events |= AFD_POLL_RECEIVE_EXPEDITED;
  if (events & (WPOLLOUT | WPOLLWRNORM | WPOLLRDBAND))
    afd_events |= AFD_POLL_SEND | AFD_POLL_CONNECT;

  op->generation = ++sock_data->op_generation;
  op->sock_data = sock_data;

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
  port_data->pending_ops_count++;

  return 0;
}


int wpoll_ctl(wpoll_t port_handle, int op, SOCKET sock,
    struct wpoll_event* event) {
  wpoll_port_data_t* port_data;

  port_data = (wpoll_port_data_t*) port_handle;

  switch (op) {
    case WPOLL_CTL_ADD: {
      wpoll_sock_data_t* sock_data;
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
      sock_data->events = event->events | WPOLLERR | WPOLLHUP;
      sock_data->user_data = event->data.u64;
      sock_data->peer_sock = peer_sock;
      sock_data->free_op = op;

      if (RB_INSERT(wpoll_sock_data_tree, &port_data->sock_data_tree, sock_data) != NULL) {
        /* Socket was already added. */
        free(sock_data);
        free(op);
        SetLastError(ERROR_ALREADY_EXISTS);
        return -1;
      }

      // Add to attention list
      sock_data->attn = 1;
      sock_data->attn_prev = NULL;
      sock_data->attn_next = port_data->attn;
      if (port_data->attn)
        port_data->attn->attn_prev = sock_data;
      port_data->attn = sock_data;

      return 0;
    }

    case WPOLL_CTL_MOD: {
      wpoll_sock_data_t lookup;
      wpoll_sock_data_t* sock_data;

      lookup.sock = sock;
      sock_data = RB_FIND(wpoll_sock_data_tree, &port_data->sock_data_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with wpoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      if (event->events & ~sock_data->submitted_events) {
        if (sock_data->free_op == NULL) {
          wpoll_op_t* op = malloc(sizeof *op);
          if (op == NULL) {
            SetLastError(ERROR_OUTOFMEMORY);
            return -1;
          }

          sock_data->free_op = NULL;
        }

        // Add to attention list, if not already added.
        if (!sock_data->attn) {
          sock_data->attn_prev = NULL;
          sock_data->attn_next = port_data->attn;
          if (port_data->attn)
            port_data->attn->attn_prev = sock_data;
          port_data->attn = sock_data;
          sock_data->attn = 1;
        }
      }

      sock_data->events = event->events | WPOLLERR | WPOLLHUP;
      sock_data->user_data = event->data.u64;
      return 0;
    }

    case WPOLL_CTL_DEL: {
      wpoll_sock_data_t lookup;
      wpoll_sock_data_t* sock_data;

      lookup.sock = sock;
      sock_data = RB_FIND(wpoll_sock_data_tree, &port_data->sock_data_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with wpoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }

      RB_REMOVE(wpoll_sock_data_tree, &port_data->sock_data_tree, sock_data);

      free(sock_data->free_op);
      sock_data->events = -1;

      /* Remove from attention list. */
      if (sock_data->attn) {
        if (sock_data->attn_prev != NULL)
          sock_data->attn_prev->attn_next = sock_data->attn_next;
        if (sock_data->attn_next != NULL)
          sock_data->attn_next->attn_prev = sock_data->attn_prev;
        if (port_data->attn == sock_data)
          port_data->attn = sock_data->attn_next;
        sock_data->attn = 0;
        sock_data->attn_prev = sock_data->attn_next = NULL;
      }

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


int wpoll_wait(wpoll_t port_handle, struct wpoll_event* events, int maxevents, int timeout) {
  wpoll_port_data_t* port_data;
  DWORD due;
  DWORD gqcs_timeout;

  port_data = (wpoll_port_data_t*) port_handle;

  /* Create overlapped poll operations for all sockets on the attention list. */
  while (port_data->attn != NULL) {
    wpoll_sock_data_t* sock_data = port_data->attn;
    assert(sock_data->attn);

    /* Check if we need to submit another req. */
    if (sock_data->events & WPOLL_EVENT_MASK & ~sock_data->submitted_events) {
      int r = wpoll__submit_poll_op(port_data, sock_data);
      /* TODO: handle error. */
    }

    /* Remove from attention list */
    port_data->attn = sock_data->attn_next;
    sock_data->attn_prev = sock_data->attn_next = NULL;
    sock_data->attn = 0;
  }
  port_data->attn = NULL;

  /* Compute the timeout for GetQueuedCompletionStatus, and the wait end */
  /* time, if the user specified a timeout other than zero or infinite. */
  if (timeout > 0) {
    due = GetTickCount() + timeout;
    gqcs_timeout = (DWORD) timeout;
  } else if (timeout == 0) {
    gqcs_timeout = 0;
  } else {
    gqcs_timeout = INFINITE;
  }

  /* Dequeue completion packets until either at least one interesting event */
  /* has been discovered, or the timeout is reached. */
  do {
    DWORD result, max_entries;
    ULONG count, i;
    OVERLAPPED_ENTRY entries[64];
    int num_events = 0;

    /* Compute how much overlapped entries can be dequeued at most. */
    max_entries = ARRAY_COUNT(entries);
    if ((int) max_entries > maxevents)
      max_entries = maxevents;

    result = GetQueuedCompletionStatusEx(port_data->iocp,
                                          entries,
                                          max_entries,
                                          &count,
                                          gqcs_timeout,
                                          FALSE);

    if (!result) {
      DWORD error = GetLastError();
      if (error == WAIT_TIMEOUT) {
        return 0;
      } else {
        return -1;
      }
    }

    port_data->pending_ops_count -= count;

    /* Successfully dequeued overlappeds. */
    for (i = 0; i < count; i++) {
      OVERLAPPED* overlapped;
      wpoll_op_t* op;
      wpoll_sock_data_t* sock_data;
      DWORD afd_events;
      int registered_events, reported_events;

      overlapped = entries[i].lpOverlapped;
      op = CONTAINING_RECORD(overlapped, wpoll_op_t, overlapped);
      sock_data = op->sock_data;

      if (op->generation < sock_data->op_generation) {
        /* This op has been superseded. Free and ignore it. */
        free(op);
        continue;
      }

      /* Dequeued the most recent op. Reset generation and submitted_events. */
      sock_data->op_generation = 0;
      sock_data->submitted_events = 0;
      sock_data->free_op = op;

      registered_events = sock_data->events;
      reported_events = 0;

      /* Check if this op was associated with a socket that was removed */
      /* with EPOLL_CTL_DEL. */
      if (registered_events == -1) {
        free(op);
        free(sock_data);
        continue;
      }

      /* Check for error. */
      if (!NT_SUCCESS(overlapped->Internal)) {
        struct wpoll_event* ev = events + (num_events++);
        ev->data.u64 = sock_data->user_data;
        ev->events = WPOLLERR;
        continue;
      }

      if (op->poll_info.NumberOfHandles == 0) {
        /* NumberOfHandles can be zero if this poll operation was canceled */
        /* due to a more recent exclusive poll operation. */
        afd_events = 0;
      } else {
        afd_events = op->poll_info.Handles[0].Events;
      }

      /* Check for a closed socket. */
      if (afd_events & AFD_POLL_LOCAL_CLOSE) {
        free(op);
        free(sock_data);
        continue;
      }

      /* Convert afd events to epoll events. */
      if (afd_events & (AFD_POLL_RECEIVE | AFD_POLL_ACCEPT))
        reported_events |= (WPOLLIN | WPOLLRDNORM);
      if (afd_events & AFD_POLL_RECEIVE_EXPEDITED)
        reported_events |= (WPOLLIN | WPOLLRDBAND);
      if (afd_events & AFD_POLL_SEND)
        reported_events |= (WPOLLOUT | WPOLLWRNORM | WPOLLWRBAND);
      if ((afd_events & AFD_POLL_DISCONNECT) && !(afd_events & AFD_POLL_ABORT))
        reported_events |= (WPOLLRDHUP | WPOLLIN | WPOLLRDNORM | WPOLLRDBAND);
      if (afd_events & AFD_POLL_ABORT)
        reported_events |= WPOLLHUP | WPOLLERR;
      if (afd_events & AFD_POLL_CONNECT)
        reported_events |= (WPOLLOUT | WPOLLWRNORM | WPOLLWRBAND);
      if (afd_events & AFD_POLL_CONNECT_FAIL)
        reported_events |= WPOLLERR;

      /* Don't report events that the user didn't specify. */
      reported_events &= registered_events;

      /* Unless WPOLLONESHOT is used or no events were reported that the */
      /* user is interested in, add the socket back to the attention list. */
      if (!registered_events & WPOLLONESHOT || reported_events == 0) {
        assert(!sock_data->attn);
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

      if (reported_events) {
        struct wpoll_event* ev = events + (num_events++);
        ev->data.u64 = sock_data->user_data;
        ev->events = reported_events;
      }
    }

    if (num_events > 0)
      return num_events;

    /* Events were dequeued, but none were relevant. Recompute timeout. */
    if (timeout > 0) {
      gqcs_timeout = due - GetTickCount();
    }
  } while (timeout > 0);

  return 0;
}


int wpoll_close(wpoll_t port_handle) {
  wpoll_port_data_t* port_data;
  wpoll_sock_data_t* sock_data;
  int i;

  port_data = (wpoll_port_data_t*) port_handle;

  /* Close all peer sockets. This will make all pending ops return. */
  for (i = 0; i < ARRAY_COUNT(port_data->peer_sockets); i++) {
    SOCKET peer_sock = port_data->peer_sockets[i];
    if (peer_sock != 0 && peer_sock != INVALID_SOCKET) {
      if (closesocket(peer_sock) != 0)
        return -1;

      port_data->peer_sockets[i] = 0;
    }
  }

  /* There is no list of wpoll_ops the free. And even if there was, just */
  /* freeing them would be dangerous since the kernel might still alter */
  /* the overlapped status contained in them. But since we are sure that */
  /* all ops will soon return, just await them all. */
  while (port_data->pending_ops_count > 0) {
    OVERLAPPED_ENTRY entries[64];
    DWORD result;
    ULONG count, i;

    printf("ops: %d\n", port_data->pending_ops_count);

    result = GetQueuedCompletionStatusEx(port_data->iocp,
                                          entries,
                                          ARRAY_COUNT(entries),
                                          &count,
                                          INFINITE,
                                          FALSE);

    if (!result) {
      DWORD error = GetLastError();
      return -1;
    }

    port_data->pending_ops_count -= count;

    for (i = 0; i < count; i++) {
      wpoll_op_t* op = CONTAINING_RECORD(entries[i].lpOverlapped,
                                         wpoll_op_t,
                                         overlapped);
      free(op);
    }
  }

  /* Remove all entries from the socket_state tree. */
  while (sock_data = RB_ROOT(&port_data->sock_data_tree)) {
    RB_REMOVE(wpoll_sock_data_tree, &port_data->sock_data_tree, sock_data);

    if (sock_data->free_op != NULL)
      free(sock_data->free_op);
    free(sock_data);
  }

  /* Close the I/O completion port. */
  CloseHandle(port_data->iocp);

  /* Finally, remove the port data. */
  free(port_data);

  return 0;
}