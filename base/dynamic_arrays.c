#ifndef WINGS_BASE_DYNAMIC_ARRAYS_C_
#define WINGS_BASE_DYNAMIC_ARRAYS_C_

#include "wings/base/types.h"
#include "wings/base/macros.c"

#include "wings/base/dynamic_arrays.h"

#include <malloc.h>
#include <string.h>

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
      array->capacity = array->capacity * 2;
      new_array = calloc(array->capacity, sizeof(s32));

      ASSERT(new_array != 0);
      memcpy(new_array, array->array, array->length * sizeof(s32));
      free(array->array);
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


struct tuple_s32_array
tuple_s32_array_make(s32 capacity)
{
   struct tuple_s32_array array = {0};
   array.capacity = capacity;
   array.length = 0;
   array.array = calloc(capacity, sizeof(struct tuple_s32));
   ASSERT(array.array != 0);
   return(array);
}

s32
tuple_s32_array_append(struct tuple_s32_array *array, struct tuple_s32 value)
{
   if (array->length == array->capacity)
   {
      struct tuple_s32 *new_array = 0;
      array->capacity = array->capacity * 2;
      new_array = calloc(array->capacity, sizeof(struct tuple_s32));

      ASSERT(new_array != 0);
      memcpy(new_array, array->array, array->length * sizeof(struct tuple_s32));
      free(array->array);
      array->array = new_array;
   }
   s32 index = array->length;
   array->length += 1;
   array->array[index] = value;
   return (index);
}

void
tuple_s32_array_ordered_delete(struct tuple_s32_array *array, s32 index)
{
   ASSERT(index < array->length);
   memmove(
         array->array + index,
         array->array + index + 1,
         (array->length - index - 1) * sizeof(struct tuple_s32));
   array->length -= 1;
}

void
tuple_s32_array_unordered_delete(struct tuple_s32_array *array, s32 index)
{
   ASSERT(index < array->length);
   array->array[index] = array->array[array->length - 1];
   array->length -= 1;
}
#endif
