#ifndef WINGS_BASE_STRINGS_H_
#define WINGS_BASE_STRINGS_H_

#include "wings/base/types.h"
#include "wings/base/allocators.c"


typedef struct string_view
{
   char *start;
   u64   length;
} String_view;

b32
string_view__equals(struct string_view a, struct string_view b);

struct string_view
string_view__set_to_next_line(struct string_view *context);

error
string__new (struct string     *string,
             u32                length,
             struct allocator  *allocator);

error
string__from_cstring (struct string     *string,
                      u64                cstring_length,
                      char              *cstring,
                      struct allocator  *allocator);
 
error
string__join_cstring (struct string     *target,
                      struct string      a,
                      u64                b_length,
                      char              *b,
                      struct allocator  *allocator);

u64
string__split (struct string      *left,
               struct string      *right,
               struct string      source,
               char               c,
               struct allocator  *allocator);

b32
string_view__equals(struct string_view a, struct string_view b);

struct string_view
make_string_view(char *text);

error
allocate_string_view(struct string_view *view, struct allocator *allocator, u64 size, char *format, ...);

#endif
