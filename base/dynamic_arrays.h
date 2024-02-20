#ifndef WINGS_BASE_DYNAMIC_ARRAYS_H_
#define WINGS_BASE_DYNAMIC_ARRAYS_H_

#include "wings/base/types.h"

struct s32_array
{
   s32  length;
   s32  capacity;
   s32 *array;
};

struct i64_array
{
   i64  length;
   i64  capacity;
   i64 *array;
};


struct tuple_s32_array
{
   s32  length;
   s32  capacity;
   struct tuple_s32 *array;
};

struct i64_array
i64_array_make(s32 capacity);

i64
i64_array_append(struct i64_array *array, i64 value);

struct s32_array
s32_array_make(s32 capacity);

s32
s32_array_append(struct s32_array *array, s32 value);


void
s32_array_ordered_delete(struct s32_array *array, s32 index);


void
s32_array_unordered_delete(struct s32_array *array, s32 index);

struct tuple_s32_array
tuple_s32_array_make(s32 capacity);

s32
tuple_s32_array_append(struct tuple_s32_array *array, struct tuple_s32 value);


void
tuple_s32_array_ordered_delete(struct tuple_s32_array *array, s32 index);


void
tuple_s32_array_unordered_delete(struct tuple_s32_array *array, s32 index);
#endif
