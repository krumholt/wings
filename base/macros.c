#ifndef WINGS_BASE_MACROS_C_
#define WINGS_BASE_MACROS_C_

#include "wings/base/errors.h"

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
      make_error("%s:%d:0: error: %s\n", __FILE__, __LINE__, error_to_string_view(x).start);       \
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


#define DYNAMIC_ARRAY_HEADER(Name) \
struct Name##_array\
{\
   s32 length;\
   s32 capacity;\
   struct Name *array;\
};\
struct Name##_array \
Name##_array_make(s32 capacity);\
\
void \
Name##_array_append(struct Name##_array *array, struct Name value)

#define DYNAMIC_ARRAY_IMPLEMENTATION(Name) \
struct Name##_array \
Name##_array_make(s32 capacity)\
{\
   struct Name##_array array = {0};\
   array.capacity = capacity;\
   array.length = 0;\
   array.array = calloc(array.capacity, sizeof(struct Name));\
   ASSERT(array.array);\
   return(array);\
}\
void \
Name##_array_append(struct Name##_array *array, struct Name value)\
{\
   if (array->length == array->capacity)\
   {\
      struct Name *new_array = 0;\
      s32 new_capacity = array->capacity * 2;\
      new_array = calloc(array->capacity, sizeof(struct Name));\
      ASSERT(new_array);\
      memcpy(new_array, array->array, array->length * sizeof(struct Name));\
      array->capacity = new_capacity;\
      array->array = new_array;\
   }\
   array->array[array->length++] = value;\
}

#endif
