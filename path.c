#ifndef _path_c_
#define _path_c_

#include "types.h"
#include "wings_strings.c"

#ifndef MAX_PATH_SIZE
    #define MAX_PATH_SIZE 1024
#endif

struct path
{
    char   string[MAX_PATH_SIZE];
    u64    used;
};

struct path      make_path        (char *string, u64 size);
void             copy_path        (struct path *target, struct path source);
void             append_path      (struct path *path, char *string);
struct string    base_name        (struct path path);
void             set_to_parent    (struct path *path);

static b32
is_path_seperator(char c)
{
    return c == '\\' || c == '/';
}

struct path
make_path(char *string, u64 size)
{
    assert(size < MAX_PATH_SIZE);

    struct path path = {0};
    path.used = 0;

    if (string)
    {
        while(size)
        {
            path.string[path.used++] = *string;
            string += 1;
            size -= 1;
        }
    }
    path.string[path.used++] = 0;

    return(path);
}

void
copy_path(struct path *target, struct path source)
{
    target->used = source.used;
    for (u32 index = 0; index < MAX_PATH_SIZE; ++index)
    {
        target->string[index] = source.string[index];
    }
}

void
append_path(struct path *path, char *string)
{
    path->used -= 1; // put cursor on 0 terminator
    while(*string)
    {
        assert(path->used + 1 <= MAX_PATH_SIZE);
        path->string[path->used++] = *string;
        string += 1;
    }
    path->string[path->used++] = 0;
}

void
set_to_parent(struct path *path)
{
    int32 last_character = path->used - 1;
    assert(path->string[last_character] == 0);
    last_character -= 1;
    if (is_path_seperator(path->string[last_character]))
        last_character -= 1;
    while(last_character >= 0)
    {
        if(is_path_seperator(path->string[last_character]))
        {
            path->string[last_character + 1] = 0;
            path->used = last_character + 2;
            return;
        }
        last_character -= 1;
    }
}

/*
path_t
copy_append(path_t path, string_t string, memory_t *memory)
{
    path_t new_path = make_path(path.c_string, memory);

    new_path.used -= 1;
    new_path.c_string[new_path.used++] = '\\';
    for (int index = 0; index < string.size; ++index)
    {
        new_path.c_string[new_path.used++] = string.memory[index];
    }
    new_path.c_string[new_path.used++] = 0;
    return(new_path);
}


path_t
copy_append(path_t path, char *c_string, memory_t *memory)
{
    path_t new_path = make_path(path.c_string, memory);

    new_path.used -= 1;
    new_path.c_string[new_path.used++] = '\\';
    while(*c_string)
    {
        new_path.c_string[new_path.used++] = *c_string;
        ++c_string;
    }
    new_path.c_string[new_path.used++] = 0;
    return(new_path);
}
*/
#endif
