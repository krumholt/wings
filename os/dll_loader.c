#ifndef WINGS_OS_DLL_LOADER_C_
#define WINGS_OS_DLL_LOADER_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/cstrings.h"
#include "wings/os/file.c"
#include "wings/os/timer.c"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>


struct dynamic_library
{
   char    *dll_path;
   char    *tmp_dll_path;
   HMODULE  handle;
   u64      last_load_time;
   b64      loaded;
   s32      number_of_functions;
   char   **function_names;
   void   **function_pointers;
};

typedef struct dynamic_library dynamic_library;

b32
dll_needs_update(dynamic_library *lib)
{
   u64 last_write_time = 0;
   u64 last_load_time = 0;
   error error = file_get_last_write_time(&last_write_time, lib->dll_path);
   IF_ERROR_RETURN(error);
   error = file_get_last_write_time(&last_load_time, lib->tmp_dll_path);
   IF_ERROR_RETURN(error);
   return (last_load_time < last_write_time);
}

error
dll_reload(dynamic_library *lib)
{
   error error = 0;
   if (lib->loaded)
   {
      lib->loaded = 0;
      BOOL result = FreeLibrary(lib->handle);
      if (result == 0) return ec_os_dynamic_loader__free_library_failed;
   }
   f64 timeout_s = 2.0;
   f64 time_used_s = 0.0;
   f64 start_time = get_os_timer_in_seconds();
   do{
      time_used_s = get_os_timer_in_seconds() - start_time;
      error = file_copy(lib->dll_path, lib->tmp_dll_path);
   } while(error && time_used_s < timeout_s);
   IF_ERROR_RETURN(error);
   lib->handle = LoadLibrary(lib->tmp_dll_path);
   for (s32 index = 0; index < lib->number_of_functions; ++index)
   {
      lib->function_pointers[index] = GetProcAddress(lib->handle, lib->function_names[index]);
   }
   lib->loaded = 1;
   error = file_get_last_write_time(&lib->last_load_time, lib->dll_path);
   return(ec__no_error);
}

error
dll_make(dynamic_library *lib,
      char *path,
      s32    number_of_functions,
      char **function_names,
      void **function_pointers,
      struct allocator *allocator)
{
   lib->dll_path = path;
   lib->number_of_functions = number_of_functions;
   lib->function_names    = function_names;
   lib->function_pointers = function_pointers;
   char *postfix = "_tmp";
   error error = cstring__join(
         &lib->tmp_dll_path,
         strlen(lib->dll_path),
         lib->dll_path,
         strlen(postfix),
         postfix,
         allocator);
   IF_ERROR_RETURN(error);
   error = dll_reload(lib);
   IF_ERROR_RETURN(error);
   return(ec__no_error);
}

#endif
