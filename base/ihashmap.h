#ifndef WINGS_BASE_IHASHMAP_H_
#define WINGS_BASE_IHASHMAP_H_

#include "wings/base/strings.h"
#include "wings/base/allocators.c"

struct ihashmap_key_value
{
   u64    key;
   void  *value;
};

struct ihashmap
{
   u64                       collisions;
   u64                       capacity;
   u64                       used;
   struct ihashmap_key_value *entries;
};

error
ihashmap__new (struct ihashmap *map, u64 capacity, struct allocator *allocator);

error
ihashmap__insert(struct ihashmap *map, struct string_view key, void *value);

void *
ihashmap__find(struct ihashmap map, struct string_view key);

#endif
