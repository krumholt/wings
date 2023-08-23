#ifndef MESH_TOOLS_C_
#define MESH_TOOLS_C_

#include "wings/base/math.c"

struct v3 unit_cube_positions[] = {
    {-1.0f,  -1.0f, 1.0f },
    { 1.0f,  -1.0f, 1.0f },
    { 1.0f,  1.0f,  1.0f },
    { -1.0f, 1.0f,  1.0f },

    { -1.0f, -1.0f, -1.0f},
    { 1.0f,  -1.0f, -1.0f},
    { 1.0f,  1.0f,  -1.0f},
    { -1.0f, 1.0f,  -1.0f}
};

struct v2 cube_uvs[] = {
    {0.0f,  0.0f},
    { 0.0f, 1.0f},
    { 1.0f, 0.0f},

    { 1.0f, 1.0f},
    { 1.0f, 0.0f},
    { 0.0f, 1.0f},
};

const s32 cube_indices[] = {
    // -X
    4,
    0,
    7,
    3,
    7,
    0,
    // +X
    1,
    5,
    2,
    6,
    2,
    5,
    // -Y
    4,
    5,
    0,
    1,
    0,
    5,
    // +Y
    3,
    2,
    7,
    6,
    7,
    2,
    // -Z
    7,
    6,
    4,
    5,
    4,
    6,
    // +Z
    1,
    2,
    0,
    3,
    0,
    2,
};

const struct v3 cube_normals[] = {
    {-1.0f, 0.0f,  0.0f },
    { 1.0f, 0.0f,  0.0f },
    { 0.0f, -1.0f, 0.0f },

    { 0.0f, 1.0f,  0.0f },
    { 0.0f, 0.0f,  -1.0f},
    { 0.0f, 0.0f,  1.0f },
};

void
write_cube_positions(f32 *target, s32 stride, s32 offset, struct v3 translation, struct v3 scale)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index * stride + 0 + offset] = unit_cube_positions[cube_indices[index]].x * scale.x + translation.x;
        target[index * stride + 1 + offset] = unit_cube_positions[cube_indices[index]].y * scale.y + translation.y;
        target[index * stride + 2 + offset] = unit_cube_positions[cube_indices[index]].z * scale.z + translation.z;
    }
}

void
write_cube_normals(f32 *target, s32 stride, s32 offset)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index * stride + 0 + offset] = cube_normals[index / 6].x;
        target[index * stride + 1 + offset] = cube_normals[index / 6].y;
        target[index * stride + 2 + offset] = cube_normals[index / 6].z;
    }
}

void
write_cube_uvs(f32 *target, s32 stride, s32 offset)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index * stride + 0 + offset] = cube_uvs[index / 6].x;
        target[index * stride + 1 + offset] = cube_uvs[index / 6].y;
    }
}

void
write_cube_colors(f32 *target, s32 stride, s32 offset, struct v4 color)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index * stride + 0 + offset] = color.x;
        target[index * stride + 1 + offset] = color.y;
        target[index * stride + 2 + offset] = color.z;
        target[index * stride + 3 + offset] = color.w;
    }
}

struct v3
pointy_hex_corner(struct v3 center, f32 size, s32 i)
{
    f32 angle_degrees = 60.0f * i - 30.0f;
    f32 angle_radians = PI / 180 * angle_degrees;

    struct v3 point = {
        center.x + size * cos(angle_radians),
        center.y + size * sin(angle_radians),
        0.0f,
    };
    return (point);
}

struct v3
hex_direction(struct v3 center, f32 size, s32 i)
{
    f32 angle_degrees = 60.0f * i;
    f32 angle_radians = PI / 180 * angle_degrees;

    struct v3 point = {
        center.x + size * cos(angle_radians),
        center.y + size * sin(angle_radians),
        0.0f,
    };
    return (point);
}

void
write_hexagon_positions(struct v3 *target, struct v3 center, f32 size, s32 height)
{
    for (s32 index = 0; index < 6; ++index)
    {
        struct v3 corner_1 = pointy_hex_corner(center, size, index);
        struct v3 corner_1_down = corner_1;
		corner_1_down.z -= height;
        struct v3 corner_2 = pointy_hex_corner(center, size, index + 1);
        struct v3 corner_2_down = corner_2;
		corner_2_down.z -= height;

        *target = center;
		++target;
        *target = corner_1;
		++target;
        *target = corner_2;
		++target;
        *target = corner_1_down;
		++target;
        *target = corner_2_down;
		++target;
        *target = corner_1;
		++target;
        *target = corner_1;
		++target;
        *target = corner_2_down;
		++target;
        *target = corner_2;
		++target;
    }
}

void
write_hexagon_normals(struct v3 *target)
{
    struct v3 center = { 0.0f, 0.0f, 0.0f };
	struct v3 up = {0.0f, 0.0f, 1.0f};
    for (s32 index = 0; index < 6; ++index)
    {
		*target = up;
		++target;
		*target = up;
		++target;
		*target = up;
		++target;
        struct v3 corner = hex_direction(center, 1.0f, index);
		*target = corner;
		++target;
		*target = corner;
		++target;
		*target = corner;
		++target;
		*target = corner;
		++target;
		*target = corner;
		++target;
		*target = corner;
		++target;
    }
}

void
write_hexagon_colors(struct v4 *target, struct v4 color)
{
    for (s32 index = 0; index < 54; ++index)
    {
		*target = color;
		++target;
    }
}

#endif
