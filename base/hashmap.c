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
   u64 result = 0;
   for (u64 index = 0; index < length; ++index)
   {
      result += key[index];
   }
   return(result);
}

error
hashmap_insert(struct hashmap *map, struct string_view key, void *value)
{

}


#endif
