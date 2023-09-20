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

struct wings_file_chunk_header
{
    char name[16];
    u64  chunk_size;
    u32  id;
    u32  parent_id;
    u8   data[];
};

struct wings_file_blender_chunk
{
    u64 number_of_models;
    u64 creation_time;
    u32 blend_file_name;
    u32 blender_version; // offset into strings chunk
};

struct wings_file_model_chunk
{
    u32 number_of_meshes;
    u32 name; // offset into strings chunk
};

struct wings_file_mesh_chunk
{
    u32 number_of_vertices;
    u32 texture_file_name; // offset into strings chunk
    u8  has_positions;
    u8  has_normals;
    u8  has_uvs;
    u8  has_colors;
    u8  has_joint_ids;
    u8  has_joint_weights;
    u8  padding[2];
};

struct wings_file_parser
{
    union
    {
        u8                             *current;
        struct wings_file_chunk_header *chunk_header;
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
    file_parser->end     = buffer.base + buffer.used;
    return (0);
}

b32
wings_file_chunk_name_equals(struct wings_file_chunk_header *chunk, const char *name)
{
    char *tmp = chunk->name;
    while (*name++ && *tmp++)
    {
        if (*name != *tmp)
            return (0);
    }
    return (*name == 0 && *tmp == 0);
}

b32
wings_file_parser_at_end(struct wings_file_parser *parser)
{
    return (parser->current == parser->end);
}

error
set_to_next_chunk(struct wings_file_parser *parser)
{
    if (parser->current >= parser->end)
        return (1);
    struct wings_file_chunk_header *header = parser->chunk_header;
    ASSERT(header->chunk_size > 0);
    parser->current = parser->current + header->chunk_size + sizeof(struct wings_file_chunk_header);
    return (0);
}

error
set_to_next_chunk_with_name(struct wings_file_parser *parser, const char *name)
{
    if (wings_file_parser_at_end(parser))
        return (1);
    do
    {
        if (wings_file_chunk_name_equals(parser->chunk_header, name))
        {
            return (0);
        }
    } while (set_to_next_chunk(parser) == NO_ERROR);
    printf("failed here\n");
    return (1);
}

#endif
