#ifndef WINGS_MATH_C
#define WINGS_MATH_C

#include "wings/base/random.c"
#include "wings/base/types.c"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#ifndef PI
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f
#endif

struct v2
{
    f32 x, y;
};
struct v2s
{
    s32 x, y;
};
struct v2u
{
    u32 x, y;
};
struct v3
{
    f32 x, y, z;
};
struct v3s
{
    s32 x, y, z;
};
struct v3u
{
    u32 x, y, z;
};
struct v4
{
    f32 x, y, z, w;
};
struct v4d
{
    f64 x, y, z, w;
};
struct v4s
{
    s32 x, y, z, w;
};
struct v4u
{
    u32 x, y, z, w;
};
struct quaternion
{
    f32 w, x, y, z;
};

struct ray_v2
{
    struct v2 origin, direction;
};
struct ray_v3
{
    struct v3 origin, direction;
};

struct ray_grid_march
{
    s32 x;
    s32 y;
    s32 z;
    s32 step_x;
    s32 step_y;
    s32 step_z;

    f32 t_delta_x;
    f32 t_delta_y;
    f32 t_delta_z;

    f32 t_max_x;
    f32 t_max_y;
    f32 t_max_z;

    s32 side_of_entry;

    struct v3 origin;
    struct v3 point;
    struct v3 direction;
};

struct ray_grid_march2
{
    struct v2s tile;
    struct v2s step;
    struct v2  t_delta;
    struct v2  t_max;
    struct v2  origin;
    struct v2  point;
    struct v2  direction;
    f32        t;
};

/*
  0  4   8  12
  1  5   9  13
  2  6  10  14
  3  7  11  15
  ==================
  m00  m10  m20  m30
  m01  m11  m21  m31
  m02  m12  m22  m32
  m03  m13  m23  m33
 */

struct mat2
{
    f32 m00, m01, m10, m11;
};
struct mat2s
{
    s32 m00, m01, m10, m11;
};
struct mat2u
{
    u32 m00, m01, m10, m11;
};
struct mat3
{
    f32 m00, m01, m02, m10, m11, m12, m20, m21, m22;
};
struct mat3s
{
    s32 m00, m01, m02, m10, m11, m12, m20, m21, m22;
};
struct mat3u
{
    u32 m00, m01, m02, m10, m11, m12, m20, m21, m22;
};
struct mat4
{
    f32 m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
};
struct mat4d
{
    f64 m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
};
struct mat4s
{
    s32 m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
};
struct mat4u
{
    u32 m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
};

struct aab3
{
    struct v3 min;
    struct v3 max;
};

// ===================================
// static variables
// ===================================
static struct mat2 mat2_identity = {
    1.0f,
    0.0f,
    0.0f,
    1.0f,
};
static struct mat4 mat4_identity = {
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
};

// ===================================
// make functions
// ===================================

struct v2
make_v2(f32 x, f32 y)
{
    struct v2 v = { x, y };
    return (v);
}

struct v2s
make_v2s(s32 x, s32 y)
{
    struct v2s v = { x, y };
    return (v);
}

struct v2u
make_v2u(u32 x, u32 y)
{
    struct v2u v = { x, y };
    return (v);
}

struct v3
make_v3(f32 x, f32 y, f32 z)
{
    struct v3 v = { x, y, z };
    return (v);
}

struct v3s
make_v3s(s32 x, s32 y, s32 z)
{
    struct v3s v = { x, y, z };
    return (v);
}

struct v4
make_v4(f32 x, f32 y, f32 z, f32 w)
{
    struct v4 v = { x, y, z, w };
    return (v);
}

struct f32_tuple
{
    f32 a;
    f32 b;
};

struct f32_tuple
calculate_mean_variance_s32(s32 *array, s32 size)
{

    f32 mean = 0, M2 = 0, variance = 0;

    for (s32 index = 0; index < size; ++index)
    {
        f32 delta = array[index] - mean;
        mean += delta / (f32)(index + 1);
        M2 += delta * (array[index] - mean);
        variance = M2 / (f32)(index + 1);
    }

    return (struct f32_tuple) { .a = mean, .b = variance };
}

struct f32_tuple
calculate_mean_variance_f32(f32 *array, s32 size)
{

    f32 mean = 0, m2 = 0, variance = 0;

    for (s32 index = 0; index < size; ++index)
    {
        f32 delta = array[index] - mean;
        mean += delta / (f32)(index + 1);
        m2 += delta * (array[index] - mean);
        variance = m2 / (f32)(index + 1);
    }

    return (struct f32_tuple) { .a = mean, .b = variance };
}

struct v2
random_v2(struct v2 min, struct v2 max)
{
    struct v2 result = { random_f32(min.x, max.x), random_f32(min.y, max.y) };
    return (result);
}

struct v2s
random_v2s(struct v2s min, struct v2s max)
{
    struct v2s result = { random_s32(min.x, max.x), random_s32(min.y, max.y) };
    return (result);
}

struct v2u
random_v2u(struct v2u min, struct v2u max)
{
    struct v2u result = { random_u32(min.x, max.x), random_u32(min.y, max.y) };
    return (result);
}

struct v3
random_v3(struct v3 min, struct v3 max)
{
    struct v3 v = {
        random_f32(min.x, max.x),
        random_f32(min.y, max.y),
        random_f32(min.z, max.z)
    };
    return (v);
}

struct v3s
random_v3s(struct v3s min, struct v3s max)
{
    struct v3s v = {
        random_s32(min.x, max.x),
        random_s32(min.y, max.y),
        random_s32(min.z, max.z)
    };
    return (v);
}

struct v3u
random_v3u(struct v3u min, struct v3u max)
{
    struct v3u v = {
        random_u32(min.x, max.x),
        random_u32(min.y, max.y),
        random_u32(min.z, max.z)
    };
    return (v);
}

struct v4
random_v4(struct v4 min, struct v4 max)
{
    struct v4 v = {
        random_f32(min.x, max.x),
        random_f32(min.y, max.y),
        random_f32(min.z, max.z),
        random_f32(min.z, max.w)
    };
    return (v);
}

struct v4s
random_v4s(struct v4s min, struct v4s max)
{
    struct v4s v = {
        random_s32(min.x, max.x),
        random_s32(min.y, max.y),
        random_s32(min.z, max.z),
        random_s32(min.z, max.w)
    };
    return (v);
}

struct v4u
random_v4u(struct v4u min, struct v4u max)
{
    struct v4u v = {
        random_u32(min.x, max.x),
        random_u32(min.y, max.y),
        random_u32(min.z, max.z),
        random_u32(min.z, max.w)
    };
    return (v);
}

// ======================================
// vector to vector conversion functions
// ======================================

struct v2
v2s_to_v2(struct v2s v)
{
    struct v2 result = { (f32)v.x, (f32)v.y };
    return (result);
}

struct v2s
v2_to_v2s(struct v2 v)
{
    struct v2s result;
    result.x = (s32)floorf(v.x);
    result.y = (s32)floorf(v.y);
    return result;
}

struct v2u
v2s_to_v2u(struct v2s v)
{
    struct v2u result = { (u32)v.x, (u32)v.y };
    return (result);
}

struct v2s
v2u_to_v2s(struct v2u v)
{
    struct v2s result;
    result.x = (s32)floorf(v.x);
    result.y = (s32)floorf(v.y);
    return result;
}

struct v2
v2u_to_v2(struct v2u v)
{
    struct v2 result;
    result.x = (f32)v.x;
    result.y = (f32)v.y;
    return result;
}

struct v3s
v3_to_v3s(struct v3 v)
{
    struct v3s result = {
        (s32)floorf(v.x),
        (s32)floorf(v.y),
        (s32)floorf(v.z)
    };
    return (result);
}

struct v3
v3s_to_v3(struct v3s v)
{
    struct v3 result = { (f32)v.x, (f32)v.y, (f32)v.z };
    return (result);
}

b32
equals_v2(struct v2 a, struct v2 b)
{
    return (a.x == b.x && a.y == b.y);
}
b32
equals_v2s(struct v2s a, struct v2s b)
{
    return (a.x == b.x && a.y == b.y);
}
b32
equals_v2u(struct v2u a, struct v2u b)
{
    return (a.x == b.x && a.y == b.y);
}

b32
equals_v3(struct v3 a, struct v3 b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
b32
equals_v3s(struct v3s a, struct v3s b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}
b32
equals_v3u(struct v3u a, struct v3u b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

b32
equals_v4(struct v4 a, struct v4 b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}
b32
equals_v4s(struct v4s a, struct v4s b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}
b32
equals_v4u(struct v4u a, struct v4u b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

struct v2
add_v2(struct v2 a, struct v2 b)
{
    return (struct v2) { a.x + b.x, a.y + b.y };
}
struct v2s
add_v2s(struct v2s a, struct v2s b)
{
    return (struct v2s) { a.x + b.x, a.y + b.y };
}
struct v2u
add_v2u(struct v2u a, struct v2u b)
{
    return (struct v2u) { a.x + b.x, a.y + b.y };
}

struct v3
add_v3(struct v3 a, struct v3 b)
{
    return (struct v3) { a.x + b.x, a.y + b.y, a.z + b.z };
}
struct v3s
add_v3s(struct v3s a, struct v3s b)
{
    return (struct v3s) { a.x + b.x, a.y + b.y, a.z + b.z };
}
struct v3u
add_v3u(struct v3u a, struct v3u b)
{
    return (struct v3u) { a.x + b.x, a.y + b.y, a.z + b.z };
}

struct v4
add_v4(struct v4 a, struct v4 b)
{
    return (struct v4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
struct v4s
add_v4s(struct v4s a, struct v4s b)
{
    return (struct v4s) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
struct v4u
add_v4u(struct v4u a, struct v4u b)
{
    return (struct v4u) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

struct v2
sub_v2(struct v2 a, struct v2 b)
{
    return (struct v2) { a.x - b.x, a.y - b.y };
}
struct v2s
sub_v2s(struct v2s a, struct v2s b)
{
    return (struct v2s) { a.x - b.x, a.y - b.y };
}
struct v2s
sub_v2u(struct v2u a, struct v2u b)
{
    return (struct v2s) { a.x - b.x, a.y - b.y };
}

struct v3
sub_v3(struct v3 a, struct v3 b)
{
    return (struct v3) { a.x - b.x, a.y - b.y, a.z - b.z };
}
struct v3s
sub_v3s(struct v3s a, struct v3s b)
{
    return (struct v3s) { a.x - b.x, a.y - b.y, a.z - b.z };
}
struct v3s
sub_v3s_s32(struct v3s a, s32 b)
{
    return (struct v3s) { a.x - b, a.y - b, a.z - b };
}
struct v3u
sub_v3u(struct v3u a, struct v3u b)
{
    return (struct v3u) { a.x - b.x, a.y - b.y, a.z - b.z };
}

struct v4
sub_v4(struct v4 a, struct v4 b)
{
    return (struct v4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}
struct v4s
sub_v4s(struct v4s a, struct v4s b)
{
    return (struct v4s) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}
struct v4u
sub_v4u(struct v4u a, struct v4u b)
{
    return (struct v4u) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

struct v2
mul_v2_f32(struct v2 a, f32 b)
{
    return (struct v2) { a.x * b, a.y * b };
}
struct v2s
mul_v2s_f32(struct v2s a, s32 b)
{
    return (struct v2s) { a.x * b, a.y * b };
}
struct v2u
mul_v2u_f32(struct v2u a, u32 b)
{
    return (struct v2u) { a.x * b, a.y * b };
}

struct v2
mul_f32_v2(f32 a, struct v2 b)
{
    return (struct v2) { a * b.x, a * b.y };
}

struct v3
mul_v3_f32(struct v3 a, f32 b)
{
    return (struct v3) { a.x * b, a.y * b, a.z * b };
}
struct v3s
mul_v3s_f32(struct v3s a, s32 b)
{
    return (struct v3s) { a.x * b, a.y * b, a.z * b };
}
struct v3u
mul_v3u_f32(struct v3u a, u32 b)
{
    return (struct v3u) { a.x * b, a.y * b, a.z * b };
}

struct v3
mod_v3_f32(struct v3 a, f32 value)
{
    struct v3 result = {
        fmodf(a.x, value),
        fmodf(a.y, value),
        fmodf(a.z, value),
    };
    return (result);
}

struct v3s
mod_v3s(struct v3s a, struct v3s b)
{
    struct v3s result = {
        a.x % b.x,
        a.y % b.y,
        a.z % b.z,
    };
    return (result);
}

struct v3s
mul_v3s(struct v3s a, struct v3s b)
{
    return (struct v3s) { a.x * b.x, a.y * b.y, a.z * b.z };
}

struct v3
mul_v3(struct v3 a, struct v3 b)
{
    return (struct v3) { a.x * b.x, a.y * b.y, a.z * b.z };
}
struct v3
mul_f32_v3(f32 a, struct v3 b)
{
    return (struct v3) { a * b.x, a * b.y, a * b.z };
}
struct v3s
mul_f32_v3s(f32 a, struct v3s b)
{
    return (struct v3s) { a * b.x, a * b.y, a * b.z };
}
struct v3u
mul_f32_v3u(f32 a, struct v3u b)
{
    return (struct v3u) { a * b.x, a * b.y, a * b.z };
}

struct v3
add_f32_v3(f32 a, struct v3 b)
{
    return (struct v3) { a + b.x, a + b.y, a + b.z };
}
struct v3s
add_s32_v3s(s32 a, struct v3s b)
{
    return (struct v3s) { a + b.x, a + b.y, a + b.z };
}
struct v3u
add_f32_v3u(f32 a, struct v3u b)
{
    return (struct v3u) { a + b.x, a + b.y, a + b.z };
}

struct v4
mul_v4_f32(struct v4 a, f32 b)
{
    return (struct v4) { a.x * b, a.y * b, a.z * b, a.w * b };
}
struct v4s
mul_v4s_f32(struct v4s a, s32 b)
{
    return (struct v4s) { a.x * b, a.y * b, a.z * b, a.w * b };
}
struct v4u
mul_v4u_f32(struct v4u a, u32 b)
{
    return (struct v4u) { a.x * b, a.y * b, a.z * b, a.w * b };
}

struct v4
mul_f32_v4(f32 a, struct v4 b)
{
    return (struct v4) { a * b.x, a * b.y, a * b.z, a * b.w };
}
struct v4s
mul_f32_v4s(f32 a, struct v4s b)
{
    return (struct v4s) { a * b.x, a * b.y, a * b.z, a * b.w };
}
struct v4u
mul_f32_v4u(f32 a, struct v4u b)
{
    return (struct v4u) { a * b.x, a * b.y, a * b.z, a * b.w };
}

struct mat2
add_mat2(struct mat2 a, struct mat2 b)
{
    struct mat2 result = {
        a.m00 + b.m00,
        a.m01 + b.m01,
        a.m10 + b.m10,
        a.m11 + b.m11,
    };
    return (result);
}

struct quaternion
mul_quaternion_f32(struct quaternion a, f32 b)
{
    return (struct quaternion) { a.w * b, a.x * b, a.y * b, a.z * b };
}

struct quaternion
add_quaternion(struct quaternion a, struct quaternion b)
{
    return (struct quaternion) { a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z };
}

struct v2
div_v2_f32(struct v2 a, f32 b)
{
    f32 inv_b = 1.0f / b;
    return (struct v2) { a.x * inv_b, a.y * inv_b };
}

struct v2s
div_v2s_f32(struct v2s a, f32 b)
{
    f32 inv_b = 1.0f / b;
    return (struct v2s) { a.x * inv_b, a.y * inv_b };
}

struct v2u
div_v2u_f32(struct v2u a, f32 b)
{
    f32 inv_b = 1.0f / b;
    return (struct v2u) { a.x * inv_b, a.y * inv_b };
}

struct v3s
div_v3s_s32(struct v3s a, s32 b)
{
    return (struct v3s) { a.x / b, a.y / b, a.z / b };
}

struct v3
div_v3_f32(struct v3 a, f32 b)
{
    f32 inv_b = 1.0f / b;
    return (struct v3) { a.x * inv_b, a.y * inv_b, a.z * inv_b };
}

f32
length_v2(struct v2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}
f32
length_v2s(struct v2s v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}
f32
length_v2u(struct v2u v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}
f32
length_v3(struct v3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_v3s(struct v3s v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_v3u(struct v3u v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_v4(struct v4 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
f32
length_v4s(struct v4s v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
f32
length_v4u(struct v4u v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
f32
length_squared_v2(struct v2 v)
{
    return (v.x * v.x + v.y * v.y);
}
f32
length_squared_v2s(struct v2s v)
{
    return (v.x * v.x + v.y * v.y);
}
f32
length_squared_v2u(struct v2u v)
{
    return (v.x * v.x + v.y * v.y);
}
f32
length_squared_v3(struct v3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_squared_v3s(struct v3s v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_squared_v3u(struct v3u v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}
f32
length_squared_v4(struct v4 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
f32
length_squared_v4s(struct v4s v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
f32
length_squared_v4u(struct v4u v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

f32
length_quaternion(struct quaternion q)
{
    return sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

b32
almost_zero_f32(f32 value, f32 epsilon)
{
    return value * value < epsilon;
}

b32
almost_zero_v2(struct v2 vector, f32 epsilon)
{
    return (length_squared_v2(vector) < epsilon);
}

b32
almost_zero_v3(struct v3 vector, f32 epsilon)
{
    return (length_squared_v3(vector) < epsilon);
}

struct v2
normalize_v2(struct v2 v)
{
    f32 length     = length_v2(v);
    f32 inv_length = 1.0f / length;
    return (struct v2) { v.x * inv_length, v.y * inv_length };
}

struct v3
normalize_v3(struct v3 v)
{
    f32       length     = length_v3(v);
    f32       inv_length = 1.0f / length;
    struct v3 result     = { v.x * inv_length, v.y * inv_length, v.z * inv_length };
    return (result);
}

struct v4
normalize_v4(struct v4 v)
{
    f32       length     = length_v4(v);
    f32       inv_length = 1.0f / length;
    struct v4 result     = { v.x * inv_length, v.y * inv_length, v.z * inv_length, v.w * inv_length };
    return (result);
}

struct quaternion
normalize_quaternion(struct quaternion v)
{
    f32               length     = length_quaternion(v);
    f32               inv_length = 1.0f / length;
    struct quaternion result     = { v.w * inv_length, v.x * inv_length, v.y * inv_length, v.z * inv_length };
    return (result);
}

struct v2
normalize_v2_with_default(struct v2 v, struct v2 or_else)
{
    f32       length     = length_v2(v);
    f32       inv_length = 1.0f / length;
    struct v2 result     = { 0 };
    if (almost_zero_f32(length, 0.00001))
        result = or_else;
    else
        result = (struct v2) { v.x * inv_length, v.y * inv_length };
    return (result);
}

struct v3
normalize_v3_with_default(struct v3 v, struct v3 or_else)
{
    f32       length     = length_v3(v);
    f32       inv_length = 1.0f / length;
    struct v3 result     = { 0 };
    if (almost_zero_f32(length, 0.00001))
        result = or_else;
    else
        result = (struct v3) { v.x * inv_length, v.y * inv_length, v.z * inv_length };
    return (result);
}

struct v4
normalize_v4_with_default(struct v4 v, struct v4 or_else)
{
    f32       length     = length_v4(v);
    f32       inv_length = 1.0f / length;
    struct v4 result     = { 0 };
    if (almost_zero_f32(length, 0.00001))
        result = or_else;
    else
        result = (struct v4) { v.x * inv_length, v.y * inv_length, v.z * inv_length, v.w * inv_length };
    return (result);
}

f32
cross_v2(struct v2 a, struct v2 b)
{
    return (a.x * b.y - a.y * b.x);
}

struct v3
cross_v3(struct v3 a, struct v3 b)
{
    struct v3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

struct v3s
cross_v3s(struct v3s a, struct v3s b)
{
    struct v3s result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

struct v3u
cross_v3u(struct v3u a, struct v3u b)
{
    struct v3u result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

f32
dot_v2(struct v2 a, struct v2 b)
{
    f32 value = a.x * b.x + a.y * b.y;
    return value;
}

f32
dot_v2s(struct v2s a, struct v2s b)
{
    f32 value = a.x * b.x + a.y * b.y;
    return value;
}

f32
dot_v2u(struct v2u a, struct v2u b)
{
    f32 value = a.x * b.x + a.y * b.y;
    return value;
}

f32
dot_v3(struct v3 a, struct v3 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return (result);
}

f32
dot_v3s(struct v3s a, struct v3s b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return (result);
}

f32
dot_v3u(struct v3u a, struct v3u b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return (result);
}

f32
dot_v4(struct v4 a, struct v4 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

f32
dot_quaternion(struct quaternion a, struct quaternion b)
{
    f32 result = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

struct mat4
mat4_diagonal(f32 value)
{
    struct mat4 result = {
        value, 0.0f, 0.0f, 0.0f,
        0.0f, value, 0.0f, 0.0f,
        0.0f, 0.0f, value, 0.0f,
        0.0f, 0.0f, 0.0f, value
    };
    return (result);
}

struct mat4
mul_mat4(struct mat4 a, struct mat4 b)
{
    struct mat4 m = { 0 };

    m.m00 = a.m00 * b.m00 + a.m10 * b.m01 + a.m20 * b.m02 + a.m30 * b.m03;
    m.m10 = a.m00 * b.m10 + a.m10 * b.m11 + a.m20 * b.m12 + a.m30 * b.m13;
    m.m20 = a.m00 * b.m20 + a.m10 * b.m21 + a.m20 * b.m22 + a.m30 * b.m23;
    m.m30 = a.m00 * b.m30 + a.m10 * b.m31 + a.m20 * b.m32 + a.m30 * b.m33;

    m.m01 = a.m01 * b.m00 + a.m11 * b.m01 + a.m21 * b.m02 + a.m31 * b.m03;
    m.m11 = a.m01 * b.m10 + a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13;
    m.m21 = a.m01 * b.m20 + a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23;
    m.m31 = a.m01 * b.m30 + a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33;

    m.m02 = a.m02 * b.m00 + a.m12 * b.m01 + a.m22 * b.m02 + a.m32 * b.m03;
    m.m12 = a.m02 * b.m10 + a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13;
    m.m22 = a.m02 * b.m20 + a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23;
    m.m32 = a.m02 * b.m30 + a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33;

    m.m03 = a.m03 * b.m00 + a.m13 * b.m01 + a.m23 * b.m02 + a.m33 * b.m03;
    m.m13 = a.m03 * b.m10 + a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13;
    m.m23 = a.m03 * b.m20 + a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23;
    m.m33 = a.m03 * b.m30 + a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33;

    return (m);
}

struct mat4
_make_look_at_matrix(f32 origin_x, f32 origin_y, f32 origin_z,
                     f32 target_x, f32 target_y, f32 target_z)
{
    f32 forward_x = target_x - origin_x;
    f32 forward_y = target_y - origin_y;
    f32 forward_z = target_z - origin_z;
    origin_x      = -origin_x;
    origin_y      = -origin_y;
    origin_z      = -origin_z;

    f32 forward_length = sqrtf(forward_x * forward_x + forward_y * forward_y + forward_z * forward_z);
    forward_x          = forward_x / forward_length;
    forward_y          = forward_y / forward_length;
    forward_z          = forward_z / forward_length;

    f32 up_x = 0.0f;
    f32 up_y = 0.0f;
    f32 up_z = 1.0f;
    if (forward_x == 0.0f && forward_y == 0.0f)
    {
        up_y = -forward_z;
        up_z = 0.0f;
    }

    // 0, 0, -1
    f32 right_x = forward_y * up_z - forward_z * up_y;
    f32 right_y = forward_z * up_x - forward_x * up_z;
    f32 right_z = forward_x * up_y - forward_y * up_x;

    f32 right_length = sqrtf(right_x * right_x + right_y * right_y + right_z * right_z);
    right_x          = right_x / right_length;
    right_y          = right_y / right_length;
    right_z          = right_z / right_length;

    up_x = right_y * forward_z - right_z * forward_y;
    up_y = right_z * forward_x - right_x * forward_z;
    up_z = right_x * forward_y - right_y * forward_x;

    struct mat4 result;
    result.m00 = right_x;
    result.m01 = forward_x;
    result.m02 = up_x;
    result.m03 = 0;

    result.m10 = right_y;
    result.m11 = forward_y;
    result.m12 = up_y;
    result.m13 = 0;

    result.m20 = right_z;
    result.m21 = forward_z;
    result.m22 = up_z;
    result.m23 = 0;

    result.m30 = origin_x * right_x + origin_y * right_y + origin_z * right_z;
    result.m31 = origin_x * forward_x + origin_y * forward_y + origin_z * forward_z;
    result.m32 = origin_x * up_x + origin_y * up_y + origin_z * up_z;
    result.m33 = 1;

    return (result);
}

struct mat4
make_look_at_matrix(struct v3 position,
                    struct v3 target,
                    struct v3 up)
{

    struct v3 forward = normalize_v3(sub_v3(target, position));
    struct v3 right   = normalize_v3(cross_v3(forward, up));
    up                = normalize_v3(cross_v3(right, forward));
    position          = mul_f32_v3(-1.0f, position);

    struct mat4 result = { 0 };
    result.m00         = right.x;
    result.m10         = right.y;
    result.m20         = right.z;
    result.m30         = position.x * right.x + position.y * right.y + position.z * right.z;

    result.m01 = up.x;
    result.m11 = up.y;
    result.m21 = up.z;
    result.m31 = position.x * up.x + position.y * up.y + position.z * up.z;

    result.m02 = forward.x;
    result.m12 = forward.y;
    result.m22 = forward.z;
    result.m32 = position.x * forward.x + position.y * forward.y + position.z * forward.z;

    result.m03 = 0;
    result.m13 = 0;
    result.m23 = 0;
    result.m33 = 1;

    return (result);
}

struct mat4
make_look_at_matrix_RH(struct v3 position, struct v3 target, struct v3 up)
{

    struct v3 forward = normalize_v3(sub_v3(target, position));
    struct v3 right   = normalize_v3(cross_v3(forward, up));
    up                = normalize_v3(cross_v3(right, forward));
    position          = mul_f32_v3(-1.0f, position);

    struct mat4 result = { 0 };
    result.m00         = right.x;
    result.m10         = right.y;
    result.m20         = right.z;
    result.m30         = position.x * right.x + position.y * right.y + position.z * right.z;

    result.m01 = up.x;
    result.m11 = up.y;
    result.m21 = up.z;
    result.m31 = position.x * up.x + position.y * up.y + position.z * up.z;

    forward    = mul_f32_v3(-1.0f, forward);
    result.m02 = forward.x;
    result.m12 = forward.y;
    result.m22 = forward.z;
    result.m32 = position.x * forward.x + position.y * forward.y + position.z * forward.z;

    result.m03 = 0;
    result.m13 = 0;
    result.m23 = 0;
    result.m33 = 1;

    return (result);
}

struct mat4
make_rotation_x_axis_mat4(f32 angle)
{
    f32 cos_angle = cosf(angle);
    f32 sin_angle = sinf(angle);

    struct mat4 result = { 0 };
    result.m00         = 1.0f;

    result.m11 = cos_angle;
    result.m12 = sin_angle;

    result.m21 = -sin_angle;
    result.m22 = cos_angle;

    result.m33 = 1.0f;

    return (result);
}

struct mat4
make_rotation_y_axis_mat4(f32 angle)
{
    f32 cos_angle = cosf(angle);
    f32 sin_angle = sinf(angle);

    struct mat4 result = { 0 };
    result.m00         = cos_angle;
    result.m02         = sin_angle;

    result.m11 = 1.0f;

    result.m20 = -sin_angle;
    result.m22 = cos_angle;

    result.m33 = 1.0f;

    return (result);
}

struct mat4
make_rotation_z_axis_mat4(f32 angle)
{
    f32 cos_angle = cosf(angle);
    f32 sin_angle = sinf(angle);

    struct mat4 result = { 0 };
    result.m00         = cos_angle;
    result.m01         = sin_angle;

    result.m10 = -sin_angle;
    result.m11 = cos_angle;

    result.m22 = 1.0f;

    result.m33 = 1.0f;

    return (result);
}

struct mat4
make_scale_mat4(struct v3 v)
{
    struct mat4 result = { 0 };
    result.m00         = v.x;
    result.m11         = v.y;
    result.m22         = v.z;
    result.m33         = 1.0f;
    return (result);
}

struct mat4
_make_translation_mat4(f32 x, f32 y, f32 z)
{
    struct mat4 result = { 0 };
    result.m00         = 1.0f;
    result.m11         = 1.0f;
    result.m22         = 1.0f;
    result.m30         = x;
    result.m31         = y;
    result.m32         = z;
    result.m33         = 1.0f;
    return (result);
}

struct mat4
make_translation_matrix(struct v3 v)
{
    struct mat4 result = { 0 };
    result.m00         = 1.0f;
    result.m11         = 1.0f;
    result.m22         = 1.0f;
    result.m30         = v.x;
    result.m31         = v.y;
    result.m32         = v.z;
    result.m33         = 1.0f;
    return (result);
}

struct mat4
make_perspective_projection(f32 aspect_ratio, f32 near_plane, f32 far_plane, f32 horizontal_fov)
{
    struct mat4 result = { 0 };

    result.m00 = 1.0f / tanf(0.5f * horizontal_fov);

    result.m11 = 1.0f / tanf(0.5f * horizontal_fov) / aspect_ratio;

    result.m22 = far_plane / (far_plane - near_plane);
    result.m32 = -(far_plane * near_plane) / (far_plane - near_plane);
    result.m23 = 1.0f;

    return (result);
}

/*
m00 m10 m20 m30
m01 m11 m21 m31
m02 m12 m22 m32
m03 m13 m23 m33
*/

struct mat4
make_orthographic_projection(f32 width, f32 height, f32 near_plane, f32 far_plane)
{
    struct mat4 result = { 0 };
    result.m00         = 2.0f / width;
    result.m11         = 2.0f / height;
    result.m22         = 2.0f / (far_plane - near_plane);

    result.m32 = -1.0f * (far_plane + near_plane) / (far_plane - near_plane);
    result.m33 = 1.0f;
    return (result);
}

b32
_invert_mat4(struct mat4 in, struct mat4 *out)
{
    f32 *m      = (f32 *)&in;
    f32 *invOut = (f32 *)out;
    f32  inv[16], det;
    s32  i;

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return 1;

    det = 1.0f / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return 0;
}

struct mat4d
mat4_to_mat4d(struct mat4 matrix)
{
    struct mat4d result = { 0 };
    f32         *in     = (f32 *)&matrix;
    f32         *out    = (f32 *)&result;
    for (int index = 0;
         index < 16;
         ++index)
    {
        out[index] = in[index];
    }
    return (result);
}

struct mat4
invert_mat4(struct mat4 m)
{
    struct mat4 result = { 0 };
    b32         error  = _invert_mat4(m, &result);
    if (error)
        return (mat4_identity);
    else
        return (result);
}

b32
_invert_mat2(struct mat2 in, struct mat2 *out)
{
    f32 s = in.m00 * in.m11 - in.m10 * in.m01;
    if (s == 0.0f)
        return 1;
    s        = 1.0f / s;
    out->m00 = s * in.m11;
    out->m01 = s * -in.m01;
    out->m10 = s * -in.m10;
    out->m11 = s * in.m00;
    return 0;
}

struct mat2
invert_mat2(struct mat2 m)
{
    struct mat2 result = { 0 };
    b32         error  = _invert_mat2(m, &result);
    if (error)
        return (mat2_identity);
    else
        return (result);
}

struct mat2
transpose_mat2(struct mat2 m)
{
    struct mat2 result = { 0 };
    result.m00         = m.m00;
    result.m01         = m.m10;
    result.m10         = m.m01;
    result.m11         = m.m11;
    return (result);
}

struct mat4
transpose_mat4(struct mat4 m)
{
    struct mat4 result = { 0 };
    result.m00         = m.m00;
    result.m01         = m.m10;
    result.m02         = m.m20;
    result.m03         = m.m30;

    result.m10 = m.m01;
    result.m11 = m.m11;
    result.m12 = m.m21;
    result.m13 = m.m31;

    result.m20 = m.m02;
    result.m21 = m.m12;
    result.m22 = m.m22;
    result.m23 = m.m32;

    result.m30 = m.m03;
    result.m31 = m.m13;
    result.m32 = m.m23;
    result.m33 = m.m33;

    return (result);
}

f32
to_radians(f32 degrees)
{
    return degrees * PI / 180.0f;
}

f32
to_degrees(f32 radians)
{
    return radians * 180.0f / PI;
}

f32
sign_f32(f32 value)
{
    return ((f32)((0.0f < value) - (value < 0.0f)));
}

struct v3
sign_v3(struct v3 value)
{
    return ((struct v3) {
        copysign(1.0f, value.x),
        copysign(1.0f, value.y),
        copysign(1.0f, value.z),
    });
}

struct v3s
is_negative_v3(struct v3 value)
{
    union _re_interpret_i_f
    {
        f32 f;
        u32 i;
    } x = { value.x }, y = { value.y }, z = { value.z };

    return ((struct v3s) {
        x.i >> 31,
        y.i >> 31,
        z.i >> 31,
    });
}

struct v3
trunc_v3(struct v3 value)
{
    return ((struct v3) {
        truncf(value.x),
        truncf(value.y),
        truncf(value.z),
    });
}

f32
fractional_part(f32 value)
{
    return value - truncf(value);
}

f32
inverted_truncf(f32 value)
{
    return (value < 0 ? floorf(value) : ceilf(value));
}

b32
are_equal_f32(f32 a, f32 b, f32 epsilon)
{
    return (fabsf(a - b) <= epsilon ? 1u : 0u);
}

b32
are_equal_f32_eps(f32 a, f32 b)
{
    return (fabsf(a - b) <= 0.0001f ? 1u : 0u);
}

b32
are_equal_v2(struct v2 a, struct v2 b, f32 epsilon)
{
    return (are_equal_f32(a.x, b.x, epsilon)
            && are_equal_f32(a.y, b.y, epsilon));
}

b32
are_equal_v3s(struct v3s a, struct v3s b)
{
    return (a.x == b.x
            && a.y == b.y
            && a.z == b.z);
}

b32
are_equal_v4(struct v4 a, struct v4 b, f32 epsilon)
{
    return (are_equal_f32(a.x, b.x, epsilon)
            && are_equal_f32(a.y, b.y, epsilon)
            && are_equal_f32(a.z, b.z, epsilon)
            && are_equal_f32(a.w, b.w, epsilon));
}

b32
are_equal_mat4(struct mat4 m1, struct mat4 m2)
{
    b32  result       = 1;
    f32 *m1_as_floats = &m1.m00;
    f32 *m2_as_floats = &m2.m00;
    for (int index = 0; index < 16; ++index)
    {
        result &= are_equal_f32(m1_as_floats[index], m2_as_floats[index], 0.035f);
    }
    return result;
}

f32
clamp_f32(f32 value, f32 from, f32 to)
{
    if (value < from)
        return (from);
    else if (value > to)
        return (to);
    else
        return (value);
}

s32
clamp_s32(s32 value, s32 from, s32 to)
{
    if (value < from)
        return (from);
    else if (value >= to)
        return (to);
    else
        return (value);
}

u32
clamp_u32(u32 value, u32 from, u32 to)
{
    if (value < from)
        return (from);
    else if (value > to)
        return (to);
    else
        return (value);
}

struct v2
clamp_v2(struct v2 target, struct v2 min, struct v2 max)
{
    struct v2 result = {
        clamp_f32(target.x, min.x, max.x),
        clamp_f32(target.y, min.y, max.y),
    };
    return (result);
}

struct v3
clamp_v3(struct v3 target, struct v3 min, struct v3 max)
{
    struct v3 result = {
        clamp_f32(target.x, min.x, max.x),
        clamp_f32(target.y, min.y, max.y),
        clamp_f32(target.z, min.z, max.z)
    };
    return (result);
}

struct v3s
clamp_v3s(struct v3s target, struct v3s min, struct v3s max)
{
    struct v3s result = {
        clamp_s32(target.x, min.x, max.x),
        clamp_s32(target.y, min.y, max.y),
        clamp_s32(target.z, min.z, max.z)
    };
    return (result);
}

struct v3u
clamp_v3u(struct v3u target, struct v3u min, struct v3u max)
{
    struct v3u result = {
        clamp_u32(target.x, min.x, max.x),
        clamp_u32(target.y, min.y, max.y),
        clamp_u32(target.z, min.z, max.z)
    };
    return (result);
}

struct v4
clamp_v4(struct v4 target, struct v4 min, struct v4 max)
{
    struct v4 result = {
        clamp_f32(target.x, min.x, max.x),
        clamp_f32(target.y, min.y, max.y),
        clamp_f32(target.z, min.z, max.z),
        clamp_f32(target.z, min.w, max.w)
    };
    return (result);
}

s32
minimum_s32(s32 a, s32 b)
{
    if (a < b)
        return a;
    else
        return b;
}

s32
maximum_s32(s32 a, s32 b)
{
    if (a > b)
        return a;
    else
        return b;
}

s32
maximum_s32_3(s32 value1, s32 value2, s32 value3)
{
    return (maximum_s32(maximum_s32(value1, value2), value3));
}

f32
lerp_f32(f32 start, f32 end, f32 t)
{
    return (1.0f - t) * start + t * end;
}

struct v2
lerp_v2(struct v2 start, struct v2 end, f32 t)
{
    return (add_v2(mul_v2_f32(sub_v2(end, start), t), start));
}

struct v3
lerp_v3(struct v3 start, struct v3 end, f32 t)
{
    return (add_v3(mul_v3_f32(sub_v3(end, start), t), start));
}

struct v4
lerp_v4(struct v4 start, struct v4 end, f32 t)
{
    return (add_v4(mul_v4_f32(sub_v4(end, start), t), start));
}

f32
map_s32(s32 value, s32 from_in, s32 to_in, f32 from_out, f32 to_out)
{
    f32 t = (f32)(value - from_in) / (f32)(to_in - from_in);
    return (lerp_f32(from_out, to_out, t));
}

f32
map_u32(u32 value, u32 from_in, u32 to_in, f32 from_out, f32 to_out)
{
    f32 t = (f32)(value - from_in) / (f32)(to_in - from_in);
    return (lerp_f32(from_out, to_out, t));
}

f32
map_f32(f32 value, f32 from_in, f32 to_in, f32 from_out, f32 to_out)
{
    f32 t = (value - from_in) / (to_in - from_in);
    return (lerp_f32(from_out, to_out, t));
}

struct mat2
mul_mat2(struct mat2 a, struct mat2 b)
{
    struct mat2 result = {
        a.m00 * b.m00 + a.m01 * b.m10,
        a.m00 * b.m01 + a.m01 * b.m11,

        a.m10 * b.m00 + a.m11 * b.m10,
        a.m10 * b.m01 + a.m11 * b.m11,
    };
    return (result);
}

struct mat3
mul_mat3(struct mat3 a, struct mat3 b)
{
    struct mat3 result = {
        a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20,
        a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21,
        a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22,

        a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20,
        a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21,
        a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22,

        a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20,
        a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21,
        a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22,
    };
    return (result);
}

struct v2
rotate_ccw_90_v2(struct v2 v)
{
    struct v2 result = { -v.y, v.x };
    return (result);
}

struct v2
rotate_cw_90_v2(struct v2 v)
{
    return (struct v2) { v.y, -v.x };
}

struct v2
rotate_v2_cos_sin(struct v2 v, f32 cos_angle, f32 sin_angle)
{
    struct v2 result = {
        cos_angle * v.x - sin_angle * v.y,
        sin_angle * v.x + cos_angle * v.y
    };
    return (result);
}

struct v2
rotate_v2(struct v2 v, f32 angle)
{
    f32 cos_angle = cosf(angle);
    f32 sin_angle = sinf(angle);
    return (rotate_v2_cos_sin(v, cos_angle, sin_angle));
}

struct v3
pow_v3(struct v3 a, struct v3 x)
{
    struct v3 result = { powf(a.x, x.x), powf(a.y, x.y), powf(a.z, x.z) };
    return (result);
}

struct v3
rotate_xy(struct v3 v, f32 cosf_angle, f32 sinf_angle)
{
    struct v3 result = {
        cosf_angle * v.x - sinf_angle * v.y,
        sinf_angle * v.x + cosf_angle * v.y,
        v.z
    };
    return (result);
}

struct v2
floor_v2(struct v2 v)
{
    struct v2 result = {
        floorf(v.x),
        floorf(v.y)
    };
    return (result);
}

struct v3
floor_v3(struct v3 v)
{
    struct v3 result = {
        floorf(v.x),
        floorf(v.y),
        floorf(v.z)
    };
    return (result);
}

struct v2
round_v2(struct v2 v)
{
    struct v2 result = {
        roundf(v.x),
        roundf(v.y)
    };
    return (result);
}

struct v3
round_v3(struct v3 v)
{
    struct v3 result = {
        roundf(v.x),
        roundf(v.y),
        roundf(v.z)
    };
    return (result);
}

struct v2
abs_v2(struct v2 v)
{
    struct v2 result = {
        fabsf(v.x),
        fabsf(v.y)
    };
    return (result);
}

struct v3
abs_v3(struct v3 v)
{
    struct v3 result = {
        fabsf(v.x),
        fabsf(v.y),
        fabsf(v.z)
    };
    return (result);
}

struct mat2
abs_mat2(struct mat2 m)
{
    struct mat2 result = {
        fabsf(m.m00),
        fabsf(m.m10),
        fabsf(m.m01),
        fabsf(m.m11),
    };
    return (result);
}

/*
f32
fast_length(struct v3 a)
{
    __m128 a_4x = _mm_set_ps1(a.x*a.x + a.y*a.y + a.z*a.z);
    __m128 result_4x = _mm_rsqrt_ss(a_4x);
    f32 result;
    _mm_store_ps1(&result, result_4x);
    return result;
}
*/

/*
struct v3
fast_normalize(struct v3 a)
{
    f32 Length = fast_length(a);
    struct v3 Result = {a.x / Length, a.y / Length, a.z / Length};
    return Result;
}
*/

void
clamp_v3_(struct v3 *Value, struct v3 From, struct v3 To)
{
    if (Value->x <= From.x)
    {
        Value->x = From.x;
    }
    if (Value->x >= To.x)
    {
        Value->x = To.x;
    }
    if (Value->y <= From.y)
    {
        Value->y = From.y;
    }
    if (Value->y >= To.y)
    {
        Value->y = To.y;
    }
    if (Value->z <= From.z)
    {
        Value->z = From.z;
    }
    if (Value->z >= To.z)
    {
        Value->z = To.z;
    }
}

f32
distance_v3(struct v3 a, struct v3 b)
{
    return length_v3(sub_v3(a, b));
}

f32
distance_v3_squared(struct v3 a, struct v3 b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}

struct v2
project_v2(struct v2 a, struct v2 b)
{
    struct v2 normalized_b = normalize_v2(b);
    return mul_f32_v2(dot_v2(a, normalized_b), normalized_b);
}

f32
project_scalar_v2(struct v2 a, struct v2 b)
{
    struct v2 normal_b = normalize_v2(b);
    return dot_v2(a, normal_b);
}

struct v2
project_point_on_line_v2(struct v2 point,
                         struct v2 a,
                         struct v2 b)
{
    struct v2 projected = project_v2(
        sub_v2(point, a),
        sub_v2(b, a));
    return add_v2(a, projected);
}

struct v2
project_point_on_line_segment_v2(struct v2 point,
                                 struct v2 a,
                                 struct v2 b)
{
    struct v2 a_to_b    = sub_v2(b, a);
    struct v2 projected = project_v2(
        sub_v2(point, a),
        a_to_b);
    if (dot_v2(a_to_b, projected) > 0.0f)
    {
        if (length_squared_v2(projected) <= length_squared_v2(a_to_b))
            return add_v2(a, projected);
        else
            return b;
    }
    return a;
}

struct v3
project_v3(struct v3 a, struct v3 b)
{
    struct v3 normalized_b = normalize_v3(b);
    return mul_f32_v3(dot_v3(a, normalized_b), normalized_b);
}

struct v2
map_f32_v2(f32 value, f32 from_start, f32 from_end, struct v2 to_start, struct v2 to_end)
{
    f32 t = (value - from_start) / (from_end - from_start);
    return (lerp_v2(to_start, to_end, t));
}

struct v2
reflect_v2(struct v2 v, struct v2 base)
{
    struct v2 base_rotated = rotate_ccw_90_v2(base);

    struct v2 right = project_v2(v, base_rotated);
    struct v2 up    = project_v2(v, base);

    return (sub_v2(right, up));
}

f32
angle_between_vectors(struct v3 a, struct v3 b)
{
    f32 Length_a = length_v3(a);
    f32 Length_b = length_v3(b);
    if (Length_a == 0 || Length_b == 0)
        return 0;
    f32 angle = acosf(clamp_f32(dot_v3(a, b) / (Length_a * Length_b), -1.0f, 1.0f));
    return angle;
}

struct v3
reflect(struct v3 v, struct v3 n)
{
    struct v3 result = sub_v3(v, mul_f32_v3(2.0f * dot_v3(v, n), n));
    return (result);
}

f32
ccw_angle(struct v3 target, struct v3 basis, struct v3 up)
{
    f32 angle_in_radians = acosf(clamp_f32(dot_v3(normalize_v3(basis), normalize_v3(target)), -1.0f, 1.0f));

    if (dot_v3(cross_v3(basis, target), up) < 0)
    {
        return 2 * PI - angle_in_radians;
    }
    else
    {
        return angle_in_radians;
    }
}

f32
angle_between_v2(struct v2 a, struct v2 b)
{
    f32 length_a = length_v2(a);
    f32 length_b = length_v2(b);
    if (almost_zero_f32(length_a, 0.00001) || almost_zero_f32(length_b, 0.00001))
        return 0.0f;
    f32 angle = acosf(clamp_f32(dot_v2(a, b) / (length_a * length_b), -1.0f, 1.0f));
    return angle;
}

f32
clockwise_angle_in_radians_v3(struct v3 base, struct v3 target, struct v3 up)
{
    f32 signed_angle     = dot_v3(normalize_v3(base), normalize_v3(target));
    f32 angle_in_radians = acosf(signed_angle);

    f32 direction = dot_v3(cross_v3(base, target), up);
    if (direction < 0)
    {
        return angle_in_radians;
    }
    else
    {
        return 2 * PI - angle_in_radians;
    }
}

f32
clockwise_angle_in_radians(struct v2 basis, struct v2 Target)
{
    f32 SignedAngleInRadians = dot_v2(normalize_v2(basis), normalize_v2(Target));

    f32 AngleInRadians = PI * ((1.0f - SignedAngleInRadians) / 2.0f);

    if (cross_v2(basis, Target) < 0)
    {
        return AngleInRadians;
    }
    else
    {
        return 2 * PI - AngleInRadians;
    }
}

f32
counterclockwise_angle_in_radians(struct v2 basis, struct v2 Target)
{
    f32 SignedAngleInRadians = dot_v2(normalize_v2(basis), normalize_v2(Target));

    f32 AngleInRadians = PI * ((1.0f - SignedAngleInRadians) / 2.0f);

    if (cross_v2(basis, Target) < 0)
    {
        return 2 * PI - AngleInRadians;
    }
    else
    {
        return AngleInRadians;
    }
}

struct v4
darken_color(struct v4 color, f32 strength)
{
    f32       s      = strength;
    struct v4 result = { color.x * s, color.y * s, color.z * s, color.w };
    return result;
}

struct v4
lighten_color(struct v4 color, f32 strength)
{
    f32       s      = strength;
    struct v4 result = { color.x * s, color.y * s, color.z * s, color.w };
    return result;
}

struct quaternion
nlerp(struct quaternion left, struct quaternion right, f32 t)
{
    f32 left_t  = 1.0f - t;
    f32 right_t = dot_quaternion(left, right) > 0 ? t : -t;
    return (normalize_quaternion(add_quaternion(mul_quaternion_f32(left, left_t),
                                                mul_quaternion_f32(right, right_t))));
}

b32
point_in_triangle(struct v3s vertex, struct v3s a, struct v3s b, struct v3s c)
{
    // Compute vectors
    struct v3s V0 = sub_v3s(c, a);
    struct v3s V1 = sub_v3s(b, a);
    struct v3s V2 = sub_v3s(vertex, a);

    // Compute dot products
    s32 Dot00 = dot_v3s(V0, V0);
    s32 Dot01 = dot_v3s(V0, V1);
    s32 Dot02 = dot_v3s(V0, V2);
    s32 Dot11 = dot_v3s(V1, V1);
    s32 Dot12 = dot_v3s(V1, V2);

    // Compute barycentric coordinates
    s32 Denom = (Dot00 * Dot11 - Dot01 * Dot01);
    if (Denom == 0)
    {
        return 0;
    }
    f32 InvDenom = 1.0f / (f32)Denom;
    f32 U        = (f32)(Dot11 * Dot02 - Dot01 * Dot12) * InvDenom;
    f32 V        = (f32)(Dot00 * Dot12 - Dot01 * Dot02) * InvDenom;

    // Check if point is in triangle
    return (U >= 0) && (V >= 0) && (U + V < 1);
}

struct mat2
make_scale_mat2(f32 x, f32 y)
{
    struct mat2 m = { x, 0, 0, y };
    return (m);
}

struct mat2
make_rotation_mat2(f32 angle)
{
    f32         cos_angle = cosf(angle);
    f32         sin_angle = sinf(angle);
    struct mat2 m         = {
        cos_angle, sin_angle,
        -sin_angle, cos_angle
    };
    return (m);
}

struct mat3
mat4_to_mat3(struct mat4 matrix)
{
    struct mat3 result = { 0 };
    result.m00         = matrix.m00;
    result.m01         = matrix.m01;
    result.m02         = matrix.m02;
    result.m10         = matrix.m10;
    result.m11         = matrix.m11;
    result.m12         = matrix.m12;
    result.m20         = matrix.m20;
    result.m21         = matrix.m21;
    result.m22         = matrix.m22;

    return (result);
}

struct v2
mul_mat2_v2(struct mat2 m, struct v2 v)
{
    struct v2 result = {
        m.m00 * v.x + m.m10 * v.y,
        m.m01 * v.x + m.m11 * v.y,
    };
    return (result);
}

struct v3
mul_mat3_v3(struct mat3 m, struct v3 v)
{
    struct v3 result = {
        m.m00 * v.x + m.m10 * v.y + m.m20 * v.z,
        m.m01 * v.x + m.m11 * v.y + m.m21 * v.z,
        m.m02 * v.x + m.m12 * v.y + m.m22 * v.z
    };
    return (result);
}

struct v4
mul_mat4_v4(struct mat4 m, struct v4 v)
{
    struct v4 result = {
        m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * v.w,
        m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * v.w,
        m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * v.w,
        m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * v.w
    };
    return (result);
}

struct mat3
make_translation_mat3(struct v2 position)
{
    struct mat3 m = {
        1.0f, 0.0f, position.x,
        0.0f, 1.0f, position.y,
        0.0f, 0.0f, 1.0f
    };
    return (m);
}

//@Questionable
struct mat3
make_camera_matrix_mat3(struct v2 position, f32 left, f32 right, f32 bottom, f32 top)
{
    f32 width  = right - left;
    f32 height = top - bottom;

    struct mat3 projection = {
        2.0f / width, 0.0f, -(left + right) / width,
        0.0f, 2.0f / height, -(top + bottom) / height,
        0.0f, 0.0f, 1.0
    };
    struct mat3 translation = make_translation_mat3(position);
    return (mul_mat3(projection, translation));
}

void
initialise_ray_grid_march2(struct ray_grid_march2 *march, struct v2 origin, struct v2 direction, struct v2 tile_size)
{
    march->t         = 0.0f;
    march->origin    = origin;
    march->point     = origin;
    march->direction = direction;
    march->tile.x    = (s32)floorf(origin.x / tile_size.x);
    march->tile.y    = (s32)floorf(origin.y / tile_size.y);
    march->step.x    = direction.x < 0.0f ? -1 : 1;
    march->step.y    = direction.y < 0.0f ? -1 : 1;

    march->t_delta.x     = fabsf(tile_size.x / direction.x);
    march->t_delta.y     = fabsf(tile_size.y / direction.y);
    f32 t_max_x_fraction = (f32)origin.x / (f32)tile_size.x;
    t_max_x_fraction     = t_max_x_fraction - floorf(t_max_x_fraction);
    f32 t_max_y_fraction = (f32)origin.y / (f32)tile_size.y;
    t_max_y_fraction     = t_max_y_fraction - floorf(t_max_y_fraction);
    if (direction.x == 0.0f)
        t_max_x_fraction = 0.5f; // prevent inf * 0.0f
    if (direction.y == 0.0f)
        t_max_y_fraction = 0.5f; // prevent inf * 0.0f
    march->t_max.x = march->t_delta.x * (direction.x > 0.0f ? 1.0f - t_max_x_fraction : t_max_x_fraction);
    march->t_max.y = march->t_delta.y * (direction.y > 0.0f ? 1.0f - t_max_y_fraction : t_max_y_fraction);
}

void
next_ray_grid_march2(struct ray_grid_march2 *march)
{
    if (march->t_max.x < march->t_max.y)
    {
        march->point   = add_v2(march->origin,
                                mul_f32_v2(march->t_max.x, march->direction));
        march->t_max.x = march->t_max.x + march->t_delta.x;
        march->tile.x  = march->tile.x + march->step.x;
        march->t       = march->t_max.x;
    }
    else
    {
        march->point   = add_v2(march->origin,
                                mul_f32_v2(march->t_max.y, march->direction));
        march->t_max.y = march->t_max.y + march->t_delta.y;
        march->tile.y  = march->tile.y + march->step.y;
        march->t       = march->t_max.y;
    }
}

void
initialise_ray_grid_march(struct ray_grid_march *march, struct v2 origin, struct v2 direction, struct v2 tile_size)
{
    march->x      = (s32)floorf(origin.x / tile_size.x);
    march->y      = (s32)floorf(origin.y / tile_size.y);
    march->step_x = direction.x < 0.0f ? -1 : 1;
    march->step_y = direction.y < 0.0f ? -1 : 1;

    march->t_delta_x     = fabsf(tile_size.x / direction.x);
    march->t_delta_y     = fabsf(tile_size.y / direction.y);
    f32 t_max_x_fraction = (f32)origin.x / (f32)tile_size.x;
    t_max_x_fraction     = t_max_x_fraction - floorf(t_max_x_fraction);
    f32 t_max_y_fraction = (f32)origin.y / (f32)tile_size.y;
    t_max_y_fraction     = t_max_y_fraction - floorf(t_max_y_fraction);
    if (direction.x == 0.0f)
        t_max_x_fraction = 0.5f; // prevent inf * 0.0f
    if (direction.y == 0.0f)
        t_max_y_fraction = 0.5f; // prevent inf * 0.0f
    march->t_max_x = march->t_delta_x * (direction.x > 0.0f ? 1.0f - t_max_x_fraction : t_max_x_fraction);
    march->t_max_y = march->t_delta_y * (direction.y > 0.0f ? 1.0f - t_max_y_fraction : t_max_y_fraction);
}

void
initialise_ray_grid_march_3d(struct ray_grid_march *march, struct v3 origin, struct v3 direction)
{
    march->side_of_entry = 0;
    march->origin        = origin;
    march->point         = origin;
    march->direction     = direction;
    march->x             = (s32)floorf(origin.x);
    march->y             = (s32)floorf(origin.y);
    march->z             = (s32)floorf(origin.z);
    march->step_x        = direction.x < 0.0f ? -1 : 1;
    march->step_y        = direction.y < 0.0f ? -1 : 1;
    march->step_z        = direction.z < 0.0f ? -1 : 1;

    march->t_delta_x = fabsf(1.0f / direction.x);
    march->t_delta_y = fabsf(1.0f / direction.y);
    march->t_delta_z = fabsf(1.0f / direction.z);

    f32 t_max_x_fraction = origin.x - floorf(origin.x);
    f32 t_max_y_fraction = origin.y - floorf(origin.y);
    f32 t_max_z_fraction = origin.z - floorf(origin.z);

    if (direction.x == 0.0f)
        t_max_x_fraction = 0.5f; // prevent inf * 0.0f
    if (direction.y == 0.0f)
        t_max_y_fraction = 0.5f; // prevent inf * 0.0f
    if (direction.z == 0.0f)
        t_max_z_fraction = 0.5f; // prevent inf * 0.0f
    march->t_max_x = march->t_delta_x * (direction.x > 0.0f ? 1.0f - t_max_x_fraction : t_max_x_fraction);
    march->t_max_y = march->t_delta_y * (direction.y > 0.0f ? 1.0f - t_max_y_fraction : t_max_y_fraction);
    march->t_max_z = march->t_delta_z * (direction.z > 0.0f ? 1.0f - t_max_z_fraction : t_max_z_fraction);
}

void
next_ray_grid_march(struct ray_grid_march *march)
{
    if (march->t_max_x < march->t_max_y)
    {
        if (march->t_max_x < march->t_max_z)
        {
            march->side_of_entry = march->step_x > 0 ? 0 : 1;
            march->point         = add_v3(march->origin,
                                          mul_f32_v3(march->t_max_x, march->direction));
            march->x             = march->x + march->step_x;
            march->t_max_x       = march->t_max_x + march->t_delta_x;
        }
        else
        {
            march->side_of_entry = march->step_z > 0 ? 4 : 5;
            march->point         = add_v3(march->origin,
                                          mul_f32_v3(march->t_max_z, march->direction));
            march->z             = march->z + march->step_z;
            march->t_max_z       = march->t_max_z + march->t_delta_z;
        }
    }
    else
    {
        if (march->t_max_y < march->t_max_z)
        {
            march->side_of_entry = march->step_y > 0 ? 2 : 3;
            march->point         = add_v3(march->origin,
                                          mul_f32_v3(march->t_max_y, march->direction));
            march->y             = march->y + march->step_y;
            march->t_max_y       = march->t_max_y + march->t_delta_y;
        }
        else
        {
            march->side_of_entry = march->step_z > 0 ? 4 : 5;
            march->point         = add_v3(march->origin,
                                          mul_f32_v3(march->t_max_z, march->direction));
            march->z             = march->z + march->step_z;
            march->t_max_z       = march->t_max_z + march->t_delta_z;
        }
    }
}

struct v3
rotate_vector_by_quaternion(struct quaternion q, struct v3 vector)
{
    struct v3 vector_part = { q.x, q.y, q.z };
    f32       scalar_part = q.w;

    struct v3 a = mul_f32_v3(2.0f * dot_v3(vector_part, vector), vector_part);
    struct v3 b = mul_f32_v3(scalar_part * scalar_part - dot_v3(vector_part, vector_part), vector);
    struct v3 c = mul_f32_v3(2.0f * scalar_part, cross_v3(vector_part, vector));
    return (add_v3(a, add_v3(b, c)));
}

struct mat4
quaternion_to_matrix(struct quaternion q)
{
    q                  = normalize_quaternion(q);
    struct mat4 result = { 0 };
    result.m00         = 1 - 2 * q.y * q.y - 2 * q.z * q.z;
    result.m01         = 2 * q.x * q.y + 2 * q.z * q.w;
    result.m02         = 2 * q.x * q.z - 2 * q.y * q.w;
    result.m03         = 0.0f;

    result.m10 = 2 * q.x * q.y - 2 * q.z * q.w;
    result.m11 = 1 - 2 * q.x * q.x - 2 * q.z * q.z;
    result.m12 = 2 * q.y * q.z + 2 * q.x * q.w;
    result.m13 = 0.0f;

    result.m20 = 2 * q.x * q.z + 2 * q.y * q.w;
    result.m21 = 2 * q.y * q.z - 2 * q.x * q.w;
    result.m22 = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
    result.m23 = 0.0f;

    result.m30 = 0.0f;
    result.m31 = 0.0f;
    result.m32 = 0.0f;
    result.m33 = 1.0f;
    return (result);
}

struct v3
calculate_point_on_sphere(f32 azimuth, f32 altitude)
{
    f32 forward_length, up_length;
    forward_length    = sinf(altitude);
    struct v3 forward = { sinf(azimuth), cosf(azimuth), 0.0f };
    forward           = mul_f32_v3(forward_length, forward);
    up_length         = cosf(altitude);
    struct v3 up      = { 0.0f, 0.0f, 1.0f };
    up                = mul_f32_v3(up_length, up);
    return (add_v3(up, forward));
}

b32
is_point_in_sphere(struct v3 point, struct v3 center, f32 radius)
{
    return fabsf(length_v3(sub_v3(center, point))) <= radius;
}

struct quaternion
make_rotation_quaternion(struct v3 axis, f32 angle)
{
    axis                              = normalize_v3(axis);
    f32               sinf_half_angle = sinf(0.5f * angle);
    struct quaternion result          = { 0 };
    result.w                          = cosf(0.5f * angle);
    result.x                          = sinf_half_angle * axis.x;
    result.y                          = sinf_half_angle * axis.y;
    result.z                          = sinf_half_angle * axis.z;
    return (result);
}

struct ray_v3
screen_position_to_camera_ray(struct v3   camera_position,
                              struct mat4 projection,
                              struct mat4 view,
                              struct v2u  screen_size,
                              struct v2s  mouse_position)
{
    struct mat4 inv_projection = invert_mat4(projection);
    struct mat4 inv_view       = invert_mat4(view);

    f32 x = ((2.0f * (f32)mouse_position.x / (f32)screen_size.x) - 1.0f);
    f32 y = ((2.0f * (f32)mouse_position.y / (f32)screen_size.y) - 1.0f);

    struct v4 ndc           = { x, -y, -1.0f, 1.0f };
    struct v4 ray_eye       = mul_mat4_v4(inv_projection, ndc);
    ray_eye.w               = 0.0f;
    struct v4 ray_world     = mul_mat4_v4(inv_view, ray_eye);
    struct v3 ray_direction = { ray_world.x, ray_world.y, ray_world.z };

    struct ray_v3 ray = { camera_position, ray_direction };
    return ray;
}
/*

void
check_collision_aab_aab(contact_t *collision, struct v3 position_a, struct v3 half_extend_a, struct v3 position_b, struct v3 half_extend_b)
{
    struct v3 minimum_a = position_a - half_extend_a;
    struct v3 maximum_a = position_a + half_extend_a;
    struct v3 minimum_b = position_b - half_extend_b;
    struct v3 maximum_b = position_b + half_extend_b;
    struct v3 minimum_overlap = {0.0f, 0.0f, 0.0f};

    minimum_overlap.x = -fminf(maximum_a.x - minimum_b.x, maximum_b.x - minimum_a.x);
    minimum_overlap.y = -fminf(maximum_a.y - minimum_b.y, maximum_b.y - minimum_a.y);
    minimum_overlap.z = -fminf(maximum_a.z - minimum_b.z, maximum_b.z - minimum_a.z);
    collision->distance = fmaxf(minimum_overlap.x, fmaxf(minimum_overlap.y, minimum_overlap.z));
    if (minimum_overlap.x > minimum_overlap.y && minimum_overlap.x > minimum_overlap.z)
    {
        if (position_a.x < position_b.x)
        {
            collision->normal = {collision->distance, 0.0f, 0.0f};
        }
        else
        {
            collision->normal = {-collision->distance, 0.0f, 0.0f};
        }
    }
    else if (minimum_overlap.y > minimum_overlap.x && minimum_overlap.y > minimum_overlap.z)
    {
        if (position_a.y < position_b.y)
        {
            collision->normal = {0.0f, collision->distance, 0.0f};
        }
        else
        {
            collision->normal = {0.0f, -collision->distance, 0.0f};
        }
    }
    else if (minimum_overlap.z > minimum_overlap.x && minimum_overlap.z > minimum_overlap.y)
    {
        if (position_a.z < position_b.z)
        {
            collision->normal = {0.0f, 0.0f, collision->distance};
        }
        else
        {
            collision->normal = {0.0f, 0.0f, -collision->distance};
        }
    }
}


void
check_collision_aab_aab (contact_t *collision, struct aab3 aab_a, struct aab3 aab_b)
{
    struct v3 half_extend_a = (aab_a.max - aab_a.min) * 0.5f;
    struct v3 half_extend_b = (aab_b.max - aab_b.min) * 0.5f;
    struct v3 location_a = aab_a.min + half_extend_a;
    struct v3 location_b = aab_b.min + half_extend_b;
    check_collision_aab_aab(collision, location_a, half_extend_a, location_b, half_extend_b);
}

void
check_collision_sphere_aab(contact_t *collision, struct v3 position_a, f32 radius_a, struct v3 position_b, struct v3 half_extend_b)
{
    struct v3 minimum = position_b - half_extend_b;
    struct v3 maximum = position_b + half_extend_b;
    struct v3 closest_point_on_b = clamp(position_a, minimum, maximum);

    f32 inside = 1.0f;
    if (closest_point_on_b.x == position_a.x && closest_point_on_b.y == position_a.y && closest_point_on_b.z == position_a.z)
    {
        inside = -1.0f;
        f32 difference_x = position_a.x - position_b.x;
        f32 difference_y = position_a.y - position_b.y;
        f32 difference_z = position_a.z - position_b.z;
        if (fabs(difference_x) > fabs(difference_y) && fabs(difference_x) > fabs(difference_z)) {
            if (difference_x > 0)
                closest_point_on_b.x = maximum.x;
            else
                closest_point_on_b.x = minimum.x;
        } else if (fabs(difference_y) > fabs(difference_x) && fabs(difference_y) > fabs(difference_z)) {
            if (difference_y > 0)
                closest_point_on_b.y = maximum.y;
            else
                closest_point_on_b.y = minimum.y;
        }
        if (fabs(difference_z) > fabs(difference_y) && fabs(difference_z) > fabs(difference_x)) {
            if (difference_z > 0)
                closest_point_on_b.z = maximum.z;
            else
                closest_point_on_b.z = minimum.z;
        }
    }
    struct v3 b_to_a = position_a - closest_point_on_b;
    collision->distance = -radius_a + (inside * length(b_to_a));
    collision->normal = (inside * -collision->distance) * normalize(b_to_a);
}

void
check_collision_aab_sphere(contact_t *collision, struct v3 position_a, struct v3 half_extend_a, struct v3 position_b, f32 radius_b)
{
    check_collision_sphere_aab(collision, position_b, radius_b, position_a, half_extend_a);
    collision->normal = -1.0f * collision->normal;
}

void
check_collision_sphere_sphere(contact_t *collision, struct v3 position_a, f32 radius_a, struct v3 position_b, f32 radius_b)
{
    struct v3 a_to_b = position_b - position_a;
    collision->distance = length(a_to_b) - radius_a - radius_b;
    collision->normal = collision->distance * normalize(a_to_b);
}

bool
check_collision_ray_aab(ray_v3 ray, struct v3 min, struct v3 max, f32 *distance)
{
    struct v3 invdir = {
        1.0f / ray.direction.x,
        1.0f / ray.direction.y,
        1.0f / ray.direction.z
    };
    bool sign[3] = {invdir.x < 0, invdir.y < 0, invdir.z < 0};
    struct v3 bounds[2] = {min, max};
    f32 tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[sign[0]].x - ray.origin.x) * invdir.x;
    tmax = (bounds[1-sign[0]].x - ray.origin.x) * invdir.x;
    tymin = (bounds[sign[1]].y - ray.origin.y) * invdir.y;
    tymax = (bounds[1-sign[1]].y - ray.origin.y) * invdir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[sign[2]].z - ray.origin.z) * invdir.z;
    tzmax = (bounds[1-sign[2]].z - ray.origin.z) * invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    *distance = tmin;

    if (*distance < 0)
    {
        *distance = tmax;
        if (*distance < 0)
            return false;
    }

    return true;
}


bool
check_collision_ray_aab(struct ray_v3ray, struct aab3 aab, f32 *distance)
{
    return check_collision_ray_aab(ray, aab.min, aab.max, distance);
}


bool
check_collision_ray_sphere(struct ray_v3ray, struct v3 center, f32 radius)
{
    struct v3 oc = ray.origin - center;
    f32 a = dot(ray.direction, ray.direction);
    f32 b = 2.0f * dot(oc, ray.direction);
    f32 c = dot(oc,oc) - radius*radius;
    f32 discriminant = b*b - 4.0f*a*c;
    return (discriminant>0);

}

bool
check_collision_ray_plane(struct v3 plane_normal, struct v3 plane_origin, struct ray_v3ray, f32 *t)
{
    plane_normal = -1.0f * plane_normal;
    f32 denom = dot(plane_normal, ray.direction);
    if (denom > 0.000001f) {
        struct v3 p0l0 = plane_origin - ray.origin;
        *t = dot(p0l0, plane_normal) / denom;
        return (*t >= 0);
    }

    return false;
}

*/

b32
ray_triangle_collision(struct ray_v3 ray, struct v3 va, struct v3 vb, struct v3 vc, f32 *distance)
{
    const f32 EPSILON = 0.00001f;
    struct v3 vertex0 = va;
    struct v3 vertex1 = vb;
    struct v3 vertex2 = vc;
    struct v3 edge1, edge2, h, s, q;
    f32       a, f, u, v;
    edge1 = sub_v3(vertex1, vertex0);
    edge2 = sub_v3(vertex2, vertex0);
    h     = cross_v3(ray.direction, edge2);
    a     = dot_v3(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return 0;
    f = 1.0f / a;
    s = sub_v3(ray.origin, vertex0);
    u = f * dot_v3(s, h);
    if (u < 0.0 || u > 1.0)
        return 0;
    q = cross_v3(s, edge1);
    v = f * dot_v3(ray.direction, q);
    if (v < 0.0 || u + v > 1.0)
        return 0;
    f32 t = f * dot_v3(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        *distance = t;
        // outIntersectionPoint = rayOrigin + rayVector * t;
        return 1;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return 0;
}

b32
check_collision_ray_triangle(struct ray_v3 ray, struct v3 a, struct v3 b, struct v3 c, f32 *t)
{
    struct v3 a_to_b = sub_v3(b, a);
    struct v3 b_to_c = sub_v3(c, b);
    struct v3 c_to_a = sub_v3(a, c);

    struct v3 normal = cross_v3(a_to_b, mul_f32_v3(-1.0f, c_to_a));

    f32 n_dot_ray_direction = dot_v3(normal, ray.direction);
    if (almost_zero_f32(fabsf(n_dot_ray_direction), 0.00001f))
        return 0; // plane and ray are parallel
                  // there might be an intersection but we don't care

    f32 d = dot_v3(normal, a);

    *t = -(dot_v3(normal, ray.origin) + d) / n_dot_ray_direction;

    if (*t < 0)
        return 0; // the triangle is behind the ray

    struct v3 intersection_point = add_v3(ray.origin, mul_f32_v3(*t, ray.direction));

    struct v3 perpendicular = { 0 };

    struct v3 a_to_p = sub_v3(intersection_point, a);
    perpendicular    = cross_v3(a_to_b, a_to_p);
    if (dot_v3(normal, perpendicular) < 0)
        return 0;

    struct v3 b_to_p = sub_v3(intersection_point, b);
    perpendicular    = cross_v3(b_to_c, b_to_p);
    if (dot_v3(normal, perpendicular) < 0)
        return 0;

    struct v3 c_to_p = sub_v3(intersection_point, c);
    perpendicular    = cross_v3(c_to_a, c_to_p);
    if (dot_v3(normal, perpendicular) < 0)
        return 0;

    return 1;
}

#endif
