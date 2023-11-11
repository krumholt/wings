#ifndef WINGS_BASE_STRINGS_C_
#define WINGS_BASE_STRINGS_C_

#include "wings/base/types.c"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"

#include <stdio.h>
#include <string.h>

struct string
{
   char *first;
   u64   length;
};

struct token
{
   struct string string;
   b32           is_incomplete_string;
   b32           is_float;
   b32           is_integer;
   b32           is_newline;
   b32           is_empty;
};

enum string_to_f32_error
{
   string_to_f32_no_error,
   string_to_f32_illegal_character,
   string_to_f32_empty_string,
};

enum string_to_s32_error
{
   string_to_s32_no_error,
   string_to_s32_illegal_character,
   string_to_s32_empty_string,
};

error
make_string(struct string *string, u32 length, struct allocator *allocator)
{
   error error    = ec__no_error;
   string->length = length;
   error          = allocate_array(&string->first, allocator, string->length, char);
   return (error);
}

int
is_digit(char c)
{
   return c >= '0' && c <= '9';
}

int
is_whitespace(char c)
{
   return ((c >= 9 && c <= 13) || c == ' ');
}

char *
cstring_end_pointer(char *s)
{
   char *tmp = s;
   while (*tmp)
      ++tmp;
   return (tmp);
}

u32
split_cstring(char *s, char c, struct string *left, struct string *right) // @TODO: this is wrong
{
   u32 count       = 0;
   u32 left_length = 0;
   left->first     = s;
   while (*s)
   {
      count += 1;
      if (*s == c)
      {
         if (left_length == 0)
            left_length = count;
      }
      s += 1;
   }
   if (left_length == 0)
      left_length = count;

   left->length  = left_length - 1;
   right->first  = left->first + left_length;
   right->length = count - left_length;
   //if (right->length == -1)
   //   right->length = 0;
   return left_length;
}

b32
compare_string_cstring(struct string a, char *b)
{
   for (u32 index = 0; index < a.length && *b; ++index, ++b)
   {
      if (a.first[index] != *b)
         return 1;
   }
   return (0);
}

void
move_forward_to_string(struct string *haystack, char *needle)
{
   while (haystack->length
          && compare_string_cstring(*haystack, needle) != 0)
   {
      haystack->first++;
      haystack->length--;
   }
}

enum string_to_f32_error
string_to_f64(char *string, s32 string_length, f64 *result)
{
   if (string_length == 0)
      return string_to_f32_empty_string;
   *result   = 0.0f;
   s32 index = 0;
   f64 sign  = 1.0;
   if (string[0] == '+')
   {
      index += 1;
   }
   else if (string[0] == '-')
   {
      sign = -1.0f;
      index += 1;
   }
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (c == '.')
      {
         index += 1; // eat the dot
         break;
      }
      if (!is_digit(c))
         return string_to_f32_illegal_character;
      *result *= 10.0;
      *result += (c - '0');
   }
   f64 after_comma_result = 0.0;
   f64 first              = 0.1;
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (!is_digit(c))
         return string_to_f32_illegal_character;
      after_comma_result += (c - '0') * first;
      first *= 0.1;
   }
   *result += after_comma_result;
   *result *= sign;
   return string_to_f32_no_error;
}

enum string_to_f32_error
string_to_f32(char *string, s32 string_length, f32 *result)
{
   if (string_length == 0)
      return string_to_f32_empty_string;
   *result   = 0.0f;
   s32 index = 0;
   f32 sign  = 1.0f;
   if (string[0] == '+')
   {
      index += 1;
   }
   else if (string[0] == '-')
   {
      sign = -1.0f;
      index += 1;
   }
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (c == '.')
      {
         index += 1; // eat the dot
         break;
      }
      if (!is_digit(c))
         return string_to_f32_illegal_character;
      *result *= 10.0f;
      *result += (c - '0');
   }
   f32 after_comma_result = 0.0f;
   f32 first              = 0.1f;
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (!is_digit(c))
         return string_to_f32_illegal_character;
      after_comma_result += (c - '0') * first;
      first *= 0.1f;
   }
   *result += after_comma_result;
   *result *= sign;
   return string_to_f32_no_error;
}

enum string_to_s32_error
string_to_s32(char *string, s32 string_length, s32 *result)
{
   if (string_length == 0)
      return string_to_s32_empty_string;
   s32 sign  = 1;
   s32 index = 0;
   if (string[0] == '+')
   {
      index++;
   }
   else if (string[0] == '-')
   {
      index++;
      sign = -1;
   }
   *result = 0;
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (!is_digit(c))
         return string_to_s32_illegal_character;
      *result *= 10;
      *result += c - '0';
   }

   *result = *result * sign;
   return string_to_s32_no_error;
}

void
cstring_replace(char *target, s32 length, char old_char, char new_char)
{
   for (s32 index = 0; index < length; ++index)
   {
      if (target[index] == old_char)
         target[index] = new_char;
   }
}

char
cstring_is_one_of(char c, char *c_string)
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
cstring_ends_with(char *haystack, s32 haystack_length, char *needle, s32 needle_length)
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
begins_with_cstring(char *a, u64 length_a, char *b, u64 length_b)
{
   u64 index = 0;
   for (; index < length_a && index < length_b; index += 1)
   {
      if (a[index] != b[index])
         return 0;
   }
   return index == length_b;
}

struct string
next_word(struct string *context)
{
   struct string word = *context;
   word.length        = 0;
   for (u32 at = 0; at < context->length; ++at)
   {
      if (is_whitespace(context->first[at]))
      {
         context->length -= (word.length + 1);
         context->first += (word.length + 1);
         while (context->length > 0 && is_whitespace(context->first[0]))
         {
            context->length -= 1;
            context->first += 1;
         }
         return (word);
      }
      word.length++;
   }
   context->first += context->length;
   context->length = 0;
   return (word);
}

s32
is_token_separator(char c)
{
   return 0
          || c == ','
          || c == '/'
          || c == '|'
          || c == ' ';
}

struct string
next_line(struct string *context)
{
   int           line_length = 0;
   struct string line        = { 0 };
   line.first                = context->first;
   while (context->length)
   {
      if (context->length >= 2
          && ((context->first[0] == '\n' && context->first[1] == '\r') || (context->first[0] == '\r' && context->first[1] == '\n')))
      {
         line.length = line_length;
         context->length -= 2;
         context->first += 2;
         return (line);
      }
      else if (context->length >= 1 && (context->first[0] == '\n' || context->first[0] == '\r'))
      {
         line.length = line_length;
         context->length -= 1;
         context->first += 1;
         return (line);
      }
      context->length -= 1;
      context->first += 1;
      line_length += 1;
   }
   line.length = line_length;
   context->first += line_length;
   context->length = 0;
   return (line);
}

typedef int (*seperator_fn)(char);

struct token
next_token_with_separator(struct string *context, seperator_fn is_token_separator)
{
   struct token token = { 0 };
   if (context->length == 0)
   {
      token.is_empty = 1;
      return (token);
   }
   while (is_token_separator(context->first[0]))
   {
      context->length -= 1;
      context->first += 1;
      if (context->length == 0)
      {
         token.string.length = 0;
         return token;
      }
   }
   token.string        = *context;
   token.string.length = 0;
   if (context->length >= 2
       && ((context->first[0] == '\n' && context->first[1] == '\r') || (context->first[0] == '\r' && context->first[1] == '\n')))
   {
      token.is_newline    = 1;
      token.string.length = 2;
      context->length -= 2;
      context->first += 2;
      return (token);
   }
   else if (context->length >= 1 && (context->first[0] == '\n' || context->first[0] == '\r'))
   {
      token.is_newline    = 1;
      token.string.length = 1;
      context->length -= 1;
      context->first += 1;
      return (token);
   }
   else if (
       context->length >= 1 && (context->first[0] == ':' || context->first[0] == '=' || context->first[0] == ';'))

   {
      token.string.length = 1;
      context->length -= 1;
      context->first += 1;
      return (token);
   }
   else if (context->length && context->first[0] == '\"')

   {
      token.string.first += 1;
      context->length -= 1;
      context->first += 1;
      while (context->length && context->first[0] != '\"')
      {
         context->length -= 1;
         context->first += 1;
         token.string.length += 1;
      }
      if (context->length)
      {
         context->length -= 1;
         context->first += 1;
      }
      else
      {
         token.is_incomplete_string = 1;
      }
      return (token);
   }
   b32 only_numbers = 1;
   b32 contains_dot = 0;
   for (u32 at = 0; at < context->length; ++at)
   {
      if (is_token_separator(context->first[at]) || context->first[at] == '\n' || context->first[at] == '\r')
      {
         if (only_numbers)
         {
            if (contains_dot == 1)
               token.is_float = 1;
            else if (contains_dot == 0)
               token.is_integer = 1;
         }
         context->length -= (token.string.length);
         context->first += (token.string.length);
         return (token);
      }
      if (context->first[at] == '.')
      {
         contains_dot += 1;
      }
      else if (!is_digit(context->first[at]))
      {
         only_numbers = 0;
      }
      token.string.length++;
   }
   if (only_numbers)
   {
      if (contains_dot == 1)
         token.is_float = 1;
      else if (contains_dot == 0)
         token.is_integer = 1;
   }
   context->first += context->length;
   context->length = 0;
   return (token);
}

struct token
next_token(struct string *context)
{
   return (next_token_with_separator(context, is_token_separator));
}

void
ws_string_copy(char *target, s32 target_length, char *source, s32 source_length)
{
   while (*target && *source && target_length && source_length)
   {
      *target = *source;
      target += 1;
      source += 1;
      target_length -= 1;
      source_length -= 1;
   }
}

struct token
skip_newlines(struct string *context, s32 *lines_skiped)
{
   struct token token = next_token(context);
   while (token.is_newline)
   {
      if (lines_skiped)
         *lines_skiped += 1;
      token = next_token(context);
   }
   return (token);
}

#endif
