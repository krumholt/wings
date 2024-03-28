#ifndef WINGS_BASE_ERRORS_H_
#define WINGS_BASE_ERRORS_H_

#include "types.h"
#define ERROR_LOG_STRING_LENGTH 1024

struct error_log
{
   u32      length;
   u32      capacity;
   char    *array;
};

error
make_error(char *format, ...);

string_view
error_to_string_view(error error);


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


#endif
