#ifndef WINGS_OS_WINDOWS_FILE_C_
#define WINGS_OS_WINDOWS_FILE_C_

#include "wings/base/types.c"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"
#pragma warning(push, 0)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

error
read_file(struct buffer *buffer, const char *file_path, b32 zero_terminate,
          struct allocator *allocator)
{
    HANDLE file_handle = { 0 };
    file_handle        = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 0,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (file_handle == INVALID_HANDLE_VALUE)
        return (ERROR_os_file__not_found);

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
        return (ERROR_os_file__not_found);
    }
    CloseHandle(file_handle);

    return (NO_ERROR);
}

error
write_file(struct buffer buffer, char *file_path, b32 create)
{
    u32    create_flags = create ? CREATE_ALWAYS : OPEN_EXISTING;
    PSTR   filename     = file_path;
    HANDLE hFile;

    hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 0,
                       create_flags, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return (ERROR_os_file__not_found);
    }

    DWORD size_written = 0;
    b32   success      = WriteFile(hFile, (void *)buffer.base, (DWORD)buffer.size, &size_written, 0) ? 1u : 0u;
    if (!success)
    {
        CloseHandle(hFile);
        return (ERROR_os_file__write_failed);
    }

    CloseHandle(hFile);
    return (0);
}

error
delete_file(char *file_name)
{
    u32 success = DeleteFile(file_name);
    if (!success)
    {
        DWORD last_error = GetLastError();
        if (last_error == 2)
            return ERROR_os_file__not_found;
        if (last_error == 5)
            return ERROR_os_file__access_denied;
    }
    return (!success);
}

error
move_file(char *from_file_name, char *to_file_name)
{
    u32 success = MoveFile(from_file_name, to_file_name);
    if (!success)
    {
        DWORD last_error = GetLastError();
        if (last_error == 2)
            return ERROR_os_file__not_found;
        if (last_error == 5)
            return ERROR_os_file__access_denied;
    }
    return (!success);
}

error
copy_file(char *from_file_name, char *to_file_name)
{
    u32 success = CopyFile(from_file_name, to_file_name, 1);
    if (!success)
    {
        DWORD last_error = GetLastError();
        if (last_error == 2)
            return ERROR_os_file__not_found;
        if (last_error == 5)
            return ERROR_os_file__access_denied;
    }
    return (!success);
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

#endif
