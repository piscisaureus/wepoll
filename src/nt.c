#include <stdlib.h>

#include "nt.h"
#include "win.h"

#define X(return_type, declarators, name, ...) \
  return_type(declarators* name)(__VA_ARGS__) = NULL;
NTDLL_IMPORT_LIST(X)
#undef X

int nt_initialize() {
  HMODULE ntdll;

  ntdll = LoadLibraryW(L"ntdll.dll");
  if (ntdll == NULL)
    return -1;

#define X(return_type, declarators, name, ...)                               \
  name =                                                                     \
      (return_type(declarators*)(__VA_ARGS__)) GetProcAddress(ntdll, #name); \
  if (name == NULL)                                                          \
    return -1;
  NTDLL_IMPORT_LIST(X)
#undef X

  return 0;
}
