#ifndef TEST_UTIL_H_
#define TEST_UTIL_H_

#ifdef _MSC_VER
#define no_return _declspec(noreturn)
#else /* GCC/clang */
#define no_return __attribute__((noreturn))
#endif

#ifdef _MSC_VER
#define no_inline _declspec(noinline)
#else /* GCC/clang */
#define no_inline __attribute__((noinline))
#endif

void no_inline no_return check_fail(const char* message);

#define __check_to_string_helper(v) #v
#define __check_to_string(v) __check_to_string_helper(v)

#define check(expression)                                           \
  (void) ((!!(expression)) ||                                       \
          (check_fail("Check failed:\n"                             \
                      "  test: " #expression "\n"                   \
                      "  file: " __FILE__ "\n"                      \
                      "  line: " __check_to_string(__LINE__) "\n"), \
           0))

#endif /* TEST_UTIL_H_ */
