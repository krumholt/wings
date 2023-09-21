// 'inspired' by https://github.com/tsoding/musializer   see nob.h and nob.c
// or https://github.com/tsoding/nobuild

#ifndef NOBUILD_C_
#define NOBUILD_C_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_COMMAND_RESULT 1024 * 1024

struct compiler
{
    char *name;
};

struct builder
{
    uint32_t string_memory_used;
    uint32_t string_memory_size;
    char    *string_memory;
    char    *command_result;
} builder;

struct process
{
    PROCESS_INFORMATION process_info;
};

#define ERROR_COMMAND_NOT_FOUND 7
#define GENERAL_ERROR 1


int32_t
run_command(char *command)
{
    SECURITY_ATTRIBUTES pipe_security_attributes = { 0 };
    pipe_security_attributes.nLength             = sizeof(SECURITY_ATTRIBUTES);
    pipe_security_attributes.bInheritHandle      = TRUE;

    HANDLE in_pipe_read   = 0;
    HANDLE in_pipe_write  = 0;
    HANDLE out_pipe_read  = 0;
    HANDLE out_pipe_write = 0;

    uint32_t error = 0;

    error = CreatePipe(&in_pipe_read,
                       &in_pipe_write,
                       &pipe_security_attributes,
                       0);
    error = CreatePipe(&out_pipe_read,
                       &out_pipe_write,
                       &pipe_security_attributes,
                       0);

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
    int32_t       success    = 0;
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

void
check_available_compilers(void)
{
    int32_t noclang, nocl, nogcc, notcc;
    noclang = run_command("clang --version");
    nocl    = run_command("cl");
    nogcc   = run_command("gcc --version");
    notcc   = run_command("tcc");

    if (!noclang)
        printf("we have clang yey\n");
    if (!nocl)
        printf("we have cl yey\n");
    if (!nogcc)
        printf("we have gcc yey\n");
    if (!notcc)
        printf("we have tcc yey\n");
}

struct c_compiler
{
    char *command;
};

struct c_compiler
get_prefered_c_compiler(void)
{
    struct c_compiler c_compiler = { 0 };
    return c_compiler;
}

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__LCC__) && !defined(_MSC_VER)
#define COMPILED_WITH "gcc"
#elif defined(__clang__)
#define COMPILED_WITH "clang"
#elif defined(_MSC_VER)
#define COMPILED_WITH "cl"
#else
#define COMPILED_WITH "UNKNOWN COMPILER"
#endif

void
compile(char *filename, char *include_directories)
{
}

int
main(void)
{
    builder.string_memory  = calloc(200 * 1024 * 1024, 1);
    builder.command_result = calloc(MAX_COMMAND_RESULT, 1);
    check_available_compilers();
    printf("I was build with %s,\n", COMPILED_WITH);
    printf("%s", __BASE_FILE__);
    compile("hello_world.c", "");
}

#endif
