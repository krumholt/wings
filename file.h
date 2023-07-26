#ifndef FILE_H
#define FILE_H

#include "types.h"
#include "memory.c"
#include "path.c"
#include "wings_strings.c"


struct file;

struct file_description
{
    struct path  file_path;
    u64          last_write_time;
    s32        file_size;
    s32        is_directory;
};

typedef b32 file_filter_function(char *path);

b32   open_file              (struct file *file, char *file_path, s32 flags);
b32   open_existing_file     (struct file *file, char *file_path);
b32   open_or_create_file    (struct file *file, char *file_path);
b32   read_file_as_string    (char *file_path,
                              char **text,
                              u32 *size,
                              struct memory *memory);
b32   load_file              (u8 **data,
                              u32 *size,
                              char *file_path,
                              b32 zero_terminate,
                              struct memory *block
                             );
b32   write_file             (struct file file, u8 *data, u32 size, u32 *bytes_written);
b32   write_file_path        (char *file_path, u8 *data, u32 size, b32 create);
b32   file_exists            (char *file_path);
u64   get_last_write         (char *file_path);
void  close_file             (struct file file);

s32   list_directory         (struct path path,
                              u32 offset,
                              struct file_description *file_list,
                              u32 *number_of_files,
                              file_filter_function *file_filter);
void  get_current_directory  (struct path *path);
void  get_full_path_name     (struct path *target, char *source);

#endif
