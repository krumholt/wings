#if 0
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


struct string
next_word(struct string *context)
{
   struct string word = *context;
   word.length        = 0;
   for (u32 at = 0; at < context->length; ++at)
   {
      if (cstring__is_whitespace(context->first[at]))
      {
         context->length -= (word.length + 1);
         context->first += (word.length + 1);
         while (context->length > 0 && cstring__is_whitespace(context->first[0]))
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
      else if (!cstring__is_digit(context->first[at]))
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
