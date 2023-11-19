#ifndef WINGS_BASE_MANAGED_STRING_C
#define WINGS_BASE_MANAGED_STRING_C

#include "wings/base/types.h"
#include "wings/base/macros.c"
#include "wings/base/string_store.c"
#include "wings/base/allocators.c"

struct managed_string
{
   char  *cstring;
   u64    length;
   b8     writeable;
   b8     readable;
};

struct string_store _managed_strings__string_store = {0};
b32 _managed_strings__initialised = 0;
error _managed_strings__error = 0;

error
managed_strings__initialise(u64 size, struct allocator *allocator)
{
   ASSERT(!_managed_strings__initialised);
   _managed_strings__initialised = 1;
   error error = string_store__new(&_managed_strings__string_store, size, allocator);
   return (error);
}

struct managed_string
managed_strings__new(char *value)
{
   struct managed_string result =
   {
      .cstring = 0,
      .writeable = 1,
      .readable = 1,
   };
   error error = string_store__store(
         &_managed_strings__string_store,
         &result.cstring,
         value);
   
   if (error)
   {
      _managed_strings__error = error;
      result.writeable = 0;
      result.readable = 0;
   }

   return (result);
}

#endif
