#ifndef WINGS_OS_LINUX_PROCESS_C_
#define WINGS_OS_LINUX_PROCESS_C_

#include "wings/base/macros.c"
#include "wings/base/error_codes.c"
#include "wings/base/types.c"

#include <stdio.h>
#include <stdlib.h>

error
run_command_at(char *command, char *directory, char *result_buffer, u32 result_buffer_size)
{
   char final_command[2048] = { 0 };
   snprintf(final_command, 2047, "cd %s; %s", directory, command);
   printf("%s\n", final_command);
   FILE *file   = popen(final_command, "r");
   char *result = fgets(result_buffer, result_buffer_size, file);
   if (result != result_buffer)
      return (1);
   return (0);
}

error
run_command(char *command, char *result_buffer, u32 result_buffer_size)
{
   return (run_command_at(command, "./", result_buffer, result_buffer_size));
}

error
process_new(char *command, char *path)
{
   char final_command[2048] = { 0 };
   snprintf(final_command, 2047, "cd %s; %s", path, command);
   system(final_command);
   return (ec__no_error);
}

#endif
