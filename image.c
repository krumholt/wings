#ifndef image_c
#define image_c

#include "types.h"
#include "wings_math.c"
#include "file.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

struct image
{
    struct v2u  size;
    s32         number_of_components;
    u8         *raw_data;
};

b32
load_image_from_memory(struct image *image, u8 *data, u32 size)
{
    s32 width = 0, height = 0;
    image->raw_data = stbi_load_from_memory(data, size, &width, &height, &image->number_of_components, 0);
    if (!image->raw_data)
        return(1);

    image->size.x = width;
    image->size.y = height;
    return(0);
}

b32
load_image(struct image *image, char *path, struct memory *memory)
{
    u32  size = 0;
    u8  *data = 0;
    b32 error = 0;
    error = load_file(&data, &size, path, 0, memory);
    if (error)
        return 1;
    error = load_image_from_memory(image, data, size);
    if (error)
        return 2;

    return(0);
}

#endif
