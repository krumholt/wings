#ifndef WINGS_BASE_DYNAMIC_ARRAYS_H_
#define WINGS_BASE_DYNAMIC_ARRAYS_H_

#include "wings/base/types.h"

struct s32_array
{
   s32  length;
   s32  capacity;
   s32 *array;
};

struct s32_array
s32_array_make(s32 capacity);

s32
s32_array_append(struct s32_array *array, s32 value);


void
s32_array_ordered_delete(struct s32_array *array, s32 index);


void
s32_array_unordered_delete(struct s32_array *array, s32 index);

#endif
