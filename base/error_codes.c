#ifndef ERROR_CODES_C_
#define ERROR_CODES_C_

enum error_codes
{
    NO_ERROR,

    ERROR_os_memory__failed_to_allocate,
    ERROR_os_memory__query_unknown_state,
    ERROR_os_memory__query_failed,

    ERROR_os_file__not_found,
    ERROR_os_file__write_failed,
    ERROR_os_file__access_denied,

    ERROR_os_process__failed_to_create_pipe,
    ERROR_os_process__command_not_found,
    ERROR_os_process__creation_failed,
    ERROR_os_process__command_failed,

    ERROR_graphics_mesh_tools__no_space_left_in_mesh,
};

#endif
