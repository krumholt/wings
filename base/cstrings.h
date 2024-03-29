#ifndef WINGS_BASE_CSTRINGS_H_
#define WINGS_BASE_CSTRINGS_H_

#include "types.h"
#include "allocators.h"


b32
cstring__is_digit (char c);

b32
cstring__is_whitespace (char c);

char *
cstring__pointer_to_trailing_zero (char *s);

error
cstring__copy(char **target, u32 source_length, char *source, struct allocator *allocator);

error
cstring__base_name(char **result,
                   u64 source_length,
                   char *source,
                   struct allocator *allocator);

error
cstring__remove_file_extension (char **result,
                                char  *path,
                                struct allocator *allocator);

void
cstring__replace (s32 length, char *target, char old_char, char new_char);

b32
cstring__get_first_index(u64 *index, char *haystack, char needle);

b32
cstring__get_last_index(u64 *index, char *haystack, char needle);

b32
cstring__ends_with(char *haystack, s32 haystack_length, char *needle, s32 needle_length);

b32
cstring__begins_with(char *haystack, u64 haystack_length, char *needle, u64 needle_length);

void
cstring__format(char *cstring, u64 size, char *format, ...);

void
cstring__vformat(char *cstring, u64 size, char *format, va_list arg_list);

error
cstring__join(char **result,
              u64 a_length,
              char  *a,
              u64 b_length,
              char *b,
              struct allocator *allocator);
#endif
