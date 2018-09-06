#include <stdlib.h>

#include "nt.h"
#include "win.h"

/* Set up a workaround for the following problem:
 *   FARPROC addr = GetProcAddress(...);
 *   MY_FUNC func = (MY_FUNC) addr;          <-- GCC 8 warning/error.
 *   MY_FUNC func = (MY_FUNC) (void*) addr;  <-- MSVC  warning/error.
 * To compile cleanly with either compiler, do casts with this "bridge" type:
 *   MY_FUNC func = (MY_FUNC) (nt__fn_ptr_cast_t) addr; */
#ifdef __GNUC__
typedef void* nt__fn_ptr_cast_t;
#else
typedef FARPROC nt__fn_ptr_cast_t;
#endif

#define X(return_type, attributes, name, parameters) \
  WEPOLL_INTERNAL return_type(attributes* name) parameters = NULL;
NT_NTDLL_IMPORT_LIST(X)
#undef X

int nt_global_init(void) {
  HMODULE ntdll;
  FARPROC fn_ptr;

  ntdll = GetModuleHandleW(L"ntdll.dll");
  if (ntdll == NULL)
    return -1;

#define X(return_type, attributes, name, parameters) \
  fn_ptr = GetProcAddress(ntdll, #name);             \
  if (fn_ptr == NULL)                                \
    return -1;                                       \
  name = (return_type(attributes*) parameters)(nt__fn_ptr_cast_t) fn_ptr;
  NT_NTDLL_IMPORT_LIST(X)
#undef X

  return 0;
}
