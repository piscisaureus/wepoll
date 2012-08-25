 
#include <assert.h>

#include <wpoll.h>
#include "msafd.h"
#include "tree.h"

#define ARRAY_COUNT(a) (sizeof(a) / (sizeof((a)[0])))
#define WPOLL_KEY 0xE9011

typedef struct wpoll_op_s wpoll_op_t;

/* State associated with a wpoll handle. */
typedef struct wpoll_handle_state_s {
  HANDLE iocp;
  SOCKET peer_sockets[ARRAY_COUNT(AFD_PROVIDER_IDS)];
  size_t pending_ops_count;
  RB_HEAD(wpoll_socket_state_tree, wpoll_socket_state_s) socket_state_tree; 
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
  RB_ENTRY(wpoll_socket_state_s) tree_entry;
} wpoll_socket_state_t;

/* State associated with a AFD_POLL request. */
struct wpoll_op_s {
  IO_STATUS_BLOCK status;
  AFD_POLL_INFO poll_info;
  int generation;
  struct wpoll_socket_state* socket_state;
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
  
  assert(op != NULL);

  op->generation = ++sock_data->op_generation;
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
     
      // TODO: Add to attention list


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

        // TODO: Add to attention list
      }

      sock_data->user_data = event->data.u64;
      return 0;
    }

    case WPOLL_CTL_DEL: {
      wpoll_socket_state_t lookup;
      wpoll_socket_state_t* sock_data;

      lookup.sock = sock;
      sock_data = RB_FIND(wpoll_socket_state_tree, &port_data->socket_state_tree, &lookup);
      if (sock_data == NULL) {
        /* Socket has not been registered with wpoll instance. */
        SetLastError(ERROR_NOT_FOUND);
        return -1;
      }



      SetLastError(ERROR_NOT_FOUND);
      break;
    }


    default:
      WSASetLastError(WSAEINVAL);
      return -1;
  }
}

int epoll_wait(wpoll_t wpoll_hnd, struct wpoll_event* events, int maxevents, int timeout);

