// 'inspired' by https://github.com/tsoding/musializer   see nob.h and nob.c
// or https://github.com/tsoding/nobuild

#ifndef BOOTSTRAP_BON_C_
#define BOOTSTRAP_BON_C_

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__LCC__) && !defined(_MSC_VER)
#define COMPILED_WITH "gcc"
#define COMPILER_FOUND 1
#elif defined(__clang__)
#define COMPILED_WITH "clang"
#define COMPILER_FOUND 1
#elif defined(_MSC_VER)
#define COMPILED_WITH "cl"
#define COMPILER_FOUND 1
#else
#define COMPILED_WITH "UNKNOWN COMPILER"
#define COMPILER_FOUND 0
#endif

#if defined(_WIN32)
#define TARGET_OPERATING_SYSTEM "OS_WINDOWS"
#elif defined(__linux__)
#define TARGET_OPERATING_SYSTEM "OS_LINUX"
#else
#define TARGET_OPERATING_SYSTEM 0
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_COMMAND_RESULT 1024 * 1024

typedef uint32_t error;
typedef uint32_t u32;
typedef int32_t  s32;
typedef int32_t  b32;

b32         compiler_found = COMPILER_FOUND;
const char *compiler_name  = COMPILED_WITH;

struct string_builder
{
    uint32_t used;
    uint32_t size;
    char    *base;
};

error
make_string_builder(struct string_builder *builder, uint32_t size)
{
    builder->used = 0;
    builder->size = size + 1; // + 1 for zero termination
    builder->base = calloc(builder->size, sizeof(char));
    if (!builder->base)
        return (1);
    return (0);
}

void
free_string_builder(struct string_builder *builder)
{
    free(builder->base);
}

error
string_builder_append(struct string_builder *builder, const char *string)
{
    uint64_t length = strlen(string);
    if (builder->used + length > builder->size)
        return (1);
    for (u32 index = 0; index < length; ++index)
    {
        builder->base[builder->used + index] = string[index];
    }
    builder->used += length;
    return (0);
}

struct builder
{
    u32   string_memory_used;
    u32   string_memory_size;
    char *string_memory;
    char *command_result;
} builder;

struct process
{
    PROCESS_INFORMATION process_info;
};

#define ERROR_COMMAND_NOT_FOUND 7
#define GENERAL_ERROR 1

error
run_command(char *command, char *result_buffer, u32 result_buffer_size)
{
    SECURITY_ATTRIBUTES pipe_security_attributes = { 0 };
    pipe_security_attributes.nLength             = sizeof(SECURITY_ATTRIBUTES);
    pipe_security_attributes.bInheritHandle      = TRUE;

    HANDLE in_pipe_read   = 0;
    HANDLE in_pipe_write  = 0;
    HANDLE out_pipe_read  = 0;
    HANDLE out_pipe_write = 0;

    error error   = 0;
    s32   success = 1;

    success = CreatePipe(&in_pipe_read,
                         &in_pipe_write,
                         &pipe_security_attributes,
                         0);
    if (!success)
        return (1);
    error = CreatePipe(&out_pipe_read,
                       &out_pipe_write,
                       &pipe_security_attributes,
                       0);
    if (!success)
        return (1);

    STARTUPINFO         startup_info = { 0 };
    PROCESS_INFORMATION process_info;
    startup_info.cb         = sizeof(STARTUPINFO);
    startup_info.hStdError  = out_pipe_write;
    startup_info.hStdOutput = out_pipe_write;
    startup_info.hStdInput  = in_pipe_read;
    startup_info.dwFlags    = STARTF_USESTDHANDLES;
    int result              = CreateProcess(
        0,
        command,
        0,
        0,
        TRUE,
        0,
        0,
        0,
        &startup_info,
        &process_info);
    if (result == 0)
    {
        int32_t last_error = GetLastError();
        if (last_error == 2)
            return (ERROR_COMMAND_NOT_FOUND);
        else
            return (GENERAL_ERROR);
    }
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(in_pipe_read);
    CloseHandle(out_pipe_write);

    unsigned long chars_read = 0;
    for (;;)
    {
        success = ReadFile(out_pipe_read,
                           result_buffer,
                           result_buffer_size,
                           &chars_read,
                           0);
        if (!success || chars_read == 0)
            break;
    }
    CloseHandle(in_pipe_write);
    CloseHandle(out_pipe_read);
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);

    return (NO_ERROR);
}

char *compiler_list[] = {
    "gcc",
    "cl",
    "clang",
    "tcc",
};

error
try_to_find_compiler(void)
{
    s32   result_buffer_size = 4096;
    char *result_buffer      = calloc(result_buffer_size, sizeof(char));

    for (u32 index = 0; index < (sizeof(compiler_list) / sizeof(compiler_list[0])); ++index)
    {
        error error = 0;
        error       = run_command(compiler_list[index], result_buffer, result_buffer_size);
        if (!error)
        {
            compiler_found = 1;
            compiler_name  = compiler_list[index];
            return 0;
        }
    }
    free(result_buffer);
    return (1); // no suitable compiler found
}

b32
string_ends_with(char *haystack, u32 haystack_length, char *needle, u32 needle_length)
{
	printf("%s[%d] > %s[%d]\n", haystack, haystack_length, needle, needle_length);
    if (haystack_length < needle_length)
	{
		printf("leaving\n");
        return (0);
	}
    u32 haystack_index_offset = haystack_length - needle_length;
    for (u32 index = 0; index < needle_length; ++index)
    {
        if (haystack[index + haystack_index_offset] != needle[index])
            return (0);
    }
    return (1);
}

static b32
_is_path_seperator(char c)
{
    return c == '\\' || c == '/';
}

void
set_to_parent(char **path_in, u32 *length)
{
    if (*length == 0)
        return;
    char *path           = *path_in;
    s32   last_character = *length - 1;
    if (_is_path_seperator(path[last_character]))
        last_character -= 1;
    while (last_character >= 0)
    {
        if (_is_path_seperator(path[last_character]))
        {
            path[last_character]     = '/';
            path[last_character + 1] = 0;
            *length                  = last_character + 1;
            return;
        }
        last_character -= 1;
    }
    path[0] = 0;
    *length = 0;
}

error
find_wings(char *path_to_bootstrap)
{
    if (!path_to_bootstrap)
        return 1;
    u32 path_to_bootstrap_length = strlen(path_to_bootstrap);

    char *paths_to_walk_back[] = {
        "jim_bootstrap.c",
        "jim/",
        "wings/",
    };
    for (u32 index = 0; index < sizeof(paths_to_walk_back) / sizeof(paths_to_walk_back[0]); ++index)
    {
        u32 length_substring = strlen(paths_to_walk_back[index]);
        b32 found            = 0;

        found = string_ends_with(path_to_bootstrap, path_to_bootstrap_length, paths_to_walk_back[index], length_substring);
        if (!found)
		{
			if (strlen(path_to_bootstrap) == 0)
				printf("We are inside wings INSANE\n");
			return(1);
		}

		printf("1. %s\n", path_to_bootstrap);
        set_to_parent(&path_to_bootstrap, &path_to_bootstrap_length);
		printf("2. %s\n", path_to_bootstrap);
    }

    return (0);
}

int
main(void)
{
    error error = 0;
    // allocate some memory
    struct string_builder wings_path = { 0 };
    struct string_builder command    = { 0 };

    error = make_string_builder(&wings_path, 10000);
    if (error)
    {
        printf("Failed to allocate memory. Developer info: %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    error = make_string_builder(&command, 4096);
    if (error)
    {
        printf("Failed to allocate memory. Developer info: %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    if (!compiler_found)
    {
        error = try_to_find_compiler();
        if (error)
        {
            printf("Failed to find suitable compiler. Tried gcc, clang, cl, tcc.\n");
            exit(-1);
        }
    }

    char *target_operating_system = TARGET_OPERATING_SYSTEM;
    if (!target_operating_system)
    {
        printf("Couldn't figure out if we are on windows or linux. FU Apple.\n");
        exit(-1);
    }

    printf("I think I should use '%s' building for '%s'.\n",
           compiler_name,
           target_operating_system);
    char *path_to_bootstrap = calloc(4096, sizeof(char));
    strcpy_s(path_to_bootstrap, 4096, __BASE_FILE__);

    error = find_wings(path_to_bootstrap);
    if (error)
    {
        printf("I couldn't find wings\n");
        exit(-1);
    }
    char *path_to_wings = path_to_bootstrap;
    if (path_to_wings[0] == 0)
    {
        path_to_wings[0] = '.';
        path_to_wings[1] = '/';
        path_to_wings[2] = 0;
    }
    printf("I think the path to wings is '%s'\n", path_to_wings);

    string_builder_append(&command, compiler_name);
    string_builder_append(&command, " -D ");
    string_builder_append(&command, target_operating_system);
    string_builder_append(&command, " -I ");
    string_builder_append(&command, path_to_wings);
    string_builder_append(&command, " -o jim.exe ");
    string_builder_append(&command, path_to_wings);
    string_builder_append(&command, "wings/jim/jim.c");

    s32   result_buffer_size = 4096;
    char *result_buffer      = calloc(result_buffer_size, sizeof(char));
    // run_command("gcc -DOS_WINDOWS -I./ -obon.exe wings/experimental/bon.c");
    printf("Calling: '%s'\n", command.base);
    error = run_command(command.base, result_buffer, result_buffer_size);
    if (error)
    {
        printf("%s\n", result_buffer);
        free(result_buffer);
        exit(-3);
    }
    free(result_buffer);
    printf("jim is ready\n");
}

#endif
