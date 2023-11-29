#ifndef WINGS_BASE_CSTRINGS_C_
#define WINGS_BASE_CSTRINGS_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"
#include "wings/base/cstrings.h"

b32
cstring__is_digit(char c)
{
   return c >= '0' && c <= '9';
}

b32
cstring__is_whitespace(char c)
{
   return ((c >= 9 && c <= 13) || c == ' ');
}

char *
cstring__pointer_to_trailing_zero(char *s)
{
   char *tmp = s;
   while (*tmp)
      ++tmp;
   return (tmp);
}

error
cstring__copy(char **target, u32 source_length, char *source, struct allocator *allocator)
{
   error error = allocate_array(target, allocator, source_length + 1, char);
   IF_ERROR_RETURN(error);
   if (source_length)
   {
      memcpy(*target, source, source_length);
   }

   return(0);
}

error
cstring__base_name(char **result,
                   u64 path_length,
                   char *path,
                   struct allocator *allocator)
{
   u64 index = 0;
   error error = 0;
   error = cstring__get_last_index(&index, path, '\\');
   if (error)
   {
      error = cstring__copy(result, path_length, path, allocator);
      return (error);
   }
   u64 length = path_length - index - 1;
   error = cstring__copy(result, length, path + index + 1, allocator);
   return(error);
}

void
cstring__replace(s32 length, char *target, char old_char, char new_char)
{
   for (s32 index = 0; index < length; ++index)
   {
      if (target[index] == old_char)
         target[index] = new_char;
   }
}

error
cstring__get_first_index(u64 *index, char *haystack, char needle)
{
   ASSERT(index);
   ASSERT(haystack);
   char *haystack_start = haystack;
   while (*haystack)
   {
      if (*haystack == needle)
      {
         *index = haystack - haystack_start;
         return 0;
      }
      ++haystack;
   }
   return ec_base_cstrings__not_found;
}

error
cstring__get_last_index(u64 *index, char *haystack, char needle)
{
   ASSERT(index);
   ASSERT(haystack);
   char *haystack_start = haystack;
   while (*haystack)
   {
      ++haystack;
   };
   while(haystack != haystack_start)
   {
      --haystack;
      if (*haystack == needle)
      {
         *index = haystack - haystack_start;
         return ec__no_error;
      }
   }
   if (*haystack == needle)
   {
      *index = 0;
      return ec__no_error;
   }
   return ec_base_cstrings__not_found;
}

b32
cstring__ends_with(char *haystack, s32 haystack_length, char *needle, s32 needle_length)
{
   if (needle_length > haystack_length)
      return 0;
   while (needle_length)
   {
      --needle_length;
      --haystack_length;
      if (haystack[haystack_length] != needle[needle_length])
         return 0;
   }
   return 1;
}

b32
cstring__begins_with(char *a, u64 length_a, char *b, u64 length_b)
{
   u64 index = 0;
   for (; index < length_a && index < length_b; index += 1)
   {
      if (a[index] != b[index])
         return 0;
   }
   return index == length_b;
}


#endif
