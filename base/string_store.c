#ifndef WINGS_BASE_STRING_STORE_C_
#define WINGS_BASE_STRING_STORE_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"

struct string_store
{
   struct string  strings;
   char          *next_free;
};

b32
string_has_room_for(u64 size, char *offset, struct string string)
{
   return((offset - string.first) + size <= string.length);
}

error
string_store__new(struct string_store *string_store,
                  u64 size,
                  struct allocator *allocator)
{
   allocate_string(&string_store->strings, size, allocator);
   string_store->next_free = string_store->strings.first;

   return ec__no_error;
}

error
string_store__store(struct string_store *string_store,
                    char **out,
                    char *in)
{
   u64 in_size = strlen(in) + 1;

   if (!string_has_room_for(in_size, string_store->next_free, string_store->strings))
   {
      return (ec_base_string_store__no_space_left);
   }
   *out = string_store->next_free;
   for(u32 index = 0; index < in_size; ++index)
   {
      string_store->next_free[index] = in[index];
   }
   string_store->next_free += in_size;

   return (ec__no_error);
}

#endif
