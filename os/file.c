#ifndef WINGS_OS_FILE_C_
#define WINGS_OS_FILE_C_

#include "wings/base/types.h"
#include "wings/os/memory.c"


error read_file(u8           **data,
               u32           *size,
               char          *file_path,
               b32            zero_terminate,
               struct allocator *allocator);

#endif
