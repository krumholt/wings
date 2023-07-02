#ifndef mesh_c
#define mesh_c
#include "memory.c"
#include "mesh_tools.c"
#include "opengl.c"
#include "types.h"
#include "wings_math.c"

struct vertex_p3c4
{
    struct v3 position;
    struct v4 color;
};

struct mesh_p3c4
{
    struct vertex_p3c4 *vertices;

    u32 used;
    u32 size;

    u32 va;
    u32 vb;
};

struct mesh_p3c4
make_mesh_p3c4(struct memory *memory, s32 number_of_vertices)
{
    struct mesh_p3c4 mesh = { 0 };
    mesh.size             = number_of_vertices;

    mesh.vertices = allocate_array(memory,
                                   number_of_vertices,
                                   struct vertex_p3c4);

    glGenVertexArrays(1, &mesh.va);
    glBindVertexArray(mesh.va);
    glGenBuffers(1, &mesh.vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(struct vertex_p3c4),
                          (void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(struct vertex_p3c4),
                          (void *)(offsetof(struct vertex_p3c4, color)));

    glBufferData(GL_ARRAY_BUFFER,
                 mesh.size * (s64)sizeof(struct vertex_p3c4),
                 0,
                 GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    return (mesh);
}

void
update_mesh_p3c4(struct mesh_p3c4 mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.used * (s64)sizeof(struct vertex_p3c4), mesh.vertices);
}

void
render_mesh_p3c4(struct mesh_p3c4 mesh)
{
    if (mesh.used == 0)
        return;
    glBindVertexArray(mesh.va);

    glDrawArrays(GL_TRIANGLES, 0, mesh.used);
    glBindVertexArray(0);
}

void
clear_mesh_p3c4(struct mesh_p3c4 *mesh)
{
    mesh->used = 0;
}

void
mesh_p3c4_push_triangle_v3(struct mesh_p3c4 *mesh,
                           struct v3 a, struct v3 b, struct v3 c,
                           struct v4 color)
{
    if (mesh->used + 3 > mesh->size)
    {
        printf("Warning: Not enough space (%d, %d) in mesh_p3c4 to push_triangle()\n", mesh->used, mesh->size);
        return;
    }

    struct vertex_p3c4 vertex    = { 0 };
    vertex.position              = a;
    vertex.color                 = color;
    mesh->vertices[mesh->used++] = vertex;
    vertex.position              = b;
    vertex.color                 = color;
    mesh->vertices[mesh->used++] = vertex;
    vertex.position              = c;
    vertex.color                 = color;
    mesh->vertices[mesh->used++] = vertex;
}

void
mesh_p3c4_push_aab2(struct mesh_p3c4 *mesh,
                    struct aab2       aab,
                    f32               layer,
                    struct v4         color)
{
    struct v3 a = { aab.min.x, aab.min.y, layer };
    struct v3 b = { aab.min.x, aab.max.y, layer };
    struct v3 c = { aab.max.x, aab.max.y, layer };
    struct v3 d = { aab.max.x, aab.min.y, layer };

    mesh_p3c4_push_triangle_v3(mesh, a, b, d, color);
    mesh_p3c4_push_triangle_v3(mesh, c, d, b, color);
}

void
mesh_p3c4_push_line_v2(struct mesh_p3c4 *mesh,
                       struct v2         from,
                       struct v2         to,
                       f32               layer,
                       f32               thickness,
                       struct v4         color)
{
    if (mesh->used + 6 > mesh->size)
    {
        printf("Warning: Not enough space (%d, %d) in mesh to push_line()\n", mesh->used, mesh->size);
        return;
    }
    if (from.x == to.x && from.y == to.y)
    {
        return;
    }
    struct v2 to_minus_from = sub_v2(to, from);
    struct v2 left      = normalize_v2(rotate_cw_90_v2(to_minus_from));
    left = mul_f32_v2(thickness * 0.5f, left);

    struct v3 top_left     = {from.x + left.x, from.y + left.y, layer};
    struct v3 top_right    = {from.x + left.x + to_minus_from.x, from.y + left.y + to_minus_from.y, layer};
    struct v3 bottom_left  = {from.x - left.x, from.y - left.y, layer};
    struct v3 bottom_right = {from.x - left.x + to_minus_from.x, from.y - left.y + to_minus_from.y, layer};

    struct vertex_p3c4 vertex = {0};
    vertex.color = color;

    int index = 0;
    index = mesh->used++;
    vertex.position = top_left;
    mesh->vertices[index] = vertex;

    index = mesh->used++;
    vertex.position = bottom_left;
    mesh->vertices[index] = vertex;

    index = mesh->used++;
    vertex.position = top_right;
    mesh->vertices[index] = vertex;

    index = mesh->used++;
    vertex.position = bottom_right;
    mesh->vertices[index] = vertex;

    index = mesh->used++;
    vertex.position = top_right;
    mesh->vertices[index] = vertex;

    index = mesh->used++;
    vertex.position = bottom_left;
    mesh->vertices[index] = vertex;
}

void
mesh_p3c4_push_aab2_outline(struct mesh_p3c4 *mesh,
                            struct aab2       outline,
                            f32               layer,
                            f32               thickness,
                            struct v4         color)
{
    f32 half_thickness = 0.5f * thickness;
    struct v2 a = outline.min;
    struct v2 b = {outline.max.x, outline.min.y};
    struct v2 c = outline.max;
    struct v2 d = {outline.min.x, outline.max.y};
    a.x -= half_thickness;
    b.x += half_thickness;
    mesh_p3c4_push_line_v2(mesh, a, b, layer, thickness, color);
    a.x += half_thickness;
    b.x -= half_thickness;
    c.x += half_thickness;
    d.x -= half_thickness;
    mesh_p3c4_push_line_v2(mesh, c, d, layer, thickness, color);
    c.x -= half_thickness;
    d.x += half_thickness;
    mesh_p3c4_push_line_v2(mesh, b, c, layer, thickness, color);
    mesh_p3c4_push_line_v2(mesh, d, a, layer, thickness, color);
}
#endif
