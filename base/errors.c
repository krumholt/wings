#ifndef WINGS_BASE_ERRORS_C_
#define WINGS_BASE_ERRORS_C_


#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "wings/base/strings.h"
#include "errors.h"


static struct error_log _error_log;

error
make_error(char *format, ...)
{
   char *target = _error_log.array + _error_log.length;
   u32 size = _error_log.capacity - _error_log.length;
   va_list arg_list;
   va_start(arg_list, format);
   u32 size_needed = vsnprintf(target, size, format, arg_list);
   va_end(arg_list);
   if (size_needed + 1 > size) {
      _error_log.capacity = _error_log.capacity ? (_error_log.capacity + size_needed) * 2 : size_needed * 2;

      char *new_array = calloc(_error_log.capacity, sizeof(char));
      memcpy(new_array, _error_log.array, _error_log.length);
      if (_error_log.array) free(_error_log.array);
   
      target = _error_log.array + _error_log.length;
      size = _error_log.capacity - _error_log.length;
      va_start(arg_list, format);
      size_needed = vsnprintf(target, size, format, arg_list);
      va_end(arg_list);
   }
   u64 error = _error_log.length;
   _error_log.length += size_needed + 1;
   return (error + 1);
}

string_view
error_to_string_view(error error)
{
   if (error > _error_log.length)
   {
      return make_string_view("Error: illegal error");
   }
   return make_string_view(_error_log.array + error);
}

#endif
