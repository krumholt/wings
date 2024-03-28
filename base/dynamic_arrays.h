#ifndef WINGS_BASE_DYNAMIC_ARRAYS_H_
#define WINGS_BASE_DYNAMIC_ARRAYS_H_

#include "base/types.h"

struct _array_header
{
   u64  length;
   u64  capacity;
   u64  number_of_resizes;
};

#define array_header(Array) ((struct _array_header *) (Array) - 1)
#define array_length(Array) (array_header(Array)->length)

#define array_maybe_grow(Array, N) \
   do {\
      if (array_header(Array)->length + (N) > array_header(Array)->capacity) \
      { \
         Array = array_grow(Array, sizeof(*Array), N); \
      } \
   }\
   while (0)

#define array_capacity(Array) array_header(Array)->capacity

#define array_append(Array, Value) \
   do {\
      array_maybe_grow(Array, 1);\
      (Array)[array_header(Array)->length++] = (Value);\
   } while(0)

#define array_make(Type, Capacity)\
   (Type *)_array_make(Capacity, sizeof(Type))

#define array_free(Array)\
   free(array_header(Array))
      
void *
array_grow(void *array, u64 element_size, u64 min_growth);

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
