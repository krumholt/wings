#ifndef WINGS_OS_LINUX_FILE_C_
#define WINGS_OS_LINUX_FILE_C_

#include "wings/base/allocators.c"
#include "wings/base/types.c"

error
read_file(struct buffer    *buffer,
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
    error error = allocate_array(&buffer->data, allocator, buffer->size, u8);
    if (error)
        return (2);
    fread(buffer->data, 1, buffer->size, InputFile);
    fclose(InputFile);

    return (0);
}

#endif
