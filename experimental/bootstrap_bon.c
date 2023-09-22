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

b32   compiler_found = COMPILER_FOUND;
char *compiler_name  = COMPILED_WITH;

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

error
string_builder_append(struct string_builder *builder, char *string)
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
run_command(char *command)
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
        success = ReadFile(out_pipe_read, builder.command_result, MAX_COMMAND_RESULT, &chars_read, 0);
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

    for (u32 index = 0; index < (sizeof(compiler_list) / sizeof(compiler_list[0])); ++index)
    {
        error error = 0;
        error       = run_command(compiler_list[index]);
        if (!error)
        {
            compiler_found = 1;
            compiler_name  = compiler_list[index];
            return 0;
        }
    }
    return (1); // no suitable compiler found
}

int
main(void)
{
    if (!compiler_found)
    {
        error error = try_to_find_compiler();
        if (error)
        {
            printf("Failed to find suitable compiler. Tried gcc, clang, cl, tcc.\n");
            exit(-1);
        }
    }
    struct string_builder command = { 0 };

    error error = make_string_builder(&command, 4096);
    if (error)
    {
        printf("Failed to allocate memory. Developer info: %s:%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    char *target_operating_system = TARGET_OPERATING_SYSTEM;
    if (!target_operating_system)
    {
        printf("Couldn't figure out if we are on windows or linux. FU Apple.\n");
        exit(-1);
    }

    char *path_to_wings = ".";

    string_builder_append(&command, compiler_name);
    string_builder_append(&command, " -D ");
    string_builder_append(&command, target_operating_system);
    string_builder_append(&command, " -I ");
    string_builder_append(&command, path_to_wings);
    string_builder_append(&command, " -o jim.exe ");
    string_builder_append(&command, path_to_wings);
    string_builder_append(&command, "/wings/experimental/bon.c");

    // run_command("gcc -DOS_WINDOWS -I./ -obon.exe wings/experimental/bon.c");
    printf("%s\n", command.base);
    error = run_command(command.base);
    if (error)
        printf("Failed to compile\n");

    printf("%s\n", builder.command_result);
}

#endif
