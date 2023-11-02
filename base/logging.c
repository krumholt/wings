#ifndef WINGS_BASE_LOGGING_C_
#define WINGS_BASE_LOGGING_C_

#include "wings/base/types.c"
#include "wings/base/error_codes.c"

#include <stdio.h>
#include <stdarg.h>

#define log_error(Format, ...) \
   _log_error(__FILE__, __LINE__, Format __VA_OPT__(,) __VA_ARGS__);

void _log_error(char *file, int line, char *format, ...)
{
   va_list arg_list;
   va_start(arg_list, format);
   char tmp[4096] = {0};
   vsnprintf(tmp, 4095, format, arg_list);
   va_end(arg_list);
   printf("%s:%d:0: Error: %s", file, line, tmp);
}


#endif
