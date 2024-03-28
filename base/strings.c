#ifndef WINGS_BASE_STRINGS_C_
#define WINGS_BASE_STRINGS_C_

#include "types.h"
#include "errors.h"
#include "allocators.h"
#include "cstrings.h"

#include <stdio.h>
#include <string.h>

#include "strings.h"


error
string__new(struct string *string, u32 length, struct allocator *allocator)
{
   error error    = 0;
   string->length = length;
   error          = allocate_array(&string->start, allocator, string->length + 1, char);
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
      string->start[index] = cstring[index];
   }
   return (0);
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
      target->start[index] = local_a.start[index];
   }
   for (u64 index = 0; index < b_length; ++index)
   {
      target->start[local_a.length + index] = b[index];
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
      if (source.start[left_length] == c)
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
      left->start[index] = source.start[index];
   }
   for (u64 index = 0; index < right->length; ++index)
   {
      right->start[index] = source.start[left_length + found + index];
   }
   return (0);
}

struct string_view
string_view__set_to_next_line(struct string_view *context)
{
   int line_length = 0;
   struct string_view line = { 0 };
   line.start = context->start;
   while (context->length)
   {
      if (context->length >= 2
          && ((context->start[0] == '\n' && context->start[1] == '\r') || (context->start[0] == '\r' && context->start[1] == '\n')))
      {
         line.length = line_length;
         context->length -= 2;
         context->start += 2;
         return (line);
      }
      else if (context->length >= 1 && (context->start[0] == '\n' || context->start[0] == '\r'))
      {
         line.length = line_length;
         context->length -= 1;
         context->start += 1;
         return (line);
      }
      context->length -= 1;
      context->start += 1;
      line_length += 1;
   }
   line.length = line_length;
   context->start += line_length;
   context->length = 0;
   return (line);
}

struct string_view
make_string_view(char *text)
{
   struct string_view new_string = {
      .start = text,
      .length = strlen(text),
   };
   return(new_string);
}

error
allocate_string_view(struct string_view *view, struct allocator *allocator, u64 size, char *format, ...)
{
   char *new_string;
   error error = allocate_array(&new_string, allocator, size, char);
   IF_ERROR_RETURN(error);
   va_list arg_list;
   va_start(arg_list, format);
   vsnprintf(new_string, size - 1, format, arg_list);
   va_end(arg_list);
   view->start = new_string;
   view->length = strlen(new_string);
   return(0);
}

b32
string_view__equals(struct string_view a, struct string_view b)
{
   if (a.length != b.length)
   {
      return(0);
   }
   return(strncmp(a.start, b.start, a.length) == 0);
}

#endif
