#ifndef WINGS_GRAPHICS_IMAGE_C_
#define WINGS_GRAPHICS_IMAGE_C_

#if !defined(WINGS_BASE_TYPES_C_)
#include "wings/base/types.c"
#endif
#if !defined(WINGS_OS_FILE_C_)
#include "wings/os/file.c"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "wings/extern/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

struct image
{
    u32 width;
    u32 height;
    s32 number_of_components;
    u8 *raw_data;
};

error
load_image_from_memory(struct image *image, struct buffer buffer)
{
    s32 width = 0, height = 0;
	//stbi_set_flip_vertically_on_load(1);
    image->raw_data = stbi_load_from_memory(buffer.base, buffer.size, &width, &height, &image->number_of_components, 0);
    if (!image->raw_data)
        return (1);

    image->width  = width;
    image->height = height;
    return (0);
}

error
load_image(struct image *image, char *path, struct allocator *allocator)
{
    error         error  = 0;
    struct buffer buffer = { 0 };

    error = read_file(&buffer, path, 0, allocator);
    if (error)
        return 1;

    error = load_image_from_memory(image, buffer);
    if (error)
        return 2;

    return (0);
}

#endif
