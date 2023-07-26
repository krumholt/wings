#ifndef FILE_WINDOWS_C
#define FILE_WINDOWS_C

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#include "file.h"

#include "wings_strings.c"
#include "memory.c"
#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

#include "file.h"

#include "wings_strings.c"
#include "memory.c"


struct file
{
    HANDLE handle;
};

b32
open_file(struct file *file, char *file_path, s32 flags)
{
    file->handle =
        CreateFile(file_path,
                   GENERIC_READ | GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                   0,
                   flags,
                   FILE_ATTRIBUTE_NORMAL,
                   0);

    if (file->handle == INVALID_HANDLE_VALUE)
        return(1);

    return(0);
}

b32
open_existing_file(struct file *file, char *file_path)
{
    return(open_file(file, file_path, OPEN_EXISTING));
}


b32
open_or_create_file(struct file *file, char *file_path)
{
    return(open_file(file, file_path, OPEN_ALWAYS));
}

b32
create_or_overwrite_file(struct file *file, char *file_path)
{
    return(open_file(file, file_path, CREATE_ALWAYS));
}

b32
load_file(u8 **data, u32 *size, char *path, b32 zero_terminate, struct memory *block)
{
    HANDLE file_handle = {0}; 
    file_handle = CreateFile(path,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            0,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            0);
 
    if (file_handle == INVALID_HANDLE_VALUE)
        return 1;

    u32 size_on_disk = GetFileSize(file_handle, 0);

    *size = size_on_disk + (zero_terminate ? 1 : 0);
    u32 asize = *size;
    *data = allocate_array(block, asize, u8);

    b32 last_error = GetLastError();
    DWORD size_read = 0;
    b32 success = 0;
    success = ReadFile(file_handle, *data, *size, &size_read, 0);
    if (!success || (*size != size_read + (zero_terminate ? 1 : 0))) {
        b32 last_error = GetLastError();
        CloseHandle(file_handle);
        return last_error;
    }
    CloseHandle(file_handle);
    return 0;
}

b32
read_file_as_string(char *file_path,
                    char **text,
                    u32 *size,
                    struct memory *memory)
{
    u8 *data = 0;
    b32 error = load_file(&data, size, file_path, 1, memory);
    *text = (char *)data;
    return error;
}

b32
read_file(struct file file, u8 *data, u32 size, u32 *bytes_written)
{
    b32 success = ReadFile( 
        file.handle,
        data,
        (DWORD)size,
        (DWORD *)bytes_written,
        0);   

    if (!success)
    {
        u32 error = GetLastError();
        return(error);
    }
    return(0);
}

b32
write_file(struct file file, u8 *data, u32 size, u32 *bytes_written)
{
    b32 success = WriteFile( 
        file.handle,
        data,
        (DWORD)size,
        (DWORD *)bytes_written,
        0);   

    if (!success)
    {
        u32 error = GetLastError();
        return(error);
    }

    return(0);
}

b32
write_file_path(char *file_path, u8 *data, u32 size, b32 create_file)
{
    u32 create_flags = create_file? CREATE_ALWAYS : OPEN_EXISTING;
    PSTR filename = file_path;
    HANDLE hFile;
        
    hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 0,
                       create_flags, FILE_ATTRIBUTE_NORMAL, 0);
                                        
    if (hFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return(1);
    }

    DWORD size_written = 0;
    b32 success = WriteFile(hFile, (void *)data, (DWORD)size, &size_written, 0) ? 1u : 0u;
    if (!success)
    {
        CloseHandle(hFile);
        return(2);
    }

    CloseHandle(hFile);
    return(0);
}

b32
file_exists(char *file_path)
{
    WIN32_FIND_DATA FindFileData = {0};
    HANDLE handle = FindFirstFileA(file_path, &FindFileData);
    b32 found  = handle != INVALID_HANDLE_VALUE;
    if(found) 
        FindClose(handle);
    return found;
}

u64
get_last_write(char *file_path)
{
    HANDLE file_handle; 

    file_handle = CreateFile(file_path,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);
 
    if (file_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    FILETIME time = {0};
    GetFileTime(file_handle, 0, 0, &time);

    u64 last_write_time = 0;
    last_write_time += (u64)(time.dwHighDateTime) << 32;
    last_write_time += time.dwLowDateTime;
    CloseHandle(file_handle);
    return(last_write_time);
}

void
close_file(struct file file)
{
    CloseHandle(file.handle);
}


b32
_default_file_filter(char *file_name)
{
    unused(file_name);
    return 1;
}

s32
list_directory(struct path path,
               u32 offset,
               struct file_description *file_list,
               u32 *number_of_files,
               file_filter_function *file_filter)
{
    if (path.string[path.used - 2] != '\\')
        append_path(&path, "\\");
    if (!file_filter) file_filter = &_default_file_filter;
    HANDLE find_handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA find_data = {0};

    struct path tmp_path = {0};
    copy_path(&tmp_path, path);
    append_path(&tmp_path, "*");
    find_handle = FindFirstFile(tmp_path.string, &find_data);

    if(find_handle == INVALID_HANDLE_VALUE) {
        *number_of_files = 0;
        return 0;
    }
   
    int file_count = 0;
    int file_index = 0;
    do {
        char *file_name = find_data.cFileName;
        u64 file_name_length = strlen(file_name);
        s32 is_directory = (int)(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((file_name_length == 1 && file_name[0] == '.')
            || (file_name_length == 2 && file_name[0] == '.'
                && file_name[1] == '.')
            || !file_filter(file_name))
            continue;
        if (file_index >= offset && file_count < *number_of_files)
        {
            copy_path(&file_list[file_count].file_path, path);
            append_path(&file_list[file_count].file_path, file_name);
            file_list[file_count].file_size        = (s32)(find_data.nFileSizeHigh * MAXDWORD + find_data.nFileSizeLow);
            file_list[file_count].last_write_time  = 0;
            file_list[file_count].last_write_time += (u64)(find_data.ftLastWriteTime.dwHighDateTime)<< 32;
            file_list[file_count].last_write_time += find_data.ftLastWriteTime.dwLowDateTime;
            file_list[file_count].is_directory     = is_directory;
            file_count++;
        }
        file_index++;
    } while (FindNextFile(find_handle, &find_data) != 0);

    *number_of_files = file_count;

    FindClose(find_handle);

    return(file_index);
}

void
get_current_directory(struct path *path)
{
    u64 used  = GetCurrentDirectory((DWORD)MAX_PATH_SIZE, path->string);
    path->used = used + 1;
}

void
get_relative_path(char *base, char *target, char *result, s32 size)
{
    char *base_last_backslash   = base;
    char *target_last_backslash = target;
    while(*base && *target && *base == *target)
    {
        if (*base == '\\')
        {
            base_last_backslash   = base;
            target_last_backslash = target;
        }
        base++;
        target++;
    }
    if (*base == 0 && *target == '\\')
    {
        target += 1;
    }
    else
    {
        target = target_last_backslash + 1;
        base   = base_last_backslash;
    }
    s32 number_of_subdirectories = 0;
    while(*base)
    {
        if (*base == '\\')
            number_of_subdirectories++;
        base += 1;
    }
    for(s32 index = 0; index < number_of_subdirectories; ++index)
    {
        int num_printed = sprintf_s(result, (u32)size, "..\\");
        result += num_printed;
        size   -= num_printed;
    }
    sprintf_s(result, (u32)size, "%s", target);
}

//char *
//last_part(struct path path)
//{
    //char *result = path.string;
    //assert(path.used != 0);
    //if (path.used == 1) return result;
    //s32 last_character = path.used - 1;
    //while(last_character >= 0)
    //{
        //if(path.string[last_character] == '\\')
        //{
            //return(result + last_character + 1);
        //}
        //--last_character;
    //}
    //return(result);
//}
/*
*/

/*
char *
get_file_name(char *path, struct memory *memory)
{
    char *first_char = path;
    while(*path) ++path;
    int file_name_size = 0;
    while(path != first_char)
    {
        --path;
        file_name_size++;
        if(*path == '\\' || *path == '/')
        {
            path++;
            break;
        }
    }
    char *file_name = allocate_array(memory, file_name_size, char);
    char *tmp = file_name;
    while((*tmp++ = *path++) != 0);

    return(file_name);
}
*/

void
get_path_without_file_name(char *path, char *result)
{
    char *first_char = path;
    s32 path_size = 0;
    while(*path) 
    {
        ++path_size;
        ++path;
    }
    s32 file_name_size = 0;
    while(path != first_char)
    {
        --path;
        file_name_size++;
        if(*path == '\\' || *path == '/')
        {
            path++;
            break;
        }
    }
    while(first_char != path)
    {
        *result++ = *first_char++;
    }
    *result = 0;
}

/*
char *
get_path_without_file_name(char *path, struct memory *memory)
{
    char *first_char = path;
    s32 path_size = 0;
    while(*path) 
    {
        ++path_size;
        ++path;
    }
    s32 file_name_size = 0;
    while(path != first_char)
    {
        --path;
        file_name_size++;
        if(*path == '\\' || *path == '/')
        {
            path++;
            break;
        }
    }
    char *path_only = allocate_array(memory, path_size - file_name_size + 1, char);
    char *tmp = path_only;
    while(first_char != path)
    {
        *tmp++ = *first_char++;
    }
    return(path_only);
}
*/

void
get_file_name(char *path, char *result)
{
    char *first_char = path;
    while(*path) ++path;
    int file_name_size = 0;
    while(path != first_char)
    {
        --path;
        file_name_size++;
        if(*path == '\\' || *path == '/')
        {
            path++;
            break;
        }
    }
    while((*result++ = *path++) != 0);
    *result = 0;
}

char
get_drive_letter(struct path path)
{
    return path.string[0];
}

void
get_full_path_name(struct path *target, char *source)
{
    target->used = GetFullPathNameA(source, MAX_PATH_SIZE, target->string, 0) + 1;
}

u32
get_drives(void)
{
    return GetLogicalDrives();
}

b32
copy_file(char *from, char *to, b32 fail_if_exists)
{
    return CopyFile(from, to, fail_if_exists);
}

#endif
