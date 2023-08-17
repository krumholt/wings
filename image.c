#ifndef image_c
#define image_c

#include "wings/base/types.c"
#include "wings_math.c"
#include "wings/os/file.c"
#define STB_IMAGE_IMPLEMENTATION
#include "wings/extern/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

struct image
{
    struct v2u size;
    s32        number_of_components;
    u8        *raw_data;
};

b32
load_image_from_memory(struct image *image, struct buffer buffer)
{
    s32 width = 0, height = 0;
    image->raw_data = stbi_load_from_memory(buffer.data, buffer.size, &width, &height, &image->number_of_components, 0);
    if (!image->raw_data)
        return (1);

    image->size.x = width;
    image->size.y = height;
    return (0);
}

b32
load_image(struct image *image, char *path, struct allocator *allocator)
{
	struct buffer buffer = {0};
    error error = 0;
    error     = read_file(&buffer, path, 0, allocator);
    if (error)
        return 1;
    error = load_image_from_memory(image, buffer);
    if (error)
        return 2;

    return (0);
}

#endif
