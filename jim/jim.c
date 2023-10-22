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

#define JIM_MAX_OBJECT_FILES_PER_LIBRARY 100
#define JIM_MAX_INCLUDE_DIRECTORIES 100
#define JIM_MAX_LIBRARY_DIRECTORIES 100
#define JIM_MAX_LIBRARIES 100
#define JIM_MAX_OBJECT_FILES 1000

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
    enum jim_output_type type;
    char                *input_file;
    char                *output_file;
};

struct jim_object_file
{
    s32 foo;
};

struct jim_compilation
{
    struct jim_compiler  compiler;
    u64                  command_length;
    struct string        command;
    enum jim_output_type output_type;
    char                *output_file;
    char                *output_directory;
    char                *input_file;
    u32                  number_of_library_directories;
    char               **library_directories;
    u32                  number_of_libraries;
    char               **libraries;
    u32                  number_of_include_directories;
    char               **include_directories;
};

struct jim_library
{
    char                   *name;
    char                   *output_directory;
    u32                     number_of_object_files;
    struct jim_compilation *object_files;
};

struct jim
{
    struct allocator        allocator;
    struct jim_compiler     default_compiler;
    u32                     number_of_include_directories;
    char                  **include_directories;
    u32                     number_of_object_files;
    struct jim_object_file *object_files;
    error                   error;
    struct jim_compilation  compilation;
    struct string           compilation_result;
    struct jim_library      library;

} _jim = {
    .default_compiler
    = {
       .command                        = "gcc",
       .debug_flags                    = "-g ",
       .output_format                  = "-o %s%s ",
       .add_library_search_path_format = "-L %s ",
       .add_link_library_format        = "-l %s ",
       .add_include_directory_format   = "-I %s ",
       .compile_no_link                = "-c ",
       .flags                          = 0,
       },
};

error
jim_make_library(struct jim_library *library,
                 struct allocator   *allocator)
{
    library->number_of_object_files = 0;

    error error = allocate_array(
        &library->object_files,
        allocator,
        JIM_MAX_OBJECT_FILES_PER_LIBRARY,
        struct jim_compilation);

    return (error);
}

error
jim_library_add(struct jim_library    *library,
                struct jim_compilation compilation)
{
    if (library->number_of_object_files == JIM_MAX_LIBRARY_DIRECTORIES)
        return (1);

    u32 index                    = library->number_of_object_files++;
    library->object_files[index] = compilation;

    return (0);
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

void
jim_reset_compilation(struct jim_compilation *compilation)
{
    compilation->number_of_include_directories = 0;
    compilation->number_of_libraries           = 0;
    compilation->output_type                   = 0;
    compilation->output_file                   = 0;
    compilation->output_directory              = 0;
    compilation->command_length                = 0;
    compilation->input_file                    = 0;
}

void
_jim_string_append_vaargs(
    struct string *string,
    u64           *length,
    char          *format,
    va_list        arg_list)
{
    *length
        += vsnprintf(string->first
                         + (*length),
                     string->length
                         - (*length)
                         - 1,
                     format,
                     arg_list);
}
void
_jim_string_append(struct string *string, u64 *length, char *format, ...)
{
    va_list arg_list;
    va_start(arg_list, format);
    _jim_string_append_vaargs(string, length, format, arg_list);
    va_end(arg_list);
}

void
_jim_command_append(struct jim_compilation *compilation, char *format, ...)
{
    va_list arg_list;
    va_start(arg_list, format);
    _jim_string_append_vaargs(&compilation->command,
                              &compilation->command_length,
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
                        compilation.output_directory,
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

error
jim_build_library(struct jim_library library, struct allocator *allocator)
{
    error         error          = 0;
    struct string command        = { 0 };
    struct string result         = { 0 };
    u64           command_length = 0;
    error                        = make_string(&command, 4096 * 10, allocator);
    IF_ERROR_RETURN(error);
    error = make_string(&result, 4096 * 10, allocator);
    IF_ERROR_RETURN(error);

    _jim_string_append(&command,
                       &command_length,
                       "ar rcs %s%s.lib ",
                       library.output_directory,
                       library.name);

    for (u32 index = 0;
         index < library.number_of_object_files;
         ++index)
    {
        _jim_string_append(
            &command,
            &command_length,
            "%s%s ",
			library.object_files[index].output_directory,
            library.object_files[index].output_file);
    }

    printf("%s\n", command.first);
    error = run_command(command.first, result.first, result.length);

    return (error);
}

// jim_please api
//
void
jim_please_listen(void)
{
    error error                        = 0;
    _jim.allocator                     = make_growing_linear_allocator(mebibytes(1));
    _jim.number_of_include_directories = 0;

    error = make_string(&_jim.compilation_result, 4096 * 10, &_jim.allocator);
    if (error)
        _jim.error = error;

    error = allocate_array(
        &_jim.include_directories,
        &_jim.allocator,
        JIM_MAX_INCLUDE_DIRECTORIES,
        char *);
    if (error)
        _jim.error = error;

    error = allocate_array(
        &_jim.object_files,
        &_jim.allocator,
        JIM_MAX_OBJECT_FILES,
        struct jim_object_file);
    if (error)
        _jim.error = error;

    error = jim_make_compilation(&_jim.compilation,
                                 _jim.default_compiler,
                                 &_jim.allocator);
    error = jim_make_library(&_jim.library, &_jim.allocator);
}

void
jim_please_add_include_directory(char *path)
{
    error error = jim_add_include_directory(&_jim.compilation, path);
    if (error)
    {
        _jim.error = error;
    }
}

void
jim_please_add_library_directory(char *path)
{
    error error = jim_add_library_directory(&_jim.compilation, path);
    if (error)
    {
        _jim.error = error;
    }
}

void
jim_please_use_output_directory(char *path)
{
    _jim.compilation.output_directory = path;
}

void
jim_please_build_object_file(char *file_name)
{
    struct jim_object_file object_file = { 0 };
    _jim.compilation.output_type       = jim_output_type__object_file;
    _jim.compilation.output_file       = file_name;

    struct string command = jim_make_command(_jim.compilation);

    printf("%s\n", command.first);
    error error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
    if (error)
        _jim.error = error;
    error = jim_library_add(&_jim.library, _jim.compilation);
    if (error)
        _jim.error = error;
}
void
jim_please_create_executable(char *file_name)
{
    struct jim_object_file object_file = { 0 };
    _jim.compilation.output_type       = jim_output_type__executable;
    _jim.compilation.output_file       = file_name;

    jim_add_library(&_jim.compilation, _jim.library.name);

    struct string command = jim_make_command(_jim.compilation);

    printf("%s\n", command.first);
    error error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
    if (error)
        _jim.error = error;
}

void
jim_please_compile(char *file)
{
    _jim.compilation.input_file = file;
}

void
jim_please_build_library(char *name)
{
    _jim.library.name             = name;
    _jim.library.output_directory = _jim.compilation.output_directory;
    error error                   = jim_build_library(_jim.library, &_jim.allocator);
    if (error)
        _jim.error = error;
}

#endif
