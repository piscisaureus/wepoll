#include <stdlib.h>

#include "nt.h"
#include "win.h"

#define X(return_type, declarators, name, parameters) \
  return_type(declarators* name) parameters = NULL;
NTDLL_IMPORT_LIST(X)
#undef X

int nt_initialize(void) {
  HMODULE ntdll;

  ntdll = LoadLibraryW(L"ntdll.dll");
  if (ntdll == NULL)
    return -1;

#define X(return_type, declarators, name, parameters)                         \
  name = (return_type(declarators*) parameters) GetProcAddress(ntdll, #name); \
  if (name == NULL)                                                           \
    return -1;
  NTDLL_IMPORT_LIST(X)
#undef X

  return 0;
}
