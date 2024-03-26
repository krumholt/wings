#ifndef WINGS_BASE_ERROR_CODES_C_
#define WINGS_BASE_ERROR_CODES_C_

#include "macros.c"
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#include "error_codes.h"


static char *_ec__last_error_message;
static char *error_code_as_text[] = {
   ERROR_CODES_TABLE(FIRST_AS_STRING_AND_COMMA)
};


void
error_code_set_message(char *format, ...)
{
   if (!_ec__last_error_message)
      _ec__last_error_message = (char *)calloc(40960, 1);
   va_list arg_list;
   va_start(arg_list, format);
   vsnprintf(_ec__last_error_message, 40960 - 1, format, arg_list);
   va_end(arg_list);
}

char *
error_code_to_string(enum error_codes error_code)
{
   if (error_code < 0 || error_code >= ec__number_of_error_codes)
      return "ERROR_CODE_UNKNOWN";
   return error_code_as_text[error_code];
}

#endif
