#ifndef MESH_TOOLS_C_
#define MESH_TOOLS_C_

#include "wings/base/math.c"
#include "wings/graphics/mesh.c"

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
write_cube_positions(struct v3 *target, struct v3 translation, struct v3 scale)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index] = add_v3(mul_v3(unit_cube_positions[cube_indices[index]], scale), translation);
    }
}

void
write_cube_normals(struct v3 *target)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index] = cube_normals[index / 6];
    }
}

void
write_cube_uvs(struct v2 *target)
{
    for (s32 index = 0; index < 36; ++index)
    {

        target[index] = cube_uvs[index / 6];
    }
}

void
write_cube_colors(struct v4 *target, struct v4 *colors, u32 number_of_colors)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index] = colors[index % number_of_colors];
    }
}

struct v3
_mt_pointy_hex_corner(struct v3 center, f32 size, s32 i)
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
_mt_hex_direction(struct v3 center, f32 size, s32 i)
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
        struct v3 corner_1      = _mt_pointy_hex_corner(center, size, index);
        struct v3 corner_1_down = corner_1;
        corner_1.z              = height;

        struct v3 corner_2      = _mt_pointy_hex_corner(center, size, index + 1);
        struct v3 corner_2_down = corner_2;
        corner_2.z              = height;

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
    struct v3 up     = { 0.0f, 0.0f, 1.0f };
    for (s32 index = 0; index < 6; ++index)
    {
        *target = up;
        ++target;
        *target = up;
        ++target;
        *target = up;
        ++target;
        struct v3 corner = _mt_hex_direction(center, 1.0f, index);
        *target          = corner;
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
write_hexagon_colors(struct v4 *target, struct v4 *colors, u32 number_of_colors)
{
    for (s32 index = 0; index < 54; ++index)
    {
        *target = colors[index % number_of_colors];
        ++target;
    }
}

void
push_sphere(struct mesh *mesh,
            struct v3    position,
            f32          radius,
            struct v4    color)
{
    const s32 number_of_vertical_slices   = 32;
    const s32 number_of_horizontal_slices = 32;
    const s32 number_of_vertices          = number_of_vertical_slices * number_of_horizontal_slices * 3 * 2; // two triangles per horizontal slice
    if (mesh->used + number_of_vertices > mesh->size)
    {
        static s32 frame_counter = 0;
        frame_counter++;
        if (frame_counter > 1000)
        {
            frame_counter = 0;
            printf("[WARNING]: Mesh overflow\n");
        }
        return;
    }

    for (int altitude_index = 0;
         altitude_index < number_of_horizontal_slices;
         ++altitude_index)
    {
        for (int azimuth_index = 0;
             azimuth_index < number_of_vertical_slices;
             ++azimuth_index)
        {
            f32       altitude_1 = to_radians(180.0f + (f32)(altitude_index)*180.0f / (f32)(number_of_horizontal_slices));
            f32       altitude_2 = to_radians(180.0f + (f32)(altitude_index + 1) * 180.0f / (f32)(number_of_horizontal_slices));
            f32       azimuth_1  = to_radians((f32)(azimuth_index)*360.0f / (f32)(number_of_vertical_slices));
            f32       azimuth_2  = to_radians((f32)(azimuth_index + 1) * 360.0f / (f32)(number_of_vertical_slices));
            struct v3 point_1    = calculate_point_on_sphere(azimuth_1, altitude_1);
            struct v3 point_2    = calculate_point_on_sphere(azimuth_2, altitude_1);
            struct v3 point_3    = calculate_point_on_sphere(azimuth_1, altitude_2);
            struct v3 point_4    = calculate_point_on_sphere(azimuth_2, altitude_2);

            u32 index = mesh->used;
            if (mesh->attributes & mesh_attribute_positions)
            {
                index                    = mesh->used;
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_1),
                                                  position);
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_3),
                                                  position);
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_2),
                                                  position);
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_3),
                                                  position);
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_4),
                                                  position);
                mesh->positions[index++] = add_v3(mul_f32_v3(radius, point_2),
                                                  position);
            }
            if (mesh->attributes & mesh_attribute_normals)
            {
                index                  = mesh->used;
                mesh->normals[index++] = normalize_v3(point_1);
                mesh->normals[index++] = normalize_v3(point_3);
                mesh->normals[index++] = normalize_v3(point_2);
                mesh->normals[index++] = normalize_v3(point_3);
                mesh->normals[index++] = normalize_v3(point_4);
                mesh->normals[index++] = normalize_v3(point_2);
            }
            if (mesh->attributes & mesh_attribute_colors)
            {
                index                 = mesh->used;
                mesh->colors[index++] = color;
                mesh->colors[index++] = color;
                mesh->colors[index++] = color;
                mesh->colors[index++] = color;
                mesh->colors[index++] = color;
                mesh->colors[index++] = color;
            }
            mesh->used = index;
        }
    }
}

void
clear_mesh(struct mesh *mesh)
{
    mesh->used = 0;
}

#endif
