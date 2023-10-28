// 'inspired' by https://github.com/tsoding/musializer   see nob.h and nob.c
// or https://github.com/tsoding/nobuild

#ifndef NOBUILD_C_
#define NOBUILD_C_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

struct process
{
   PROCESS_INFORMATION process_info;
};

#define ERROR_COMMAND_NOT_FOUND 7
#define GENERAL_ERROR 1

error
run_command(char *command)
{
   STARTUPINFO         startup_info = { 0 };
   PROCESS_INFORMATION process_info;
   startup_info.cb = sizeof(STARTUPINFO);

   b32 result = CreateProcess(
       0,
       command,
       0,
       0,
       0,
       0,
       0,
       0,
       &startup_info,
       &process_info);
   if (result == 0)
   {
      u32 last_error = GetLastError();
      if (last_error == 2)
         return (ERROR_COMMAND_NOT_FOUND);
      else
         return (GENERAL_ERROR);
   }
   WaitForSingleObject(process_info.hProcess, INFINITE);
   CloseHandle(process_info.hProcess);
   CloseHandle(process_info.hThread);

   return (NO_ERROR);
}

void
check_available_compilers(void)
{
   int noclang, nocl, nogcc, notcc;
   noclang = run_command("clang --version");
   nocl    = run_command("cl");
   nogcc   = run_command("gcc --version");
   notcc   = run_command("tcc");

   if (!noclang)
      printf("we have clang yey\n");
   if (!nocl)
      printf("we have cl yey\n");
   if (!nogcc)
      printf("we have gcc yey\n");
   if (!notcc)
      printf("we have tcc yey\n");
}

struct c_compiler
{
   char *command;
};

struct c_compiler
get_prefered_c_compiler(void)
{
   struct c_compiler c_compiler = { 0 };
   return c_compiler;
}

int
main(void)
{
}

#endif
