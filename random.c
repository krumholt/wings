#ifndef RANDOM_C_
#define RANDOM_C_

#include "types.h"

struct xorshiftstar_state
{
    u64 value;
};


struct xorshiftstar_state _xorshiftstar_state = { 0x391838921 };

void
xorshiftstar_seed(u64 seed)
{
    _xorshiftstar_state.value = seed;
}

u64
xorshiftstar(void)
{
    //@TODO: Not thread safe: two threads will start at same value!
    u64 x                     = _xorshiftstar_state.value;
    x                         = x ^ (x >> 12);
    x                         = x ^ (x << 25);
    x                         = x ^ (x >> 27);
    _xorshiftstar_state.value = x;
    return x * 0x2545F4914F6CDD1DULL;
}

s32
random_s32(s32 min, s32 max)
{
    s32 diff = max - min;
    return ((u32)(xorshiftstar() >> 32) % diff + min);
}
u32
random_u32(u32 min, u32 max)
{
    u32 diff = max - min;
    return ((u32)(xorshiftstar() >> 32) % diff + min);
}

s64
random_s64(s64 min, s64 max)
{
    s64 diff = max - min;
    return (xorshiftstar() % diff + min);
}

u64
random_u64(u64 min, u64 max)
{
    u64 diff = max - min;
    return (xorshiftstar() % diff + min);
}

f32
random_f32(f32 min, f32 max)
{
    f32 diff = max - min;
    return ((f32)(random_u64(0, ULLONG_MAX) / (f32)ULLONG_MAX) * diff + min);
}

f64
random_f64(f64 min, f64 max)
{
    f64 diff = max - min;
    return ((f64)(random_u64(0, ULLONG_MAX) / (f64)ULLONG_MAX) * diff + min);
}


#endif
