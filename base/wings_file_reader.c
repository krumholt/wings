#ifndef WINGS_BASE_WINGS_FILE_READER_C_
#define WINGS_BASE_WINGS_FILE_READER_C_

#include "wings/base/types.c"
#include "wings/base/allocators.c"

struct wings_file_parser
{
    struct buffer buffer;
};

struct wings_file_header
{
    char id[8]; // "WINGS\0\0\0"
    u32  major_version; // 1
    u32  minor_version; // 0
    u8   data[];
};

struct wings_file_chunk
{
    char id[16];
    u64  size;
    u64  number_of_child_blocks;
    u8   data[];
};

struct wings_file_model_chunk
{
    u32 number_of_meshes;
    u32 name;
    u8  data[];
};

struct wings_file_blender_chunk
{
    u64 number_of_models;
    u64 creation_time;
    u32 blend_file_name;
    u32 blender_version;
};

error
make_wings_file_parser(struct wings_file_parser *file_parser, struct buffer buffer)
{
    struct wings_file_header *header = (struct wings_file_header *)buffer.base;
    if (header->id[0] != 'W'
        || header->id[1] != 'I'
        || header->id[2] != 'N'
        || header->id[3] != 'G'
        || header->id[4] != 'S'
        || header->id[5] != 0
        || header->id[6] != 0
        || header->id[7] != 0)
        return (1);
    if (header->major_version != 1 || header->minor_version != 0)
        return (2);
    file_parser->buffer = buffer;
    return (0);
}

error
set_to_next_model_chunk(struct wings_file_parser parser)
{
}

struct wings_file_model_chunk *
get_next_model_chunk(struct wings_file *file)
{
    u8 *file_pointer = file->data;
    for (struct wings_file_chunk *chunk = (struct wings_file_chunk *)file_pointer;
         (u64)(file_pointer - file->data.base) < file->data.size;
         file_pointer += chunk->size)
    {
    }
    printf("%s\n", file->id);
    printf("version %d.%d\n", file->major_version, file->minor_version);

    char                          *strings     = 0;
    struct wings_file_model_chunk *model_chunk = 0;
    chunk                                      = (struct wings_file_chunk *)file_pointer;
    if (wings_file_id_equals(chunk, "blender"))
    {
        struct wings_file_blender_chunk *blender_chunk
            = (struct wings_file_blender_chunk *)chunk->data;
        char      tmp[32] = { 0 };
        struct tm t       = { 0 };
        time_t    in      = blender_chunk->creation_time;
        localtime_s(&t, &in);
        strftime(tmp, 32, "%H:%M %d.%m.%Y", &t);
        printf("created at %s with %zu models from file %s with blender version %s\n",
               tmp,
               blender_chunk->number_of_models,
               strings + blender_chunk->blend_file_name,
               strings + blender_chunk->blender_version);
    }
    else if (wings_file_id_equals(chunk, "strings"))
    {
        strings = (char *)(file_pointer + sizeof(struct wings_file_chunk));
    }
    else if (wings_file_id_equals(chunk, "model"))
    {
        model_chunk = (struct wings_file_model_chunk *)(file_pointer + sizeof(struct wings_file_chunk));
    }
    else if (wings_file_id_equals(chunk, "mesh"))
    {
        printf("mesh found\n");
    }
    printf("chunk: %.16s\n", chunk->id);
    printf("size: %zu\n", chunk->size);
    printf("children: %zu\n", chunk->number_of_child_blocks);
}

printf("model chunk:\n");
printf("name: %s\n", strings + model_chunk->name);
printf("meshes: %d\n", model_chunk->number_of_meshes);

return (0);
}

#endif
