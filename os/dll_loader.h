#ifndef WINGS_OS_DLL_LOADER_H_
#define WINGS_OS_DLL_LOADER_H_

#include "base/types.h"
#include "base/allocators.h"

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
dll_needs_update(dynamic_library *lib);

error
dll_reload(dynamic_library *lib);

error
dll_make(
      dynamic_library *lib,
      char *path,
      s32    number_of_functions,
      char **function_names,
      void **function_pointers,
      struct allocator *allocator);

#endif
