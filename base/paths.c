#ifndef WINGS_BASE_PATHS_C_
#define WINGS_BASE_PATHS_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/cstrings.h"
#include "wings/base/macros.c"
#include "wings/base/strings.c"
#include "wings/base/paths.h"


/*
static b32
_is_path_seperator(char c)
{
   return c == '\\' || c == '/';
}
*/

error
path__from_cstring (struct path *path,
                    u64  cstring_length,
                    char *cstring,
                    struct allocator *allocator)
{
   error error = string__from_cstring(
         &path->string,
         cstring_length,
         cstring,
         allocator);
   return (error);
}

error
path__copy (struct path *target,
            struct path source,
            struct allocator *allocator)
{
   error error = string__from_cstring(
         &target->string,
         source.string.length,
         source.string.first,
         allocator);
   return (error);
}

error
path__append (struct path       *result,
              char              *cstring,
              struct allocator  *allocator)
{
   error error = 0;
   error = string__join_cstring(
         &result->string,
         result->string,
         strlen(cstring),
         cstring,
         allocator);

   return error;
}

error
path__ensure_is_folder (struct path      *path,
                        struct allocator *allocator)
{
   // the -1 is correct because paths can not be 0 length
   // the 'smallest' path is "."
   u64 last_character_index = path->string.length - 1;
   if (path->string.first[last_character_index] != '\\')
   {
      error error = string__join_cstring(
            &path->string,
            path->string,
            1,
            "\\",
            allocator);
      IF_ERROR_RETURN(error);
   }

   return (ec__no_error);
}

/*
error
path__ensure_is_folder (struct path      *path,
                        struct allocator *allocator)
{
   // the -1 is correct because paths can not be 0 length
   // the 'smallest' path is "."
   //u64 last_character_index = path.string.length - 1;
   //if (path.string.first[last_character_index] == '\\')
   //   return ec__no_error;

   //string_append path.string.length
   //if (path.string.first[
}
*/

/*
struct path
make_path(char *string, u64 size)
{
   ASSERT(size < MAX_PATH_SIZE);

   struct path path = { 0 };
   path.used        = 0;

   if (string)
   {
      while (size)
      {
         path.string[path.used++] = *string;
         string += 1;
         size -= 1;
      }
   }
   path.string[path.used++] = 0;

   return (path);
}

void
copy_path(struct path *target, struct path source)
{
   target->used = source.used;
   for (u32 index = 0; index < MAX_PATH_SIZE; ++index)
   {
      target->string[index] = source.string[index];
   }
}

error
append_path(struct path *path, char *string, struct allocator *allocator)
{
   path->string = string_append
   u32 new_size = strlen(string) + path->string.length;
   char *new_path = 0;
   while (*string)
   {
      path->string[path->used++] = *string;
      string += 1;
   }
   path->string[path->used++] = 0;
}

void
set_to_parent(struct path *path)
{
   s32 last_character = path->used - 1;
   assert(path->string[last_character] == 0);
   last_character -= 1;
   if (_is_path_seperator(path->string[last_character]))
      last_character -= 1;
   while (last_character >= 0)
   {
      if (_is_path_seperator(path->string[last_character]))
      {
         path->string[last_character + 1] = 0;
         path->used                       = last_character + 2;
         return;
      }
      last_character -= 1;
   }
}
*/

#endif
