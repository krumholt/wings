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
managed_string__initialise(u64 size, struct allocator *allocator)
{
   ASSERT(!_managed_strings__initialised);
   _managed_strings__initialised = 1;
   error error = string_store__new(&_managed_strings__string_store, size, allocator);
   return (error);
}

struct managed_string
managed_string__new(char *value)
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
   result.length = strlen(value);

   return (result);
}

struct managed_string
managed_string__append(struct managed_string a,
                       struct managed_string b)
{
   struct managed_string result =
   {
      .writeable = 1,
      .readable  = 1,
      .length    = a.length + b.length,
   };
   error error = string_store__store_size(
         &_managed_strings__string_store,
         &result.cstring,
         result.length + 1
         );
   if (error)
   {
      _managed_strings__error = error;
      result.writeable = 0;
      result.readable = 0;
   }

   for (u32 index = 0;
        index < a.length;
        ++index)
   {
      result.cstring[index] = a.cstring[index];
   }
   for (u32 index = 0;
        index < b.length;
        ++index)
   {
      result.cstring[index + a.length] = b.cstring[index];
   }

   return (result);
}

#endif