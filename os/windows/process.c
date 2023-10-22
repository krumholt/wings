#ifndef WINGS_OS_WINDOWS_PROCESS_C_
#define WINGS_OS_WINDOWS_PROCESS_C_

#include "wings/base/error_codes.c"
#include "wings/base/types.c"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

error
run_command(char *command, char *result_buffer, u32 result_buffer_size)
{
    SECURITY_ATTRIBUTES pipe_security_attributes = { 0 };
    pipe_security_attributes.nLength             = sizeof(SECURITY_ATTRIBUTES);
    pipe_security_attributes.bInheritHandle      = TRUE;

    HANDLE in_pipe_read   = 0;
    HANDLE in_pipe_write  = 0;
    HANDLE out_pipe_read  = 0;
    HANDLE out_pipe_write = 0;

    error error   = 0;
    s32   success = 1;

    success = CreatePipe(&in_pipe_read,
                         &in_pipe_write,
                         &pipe_security_attributes,
                         0);
    if (!success)
        return (ec_os_process__failed_to_create_pipe);
    error = CreatePipe(&out_pipe_read,
                       &out_pipe_write,
                       &pipe_security_attributes,
                       0);
    if (!success)
        return (ec_os_process__failed_to_create_pipe);

    STARTUPINFO         startup_info = { 0 };
    PROCESS_INFORMATION process_info = { 0 };
    startup_info.cb                  = sizeof(STARTUPINFO);
    startup_info.hStdError           = out_pipe_write;
    startup_info.hStdOutput          = out_pipe_write;
    startup_info.hStdInput           = in_pipe_read;
    startup_info.dwFlags             = STARTF_USESTDHANDLES;

    int result = CreateProcess(
        0,
        command,
        0,
        0,
        TRUE,
        0,
        0,
        0,
        &startup_info,
        &process_info);
    if (result == 0)
    {
        s32 last_error = GetLastError();
        if (last_error == 2)
            error = ec_os_process__command_not_found;
        else
            error = ec_os_process__creation_failed;
        return (error);
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(in_pipe_read);
    CloseHandle(out_pipe_write);

    DWORD chars_read = 0;
    for (;;)
    {
        success = ReadFile(out_pipe_read,
                           result_buffer,
                           result_buffer_size,
                           &chars_read,
                           0);
        if (chars_read == 0)
            break;
        if (!success)
        {
            u32 last_error = GetLastError();
            printf("bad %d happend\n", last_error);
            break;
        }
        result_buffer += chars_read;
        result_buffer_size -= chars_read;
    }
    DWORD exit_code = 0;
    GetExitCodeProcess(process_info.hProcess, &exit_code);
    CloseHandle(in_pipe_write);
    CloseHandle(out_pipe_read);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    if (exit_code != 0)
        return (ec_os_process__command_failed);

    return (NO_ERROR);
}

error
process_new(char *command, char *path)
{
    error error = ec__no_error;

    STARTUPINFO         startup_info = { 0 };
    PROCESS_INFORMATION process_info = { 0 };
    startup_info.cb                  = sizeof(STARTUPINFO);

    int result = CreateProcess(
        0,
        command,
        0,
        0,
        TRUE,
        0,
        0,
        path,
        &startup_info,
        &process_info);
    if (result == 0)
    {
        s32 last_error = GetLastError();
        if (last_error == 2)
            error = ec_os_process__command_not_found;
        else
            error = ec_os_process__creation_failed;
        return (error);
    }

    return (NO_ERROR);
}

#endif
