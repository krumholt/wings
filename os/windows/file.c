#ifndef WINGS_OS_WINDOWS_FILE_C_
#define WINGS_OS_WINDOWS_FILE_C_

#include "wings/base/allocators.c"
#include "wings/base/types.c"

error
read_file(struct buffer *buffer, char *file_path, b32 zero_terminate,
          struct allocator *allocator)
{
    HANDLE file_handle = { 0 };
    file_handle        = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ, 0,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (file_handle == INVALID_HANDLE_VALUE)
        return (1);

    u32 size_on_disk = GetFileSize(file_handle, 0);

    buffer->size = size_on_disk + (zero_terminate ? 1 : 0);
    error error  = allocate_array(&buffer->data, allocator, buffer->size, u8);
    if (error)
        return (2);

    DWORD size_read = 0;
    b32   success   = 0;
    success         = ReadFile(file_handle, buffer->data, buffer->size, &size_read, 0);
    if (!success || (buffer->size != size_read + (zero_terminate ? 1 : 0)))
    {
        CloseHandle(file_handle);
        return (3);
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
        return (1);
    }

    DWORD size_written = 0;
    b32   success      = WriteFile(hFile, (void *)buffer.data, (DWORD)buffer.size, &size_written, 0) ? 1u : 0u;
    if (!success)
    {
        CloseHandle(hFile);
        return (2);
    }

    CloseHandle(hFile);
    return (0);
}

#endif
