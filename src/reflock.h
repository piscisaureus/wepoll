#include <stdint.h>

typedef struct reflock {
  uint64_t internal;
} reflock_t;

typedef uint32_t reflock_token_t;

EPOLL_INTERNAL int reflock_global_init(void);

EPOLL_INTERNAL void reflock_init(reflock_t* reflock);

EPOLL_INTERNAL reflock_token_t
    reflock_ref_and_announce_lock(reflock_t* reflock);
EPOLL_INTERNAL reflock_token_t reflock_lock(reflock_t* reflock,
                                            reflock_token_t token);
EPOLL_INTERNAL reflock_token_t reflock_unlock(reflock_t* reflock,
                                              reflock_token_t token);
EPOLL_INTERNAL void reflock_unref_and_unlock(reflock_t* reflock,
                                             reflock_token_t token);

EPOLL_INTERNAL reflock_token_t reflock_announce_delete(reflock_t* reflock);
EPOLL_INTERNAL void reflock_delete(reflock_t* reflock, reflock_token_t token);
