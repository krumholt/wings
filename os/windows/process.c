#ifndef WINGS_OS_WINDOWS_PROCESS_C_
#define WINGS_OS_WINDOWS_PROCESS_C_

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
        return (1);
    error = CreatePipe(&out_pipe_read,
                       &out_pipe_write,
                       &pipe_security_attributes,
                       0);
    if (!success)
        return (1);

    STARTUPINFO         startup_info = { 0 };
    PROCESS_INFORMATION process_info;
    startup_info.cb         = sizeof(STARTUPINFO);
    startup_info.hStdError  = out_pipe_write;
    startup_info.hStdOutput = out_pipe_write;
    startup_info.hStdInput  = in_pipe_read;
    startup_info.dwFlags    = STARTF_USESTDHANDLES;
    int result              = CreateProcess(
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
        int32_t last_error = GetLastError();
        if (last_error == 2)
            return (1);
        else
            return (2);
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(in_pipe_read);
    CloseHandle(out_pipe_write);

    unsigned long chars_read = 0;
    for (;;)
    {
        success = ReadFile(out_pipe_read,
                           result_buffer,
                           result_buffer_size,
                           &chars_read,
                           0);
        if (!success || chars_read == 0)
            break;
    }
	DWORD exit_code = 0;
	GetExitCodeProcess(process_info.hProcess, &exit_code);
    CloseHandle(in_pipe_write);
    CloseHandle(out_pipe_read);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
	if (exit_code != 0)
		return exit_code;

    return (NO_ERROR);
}

#endif
