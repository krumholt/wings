#ifndef WINGS_BASE_ASSETS_C_
#define WINGS_BASE_ASSETS_C_

#include "experimental_mesh.c"
#include "wings/base/allocators.c"
#include "wings/base/types.c"
#include "wings/base/wings_file_reader.c"
#include "wings/os/file.c"

error
mesh_from_wings_file(struct mesh *mesh, char *filename, struct allocator *allocator)
{
    error         error  = NO_ERROR;
    struct buffer buffer = { 0 };

    error = read_file(&buffer, filename, 0, allocator);
    if (error)
        return (1);

    struct wings_file_parser parser = { 0 };

    error = make_wings_file_parser(&parser, buffer);
    if (error)
        return (2);

    //error = set_to_next_chunk_with_name(&parser, "blender");
    //if (error)
    //    return (3);

    error = set_to_next_chunk_with_name(&parser, "model");
    if (error)
        return (4);

    struct wings_file_mesh_chunk *mesh_chunk = parser.mesh_chunk;

    make_mesh(mesh,
              mesh_attribute_position | mesh_attribute_normal | mesh_attribute_uv | mesh_attribute_color,
              mesh_chunk->number_of_vertices,
              allocator);

    error = set_to_next_chunk_with_name(&parser, "positions");
    if (error)
        return (5);
    mesh->positions = parser.positions_chunk->data;

    error = set_to_next_chunk_with_name(&parser, "normals");
    if (error)
        return (6);
    mesh->normals = parser.normals_chunk->data;

    error = set_to_next_chunk_with_name(&parser, "uvs");
    if (error)
        return (7);
    mesh->uvs = parser.uvs_chunk->data;

    mesh->attributes = mesh_attribute_position | mesh_attribute_normal | mesh_attribute_uv | mesh_attribute_color;

    return (0);
}

#endif
