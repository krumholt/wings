#ifndef WINGS_OS_WINDOWS_PROCESS_C_
#define WINGS_OS_WINDOWS_PROCESS_C_

#include "wings/base/types.h"
#include "wings/base/macros.c"
#include "wings/base/error_codes.c"

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
      return (ec_os_process__failed_to_create_pipe);

   STARTUPINFO         startup_info = { 0 };
   PROCESS_INFORMATION process_info = { 0 };
   startup_info.cb                  = sizeof(STARTUPINFO);
   startup_info.lpTitle             = 0;
   startup_info.hStdError           = 0;//out_pipe_write;
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
         error = ec_os_process__command_not_found;
      else
         error = ec_os_process__creation_failed;
      return (error);
   }
   DWORD chars_read = 0;
   CloseHandle(out_pipe_write);

   char *buffer_start = result_buffer;
   for (;;)
   {
      success = ReadFile(out_pipe_read,
                         result_buffer,
                         result_buffer_size - 1,
                         &chars_read,
                         0);
      printf("readifying %d but got %lu\n", result_buffer_size - 1, chars_read);
      if (!success || chars_read == 0)
      {
         DWORD last_error = GetLastError();
         printf("success: %d, error: %lu\n", success, last_error);
         result_buffer += chars_read;
         result_buffer_size -= chars_read;
         break;
      }
      result_buffer += chars_read;
      result_buffer_size -= chars_read;
   }
   printf("%s\n", buffer_start);
   //*result_buffer = 0;
   CloseHandle(out_pipe_read);
   DWORD exit_code = 0;
   GetExitCodeProcess(process_info.hProcess, &exit_code);
   CloseHandle(process_info.hProcess);
   CloseHandle(process_info.hThread);

   return (NO_ERROR);
}

error
run_command(char *command, u32 result_buffer_size, char *result_buffer)
{
   return (run_command_at(command, "./", result_buffer_size, result_buffer));
}

error
process_new(char *command, char *path, b32 new_console)
{
   error error = ec__no_error;

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
         error = ec_os_process__command_not_found;
      else
         error = ec_os_process__creation_failed;
      return (error);
   }
   CloseHandle(process_info.hProcess);
   CloseHandle(process_info.hThread);

   return (NO_ERROR);
}

#endif
