#ifndef WINGS_BASE_ASSETS_C_
#define WINGS_BASE_ASSETS_C_

#include "wings/base/types.c"
#include "wings/os/file.c"
#include "wings/graphics/mesh.c"
#include "wings/base/allocators.c"
#include "wings/base/wings_file_reader.c"

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

    error = set_to_next_chunk_with_name(&parser, "blender");
	if (error)
		return (3);

    error = set_to_next_chunk_with_name(&parser, "model");
    if (error)
        return (4);

    return (0);
}

#endif
