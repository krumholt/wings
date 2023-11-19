#ifndef WINGS_OS_WINDOWS_FILE_C_
#define WINGS_OS_WINDOWS_FILE_C_

#include "wings/base/types.c"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif
#include <Windows.h>

struct file_description
{
    char *file_path;
    u64   last_write_time;
    s32   file_size;
    s32   is_directory;
};

typedef b32 file_filter_function(char *path);


error
file_read(struct buffer *buffer, const char *file_path, b32 zero_terminate,
          struct allocator *allocator)
{
   HANDLE file_handle = { 0 };
   file_handle        = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 0,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

   if (file_handle == INVALID_HANDLE_VALUE)
      return (ec_os_file__not_found);

   u32 size_on_disk = GetFileSize(file_handle, 0);

   buffer->size = size_on_disk + (zero_terminate ? 1 : 0);
   buffer->used = buffer->size;
   error error  = allocate_array(&buffer->base, allocator, buffer->size, u8);
   if (error)
      return (error);

   DWORD size_read = 0;
   b32   success   = 0;
   success         = ReadFile(file_handle, buffer->base, buffer->size, &size_read, 0);
   if (!success || (buffer->size != size_read + (zero_terminate ? 1 : 0)))
   {
      CloseHandle(file_handle);
      return (ec_os_file__not_found);
   }
   CloseHandle(file_handle);

   return (NO_ERROR);
}

error
file_write(struct buffer buffer, char *file_path, b32 create)
{
   u32    create_flags = create ? CREATE_ALWAYS : OPEN_EXISTING;
   PSTR   filename     = file_path;
   HANDLE hFile;

   hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 0,
                      create_flags, FILE_ATTRIBUTE_NORMAL, 0);

   if (hFile == INVALID_HANDLE_VALUE)
   {
      CloseHandle(hFile);
      return (ec_os_file__not_found);
   }

   DWORD size_written = 0;
   b32   success      = WriteFile(hFile, (void *)buffer.base, (DWORD)buffer.size, &size_written, 0) ? 1u : 0u;
   if (!success)
   {
      CloseHandle(hFile);
      return (ec_os_file__write_failed);
   }

   CloseHandle(hFile);
   return (0);
}

error
file_delete(char *file_name)
{
   u32 success = DeleteFile(file_name);
   if (!success)
   {
      DWORD last_error = GetLastError();
      if (last_error == 2)
         return ec_os_file__not_found;
      if (last_error == 5)
         return ec_os_file__access_denied;
   }
   return (!success);
}

error
file_move(char *from_file_name, char *to_file_name)
{
   u32 success = MoveFile(from_file_name, to_file_name);
   if (!success)
   {
      DWORD last_error = GetLastError();
      if (last_error == 2)
         return ec_os_file__not_found;
      if (last_error == 5)
         return ec_os_file__access_denied;
   }
   return (!success);
}

error
file_copy(char *from_file_name, char *to_file_name)
{
   u32 success = CopyFile(from_file_name, to_file_name, 0);
   if (!success)
   {
      DWORD last_error = GetLastError();
      if (last_error == 2)
         return ec_os_file__not_found;
      if (last_error == 5)
         return ec_os_file__access_denied;
      return (ec_os_file__not_found);
   }
   return (ec__no_error);
}

error
file_get_last_write_time(u64 *time, char *file_path)
{
   HANDLE file_handle = { 0 };
   file_handle        = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 0,
                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
   if (!file_handle)
   {
      return (1);
   }
   u64 creation_time = 0, last_access_time = 0, last_write_time = 0;
   u32 success = GetFileTime(
       file_handle,
       (FILETIME *)&creation_time,
       (FILETIME *)&last_access_time,
       (FILETIME *)&last_write_time);
   if (!success)
   {
      return (1);
   }
   CloseHandle(file_handle);
   *time = last_write_time;
   return (0);
}

b32
_file_default_file_filter(char *file_name)
{
    UNUSED(file_name);
    return 1;
}

//@TODO: add list directory
/*
error
file_list_directory(char                    *path,
                    u32                      offset,
                    struct file_description *file_list,
                    u32                     *number_of_files,
                    file_filter_function     file_filter,
                    struct allocator temp_memory)
{
    if (!file_filter)
        file_filter = _file_default_file_filter;
    HANDLE          find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data   = { 0 };

    char tmp_path[1024] = {0};
    snprintf(tmp_path, 1023, "%s\\*", path);
    find_handle = FindFirstFile(path, &find_data);

    if (find_handle == INVALID_HANDLE_VALUE)
    {
        *number_of_files = 0;
        return ec_os_file__not_found;
    }

    u32 file_count = 0;
    u32 file_index = 0;
    do
    {
        char *file_name        = find_data.cFileName;
        u64   file_name_length = strlen(file_name);
        b32   is_directory     = (b32)(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((file_name_length == 1 && file_name[0] == '.')
            || (file_name_length == 2 && file_name[0] == '.' && file_name[1] == '.')
            || !file_filter(file_name))
            continue;
        if (file_index >= offset && file_count < *number_of_files)
        {
            copy_path(&file_list[file_count].file_path, path);
            append_path(&file_list[file_count].file_path, file_name);
            file_list[file_count].file_size       = (s32)(find_data.nFileSizeHigh * MAXDWORD + find_data.nFileSizeLow);
            file_list[file_count].last_write_time = 0;
            file_list[file_count].last_write_time += (u64)(find_data.ftLastWriteTime.dwHighDateTime) << 32;
            file_list[file_count].last_write_time += find_data.ftLastWriteTime.dwLowDateTime;
            file_list[file_count].is_directory = is_directory;
            file_count++;
        }
        file_index++;
    } while (FindNextFile(find_handle, &find_data) != 0);

    *number_of_files = file_count;

    FindClose(find_handle);

    return (file_index);
}
*/

error
file_create_directory(char *file_path)
{
   BOOL result = CreateDirectory(file_path, 0);
   if (result == ERROR_ALREADY_EXISTS)
      return (0);
   if (result == ERROR_PATH_NOT_FOUND)
      return (ec_os_file__not_found);
   return (0);
}

b32
file_exists(char *file_path)
{
   DWORD result = GetFileAttributes(file_path);
   if (result == INVALID_FILE_ATTRIBUTES)
      return (0);
   return (1);
}

error
file_copy_if_newer(char *from_file_name, char *to_file_name)
{
   printf("copying %s to %s\n", from_file_name, to_file_name);
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
      printf("actually need to copy\n");
      return (file_copy(from_file_name, to_file_name));
   }
   return (ec__no_error);
}

#endif
