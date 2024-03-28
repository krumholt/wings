#ifndef WINGS_BASE_RANDOM_H_
#define WINGS_BASE_RANDOM_H_

#include "macros.h"
#include "types.h"

#include <limits.h>

struct wings_base_random_xorshiftstar_state
{
   u64 value;
};

void
xorshiftstar_seed(u64 seed);

u64
xorshiftstar(void);

s32
random_s32(s32 min, s32 max);

u32
random_u32(u32 min, u32 max);

s64
random_s64(s64 min, s64 max);

u64
random_u64(u64 min, u64 max);

f32
random_f32(f32 min, f32 max);

f64
random_f64(f64 min, f64 max);

#endif
