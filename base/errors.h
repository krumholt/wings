#ifndef WINGS_BASE_ERRORS_H_
#define WINGS_BASE_ERRORS_H_

#include "wings/base/types.h"
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

#endif
