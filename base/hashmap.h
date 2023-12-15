#ifndef WINGS_BASE_HASHMAP_H_
#define WINGS_BASE_HASHMAP_H_

#include "wings/base/strings.h"
#include "wings/base/allocators.c"

struct hashmap_key_value
{
   struct string_view  key;
   void               *value;
};

struct hashmap
{
   u64                       capacity;
   u64                       used;
   struct hashmap_key_value *entries;
};

error
hashmap__new (struct hashmap *map, u64 capacity, struct allocator *allocator);


#endif
