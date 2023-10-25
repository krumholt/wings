#ifndef WINGS_BASE_ERROR_CODES_C_
#define WINGS_BASE_ERROR_CODES_C_

enum error_codes
{
    ec__no_error,

    ec_os_memory__failed_to_allocate,
    ec_os_memory__query_unknown_state,
    ec_os_memory__query_failed,

    ec_os_file__not_found,
    ec_os_file__write_failed,
    ec_os_file__access_denied,

    ec_os_process__failed_to_create_pipe,
    ec_os_process__command_not_found,
    ec_os_process__creation_failed,
    ec_os_process__command_failed,

    ec_graphics_mesh_tools__no_space_left_in_mesh,
};

#endif
