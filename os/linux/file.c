#ifndef WINGS_OS_LINUX_FILE_C_
#define WINGS_OS_LINUX_FILE_C_

#include "wings/base/allocators.c"
#include "wings/base/error_codes.c"
#include "wings/base/types.c"

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
	if (!file) return (ec_os_file__not_found);

	int result = fwrite(buffer.base, buffer.size, 1, file);
	if (result != 1)
		return (ec_os_file__write_failed);


	return (ec__no_error);
}

#endif
