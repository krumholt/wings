#ifndef WINGS_OS_DYNAMIC_LOADER_C_
#define WINGS_OS_DYNAMIC_LOADER_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>


struct dynamic_library
{
   char *path;
};

typedef struct dynamic_library dynamic_library;

error
load_functions(dynamic_library lib, s32 number_of_functions, char **function_names, void **function_pointers)
{
   HMODULE module = LoadLibrary(lib.path);
   if (module == 0) {
      return ec_os_file__not_found;
   }
   for (s32 index = 0; index < number_of_functions; ++index)
   {
      function_pointers[index] = GetProcAddress(module, function_names[index]);
   }

   return(ec__no_error);
}

#endif
