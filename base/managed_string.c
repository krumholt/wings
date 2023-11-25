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
managed_string__from_cstring(char *value)
{
   struct managed_string result =
   {
      .cstring = 0,
      .length = 0,
   };
   error error = string_store__store(
         &_managed_strings__string_store,
         &result.cstring,
         value);
   if (error)
   {
      _managed_strings__error = error;
      return (result);
   }
   result.length = strlen(value);

   return (result);
}

struct managed_string
managed_string__join_cstring(struct managed_string a,
                               char *b)
{
   u64 b_length = strlen(b);
   struct managed_string result =
   {
      .cstring = 0,
      .length  = a.length + b_length,
   };
   error error = string_store__store_size(
         &_managed_strings__string_store,
         &result.cstring,
         result.length + 1
         );
   if (error)
   {
      _managed_strings__error = error;
      result.length = 0;
      return result;
   }

   for (u32 index = 0;
        index < a.length;
        ++index)
   {
      result.cstring[index] = a.cstring[index];
   }
   for (u32 index = 0;
        index < b_length;
        ++index)
   {
      result.cstring[index + a.length] = b[index];
   }

   return (result);
}

struct managed_string
managed_string__join(struct managed_string a,
                     struct managed_string b)
{
   struct managed_string result =
   {
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
      result.length = 0;
      return result;
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

struct managed_string
managed_string__join_with_separator(
      struct managed_string a,
      struct managed_string b,
      char *separator)
{
   u32 separator_length = strlen(separator);
   struct managed_string result =
   {
      .length = a.length + b.length + separator_length,
   };
   error error = string_store__store_size(
         &_managed_strings__string_store,
         &result.cstring,
         result.length + 1
         );
   if (error)
   {
      _managed_strings__error = error;
      result.length = 0;
      return result;
   }

   for (u32 index = 0;
        index < a.length;
        ++index)
   {
      result.cstring[index] = a.cstring[index];
   }
   char *source = separator;
   char *target = result.cstring + a.length;
   while(*source)
   {
      *target = *source;
      source += 1;
      target += 1;
   }
   for (u32 index = 0;
        index < b.length;
        ++index)
   {
      result.cstring[index + a.length + separator_length] = b.cstring[index];
   }

   return (result);
}

struct managed_string
managed_string__join_with_separator_3(
      struct managed_string a,
      struct managed_string b,
      struct managed_string c,
      char *separator)
{
   u32 separator_length = strlen(separator);
   struct managed_string result =
   {
      .length = a.length + b.length + separator_length + separator_length,
   };
   error error = string_store__store_size(
         &_managed_strings__string_store,
         &result.cstring,
         result.length + 1
         );
   if (error)
   {
      _managed_strings__error = error;
      result.length = 0;
      return result;
   }

   for (u32 index = 0;
        index < a.length;
        ++index)
   {
      result.cstring[index] = a.cstring[index];
   }
   char *source = separator;
   char *target = result.cstring + a.length;
   while(*source)
   {
      *target = *source;
      source += 1;
      target += 1;
   }
   for (u32 index = 0;
        index < b.length;
        ++index)
   {
      result.cstring[index + a.length + separator_length] = b.cstring[index];
   }

   source = separator;
   target = result.cstring + a.length + separator_length + b.length;
   while(*source)
   {
      *target = *source;
      source += 1;
      target += 1;
   }
   for (u32 index = 0;
        index < c.length;
        ++index)
   {
      result.cstring[index + a.length + b.length + separator_length + separator_length] = c.cstring[index];
   }

   return (result);
}

struct managed_string
managed_string__join_3(
      struct managed_string a,
      struct managed_string b,
      struct managed_string c)
{
   struct managed_string result =
   {
      .length    = a.length + b.length + c.length,
   };
   error error = string_store__store_size(
         &_managed_strings__string_store,
         &result.cstring,
         result.length + 1
         );
   if (error)
   {
      _managed_strings__error = error;
      result.length = 0;
      return result;
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
   for (u32 index = 0;
        index < c.length;
        ++index)
   {
      result.cstring[index + a.length + b.length] = c.cstring[index];
   }

   return (result);
}

#endif
