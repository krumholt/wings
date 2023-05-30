#ifndef mesh_c
#define mesh_c
#include "types.h"
#include "wings_math.c"
#include "opengl.c"
#include "mesh_tools.c"

struct vertex_pnc
{
    struct v3 position;
    struct v3 normal;
    struct v4 color;
};

struct mesh_pnc
{
    struct vertex_pnc *vertices;
    u32  size;
    u32  used;

    u32  va;
    u32  vb;
    b32  changed;
};

struct mesh_pnc
make_mesh_pnc(struct memory *memory, s32 size)
{
    struct mesh_pnc mesh = {0};
    mesh.size   = size;
    mesh.vertices = allocate_array(memory,
                                   size,
                                   struct vertex_pnc);

    glGenVertexArrays(1, &mesh.va);
    glBindVertexArray(mesh.va);
    glGenBuffers(1, &mesh.vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct vertex_pnc),
                          (void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct vertex_pnc),
                          (void *)(offsetof(struct vertex_pnc, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
                          sizeof(struct vertex_pnc),
                          (void *)(offsetof(struct vertex_pnc, color)));

    glBufferData(GL_ARRAY_BUFFER,
                 mesh.size * (s64)sizeof(struct vertex_pnc),
                 0,
                 GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    return(mesh);
}

void
update_mesh_pnc(struct mesh_pnc mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.used * (s64)sizeof(struct vertex_pnc), mesh.vertices);
    mesh.changed = 0;
}

void
render_mesh_pnc(struct mesh_pnc mesh)
{
    if (mesh.used == 0) return;
    glBindVertexArray(mesh.va);
    if (mesh.changed)
    {
        update_mesh_pnc(mesh);
    }

    glDrawArrays(GL_TRIANGLES, 0, mesh.used);
    glBindVertexArray(0);
}

void
clear_mesh_pnc(struct mesh_pnc *mesh)
{
    mesh->used = 0;
}

void
push_sphere_pnc(struct mesh_pnc *mesh,
                struct v3 position,
                f32 radius,
                struct v4 color)
{
    mesh->changed = 1;

    const s32 number_of_vertical_slices = 16;
    const s32 number_of_horizontal_slices = 16;
    const s32 number_of_vertices = number_of_vertical_slices * number_of_horizontal_slices * 3 * 2; // two triangles per horizontal slice
    if (mesh->used + number_of_vertices > mesh->size)
    {
        static s32 frame_counter = 0;
        frame_counter++;
        if (frame_counter > 180)
        {
            frame_counter = 0;
            printf("[WARNING]: Mesh overflow\n");
        }
        return;
    }

    s32 index = mesh->used;
    for (int altitude_index = 0;
         altitude_index < number_of_horizontal_slices;
         ++altitude_index)
    {
        for (int azimuth_index = 0;
             azimuth_index < number_of_vertical_slices;
             ++azimuth_index)
        {
            f32 altitude_1 = to_radians(180.0f + (f32)(altitude_index) * 180.0f / (f32)(number_of_horizontal_slices));
            f32 altitude_2 = to_radians(180.0f + (f32)(altitude_index + 1) * 180.0f / (f32)(number_of_horizontal_slices));
            f32 azimuth_1  = to_radians((f32)(azimuth_index) * 360.0f / (f32)(number_of_vertical_slices));
            f32 azimuth_2  = to_radians((f32)(azimuth_index + 1) * 360.0f / (f32)(number_of_vertical_slices));
            struct v3 point_1 = calculate_point_on_sphere(azimuth_1, altitude_1);
            struct v3 point_2 = calculate_point_on_sphere(azimuth_2, altitude_1);
            struct v3 point_3 = calculate_point_on_sphere(azimuth_1, altitude_2);
            struct v3 point_4 = calculate_point_on_sphere(azimuth_2, altitude_2);

            mesh->vertices[index].position =
                add_v3(mul_f32_v3(radius, point_1),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_1);
            mesh->vertices[index].color    = color;
            ++index;

            mesh->vertices[index].position = 
                add_v3(mul_f32_v3(radius, point_3),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_3);
            mesh->vertices[index].color    = color;
            ++index;

            mesh->vertices[index].position =
                add_v3(mul_f32_v3(radius, point_2),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_2);
            mesh->vertices[index].color    = color;
            ++index;

            mesh->vertices[index].position =
                add_v3(mul_f32_v3(radius, point_3),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_3);
            mesh->vertices[index].color    = color;
            ++index;

            mesh->vertices[index].position =
                add_v3(mul_f32_v3(radius, point_4),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_4);
            mesh->vertices[index].color    = color;
            ++index;

            mesh->vertices[index].position =
                add_v3(mul_f32_v3(radius, point_2),
                       position);
            mesh->vertices[index].normal   = normalize_v3(point_2);
            mesh->vertices[index].color    = color;
            ++index;
        }
    }
    mesh->used = index;
}

void
push_line_segment_pnc(
    struct mesh_pnc *mesh,
    struct v3 start,
    struct v3 end,
    f32 thickness,
    struct v4 color)
{
    if (mesh->used + 36 > mesh->size)
    {
        static s32 frame_counter = 0;
        frame_counter++;
        if (frame_counter > 180)
        {
            frame_counter = 0;
            printf("[WARNING]: Mesh overflow\n");
        }
        return;
    }
    mesh->changed = 1;
    struct v3 up      = make_v3(0.0f, 0.0f, 1.0f);
    struct v3 forward = normalize_v3(sub_v3(end, start));
    if (almost_zero_f32(fabsf(dot_v3(forward, up)) - 1.0f, 0.00001f))
    {
        up = make_v3(-1.0f, 0.0f, 0.0f);
    }
    struct v3 right = normalize_v3(cross_v3(forward, up));
    up = normalize_v3(cross_v3(right, forward));

    struct v3 left = mul_f32_v3(-1.0f, right);
	struct v3 down = mul_f32_v3(-1.0f, up);

    struct v3 cube_normal[] =
    {
        left,
        right,
        mul_f32_v3(-1.0f, forward),
        forward,
        down,
        up
    };

    right = mul_f32_v3(thickness, right);
    up    = mul_f32_v3(thickness, up);
    left  = mul_f32_v3(thickness, left);
    down  = mul_f32_v3(thickness, down);


    struct v3 cube_vertex[8] =
    {
        add_v3(add_v3(start, left),  up),
        add_v3(add_v3(start, right), up),
        add_v3(add_v3(start, left),  down),
        add_v3(add_v3(start, right), down),

        add_v3(add_v3(end, left),  up),
        add_v3(add_v3(end, right), up),
        add_v3(add_v3(end, left),  down),
        add_v3(add_v3(end, right), down),
    };

    const s32 cube_index[] =
    {
        // -X
        4, 6, 0,
        2, 0, 6,
        // +X
        1, 3, 5,
        7, 5, 3,
        // -Y
        0, 2, 1,
        3, 1, 2,
        // +Y
        5, 7, 4,
        6, 4, 7,
        // -Z
        2, 6, 3,
        7, 3, 6,
        // +Z
        4, 0, 5,
        1, 5, 0,
    };
    for (int index = 0; index < 36; ++index)
    {
        mesh->vertices[mesh->used + index].position = cube_vertex[cube_index[index]];
        mesh->vertices[mesh->used + index].normal   = cube_normal[index/6];
        mesh->vertices[mesh->used + index].color    = color;
    }
    mesh->used +=36;
}

void
push_quad_pnc(struct mesh_pnc *mesh,
              struct v3 position,
              struct v3 right,
              struct v3 up,
              struct v4 color)
{
    if (mesh->used + 6 > mesh->size)
    {
        printf("[WARNING]: Mesh overflow in push_quad\n");
        return;
    }
    mesh->changed = 1;

    struct v3 left = mul_f32_v3(-1.0f, right);
    struct v3 down = mul_f32_v3(-1.0f, up);

    struct v3 normal = cross_v3(right, up);
    struct v3 top_left     = add_v3(position, add_v3(up, left));
    struct v3 top_right    = add_v3(position, add_v3(up, right));
    struct v3 bottom_left  = add_v3(position, add_v3(down, left));
    struct v3 bottom_right = add_v3(position, add_v3(down, right));

    s32 index = 0;
    index = mesh->used++;
    mesh->vertices[index].position = top_left;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
    index = mesh->used++;
    mesh->vertices[index].position = bottom_left;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
    index = mesh->used++;
    mesh->vertices[index].position = top_right;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
    index = mesh->used++;
    mesh->vertices[index].position = bottom_right;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
    index = mesh->used++;
    mesh->vertices[index].position = top_right;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
    index = mesh->used++;
    mesh->vertices[index].position = bottom_left;
    mesh->vertices[index].normal   = normal;
    mesh->vertices[index].color    = color;
}

void
push_aab3_pnc(struct mesh_pnc *mesh,
          struct aab3 box,
          struct v4 color)
{
    if (mesh->used + 36 > mesh->size)
    {
        printf("[WARNING]: Mesh overflow\n");
        return;
    }
    mesh->changed = 1;

    struct v3 half_extend = mul_f32_v3(0.5f, sub_v3(box.max, box.min));
    struct v3 center = add_v3(half_extend, box.min);
    for (int index = 0; index < 36; ++index)
    {
        struct v3 position = unit_cube_positions[cube_indices[index]];
        mesh->vertices[mesh->used + index].position = make_v3(
                position.x * half_extend.x + center.x,
                position.y * half_extend.y + center.y,
                position.z * half_extend.z + center.z
                );
        mesh->vertices[mesh->used + index].normal   = cube_normals[index/6];
        mesh->vertices[mesh->used + index].color    = color;
    }
    mesh->used += 36;
}

void
push_aab3_outline_pnc(struct mesh_pnc *mesh, struct aab3 box, f32 thickness, struct v4 color)
{
    if (mesh->used + (36 * 12) > mesh->size)
    {
        static s32 frame_counter = 0;
        frame_counter++;
        if (frame_counter > 180)
        {
            frame_counter = 0;
            printf("[WARNING]: Mesh overflow\n");
        }
        return;
    }
    struct v3 a = make_v3(box.min.x, box.min.y, box.min.z);
    struct v3 b = make_v3(box.max.x, box.min.y, box.min.z);
    struct v3 c = make_v3(box.max.x, box.min.y, box.max.z);
    struct v3 d = make_v3(box.min.x, box.min.y, box.max.z);

    struct v3 e = make_v3(box.min.x, box.max.y, box.min.z);
    struct v3 f = make_v3(box.max.x, box.max.y, box.min.z);
    struct v3 g = make_v3(box.max.x, box.max.y, box.max.z);
    struct v3 h = make_v3(box.min.x, box.max.y, box.max.z);

    push_line_segment_pnc(mesh, a, b, thickness, color);
    push_line_segment_pnc(mesh, b, c, thickness, color);
    push_line_segment_pnc(mesh, c, d, thickness, color);
    push_line_segment_pnc(mesh, d, a, thickness, color);

    push_line_segment_pnc(mesh, e, f, thickness, color);
    push_line_segment_pnc(mesh, f, g, thickness, color);
    push_line_segment_pnc(mesh, g, h, thickness, color);
    push_line_segment_pnc(mesh, h, e, thickness, color);

    push_line_segment_pnc(mesh, a, e, thickness, color);
    push_line_segment_pnc(mesh, b, f, thickness, color);
    push_line_segment_pnc(mesh, c, g, thickness, color);
    push_line_segment_pnc(mesh, d, h, thickness, color);
}

#endif
