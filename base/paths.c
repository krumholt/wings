#ifndef WINGS_BASE_PATHS_C_
#define WINGS_BASE_PATHS_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
#include "wings/base/cstrings.h"
#include "wings/base/macros.c"
#include "wings/base/strings.c"
#include "wings/base/paths.h"


error
path__from_cstring (struct path *path,
                    u64  cstring_length,
                    char *cstring,
                    struct allocator *allocator)
{
   ASSERT(cstring);
   error error = ec__no_error;
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
   return(path.string.first);
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

error
path__base_name (struct string *base_name,
                 struct path path,
                 struct allocator *allocator)
{
   u64 index = 0;
   error error = 0;
   error = cstring__get_last_index(&index, path.string.first, '\\');
   if (error)
   {
      base_name->length = path.string.length;
      error = cstring__copy(&base_name->first, path.string.length, path.string.first, allocator);
      return (error);
   }
   u64 length = path.string.length - index - 1;
   error = cstring__copy(&base_name->first, length, path.string.first + index + 1, allocator);
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
   error = cstring__get_last_index(&last_separator_index, path.string.first, '\\');
   if (error) last_separator_index = 0;
   error = cstring__get_last_index(&index, path.string.first, '.');
   if (error || last_separator_index > index)
   {
      result->length = path.string.length;
      error = cstring__copy(&result->first, path.string.length, path.string.first, allocator);
      return (error);
   }
   u64 length = index;
   error = cstring__copy(&result->first, length, path.string.first, allocator);
   result->length = length;
   return(error);
}

void
path__set_to_parent(struct path *path)
{
   if (path->string.length <= 1)
      return;
   b32 removed_path_separator = 0;
   if (path->string.first[path->string.length - 1] == '\\')
   {
      path->string.length -= 1;
      path->string.first[path->string.length] = 0;
      removed_path_separator = 1;
   }
   u64 last_index = 0;
   error error = 0;
   error = cstring__get_last_index(&last_index, path->string.first, '\\');
   if (error)
   {
      if (removed_path_separator)
      {
         path->string.first[path->string.length] = '\\';
         path->string.length += 1;
         path->string.first[path->string.length] = 0;
      }
      return;
   }
   path->string.length = last_index + 1;
   path->string.first[path->string.length] = 0;
}

#endif
