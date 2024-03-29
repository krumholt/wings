#ifndef WINGS_BASE_PATHS_H_
#define WINGS_BASE_PATHS_H_

#include "types.h"
#include "errors.h"
#include "allocators.h"


struct path
{
   struct string string;
};
typedef struct path Path;

error
path__from_cstring (struct path *path,
                    u64  cstring_length,
                    char *cstring,
                    struct allocator *allocator);

char *
path__to_cstring(struct path path);

error
path__copy (struct path *target,
            struct path source,
            struct allocator *allocator);

error
path__append (struct path       *result,
              struct path        path,
              char              *cstring,
              struct allocator  *allocator);

error
path__set_to_child (struct path       *path,
                    char              *child,
                    struct allocator  *allocator);

error
path__ensure_is_folder (struct path      *path,
                        struct allocator *allocator);


error
path__base_name (struct string      *base_name,
                 struct path         path,
                 struct allocator   *allocator);

error
path__remove_file_extension (struct string     *result,
                             struct path        path,
                             struct allocator  *allocator);

void
path__set_to_parent (struct path *path);

#endif
