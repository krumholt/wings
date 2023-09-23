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
};

#endif
