#ifndef WINGS_BASE_STRINGS_C_
#define WINGS_BASE_STRINGS_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"
#include "wings/base/string_store.c"
#include "wings/base/cstrings.h"

#include <stdio.h>
#include <string.h>


error
string__new(struct string *string, u32 length, struct allocator *allocator)
{
   error error    = ec__no_error;
   string->length = length;
   error          = allocate_array(&string->first, allocator, string->length + 1, char);
   return (error);
}

error
string__from_cstring(
      struct string *string,
      u64 cstring_length,
      char *cstring,
      struct allocator *allocator)
{
   error error = string__new(string, cstring_length, allocator);
   IF_ERROR_RETURN(error);
   for (u64 index = 0; index < cstring_length; ++index)
   {
      string->first[index] = cstring[index];
   }
   return (ec__no_error);
}

error
string__join_cstring(struct string *target,
                     struct string a,
                     u64 b_length,
                     char *b,
                     struct allocator *allocator)
{
   // make a local copy of a in case a == *target
   struct string local_a = a;
   error error = string__new(target, local_a.length + b_length, allocator);
   IF_ERROR_RETURN(error);

   for (u64 index = 0; index < local_a.length; ++index)
   {
      target->first[index] = local_a.first[index];
   }
   for (u64 index = 0; index < b_length; ++index)
   {
      target->first[local_a.length + index] = b[index];
   }

   return (0);
}

u64
string__split (struct string      *left,
               struct string      *right,
               struct string      source,
               char               c,
               struct allocator  *allocator)
{
   u64 left_length = 0;
   b32 found = 0;
   for (; left_length < source.length; ++left_length)
   {
      if (source.first[left_length] == c)
      {
         found = 1;
         break;
      }
   }
   error error = 0;
   error = string__new(left, left_length, allocator);
   IF_ERROR_RETURN(error);
   error = string__new(right, source.length - left_length - found, allocator);
   IF_ERROR_RETURN(error);

   for (u64 index = 0; index < left->length; ++index)
   {
      left->first[index] = source.first[index];
   }
   for (u64 index = 0; index < right->length; ++index)
   {
      right->first[index] = source.first[left_length + found + index];
   }
   return (ec__no_error);
}
#endif
