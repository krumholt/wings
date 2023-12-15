#ifndef WINGS_BASE_HASHMAP_C_
#define WINGS_BASE_HASHMAP_C_

#include "wings/base/types.h"
#include "wings/base/macros.c"

#include "wings/base/hashmap.h"

error
hashmap__new (struct hashmap *map, u64 capacity, struct allocator *allocator)
{
   error error = allocate_array(&map->entries, allocator, capacity, struct hashmap_key_value);
   IF_ERROR_RETURN(error);
   map->capacity = capacity;
   map->used = 0;
   return (ec__no_error);
}

u64
hashmap__hash(u64 length, char *key)
{
   //djb2
   u64 hash = 5381;

   for (u64 index = 0; index < length; ++index)
   {
      hash = ((hash << 5) + hash) + key[index];
   }
   return(hash);
}

error
hashmap__insert_at(struct hashmap *map, u64 index, struct string_view key, void *value)
{
   if (map->used == map->capacity) return(ec_base_hashmap__full);
   u64 key_index = index % map->capacity;
   struct hashmap_key_value *entry = map->entries + key_index;
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
         if (string_view__equals(entry->key, key))
         {
            return(ec_base_hashmap__key_exists);
         }
         map->collisions += 1;
      }
   }
   return(ec_base_hashmap__full);
}

void *
hashmap__find(struct hashmap map, struct string_view key)
{
   u64 key_index = hashmap__hash(key.length, key.start) % map.capacity;
   struct hashmap_key_value *entry = map.entries + key_index;
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
hashmap__insert(struct hashmap *map, struct string_view key, void *value)
{
   if (map->used == map->capacity) return(ec_base_hashmap__full);
   if (!value) return(ec_base_hashmap__null_value_not_allowed);
   u64 key_index = hashmap__hash(key.length, key.start) % map->capacity;

   error error = hashmap__insert_at(map, key_index, key, value);

   return(error);
}


#endif
