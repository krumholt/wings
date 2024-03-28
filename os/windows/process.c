#ifndef WINGS_OS_WINDOWS_PROCESS_C_
#define WINGS_OS_WINDOWS_PROCESS_C_

#include "base/types.h"
#include "base/macros.h"
#include "base/errors.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <winuser.h>

error
run_command_at(char *command, char *directory, u32 result_buffer_size, char *result_buffer)
{
   SECURITY_ATTRIBUTES pipe_security_attributes = { 0 };
   pipe_security_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
   pipe_security_attributes.bInheritHandle = TRUE;

   HANDLE out_pipe_read  = 0;
   HANDLE out_pipe_write = 0;

   error error   = 0;
   s32   success = 0;

   success = CreatePipe(&out_pipe_read,
                        &out_pipe_write,
                        &pipe_security_attributes,
                        0);
   if (!success)
      return (make_error("Failed to create pipe"));

   STARTUPINFO         startup_info = { 0 };
   PROCESS_INFORMATION process_info = { 0 };
   startup_info.cb                  = sizeof(STARTUPINFO);
   startup_info.lpTitle             = 0;
   startup_info.hStdError           = out_pipe_write;
   startup_info.hStdOutput          = out_pipe_write;
   startup_info.hStdInput           = 0;
   startup_info.dwFlags             = STARTF_USESTDHANDLES;

   int result = CreateProcess(
       0,
       command,
       0,
       0,
       TRUE,
       0,
       0,
       directory,
       &startup_info,
       &process_info);
   if (result == 0)
   {
      s32 last_error = GetLastError();
      if (last_error == 2)
         error = make_error("Command %s not found", command);
      else
         error = make_error("Failed to create process with windows error code %d", last_error);
      return (error);
   }
   DWORD chars_read = 0;
   CloseHandle(out_pipe_write);

   for (;;)
   {
      success = ReadFile(out_pipe_read,
                         result_buffer,
                         result_buffer_size - 1,
                         &chars_read,
                         0);
      if (!success || chars_read == 0)
      {
         result_buffer += chars_read;
         result_buffer_size -= chars_read;
         break;
      }
      result_buffer += chars_read;
      result_buffer_size -= chars_read;
   }
   *result_buffer = 0;
   CloseHandle(out_pipe_read);
   DWORD exit_code = 0;
   GetExitCodeProcess(process_info.hProcess, &exit_code);
   CloseHandle(process_info.hProcess);
   CloseHandle(process_info.hThread);

   if (exit_code)
      return(make_error("Command %s failed with exit code %d", command, exit_code));
   return (0);
}

error
run_command(char *command, u32 result_buffer_size, char *result_buffer)
{
   return (run_command_at(command, "./", result_buffer_size, result_buffer));
}

error
process_new(char *command, char *path, b32 new_console)
{
   error error = 0;

   STARTUPINFO         startup_info = { 0 };
   PROCESS_INFORMATION process_info = { 0 };
   startup_info.cb                  = sizeof(STARTUPINFO);
   //startup_info.dwFlags             = STARTF_USESHOWWINDOW;

   int result = CreateProcess(
       0,
       command,
       0,
       0,
       TRUE,
       (new_console ? CREATE_NEW_CONSOLE : 0),
       0,
       path,
       &startup_info,
       &process_info);
   if (result == 0)
   {
      s32 last_error = GetLastError();
      if (last_error == 2)
         error = make_error("Command %s not found", command);
      else
         error = make_error("Failed to create process with windows error code %d", last_error);
      return (error);
   }
   CloseHandle(process_info.hProcess);
   CloseHandle(process_info.hThread);

   return (0);
}

#endif
