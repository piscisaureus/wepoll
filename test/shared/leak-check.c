#include <crtdbg.h>
#include <stdlib.h>

#include "test-util.h"

#if defined(__clang__) || defined(__GNUC__)
#define constructor(fn)                              \
  static void fn(void) __attribute__((constructor)); \
  static void fn(void)
#else /* msvc */
#pragma section(".CRT$XCU", read)
#define constructor(fn)                                              \
  static void __cdecl fn(void);                                      \
  __declspec(allocate(".CRT$XCU")) void(__cdecl * fn##_)(void) = fn; \
  static void __cdecl fn(void)
#endif

static void __cdecl leak_check__finalize(void);

constructor(leak_check__init) {
  /* Enable leak checking. */
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);

  /* Write diagnostics to stderr. */
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

  /* Register the finalization function to run when the program exits. */
  atexit(leak_check__finalize);
}

void leak_check__finalize(void) {
  /* Check if there were memory leaks. */
  int leaks_found = _CrtDumpMemoryLeaks();
  check(!leaks_found);
}
