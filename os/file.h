#ifndef WINGS_OS_FILE_H_
#define WINGS_OS_FILE_H_

#include "base/types.h"
#include "base/allocators.h"

struct file_description
{
    char *file_path;
    u64   last_write_time;
    s32   file_size;
    s32   is_directory;
};

typedef b32 file_filter_function(char *path);

error
file_read(
      struct buffer *buffer,
      const char *file_path,
      b32 zero_terminate,
      struct allocator *allocator);

error
file_write(
      struct buffer buffer,
      char *file_path,
      b32 create);
error
file_delete(char *file_path);

error
file_move(char *from_file_path, char *to_file_path);

error
file_copy(char *from_file_path, char *to_file_path);

error
file_get_last_write_time(u64 *time, char *file_path);

error
file_list_directory(
      char *path,
      u32 offset,
      struct file_description *file_list,
      u32 *number_of_files,
      file_filter_function file_filter,
      struct allocator *static_memory,
      struct allocator *temp_memory);

error
file_create_directory(char *file_path);

b32
file_exists(char *file_path);

error
file_copy_if_newer(char *from_file_name, char *to_file_name);

#endif
