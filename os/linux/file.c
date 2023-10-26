#ifndef WINGS_OS_LINUX_FILE_C_
#define WINGS_OS_LINUX_FILE_C_

#include "wings/base/allocators.c"
#include "wings/base/error_codes.c"
#include "wings/base/types.c"

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

error
file_read(struct buffer    *buffer,
          char             *file_path,
          b32               zero_terminate,
          struct allocator *allocator)
{
    FILE *InputFile = 0;
    InputFile       = fopen(file_path, "rb");
    if (InputFile == 0)
        return (1);
    fseek(InputFile, 0, SEEK_END);
    buffer->size = (u64)ftell(InputFile);
    fseek(InputFile, 0, SEEK_SET);

    buffer->size += (zero_terminate ? 1 : 0);
    error error = allocate_array(&buffer->base, allocator, buffer->size, u8);
    if (error)
        return (2);
    fread(buffer->base, 1, buffer->size, InputFile);
    fclose(InputFile);

    return (0);
}

error
file_write(struct buffer buffer, char *file_path, b32 create)
{
    FILE *file = fopen(file_path, "rb");
    if (!file)
        return (ec_os_file__not_found);

    int result = fwrite(buffer.base, buffer.size, 1, file);
    if (result != 1)
        return (ec_os_file__write_failed);

    return (ec__no_error);
}
error
file_delete(char *file_name)
{
    int result = remove(file_name);
    if (result != 0)
        return (ec_os_file__access_denied);
    return (ec__no_error);
}
error
file_move(char *from_file_name, char *to_file_name)
{
    int result = rename(from_file_name, to_file_name);
    if (result != 0)
        return (ec_os_file__access_denied);
    return (ec__no_error);
}
error
file_copy(char *from_file_name, char *to_file_name)
{
    int in_fd = open(from_file_name, O_RDONLY);
    assert(in_fd >= 0);
    int out_fd = open(to_file_name, O_WRONLY);
    assert(out_fd >= 0);
    char buf[8192];

    while (1)
    {
        ssize_t read_result = read(in_fd, &buf[0], sizeof(buf));
        if (!read_result)
            break;
        if (read_result <= 0)
            return (ec_os_file__access_denied);
        ssize_t write_result = write(out_fd, &buf[0], read_result);
        if (write_result != read_result)
            return (ec_os_file__write_failed);
    }
    close(in_fd);
    close(out_fd);

    return (ec__no_error);
}

error
file_get_last_write_time(u64 *time, char *file_path)
{
    struct stat file_stat = { 0 };
    int         result    = stat(file_path, &file_stat);
    if (result == -1)
    {
        return (ec_os_file__not_found);
    }
    *time = file_stat.st_mtime;
    return (ec__no_error);
}
error
file_create_directory(char *file_path)
{
    int result = mkdir(file_path, S_IRWXU);
    if (result == -1 && errno != EEXIST)
        return (ec_os_file__access_denied);
    return (ec__no_error);
}
b32
file_exists(char *file_path)
{
    struct stat file_stat = { 0 };
    int         result    = stat(file_path, &file_stat);
    if (result == -1)
        return (0);
    else
        return (1);
}

error
file_copy_if_newer(char *from_file_name, char *to_file_name)
{
    if (!file_exists(to_file_name))
    {
        return (file_copy(from_file_name, to_file_name));
    }
    u64 from_file_time = 0;
    file_get_last_write_time(&from_file_time, from_file_name);
    u64 to_file_time = 0;
    file_get_last_write_time(&to_file_time, to_file_name);
    if (from_file_time > to_file_time)
    {
        return (file_copy(from_file_name, to_file_name));
    }
    return (ec__no_error);
}

#endif
