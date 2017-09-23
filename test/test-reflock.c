#include <assert.h>
#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "reflock.h"
#include "util.h"
#include "win.h"

#define THREAD_COUNT 20
#define TEST_ITERATIONS 10
#define TEST_LENGTH 1000

typedef unsigned(__stdcall* thread_entry_t)(void* arg);

typedef struct test_context {
  reflock_t lock;
  volatile bool ref_valid;
  volatile bool content_valid;
  volatile bool stop;
} test_context_t;

static void yield(void) {
  int count = rand() % THREAD_COUNT;
  while (count--)
    Sleep(0);
}

static unsigned int __stdcall thread1(void* arg) {
  test_context_t* context = arg;
  reflock_token_t token;
  uint64_t lock_count = 0;

  token = reflock_ref_and_announce_lock(&context->lock);

  yield();

  token = reflock_lock(&context->lock, token);

  context->content_valid = false;

  while (!context->stop) {
    lock_count++;

    assert(context->ref_valid);
    assert(!context->content_valid);
    context->content_valid = true;

    token = reflock_unlock(&context->lock, token);

    yield();

    token = reflock_lock(&context->lock, token);

    assert(context->content_valid);
    context->content_valid = false;
  }
  assert(context->ref_valid);
  assert(!context->content_valid);
  context->content_valid = true;

  reflock_unref_and_unlock(&context->lock, token);

  assert(lock_count > 100); /* Hopefully a lot more. */

  return 0;
}

static unsigned int __stdcall thread2(void* arg) {
  test_context_t* context = arg;
  uint64_t lock_count = 0;

  while (!context->stop) {
    reflock_token_t token = reflock_ref_and_announce_lock(&context->lock);

    yield();

    token = reflock_lock(&context->lock, token);

    assert(context->ref_valid);
    assert(context->content_valid);
    context->content_valid = false;

    lock_count++;

    yield();

    assert(context->ref_valid);
    assert(!context->content_valid);
    context->content_valid = true;

    reflock_unref_and_unlock(&context->lock, token);
  }

  assert(lock_count > 100); /* Hopefully a lot more. */

  return 0;
}

static void delete_ref(test_context_t* context) {
  reflock_token_t token;

  token = reflock_announce_delete(&context->lock);

  context->stop = true;

  yield();

  reflock_delete(&context->lock, token);

  assert(context->content_valid);
  context->content_valid = false;
  assert(context->ref_valid);
  context->ref_valid = false;
}

static void run_test_iteration(void) {
  test_context_t context;
  HANDLE threads[THREAD_COUNT];

  reflock_init(&context.lock);

  context.ref_valid = true;
  context.content_valid = true;
  context.stop = false;

  for (size_t i = 0; i < array_count(threads); i++) {
    thread_entry_t thread_entry = (i % 2 == 0) ? thread1 : thread2;
    HANDLE thread =
        (HANDLE) _beginthreadex(NULL, 0, thread_entry, &context, 0, NULL);
    assert(thread != INVALID_HANDLE_VALUE);
    threads[i] = thread;
  }

  Sleep(TEST_LENGTH);

  delete_ref(&context);

  for (size_t i = 0; i < array_count(threads); i++) {
    HANDLE thread = threads[i];
    DWORD wr = WaitForSingleObject(thread, INFINITE);
    assert(wr == WAIT_OBJECT_0);
    CloseHandle(thread);
  }
}

int main(void) {
  if (init() < 0)
    return 0;

  for (int i = 0; i < TEST_ITERATIONS; i++) {
    printf("Iteration %d of %d\n", i + 1, TEST_ITERATIONS);
    run_test_iteration();
  }

  return 0;
}
