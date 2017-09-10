/* Copyright (c) 2013, Ben Noordhuis <info@bnoordhuis.nl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>

typedef void* QUEUE[2];

/* Private macros. */
#define _QUEUE_NEXT(q) (*(QUEUE**) &((*(q))[0]))
#define _QUEUE_PREV(q) (*(QUEUE**) &((*(q))[1]))
#define _QUEUE_PREV_NEXT(q) (_QUEUE_NEXT(_QUEUE_PREV(q)))
#define _QUEUE_NEXT_PREV(q) (_QUEUE_PREV(_QUEUE_NEXT(q)))

/* Public macros. */
#define QUEUE_DATA(ptr, type, field) \
  ((type*) ((char*) (ptr) -offsetof(type, field)))

/* Important note: mutating the list while QUEUE_FOREACH is
 * iterating over its elements results in undefined behavior.
 */
#define QUEUE_FOREACH(q, h) \
  for ((q) = _QUEUE_NEXT(h); (q) != (h); (q) = _QUEUE_NEXT(q))

#define QUEUE_EMPTY(q) ((const QUEUE*) (q) == (const QUEUE*) _QUEUE_NEXT(q))

#define QUEUE_HEAD(q) (_QUEUE_NEXT(q))

#define QUEUE_INIT(q)     \
  do {                    \
    _QUEUE_NEXT(q) = (q); \
    _QUEUE_PREV(q) = (q); \
  } while (0)

#define QUEUE_MERGE(h, n)                 \
  do {                                    \
    _QUEUE_PREV_NEXT(h) = _QUEUE_NEXT(n); \
    _QUEUE_NEXT_PREV(n) = _QUEUE_PREV(h); \
    _QUEUE_PREV(h) = _QUEUE_PREV(n);      \
    _QUEUE_PREV_NEXT(h) = (h);            \
  } while (0)

#define QUEUE_SPLIT(h, q, n)         \
  do {                               \
    _QUEUE_PREV(n) = _QUEUE_PREV(h); \
    _QUEUE_PREV_NEXT(n) = (n);       \
    _QUEUE_NEXT(n) = (q);            \
    _QUEUE_PREV(h) = _QUEUE_PREV(q); \
    _QUEUE_PREV_NEXT(h) = (h);       \
    _QUEUE_PREV(q) = (n);            \
  } while (0)

#define QUEUE_MOVE(h, n)        \
  do {                          \
    if (QUEUE_EMPTY(h))         \
      QUEUE_INIT(n);            \
    else {                      \
      QUEUE* q = QUEUE_HEAD(h); \
      QUEUE_SPLIT(h, q, n);     \
    }                           \
  } while (0)

#define QUEUE_INSERT_HEAD(h, q)      \
  do {                               \
    _QUEUE_NEXT(q) = _QUEUE_NEXT(h); \
    _QUEUE_PREV(q) = (h);            \
    _QUEUE_NEXT_PREV(q) = (q);       \
    _QUEUE_NEXT(h) = (q);            \
  } while (0)

#define QUEUE_INSERT_TAIL(h, q)      \
  do {                               \
    _QUEUE_NEXT(q) = (h);            \
    _QUEUE_PREV(q) = _QUEUE_PREV(h); \
    _QUEUE_PREV_NEXT(q) = (q);       \
    _QUEUE_PREV(h) = (q);            \
  } while (0)

#define QUEUE_REMOVE(q)                   \
  do {                                    \
    _QUEUE_PREV_NEXT(q) = _QUEUE_NEXT(q); \
    _QUEUE_NEXT_PREV(q) = _QUEUE_PREV(q); \
  } while (0)

#endif /* QUEUE_H_ */
