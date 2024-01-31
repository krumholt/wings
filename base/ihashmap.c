#ifndef WINGS_BASE_IHASHMAP_C_
#define WINGS_BASE_IHASHMAP_C_

#include "wings/base/types.h"
#include "wings/base/macros.c"

#include "wings/base/ihashmap.h"

error
ihashmap__new (struct ihashmap *map, u64 capacity, struct allocator *allocator)
{
   error error = allocate_array(&map->entries, allocator, capacity, struct ihashmap_key_value);
   IF_ERROR_RETURN(error);
   map->capacity = capacity;
   map->used = 0;
   return (ec__no_error);
}

u64
ihashmap__hash(u64 key)
{
}

error
ihashmap__insert_at(struct ihashmap *map, u64 index, u64 key, void *value)
{
   if (map->used == map->capacity) return(ec_base_hashmap__full);
   u64 key_index = index % map->capacity;
   struct ihashmap_key_value *entry = map->entries + key_index;
   for (u32 index = 0; index < map->capacity; ++index)
   {
      entry = map->entries + ((key_index + index) % map->capacity);
      if (entry->value == 0)
      {
         entry->key = key;
         entry->value = value;
         map->used += 1;
         return (ec__no_error);
      }
      else
      {
         if (entry->key == key)
         {
            return(ec_base_hashmap__key_exists);
         }
         map->collisions += 1;
      }
   }
   return(ec_base_hashmap__full);
}

void *
ihashmap__find(struct ihashmap map, struct string_view key)
{
   u64 key_index = ihashmap__hash(key.length, key.start) % map.capacity;
   struct ihashmap_key_value *entry = map.entries + key_index;
   for (u32 index = 0; index < map.capacity; ++index)
   {
      entry = map.entries + ((key_index + index) % map.capacity);
      if (entry->value == 0) return(0);
      if (string_view__equals(key, entry->key))
      {
         return(entry->value);
      }
   }
   return(0);
}

error
ihashmap__insert(struct ihashmap *map, struct string_view key, void *value)
{
   if (map->used == map->capacity) return(ec_base_ihashmap__full);
   if (!value) return(ec_base_ihashmap__null_value_not_allowed);
   u64 key_index = ihashmap__hash(key.length, key.start) % map->capacity;

   error error = ihashmap__insert_at(map, key_index, key, value);

   return(error);
}


#endif
