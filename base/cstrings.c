#ifndef WINGS_BASE_CSTRINGS_C_
#define WINGS_BASE_CSTRINGS_C_

#include "types.h"
#include "errors.h"
#include "allocators.h"
#include "cstrings.h"

#include <stdio.h>

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

void
cstring__vformat(char *cstring, u64 size, char *format, va_list arg_list)
{
   vsnprintf(cstring, size - 1, format, arg_list);
}

void
cstring__format(char *cstring, u64 size, char *format, ...)
{
   va_list arg_list;
   va_start(arg_list, format);
   vsnprintf(cstring, size - 1, format, arg_list);
   va_end(arg_list);
}

error
cstring__copy(char **target, u32 source_length, char *source, struct allocator *allocator)
{
   error error = allocate_array(target, allocator, source_length + 1, char);
   IF_ERROR_RETURN(error);
   if (source_length)
   {
      memmove(*target, source, source_length);
   }

   return(0);
}

error
cstring__join(char **result,
              u64 a_length,
              char  *a,
              u64 b_length,
              char *b,
              struct allocator *allocator)
{
   // make a local copy of a in case a == *result
   char *local_a = a;
   error error = allocate_array(result, allocator, a_length + b_length, char);
   IF_ERROR_RETURN(error);

   for (u64 index = 0; index < a_length; ++index)
   {
      (*result)[index] = local_a[index];
   }
   for (u64 index = 0; index < b_length; ++index)
   {
      (*result)[a_length + index] = b[index];
   }

   return (0);
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

error
cstring__remove_file_extension (char **result,
                                char  *path,
                                struct allocator *allocator)
{
   u64 index = 0;
   u64 last_separator_index = 0;
   error error = 0;
   error = cstring__get_last_index(&last_separator_index, path, '\\');
   if (error) last_separator_index = 0;
   error = cstring__get_last_index(&index, path, '.');
   if (error || last_separator_index > index)
   {
      error = cstring__copy(result, strlen(path), path, allocator);
      return (error);
   }
   u64 length = index;
   error = cstring__copy(result, length, path, allocator);
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

b32
cstring__get_first_index2(u64 *index, char *haystack, char needle) //@TODO: bench me vs cstring__get_first_index
{
   char *at = strchr(haystack, needle);
   *index = at - haystack;
   return at != 0;
}

b32
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
         return 1;
      }
      ++haystack;
   }
   return 0;
}

b32
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
         return 1;
      }
   }
   if (*haystack == needle)
   {
      *index = 0;
      return 1;
   }
   return 0;
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
