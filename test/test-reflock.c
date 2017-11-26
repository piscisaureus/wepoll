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
#define TEST_ITERATIONS 50
#define TEST_LENGTH 100

typedef struct test_context {
  reflock_t reflock;
  SRWLOCK srwlock;
  volatile bool stop;
} test_context_t;

static void init_context(test_context_t* context) {
  reflock_init(&context->reflock);
  InitializeSRWLock(&context->srwlock);
  context->stop = false;
}

static void yield(void) {
  int count = rand() % 3;
  while (count--)
    Sleep(0);
}

static unsigned int __stdcall test_thread(void* arg) {
  test_context_t* context = arg;
  uint64_t lock_count = 0;

  AcquireSRWLockShared(&context->srwlock);

  while (!context->stop) {
    reflock_ref(&context->reflock);
    ReleaseSRWLockShared(&context->srwlock);

    lock_count++;
    yield();

    reflock_unref(&context->reflock);

    yield();

    AcquireSRWLockShared(&context->srwlock);
  }

  ReleaseSRWLockShared(&context->srwlock);

  assert(lock_count > 100); /* Hopefully much more. */

  return 0;
}

static void destroy_reflock(test_context_t* context) {
  AcquireSRWLockExclusive(&context->srwlock);
  context->stop = true;
  reflock_ref(&context->reflock);
  ReleaseSRWLockExclusive(&context->srwlock);

  reflock_unref_and_destroy(&context->reflock);
}

static void run_test_iteration(void) {
  test_context_t context;
  HANDLE threads[THREAD_COUNT];

  init_context(&context);

  for (size_t i = 0; i < array_count(threads); i++) {
    HANDLE thread =
        (HANDLE) _beginthreadex(NULL, 0, test_thread, &context, 0, NULL);
    assert(thread != INVALID_HANDLE_VALUE);
    threads[i] = thread;
  }

  Sleep(TEST_LENGTH);

  destroy_reflock(&context);

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
