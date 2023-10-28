#ifndef WINGS_BASE_ERROR_CODES_C_
#define WINGS_BASE_ERROR_CODES_C_

#include "wings/base/macros.c"

#define ERROR_CODES_TABLE(_)                        \
   _(ec__no_error)                                  \
                                                    \
   _(ec_os_memory__failed_to_allocate)              \
   _(ec_os_memory__query_unknown_state)             \
   _(ec_os_memory__query_failed)                    \
                                                    \
   _(ec_os_file__not_found)                         \
   _(ec_os_file__write_failed)                      \
   _(ec_os_file__access_denied)                     \
                                                    \
   _(ec_os_process__failed_to_create_pipe)          \
   _(ec_os_process__command_not_found)              \
   _(ec_os_process__creation_failed)                \
   _(ec_os_process__command_failed)                 \
                                                    \
   _(ec_graphics_mesh_tools__no_space_left_in_mesh) \
   _(ec__number_of_error_codes)

enum error_codes
{
   ERROR_CODES_TABLE(NAME_AND_COMMA)
};

char *error_code_as_text[] = {
   ERROR_CODES_TABLE(NAME_AS_STRING_AND_COMMA)
};

char *
error_code_lookup(enum error_codes error_code)
{
   if (error_code < 0 || error_code >= ec__number_of_error_codes)
      return "ERROR_CODE_UNKNOWN";
   return error_code_as_text[error_code];
}

#endif
