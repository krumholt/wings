#ifndef WINGS_BASE_DYNAMIC_ARRAYS_C_
#define WINGS_BASE_DYNAMIC_ARRAYS_C_

#include "types.h"
#include "macros.h"

#include "dynamic_arrays.h"

#include <malloc.h>

void *
_array_make(u64 capacity, u64 element_size)
{
   struct _array_header *result = calloc(1, capacity * element_size + sizeof(struct _array_header));
   result->capacity = capacity;
   return(((u8 *)result) + sizeof(struct _array_header));
}

#define array_make(Type, Capacity)\
   (Type *)_array_make(Capacity, sizeof(Type))

#define array_free(Array)\
   free(array_header(Array))
      


void *
array_grow(void *array, u64 element_size, u64 min_growth)
{
   u64 capacity = array_capacity(array);
   u64 new_capacity = capacity + min_growth;
   if (new_capacity < 2 * capacity)
      new_capacity = 2 * capacity;
   else if (new_capacity < 8)
      new_capacity = 8;

   void *pointer = array_header(array);
   array = realloc(pointer, new_capacity * element_size + sizeof(struct _array_header));
   ASSERT(array); // there is not much we can do if we don't get memory
   array += sizeof(struct _array_header);
   array_header(array)->capacity = new_capacity;
   array_header(array)->number_of_resizes += 1;
   return array;
}

#define array_delete_unordered(Array, Index)\
   do {\
      u64 index = (Index);\
      ASSERT((index) < array_length(Array));\
      struct _array_header *header = array_header(Array);\
      Array[(index)] = Array[header->length-1];\
      header->length -= 1;\
   } while(0)

#define array_delete_ordered(Array, Index)\
   do {\
      u64 index = (Index);\
      ASSERT((index) < array_length(Array));\
      memmove((Array) + (index), (Array) + index + 1, (array_length(Array) - index - 1) * sizeof(*Array));\
      array_header(Array)->length -= 1;\
   } while(0)

#endif
