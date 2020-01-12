#ifdef _MSC_VER
#define WEPOLL_EXPORT __declspec(dllexport)
#else /* GCC/Clang */
#define WEPOLL_EXPORT __attribute__((dllexport))
#endif
