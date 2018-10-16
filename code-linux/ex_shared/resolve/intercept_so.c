/*

interception/interception.h@asan

// How it works:
// To replace system functions on Linux we just need to declare functions
// with same names in our library and then obtain the real function pointers
// using dlsym().
// There is one complication. A user may also intercept some of the functions
// we intercept. To resolve this we declare our interceptors with __interceptor_
// prefix, and then make actual interceptors weak aliases to __interceptor_
// functions.

name=./libintercept.so (6 segments) address=0x7f3f5ca10000
                 header  0: address=0x7f3f5ca10000
                         type=1, flags=0x5
                 header  1: address=0x7f3f5cc10d18
                         type=1, flags=0x6
                 header  2: address=0x7f3f5cc10d30
                         type=2, flags=0x6
                 header  3: address=0x7f3f5ca10190
                         type=4, flags=0x4
                 header  4: address=0x7f3f5ca10c18
                         type=1685382480, flags=0x4
                 header  5: address=0x7f3f5ca10000
                         type=1685382481, flags=0x6

...

name=/lib/x86_64-linux-gnu/libc.so.6 (10 segments) address=0x7f3f5bc3f000
                 header  0: address=0x7f3f5bc3f040
                         type=6, flags=0x5
                 header  1: address=0x7f3f5bdad650
                         type=3, flags=0x4
                 header  2: address=0x7f3f5bc3f000
                         type=1, flags=0x5
                 header  3: address=0x7f3f5bfe0748
                         type=1, flags=0x6
                 header  4: address=0x7f3f5bfe3ba0
                         type=2, flags=0x6
                 header  5: address=0x7f3f5bc3f270
                         type=4, flags=0x4
                 header  6: address=0x7f3f5bfe0748
                         type=7, flags=0x4
                 header  7: address=0x7f3f5bdad66c
                         type=1685382480, flags=0x4
                 header  8: address=0x7f3f5bc3f000
                         type=1685382481, flags=0x6
                 header  9: address=0x7f3f5bfe0748
                         type=1685382482, flags=0x4

====>
====> try to intercept malloc..
====>
intercept_so(): {
real function address: 0x7f3f5bcbafc0
wrapper function address: 0x7f3f5ca10970
ml_util_func in 1:
interceptor_malloc: {
interceptor_malloc: size: 1000
interceptor_malloc: }
intercept_so(): myglob_1: 21 }
kyoupark@kit-debian64:~/git/kb/code-linux/ex_shared/resolve$

*/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

typedef unsigned long uptr;  // NOLINT

int myglob_1 = 10;

// # define FUNC_TYPE(x) x##_f
// # define PTR_TO_REAL(x) real_##x
// # define REAL(x) __interception::PTR_TO_REAL(x)
// # define WRAP(x) __interceptor_ ## x
// # define INTERCEPTOR_ATTRIBUTE __attribute__((visibility("default")))
// 
// # define DECLARE_WRAPPER(ret_type, func, ...) \
//     extern "C" ret_type func(__VA_ARGS__) \
//     __attribute__((weak, alias("__interceptor_" #func), visibility("default")));
// 
// # define DEFINE_REAL(ret_type, func, ...) \
//     typedef ret_type (*FUNC_TYPE(func))(__VA_ARGS__); \
//     namespace __interception { \
//       FUNC_TYPE(func) PTR_TO_REAL(func); \
//     }
// 
// #define INTERCEPTOR(ret_type, func, ...) \
//   DEFINE_REAL(ret_type, func, __VA_ARGS__) \
//   DECLARE_WRAPPER(ret_type, func, __VA_ARGS__) \
//   extern "C" \
//   INTERCEPTOR_ATTRIBUTE \
//   ret_type WRAP(func)(__VA_ARGS__)
//
// INTERCEPTOR(char*, strcat, char *to, const char *from) {  // NOLINT
// }

typedef void* (*malloc_f)(size_t size); 
namespace __interception { malloc_f real_malloc; } 
extern "C" void* malloc(size_t size) __attribute__((weak, alias("__interceptor_" "malloc"), visibility("default"))); 
extern "C" __attribute__((visibility("default"))) void* __interceptor_malloc(size_t size) 
{
  printf("interceptor_malloc: {\n");
  printf("interceptor_malloc: size: %d \n", size);
  printf("interceptor_malloc: }\n");

  return (void *)0;
}

bool GetRealFunctionAddress(
    const char *func_name, 
    uptr *func_addr,
    uptr real, 
    uptr wrapper) 
{
  *func_addr = (uptr)dlsym(RTLD_NEXT, func_name);
  printf("real function address: 0x%lx \n", *func_addr); 
  printf("wrapper function address: 0x%lx \n", wrapper); 
  return real == wrapper;
}

// extern void order_so_1(int size);

int ml_util_func(int a)
{
  printf("ml_util_func in 1:\n");
  malloc(1000);
  return a+1;
}

void intercept_so(int size)
{
  printf("intercept_so(): {\n");

  // # define INTERCEPT_FUNCTION(func) INTERCEPT_FUNCTION_LINUX(func)
  // 
  // #define INTERCEPT_FUNCTION_LINUX(func) \
  //     ::__interception::GetRealFunctionAddress( \
  //           #func, (::__interception::uptr*)&REAL(func), \
  //           (::__interception::uptr)&(func), \
  //           (::__interception::uptr)&WRAP(func))
  //
  // ASAN_INTERCEPT_FUNC(strcat);  // NOLINT
  //
  // #define ASAN_INTERCEPT_FUNC(name)                                      \
  //   do {                                                                 \
  //     if ((!INTERCEPT_FUNCTION(name) || !REAL(name)) &&                  \
  //         common_flags()->verbosity > 0)                                 \
  //       Report("AddressSanitizer: failed to intercept '" #name "'\n");   \
  //   } while (0)

  do { 
    if (!GetRealFunctionAddress( 
          "malloc", 
          (uptr*)&__interception::real_malloc, 
          (uptr)&(malloc), 
          (uptr)&__interceptor_malloc) 
        || !__interception::real_malloc
       )
      printf("failed to intercept '" "malloc" "'\n"); 
  } while (0);

  myglob_1 += ml_util_func(size);

  int *arr = (int *)__interception::real_malloc(4*3);
  arr[0] = 10;
  arr[1] = 10;
  arr[2] = 10;
  free(arr);

  printf("intercept_so(): myglob_1: %d }\n", myglob_1);
}

