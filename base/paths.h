#ifndef WINGS_BASE_PATHS_H_
#define WINGS_BASE_PATHS_H_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"

#ifndef PATHS__DEFAULT_PATH_SIZE
#define PATHS__DEFAULT_PATH_SIZE 1024
#endif

struct path
{
   struct string string;
};

struct path path__copy (
      struct path *target,
      struct path source,
      struct allocator *allocator);
error path__append (struct path *path, char *cstring);
struct string path__base_name (struct path path);
void path__set_to_parent (struct path *path);

#endif
