#ifndef WINGS_BASE_ERROR_CODES_H_
#define WINGS_BASE_ERROR_CODES_H_

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
   _(ec_os_file__in_use)                            \
   _(ec_os_file__unknown)                           \
                                                    \
   _(ec_base_string_store__no_space_left)           \
                                                    \
   _(ec_base_cstrings__not_found)                   \
                                                    \
   _(ec_base_hashmap__null_value_not_allowed)       \
   _(ec_base_hashmap__key_exists)                   \
   _(ec_base_hashmap__full)                         \
                                                    \
   _(ec_base_managed_string__string_not_writeable)  \
                                                    \
   _(ec_os_process__failed_to_create_pipe)          \
   _(ec_os_process__command_not_found)              \
   _(ec_os_process__creation_failed)                \
   _(ec_os_process__command_failed)                 \
                                                    \
   _(ec_os_dynamic_loader__free_library_failed)     \
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

enum error_codes
{
   #define FIRST_AND_COMMA(Token, ...) Token,
   ERROR_CODES_TABLE(FIRST_AND_COMMA)
   #undef FIRST_AND_COMMA
};

void
error_code_set_message(char *format, ...);

char *
error_code_to_string(enum error_codes error_code);

#endif
