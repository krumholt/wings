#ifndef WINGS_BASE_DYNAMIC_ARRAYS_H_
#define WINGS_BASE_DYNAMIC_ARRAYS_H_

#include "wings/base/types.h"
#include "wings/base/macros.c"

#include <malloc.h>
#include <malloc.h>
#include <string.h>

struct s32_array
{
   s32  length;
   s32  capacity;
   s32 *array;
};

struct s32_array
s32_array_make(s32 capacity)
{
   struct s32_array array = {0};
   array.capacity = capacity;
   array.length = 0;
   array.array = calloc(capacity, sizeof(s32));
   ASSERT(array.array != 0);
   return(array);
}

s32
s32_array_append(struct s32_array *array, s32 value)
{
   if (array->length == array->capacity)
   {
      s32 *new_array = 0;
      s32 new_capacity = array->capacity * 2;
      new_array = calloc(array->capacity, sizeof(s32));

      ASSERT(new_array != 0);
      memcpy(new_array, array->array, array->length * sizeof(s32));
      array->capacity = new_capacity;
      array->array = new_array;
   }
   s32 index = array->length;
   array->length += 1;
   array->array[index] = value;
   return (index);
}

void
s32_array_ordered_delete(struct s32_array *array, s32 index)
{
   ASSERT(index < array->length);
   memmove(
         array->array + index,
         array->array + index + 1,
         (array->length - index - 1) * sizeof(s32));
   array->length -= 1;
}

void
s32_array_unordered_delete(struct s32_array *array, s32 index)
{
   ASSERT(index < array->length);
   array->array[index] = array->array[array->length - 1];
   array->length -= 1;
}

#endif
