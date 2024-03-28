#if 0
void
ws_string_copy(char *target,
               s32 target_length,
               char *source,
               s32 source_length)
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
      if (!cstring__is_digit(c))
         return string_to_f32_illegal_character;
      *result *= 10.0;
      *result += (c - '0');
   }
   f64 after_comma_result = 0.0;
   f64 first              = 0.1;
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (!cstring__is_digit(c))
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
      if (!cstring__is_digit(c))
         return string_to_f32_illegal_character;
      *result *= 10.0f;
      *result += (c - '0');
   }
   f32 after_comma_result = 0.0f;
   f32 first              = 0.1f;
   for (; index < string_length; ++index)
   {
      char c = string[index];
      if (!cstring__is_digit(c))
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
      if (!cstring__is_digit(c))
         return string_to_s32_illegal_character;
      *result *= 10;
      *result += c - '0';
   }

   *result = *result * sign;
   return string_to_s32_no_error;
}
#endif
