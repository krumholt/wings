#ifndef WINGS_BASE_WINGS_FILE_READER_C_
#define WINGS_BASE_WINGS_FILE_READER_C_

#include "wings/base/types.c"
#include "wings/base/allocators.c"

struct wings_file_header
{
    char id[8]; // "WINGS\0\0\0"
    u32  major_version; // 1
    u32  minor_version; // 0
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

struct wings_file_chunk
{
    char id[16];
    u64  size;
    u64  number_of_child_blocks;
    u8   data[];
};

struct wings_file_parser
{
    union
    {
        u8                      *current;
        struct wings_file_chunk *chunk;
    };
    u8 *end;
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
    file_parser->current = buffer.base + sizeof(struct wings_file_header);
    file_parser->end     = buffer.base + buffer.size;
    return (0);
}

b32
wings_file_id_equals(struct wings_file_chunk *chunk, const char *id)
{

    char *tmp = chunk->id;
    while (*id++ && *tmp++)
    {
        if (*id != *tmp)
            return (0);
    }
    return (1);
}

error
set_to_next_chunk(struct wings_file_parser *parser)
{
    if (parser->current >= parser->end)
        return (1);
    struct wings_file_chunk *chunk = parser->chunk;
    parser->current                = parser->current + chunk->size;
    return (0);
}

error
set_to_next_chunk_with_id(struct wings_file_parser *parser, const char *id)
{
    while (set_to_next_chunk(parser) == NO_ERROR)
    {
        if (wings_file_id_equals(parser->chunk, id))
        {
            return (0);
        }
    }
    return (1);
}

#endif
