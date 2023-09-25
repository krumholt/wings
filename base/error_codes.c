#ifndef ERROR_CODES_C_
#define ERROR_CODES_C_

enum error_codes
{
    NO_ERROR,

    os_memory_error_FAILED_TO_ALLOCATE,
    os_memory_error_QUERY_UNKNOWN_STATE,
    os_memory_error_QUERY_FAILED,

    file_error_not_found,
    file_error_writing_failed,
    file_error_access_denied,

    process_error_failed_to_create_pipe,
    process_error_command_not_found,
    process_error_creation_failed,
    process_command_failed,
};

#endif
