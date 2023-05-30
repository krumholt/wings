#ifndef mesh_tools_c
#define mesh_tools_c

#include "wings_math.c"

struct v3 unit_cube_positions[] =
{
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f},

    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f}
};

struct v2 cube_uvs[] =
{
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 0.0f},

    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
};

const s32 cube_indices[] =
{
    // -X
    4, 0, 7,
    3, 7, 0,
    // +X
    1, 5, 2,
    6, 2, 5,
    // -Y
    4, 5, 0,
    1, 0, 5,
    // +Y
    3, 2, 7,
    6, 7, 2,
    // -Z
    7, 6, 4,
    5, 4, 6,
    // +Z
    1, 2, 0,
    3, 0, 2,
};

const struct v3 cube_normals[] =
{
    {-1.0f,  0.0f,  0.0f},
    { 1.0f,  0.0f,  0.0f},
    { 0.0f, -1.0f,  0.0f},

    { 0.0f,  1.0f,  0.0f},
    { 0.0f,  0.0f, -1.0f},
    { 0.0f,  0.0f,  1.0f},
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
        target[index * stride + 0 + offset] = cube_normals[index/6].x;
        target[index * stride + 1 + offset] = cube_normals[index/6].y;
        target[index * stride + 2 + offset] = cube_normals[index/6].z;
    }
}

void
write_cube_uvs(f32 *target, s32 stride, s32 offset)
{
    for (s32 index = 0; index < 36; ++index)
    {
        target[index * stride + 0 + offset] = cube_uvs[index/6].x;
        target[index * stride + 1 + offset] = cube_uvs[index/6].y;
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

/*
void
make_cube_positions(struct v3 offset, float scale, struct v3 *position, s32 stride)
{
    struct v3 *target = position;
    for (int i = 0; i < 36; ++i)
    {
        target[i*stride] = unit_cube_positions[cube_indices[i]] * scale + offset;
    }
}

void
make_plane_vertices(struct v2 position, struct v2 normal, float size, struct v3 *vertices, memory_t *memory)
{
    struct v2 forward = 
    vertices = allocate_array(memory, 6, float);
    vertices[0].position = v3( -size, -size, 0.0f);
    vertices[1].position = v3(  size, -size, 0.0f);
    vertices[2].position = v3( -size,  size, 0.0f);

    vertices[3].position = v3( -size,  size, 0.0f);
    vertices[4].position = v3(  size, -size, 0.0f);
    vertices[5].position = v3(  size,  size, 0.0f);
}


void
make_cube_positions(struct v3 offset, float scale, struct v3 **position, memory_t *memory)
{
    *position = allocate_array(memory, 36, struct v3);
    make_cube_positions(offset, scale, *position);
}


void
make_cube_normals(struct v3 **normal, memory_t *memory)
{
    *normal = allocate_array(memory, 36, struct v3);
    struct v3 *target = *normal;
    for (int i = 0; i < 36; ++i)
    {
        target[i] = cube_normals[i/6];
    }
}
*/

#endif
