#ifndef WINGS_BASE_STRINGS_H_
#define WINGS_BASE_STRINGS_H_

#include "wings/base/types.h"
#include "wings/base/allocators.c"

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

#endif
