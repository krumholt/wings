#ifndef JIM_C_
#define JIM_C_

#include "wings/base/allocators.c"
#include "wings/base/types.c"
#include "wings/base/units.c"
#include "wings/os/file.c"
#include "wings/os/process.c"

#include <stdlib.h>

struct arguments
{
    char **arguments;
    u32    argument_count;
};

struct command_definition
{
    char *name;
};

struct program
{
    char *source;
    char *executable;
    b32   debug;
    char *compile_flags;
};

typedef error (*command_function)(char **arguments, u32 arguments_length);

struct
{
    struct allocator static_memory;
    u32              argument_count;
    char           **arguments;
    error            last_error;
    u32              output_size;
    char            *output;
} jim = { 0 };

error
move(char **arguments, u32 arguments_length)
{
    if (arguments_length != 2)
    {
        printf("I only move one thing to another place!\njim move <from> <to>\n");
        return (1);
    }
    error error = move_file(arguments[0], arguments[1]);
    if (error)
    {
        printf("I tried, but i was too hard....\n");
    }

    return (error);
}

error
copy(char **arguments, u32 arguments_length)
{
    if (arguments_length != 2)
    {
        printf("I only move one thing to another place!\njim move <from> <to>\n");
        return (1);
    }
    error error = copy_file(arguments[0], arguments[1]);
    if (error)
    {
        printf("I tried, but i was too hard....\n");
    }

    return (NO_ERROR);
}

error
time(char **arguments, u32 arguments_length)
{
    if (arguments_length != 1)
    {
        printf("Only one you shall provide...\n");
        return (1);
    }

    u64   the_time = 0;
    error error    = file_get_last_write_time(&the_time, arguments[0]);
    if (error)
    {
        printf("I tried, but i was too hard....\n");
        return (1);
    }
    printf("Sir, the time is %zu\n", the_time);

    return (error);
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

void
jim_please_compile(struct program *program)
{
    char  command_text[1024] = { 0 };
    char *compiler           = "gcc";
    snprintf(command_text, 1024, "%s -DOS_WINDOWS -I . -o %s %s",
             compiler,
             program->executable,
             program->source);
    jim.last_error = run_command(command_text,
                                 (char *)jim.output, jim.output_size);
    if (jim.last_error == process_error_command_not_found)
    {
        printf("Sir, I couldn't find your '%s' compiler\n", compiler);
        printf("I will go and check the garage.");
    }
    if (jim.last_error)
    {
        printf("%s\n", jim.output);
    }
}

void
jim_please_run(struct program *program)
{
    char command_text[1024] = { 0 };
    snprintf(command_text, 1024, "./%s",
             program->executable);
    jim.last_error = run_command(command_text,
                                 jim.output, jim.output_size);
}

error
we_failed_jim()
{
    return jim.last_error;
}

void
jim_what_was_the_output()
{
    printf("%s\n", jim.output);
}

error
call_jim()
{
    jim.static_memory = make_growing_linear_allocator(mebibytes(1));

    jim.output_size = 1024 * 1024;
    error error     = allocate_array(&jim.output, &jim.static_memory, jim.output_size, char);
    return (error);
}

#if !defined(JIMS_BRAIN)
s32
main(s32 argument_count, char **arguments)
{
    error            error         = NO_ERROR;
    struct allocator static_memory = make_growing_linear_allocator(4096 * 10);

    jim.output_size = mebibytes(1);
    error           = allocate_array(&jim.output, &static_memory, jim.output_size, char);
    EXIT_IF_ERROR();

    error = make_command_mappings(100, &static_memory);
    EXIT_IF_ERROR();
    add_command_mapping("move", move);
    EXIT_IF_ERROR();
    add_command_mapping("copy", copy);
    EXIT_IF_ERROR();
    add_command_mapping("time", time);
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

#endif
