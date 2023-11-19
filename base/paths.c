#ifndef WINGS_BASE_PATHS_C_
#define WINGS_BASE_PATHS_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"
#include "wings/base/strings.c"

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE 1024
#endif

struct path
{
   char string[MAX_PATH_SIZE];
   u64  used;
};

struct path   make_path(char *string, u64 size);
void          copy_path(struct path *target, struct path source);
void          append_path(struct path *path, char *string);
struct string base_name(struct path path);
void          set_to_parent(struct path *path);

static b32
_is_path_seperator(char c)
{
   return c == '\\' || c == '/';
}

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

void
append_path(struct path *path, char *string)
{
   path->used -= 1; // put cursor on 0 terminator
   while (*string)
   {
      assert(path->used + 1 <= MAX_PATH_SIZE);
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

#endif
