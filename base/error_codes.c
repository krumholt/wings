#ifndef WINGS_BASE_ERROR_CODES_C_
#define WINGS_BASE_ERROR_CODES_C_

#include "macros.c"
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#define ERROR_CODES_TABLE(_)                        \
   _(ec__no_error)                                  \
                                                    \
   _(ec_os_memory__failed_to_allocate)              \
   _(ec_os_memory__query_unknown_state)             \
   _(ec_os_memory__query_failed)                    \
                                                    \
   _(ec_base_allocators__no_space_left)             \
   _(ec_base_allocators__illegal_parameter)         \
                                                    \
   _(ec_os_file__not_found)                         \
   _(ec_os_file__write_failed)                      \
   _(ec_os_file__access_denied)                     \
                                                    \
   _(ec_base_string_store__no_space_left)           \
                                                    \
   _(ec_base_cstrings__not_found)                   \
                                                    \
   _(ec_base_managed_string__string_not_writeable)  \
                                                    \
   _(ec_os_process__failed_to_create_pipe)          \
   _(ec_os_process__command_not_found)              \
   _(ec_os_process__creation_failed)                \
   _(ec_os_process__command_failed)                 \
                                                    \
   _(ec_base_asset_importers__failed_to_read_file)  \
   _(ec_base_asset_importers__failed_to_parse_file) \
                                                    \
   _(ec_jim__string_append_failed)                  \
                                                    \
   _(ec_graphics_mesh__failed_to_upload_mesh)       \
   _(ec_graphics_mesh_tools__no_space_left_in_mesh) \
                                                    \
   _(ec__number_of_error_codes)

char *_ec__last_error_message;
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

enum error_codes
{
   ERROR_CODES_TABLE(FIRST_AND_COMMA)
};

char *error_code_as_text[] = {
   ERROR_CODES_TABLE(FIRST_AS_STRING_AND_COMMA)
};

char *
error_code_lookup(enum error_codes error_code)
{
   if (error_code < 0 || error_code >= ec__number_of_error_codes)
      return "ERROR_CODE_UNKNOWN";
   return error_code_as_text[error_code];
}

#endif
