#ifndef WINGS_GRAPHICS_EXP_MESH_C_
#define WINGS_GRAPHICS_EXP_MESH_C_

#if !defined(WINGS_BASE_TYPES_C_)
#include "wings/base/types.c"
#endif
#if !defined(WINGS_BASE_ALLOCATORS_C_)
#include "wings/base/allocators.c"
#endif
#if !defined(WINGS_BASE_MATH_C_)
#include "wings/base/math.c"
#endif
#if !defined(WINGS_GRAPHICS_OPENGL_C_)
#include "wings/graphics/opengl.c"
#endif

#define MESH_ATTRIBUTE_POSITION_LOCATION 0
#define MESH_ATTRIBUTE_NORMAL_LOCATION 1
#define MESH_ATTRIBUTE_UV_LOCATION 2
#define MESH_ATTRIBUTE_COLOR_LOCATION 3
#define MESH_ATTRIBUTE_JOINT_ID_LOCATION 4
#define MESH_ATTRIBUTE_JOINT_WEIGHT_LOCATION 5
#define NUMBER_OF_MESH_ATTRIBUTES 6

enum mesh_attribute
{
    mesh_attribute_position     = 1 << 0,
    mesh_attribute_normal       = 1 << 1,
    mesh_attribute_uv           = 1 << 2,
    mesh_attribute_color        = 1 << 3,
    mesh_attribute_joint_id     = 1 << 4,
    mesh_attribute_joint_weight = 1 << 5,
};

struct mesh
{
    u8 *data;
    u64 positions_offset;
    u64 normals_offset;
    u64 uvs_offset;
    u64 colors_offset;
    u64 joint_ids_offset;
    u64 joint_weights_offset;
    u32 attributes;
    u32 used;
    u32 size;
    u64 size_in_bytes;
    u32 vb;
    u32 va;
};

b32
mesh_has_attribute(struct mesh *mesh, enum mesh_attribute attribute)
{
    return (mesh->attributes & attribute);
}

error
make_mesh(struct mesh      *mesh,
          u32               attributes,
          u32               max_number_of_vertices,
          struct allocator *allocator)
{
    error error      = NO_ERROR;
    mesh->attributes = attributes;
    mesh->size       = max_number_of_vertices;

    u64 total_size = 0;
    if (mesh_has_attribute(mesh, mesh_attribute_position))
    {
        u64 size         = sizeof(struct v3) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->positions_offset = total_size;
        total_size += aligned_size;
    }
    if (mesh_has_attribute(mesh, mesh_attribute_normal))
    {
        u64 size         = sizeof(struct v3) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->normals_offset = total_size;
        total_size += aligned_size;
    }
    if (mesh_has_attribute(mesh, mesh_attribute_uv))
    {
        u64 size         = sizeof(struct v2) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->uvs_offset = total_size;
        total_size += aligned_size;
    }
    if (mesh_has_attribute(mesh, mesh_attribute_color))
    {
        u64 size         = sizeof(struct v4) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->colors_offset = total_size;
        total_size += aligned_size;
    }
    if (mesh_has_attribute(mesh, mesh_attribute_joint_id))
    {
        u64 size         = sizeof(struct v4s) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->joint_ids_offset = total_size;
        total_size += aligned_size;
    }
    if (mesh_has_attribute(mesh, mesh_attribute_joint_weight))
    {
        u64 size         = sizeof(struct v4) * mesh->size;
        u64 aligned_size = (size + 7) & ~(7);

        mesh->joint_weights_offset = total_size;
        total_size += aligned_size;
    }

    mesh->size_in_bytes = total_size;
    error               = allocate_array(&mesh->data,
                                         allocator,
                                         mesh->size_in_bytes,
                                         u8);
    if (error)
        return error;

    glGenVertexArrays(1, &mesh->va);
    glBindVertexArray(mesh->va);
    glGenBuffers(1, &mesh->vb);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vb);
    glBufferData(GL_ARRAY_BUFFER,
                 total_size,
                 0,
                 GL_DYNAMIC_DRAW);
    if (mesh_has_attribute(mesh, mesh_attribute_position))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_POSITION_LOCATION);
        glVertexAttribPointer(
            MESH_ATTRIBUTE_POSITION_LOCATION,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)mesh->positions_offset);
    }
    if (mesh_has_attribute(mesh, mesh_attribute_normal))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_NORMAL_LOCATION);
        glVertexAttribPointer(
            MESH_ATTRIBUTE_NORMAL_LOCATION,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)mesh->normals_offset);
    }
    if (mesh_has_attribute(mesh, mesh_attribute_uv))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_UV_LOCATION);
        glVertexAttribPointer(
            MESH_ATTRIBUTE_UV_LOCATION,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)mesh->uvs_offset);
    }
    if (mesh_has_attribute(mesh, mesh_attribute_color))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_COLOR_LOCATION);
        glVertexAttribPointer(
            MESH_ATTRIBUTE_COLOR_LOCATION,
            4,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)mesh->colors_offset);
    }
    if (mesh_has_attribute(mesh, mesh_attribute_joint_id))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_JOINT_ID_LOCATION);
        glVertexAttribIPointer(
            MESH_ATTRIBUTE_JOINT_ID_LOCATION,
            4,
            GL_INT,
            0,
            (void *)mesh->joint_ids_offset);
    }
    if (mesh_has_attribute(mesh, mesh_attribute_joint_weight))
    {
        glEnableVertexAttribArray(MESH_ATTRIBUTE_JOINT_WEIGHT_LOCATION);
        glVertexAttribPointer(
            MESH_ATTRIBUTE_JOINT_WEIGHT_LOCATION,
            4,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void *)mesh->joint_weights_offset);
    }
    glBindVertexArray(0);
    IF_GL_ERROR_RETURN(1);
    return (0);
}

struct v3 *
mesh_positions(struct mesh mesh)
{
    return (struct v3 *)(mesh.data + mesh.positions_offset);
}

struct v3 *
mesh_normals(struct mesh mesh)
{
    return (struct v3 *)(mesh.data + mesh.normals_offset);
}

struct v2 *
mesh_uvs(struct mesh mesh)
{
    return (struct v2 *)(mesh.data + mesh.uvs_offset);
}

struct v4 *
mesh_colors(struct mesh mesh)
{
    return (struct v4 *)(mesh.data + mesh.colors_offset);
}

error
upload_mesh(struct mesh mesh)
{
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.size_in_bytes, mesh.data);
    IF_GL_ERROR_RETURN(1);
    return (NO_ERROR);
}

error
render_mesh(struct mesh mesh)
{
    if (mesh.used == 0)
        return (1);
    glBindVertexArray(mesh.va);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vb);
    glDrawArrays(GL_TRIANGLES, 0, mesh.used);
    glBindVertexArray(0);
    IF_GL_ERROR_RETURN(1);
    return (NO_ERROR);
}

#endif
