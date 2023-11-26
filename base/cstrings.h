#ifndef WINGS_BASE_CSTRINGS_H_
#define WINGS_BASE_CSTRINGS_H_

#include "wings/base/types.h"


b32     cstring__is_digit(char c);
b32     cstring__is_whitespace(char c);
char *  cstring__pointer_to_trailing_zero(char *s);
void    cstring__replace(char *target, s32 length, char old_char, char new_char);
char    cstring__get_character_in_string(char needle, char *haystack);
b32     cstring__ends_with(char *haystack, s32 haystack_length, char *needle, s32 needle_length);
b32     cstring__begins_with(char *haystack, u64 haystack_length, char *needle, u64 needle_length);

#endif
