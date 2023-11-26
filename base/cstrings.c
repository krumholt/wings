#ifndef WINGS_BASE_CSTRINGS_C_
#define WINGS_BASE_CSTRINGS_C_

#include "wings/base/types.h"

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
cstring__replace(char *target, s32 length, char old_char, char new_char)
{
   for (s32 index = 0; index < length; ++index)
   {
      if (target[index] == old_char)
         target[index] = new_char;
   }
}

char
cstring__get_character_in_string(char c, char *c_string)
{
   if (!c_string)
      return 0;
   while (*c_string)
   {
      if (*c_string == c)
         return c;
      ++c_string;
   }
   return 0;
}

b32
cstring__ends_with(char *haystack, s32 haystack_length, char *needle, s32 needle_length)
{
   if (needle_length > haystack_length || needle_length == 0)
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
