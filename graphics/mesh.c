#ifndef WINGS_GRAPHICS_MESH_C_
#define WINGS_GRAPHICS_MESH_C_

#include "wings/base/allocators.c"
#include "wings/base/math.c"
#include "wings/base/types.c"
#include "wings/graphics/opengl.c"

enum mesh_attributes
{
    mesh_attribute_positions     = 1 << 0,
    mesh_attribute_normals       = 1 << 1,
    mesh_attribute_uvs           = 1 << 2,
    mesh_attribute_colors        = 1 << 3,
    mesh_attribute_joint_ids     = 1 << 4,
    mesh_attribute_joint_weights = 1 << 5,
};

struct mesh
{
    struct v3  *positions;
    struct v3  *normals;
    struct v2  *uvs_1;
    struct v2  *uvs_2;
    struct v4  *colors;
    struct v4s *joint_ids;
    struct v4  *joint_weights;

    u32 attributes;

    u32 used;
    u32 size;

    u32 vb;
    u32 va;
};

error
make_mesh(struct mesh         *mesh,
          enum mesh_attributes attributes,
          u32                  max_number_of_vertices,
          struct allocator    *allocator)
{
    mesh->attributes = attributes;
    mesh->size       = max_number_of_vertices;
    error error      = NO_ERROR;

    struct
    {
        enum mesh_attributes attribute;
        u8                 **pointer;
        u64                  size;
        u8                   number_of_components;
    } buffers_to_create[] = {
        {mesh_attribute_positions,
         (u8 **)&mesh->positions,
         sizeof(struct v3),
         3},
        { mesh_attribute_normals,
         (u8 **)&mesh->normals,
         sizeof(struct v3),
         3},
        { mesh_attribute_uvs_1,
         (u8 **)&mesh->uvs_1,
         sizeof(struct v2),
         2},
        { mesh_attribute_uvs_2,
         (u8 **)&mesh->uvs_2,
         sizeof(struct v2),
         2},
        { mesh_attribute_colors,
         (u8 **)&mesh->colors,
         sizeof(struct v4),
         4},
        { mesh_attribute_joint_ids,
         (u8 **)&mesh->joint_ids,
         sizeof(struct v4s),
         4},
        { mesh_attribute_joint_weights,
         (u8 **)&mesh->joint_weights,
         sizeof(struct v4),
         4},
    };

    u64 total_size = 0;
    for (u32 index = 0;
         index < ARRAY_LENGTH(buffers_to_create);
         ++index)
    {
        total_size += (attributes & buffers_to_create[index].attribute) * buffers_to_create[index].size * mesh->size;
    }

    glGenVertexArrays(1, &mesh->va);
    glBindVertexArray(mesh->va);
    glGenBuffers(1, &mesh->vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
    glBufferData(GL_ARRAY_BUFFER,
                 total_size,
                 0,
                 GL_DYNAMIC_DRAW);

    u64 offset = 0;
    for (u32 index = 0;
         index < ARRAY_LENGTH(buffers_to_create);
         ++index)
    {
        if (attributes & buffers_to_create[index].attribute)
        {
            error = allocate(buffers_to_create[index].pointer,
                             allocator,
                             max_number_of_vertices * buffers_to_create[index].size);
            if (error)
                return error;
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                buffers_to_create[index].number_of_components,
                GL_FLOAT,
                GL_FALSE,
                0,
                (void *)offset);
            offset += buffers_to_create[index].size * max_number_of_vertices;
        }
    }
    glBindVertexArray(0);

    return (0);
}

error
upload_mesh(struct mesh mesh)
{
    struct
    {
        enum mesh_attributes attribute;
        u8                 **pointer;
        u64                  size;
        u8                   number_of_components;
    } buffers_to_create[] = {
        {mesh_attribute_positions, (u8 **)&mesh.positions, sizeof(struct v3), 3},
        { mesh_attribute_normals,  (u8 **)&mesh.normals,   sizeof(struct v3), 3},
        { mesh_attribute_uvs_1,    (u8 **)&mesh.uvs_1,     sizeof(struct v2), 2},
        { mesh_attribute_uvs_2,    (u8 **)&mesh.uvs_2,     sizeof(struct v2), 2},
        { mesh_attribute_colors,   (u8 **)&mesh.colors,    sizeof(struct v4), 4},
    };

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    u64 offset = 0;
    for (u32 index = 0;
         index < ARRAY_LENGTH(buffers_to_create);
         ++index)
    {
        if (mesh.attributes & buffers_to_create[index].attribute)
        {
            u64 size = buffers_to_create[index].size * mesh.size;
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, *buffers_to_create[index].pointer);
            offset += size;
        }
    }
    GL_CHECK(1);
    return (NO_ERROR);
}

void
render_mesh(struct mesh mesh)
{
    if (mesh.used == 0)
        return;
    glBindVertexArray(mesh.va);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    glDrawArrays(GL_TRIANGLES, 0, mesh.used);
    glBindVertexArray(0);
}

#endif
