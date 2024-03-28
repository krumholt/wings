#ifndef WINGS_BASE_HASHMAP_H_
#define WINGS_BASE_HASHMAP_H_

#include "strings.h"
#include "allocators.h"

struct hashmap_key_value
{
   struct string_view  key;
   void               *value;
};

struct hashmap
{
   u64                       collisions;
   u64                       capacity;
   u64                       used;
   struct hashmap_key_value *entries;
};

error
hashmap__new (struct hashmap *map, u64 capacity, struct allocator *allocator);

error
hashmap__insert(struct hashmap *map, struct string_view key, void *value);

void *
hashmap__find(struct hashmap map, struct string_view key);

#endif
