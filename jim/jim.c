#ifndef JIM_C_
#define JIM_C_

#include "wings/base/allocators.c"
#include "wings/base/types.c"
#include "wings/base/units.c"

#include <stdlib.h>

#define MAX_COMMAND_RESULT_SIZE (56 * 1024)
#define MAX_STRING_MEMORY_SIZE (56 * 1024)

struct bon_master
{
    u32   string_memory_used;
    u32   string_memory_size;
    char *string_memory;
    char *command_result;
} bon;

struct arguments
{
    char **arguments;
    u32    argument_count;
};

struct command_definition
{
    char *name;
};

typedef error (*command_function)(char **arguments, u32 arguments_length);

struct
{
    u32    argument_count;
    char **arguments;
} jim = { 0 };

error
move_command(char **arguments, u32 arguments_length)
{
	if (arguments_length != 2)
	{
		printf("I only move one thing to another place!\njim move <from> <to>\n");
		return(1);
	}
    MoveFile(arguments[0], arguments[1]);

    return (NO_ERROR);
}

struct command_mapping
{
    const char      *name;
    command_function function;
};

struct command_mappings
{
    u32                     used;
    u32                     size;
    struct command_mapping *array;
} command_mappings = { 0 };

#define EXIT_IF_ERROR()                       \
    do                                        \
    {                                         \
        if (error)                            \
        {                                     \
            printf("Why u no giv memory?\n"); \
            exit(-1);                         \
        }                                     \
    }                                         \
    while (0)

error
make_command_mappings(u32 max_number_of_commands, struct allocator *allocator)
{
    command_mappings.used = 0;
    command_mappings.size = max_number_of_commands;

    error error = allocate_array(&command_mappings.array, allocator, max_number_of_commands, struct command_mapping);
    return (error);
}

error
add_command_mapping(const char *name, command_function function)
{
    if (command_mappings.used + 1 > command_mappings.size)
        return (1);

    u32 new_mapping_index = command_mappings.used++;

    command_mappings.array[new_mapping_index].name     = name;
    command_mappings.array[new_mapping_index].function = function;

    return (0);
}

void
print_commands()
{
    for (u32 index = 0;
         index < command_mappings.used;
         ++index)
    {
        printf("%s\n", command_mappings.array[index].name);
    }
}

void
execute_command(char **arguments, s32 argument_count)
{
    for (u32 index = 0;
         index < command_mappings.used;
         ++index)
    {
        if (strcmp(arguments[1], command_mappings.array[index].name) == 0)
        {
            command_mappings.array[index].function(arguments + 2, argument_count - 2);
            return;
        }
    }
    printf("I don't know how to '%s'.\n", arguments[1]);
}

s32
main(s32 argument_count, char **arguments)
{
    error            error         = NO_ERROR;
    struct allocator static_memory = make_growing_linear_allocator(4096 * 10);

    error = make_command_mappings(100, &static_memory);
    EXIT_IF_ERROR();
    add_command_mapping("move", move_command);

    error = allocate_array(&bon.string_memory, &static_memory, MAX_STRING_MEMORY_SIZE, char);
    EXIT_IF_ERROR();

    error = allocate_array(&bon.command_result, &static_memory, MAX_COMMAND_RESULT_SIZE, char);
    EXIT_IF_ERROR();

    if (argument_count == 1)
    {
        print_commands();
        return (0);
    }
    else if (argument_count >= 2)
    {
        execute_command(arguments, argument_count);
    }
    else
    {
        printf("Jim is very confused. My brain reached unreachable heights\n");
    }
}

#endif
