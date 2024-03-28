#ifndef WINGS_BASE_PATHS_C_
#define WINGS_BASE_PATHS_C_

#include "types.h"
#include "errors.h"
#include "cstrings.h"
#include "macros.h"
#include "strings.h"
#include "paths.h"


error
path__from_cstring (struct path *path,
                    u64  cstring_length,
                    char *cstring,
                    struct allocator *allocator)
{
   ASSERT(cstring);
   error error = 0;
   if (cstring_length == 0)
   {
      error = string__from_cstring(
            &path->string,
            2,
            ".\\",
            allocator);
   }
   else
   {
      error = string__from_cstring(
            &path->string,
            cstring_length,
            cstring,
            allocator);
   }
   return (error);
}

char *
path__to_cstring(struct path path)
{
   return(path.string.start);
}

error
path__copy (struct path *target,
            struct path source,
            struct allocator *allocator)
{
   error error = string__from_cstring(
         &target->string,
         source.string.length,
         source.string.start,
         allocator);
   return (error);
}

error
path__append (struct path       *result,
              struct path        path,
              char              *cstring,
              struct allocator  *allocator)
{
   error error = 0;
   error = string__join_cstring(
         &result->string,
         path.string,
         strlen(cstring),
         cstring,
         allocator);

   return error;
}

error
path__set_to_child (struct path       *result,
                    char              *cstring,
                    struct allocator  *allocator)
{
   error error = 0;
   error = path__ensure_is_folder(result, allocator);
   IF_ERROR_RETURN(error);
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
   if (path->string.start[last_character_index] != '\\')
   {
      error error = string__join_cstring(
            &path->string,
            path->string,
            1,
            "\\",
            allocator);
      IF_ERROR_RETURN(error);
   }

   return (0);
}

error
path__base_name (struct string *base_name,
                 struct path path,
                 struct allocator *allocator)
{
   u64 index = 0;
   error error = 0;
   b32 found = cstring__get_last_index(&index, path.string.start, '\\');
   if (!found)
   {
      base_name->length = path.string.length;
      error = cstring__copy(&base_name->start, path.string.length, path.string.start, allocator);
      return (error);
   }
   u64 length = path.string.length - index - 1;
   error = cstring__copy(&base_name->start, length, path.string.start + index + 1, allocator);
   base_name->length = length;
   return(error);
}

error
path__remove_file_extension (struct string *result,
                             struct path path,
                             struct allocator *allocator)
{
   u64 index = 0;
   u64 last_separator_index = 0;
   error error = 0;
   b32 found = cstring__get_last_index(&last_separator_index, path.string.start, '\\');
   if (!found) last_separator_index = 0;
   found = cstring__get_last_index(&index, path.string.start, '.');
   if (!found || last_separator_index > index)
   {
      result->length = path.string.length;
      error = cstring__copy(&result->start, path.string.length, path.string.start, allocator);
      return (error);
   }
   u64 length = index;
   error = cstring__copy(&result->start, length, path.string.start, allocator);
   result->length = length;
   return(error);
}

void
path__set_to_parent(struct path *path)
{
   if (path->string.length <= 1)
      return;
   b32 removed_path_separator = 0;
   if (path->string.start[path->string.length - 1] == '\\')
   {
      path->string.length -= 1;
      path->string.start[path->string.length] = 0;
      removed_path_separator = 1;
   }
   u64 last_index = 0;
   b32 found = cstring__get_last_index(&last_index, path->string.start, '\\');
   if (!found)
   {
      if (removed_path_separator)
      {
         path->string.start[path->string.length] = '\\';
         path->string.length += 1;
         path->string.start[path->string.length] = 0;
      }
      return;
   }
   path->string.length = last_index + 1;
   path->string.start[path->string.length] = 0;
}

#endif
