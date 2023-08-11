#ifndef OS_FILE_C_
#define OS_FILE_C_

#include "base/types.h"
#include "base/memory.c"

error read_file(u8           **data,
               u32           *size,
               char          *file_path,
               b32            zero_terminate,
               struct allocator *allocator);

#endif
