#ifndef TEST_UTIL_H_
#define TEST_UTIL_H_

#ifdef __clang__
#pragma clang diagnostic ignored "-Wbad-function-cast"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wformat-non-iso"
#endif

#ifdef _MSC_VER
#define no_return __declspec(noreturn)
#else /* GCC/clang */
#define no_return __attribute__((noreturn))
#endif

#ifdef _MSC_VER
#define no_inline __declspec(noinline)
#else /* GCC/clang */
#define no_inline __attribute__((noinline))
#endif

void no_inline no_return check_fail(const char* message);

#define _check_to_string_helper(v) #v
#define _check_to_string(v) _check_to_string_helper(v)

#define check(expression)                                          \
  (void) ((!!(expression)) ||                                      \
          (check_fail("\n"                                         \
                      "Check failed:\n"                            \
                      "  test: " #expression "\n"                  \
                      "  file: " __FILE__ "\n"                     \
                      "  line: " _check_to_string(__LINE__) "\n"), \
           0))

#endif /* TEST_UTIL_H_ */
