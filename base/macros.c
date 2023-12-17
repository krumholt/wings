#ifndef WINGS_BASE_MACROS_C_
#define WINGS_BASE_MACROS_C_

#if defined(_WIN32)
#ifndef OS_WINDOWS
#define OS_WINDOWS
#endif
#elif defined(__linux__)
#ifndef OS_LINUX
#define OS_LINUX
#endif
#endif

#define ARRAY_LENGTH(x) \
   (sizeof(x) / sizeof((x)[0]))

#define UNUSED(x) \
   (void)(x)

#define WARN(x) \
   printf("[WARNING] %s\n", x)

#define IF_ERROR_RETURN(Error) \
   do {                        \
      if ((Error))             \
      {                        \
         return (Error);       \
      }                        \
   } while (0)

#define IF_ERROR_RETURN_AND_LOG(Error, ...)   \
   do {                                       \
      if ((Error))                            \
      {                                       \
         error_code_set_message(__VA_ARGS__); \
         return (Error);                      \
      }                                       \
   } while (0)

#define IF_ERROR_PRINT_AND_RETURN(x) \
   if ((x))       \
   {              \
      printf("%s:%d:0: error: %s: %s\n", __FILE__, __LINE__, error_code_lookup(x), _ec__last_error_message);       \
      return (x); \
   }

#ifndef ASSERT
#ifdef NO_ASSERTS
#define ASSERT(argument)
#else
#include <assert.h>
#define ASSERT(argument) assert(argument)
#endif
#endif

#define FIRST_AND_COMMA(Token, ...) Token,
#define FIRST_AS_STRING_AND_COMMA(Token, ...) #Token,

#endif
