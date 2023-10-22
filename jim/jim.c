#ifndef JIM_C_
#define JIM_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"
#include "wings/base/units.c"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"
#include "wings/os/windows/process.c"
#include "wings/base/strings.c"

#include <stdio.h>

#define JIM_MAX_INCLUDE_DIRECTORIES 100
#define JIM_MAX_LIBRARY_DIRECTORIES 100
#define JIM_MAX_LIBRARIES 100

enum jim_output_type
{
    jim_output_type__executable,
    jim_output_type__object_file,
};

struct jim_compiler
{
    char *command;
    char *debug_flags;
    char *output_format;
    char *add_library_search_path_format;
    char *add_link_library_format;
    char *add_include_directory_format;
    char *compile_no_link;
    char *flags;
};

struct jim_compilation_parameters
{
    struct jim_compiler  compiler;
    enum jim_output_type type;
    char                *input_file;
    char                *output_file;
};

struct jim_compilation
{
    struct jim_compiler  compiler;
    u32                  command_length;
    struct string        command;
    enum jim_output_type output_type;
    char                *output_file;
    char                *input_file;
    u32                  number_of_library_directories;
    char               **library_directories;
    u32                  number_of_libraries;
    char               **libraries;
    u32                  number_of_include_directories;
    char               **include_directories;
};

struct jim
{
    struct allocator allocator;
} _jim;

void
jim_please_listen(void)
{
    _jim.allocator = make_growing_linear_allocator(mebibytes(1));
}

error
jim_make_compilation(struct jim_compilation *compilation,
                     struct jim_compiler     compiler,
                     struct allocator       *allocator)
{
    error error = ec__no_error;

    compilation->command_length = 0;
    compilation->compiler       = compiler;

    error = make_string(&compilation->command, 4096 * 10, allocator);
    IF_ERROR_RETURN(error);
    error = allocate_array(&compilation->library_directories, allocator, JIM_MAX_LIBRARY_DIRECTORIES, char *);
    IF_ERROR_RETURN(error);
    error = allocate_array(&compilation->libraries, allocator, JIM_MAX_LIBRARIES, char *);
    IF_ERROR_RETURN(error);
    error = allocate_array(&compilation->include_directories, allocator, JIM_MAX_INCLUDE_DIRECTORIES, char *);
    IF_ERROR_RETURN(error);

    return (error);
}

struct jim_compilation
jim_please_make_compilation(struct jim_compilation_parameters parameters)
{
    error                  error       = 0;
    struct jim_compilation compilation = { 0 };

    error = jim_make_compilation(
        &compilation,
        parameters.compiler,
        &_jim.allocator);

    compilation.output_type = parameters.type;
    compilation.input_file  = parameters.input_file;
    compilation.output_file = parameters.output_file;
    return (compilation);
}

void
_jim_command_append(struct jim_compilation *compilation, char *format, ...)
{
    va_list arg_list;
    va_start(arg_list, format);
    compilation->command_length
        += vsnprintf(compilation->command.first
                         + compilation->command_length,
                     compilation->command.length
                         - compilation->command_length
                         - 1,
                     format,
                     arg_list);
    va_end(arg_list);
}

struct string
jim_make_command(struct jim_compilation compilation)
{
    _jim_command_append(&compilation, "gcc ");
    if (compilation.output_type == jim_output_type__object_file)
    {
        _jim_command_append(&compilation, "-c ");
    }

    for (u32 index = 0;
         index < compilation.number_of_include_directories;
         ++index)
    {
        _jim_command_append(&compilation,
                            compilation.compiler.add_include_directory_format,
                            compilation.include_directories[index]);
    }

    for (u32 index = 0;
         index < compilation.number_of_library_directories;
         ++index)
    {
        _jim_command_append(&compilation,
                            compilation.compiler.add_library_search_path_format,
                            compilation.library_directories[index]);
    }

    _jim_command_append(&compilation,
                        compilation.compiler.output_format,
                        compilation.output_file);

    _jim_command_append(&compilation,
                        "%s ",
                        compilation.input_file);

    for (u32 index = 0;
         index < compilation.number_of_libraries;
         ++index)
    {
        _jim_command_append(&compilation,
                            compilation.compiler.add_link_library_format,
                            compilation.libraries[index]);
    }

    return (compilation.command);
}

error
jim_add_include_directory(struct jim_compilation *compilation, char *include_directory)
{
    if (compilation->number_of_include_directories == JIM_MAX_INCLUDE_DIRECTORIES)
    {
        return (1);
    }
    u32 new_index                               = compilation->number_of_include_directories++;
    compilation->include_directories[new_index] = include_directory;
    return (0);
}

error
jim_add_library_directory(struct jim_compilation *compilation,
                          char                   *library_directory)
{
    if (compilation->number_of_library_directories == JIM_MAX_LIBRARY_DIRECTORIES)
    {
        return (1);
    }
    u32 new_index = compilation->number_of_library_directories++;

    compilation->library_directories[new_index] = library_directory;
    return (0);
}
error
jim_add_library(struct jim_compilation *compilation,
                char                   *library)
{
    if (compilation->number_of_libraries == JIM_MAX_LIBRARIES)
    {
        return (1);
    }
    u32 new_index                     = compilation->number_of_libraries++;
    compilation->libraries[new_index] = library;
    return (0);
}

void
jim_please_compile(struct jim_compilation compilation)
{
    struct string command = jim_make_command(compilation);
    error         error   = 0;
    struct string result  = { 0 };
    //@TODO: handle errors
    printf("%s\n", command.first);
    error = make_string(&result, 4096 * 10, &_jim.allocator);
    if (error)
        printf("Yalalalala\n");

    error = run_command(command.first, result.first, result.length);
    if (error)
        printf("Yalalalala\n");
}

#endif
