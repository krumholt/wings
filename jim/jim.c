#ifndef WINGS_JIM_JIM_C_
#define WINGS_JIM_JIM_C_

#include "base/macros.h"
#include "base/types.h"
#include "base/units.h"
#include "base/errors.h"
#include "base/allocators.h"
#include "base/strings.h"
#include "os/process.c"
#include "os/file.c"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define JIM_MAX_OBJECT_FILES_PER_LIBRARY 100
#define JIM_MAX_INCLUDE_DIRECTORIES 100
#define JIM_MAX_LIBRARY_DIRECTORIES 100
#define JIM_MAX_LIBRARIES 100
#define JIM_MAX_OBJECT_FILES 1000

char *jims_brain = "jims_brain.c";


struct jim_include_directories
{
   char  **directories;
   u32     number_of_directories;
};

struct jim_object_file
{
   char  *target;
   char  *source;
   struct jim_include_directories
      include_directories;
   b32    compiled;
   b32    debug;
};

struct jim_library
{
   char                     *name;
   char                     *directory;
   u32                       number_of_object_files;
   struct jim_object_file   *object_files;
};


struct jim_executable
{
   char                    *output_file;
   char                    *output_directory;
   struct jim_object_file  *object_files;
   struct jim_library      *libraries;
   u32                      number_of_object_files;
   u32                      number_of_libraries;
   b32                      debug;
};

struct jim_compiler
{
   error (*compile)(struct string result, struct jim_object_file object_file);
   error (*link)(struct string result, struct jim_executable);
   error (*create_library)(struct string result, struct jim_library);
   error (*link_dll)(struct string result, struct jim_executable);
};


struct jim
{
   struct allocator        allocator;
   struct jim_compiler     default_compiler;
   error                   error;
   struct string           error_message;
   struct string           compilation_result;
   b32                     silent;
   b32                     default_compiler_set;
   b32                     on_windows;
} _jim = { 0 };

   error
_jim_string_append_vaargs(
      struct string *string,
      char          *format,
      va_list        arg_list)
{
   s32 chars_written = vsnprintf(
         string->start,
         string->length - 1,
         format,
         arg_list);
   if (chars_written < 0)
      return (make_error("Failed to append to string"));
   if ((u32)chars_written > string->length)
   {
      return (make_error("Failed to append to string"));
   }
   string->length -= chars_written;
   string->start += chars_written;

   return (0);
}

   error
_jim_string_append(struct string *string, char *format, ...)
{
   va_list arg_list;
   va_start(arg_list, format);
   error error = _jim_string_append_vaargs(string, format, arg_list);
   va_end(arg_list);
   return (error);
}

   error
_jim_msvc_compile(
      struct string command,
      struct jim_object_file object_file)
{
   error error = 0;
   error = _jim_string_append(&command, "cl /TC /c %s /nologo ", object_file.debug ? "/Zi /Od" : "/O2");
   IF_ERROR_RETURN(error);

   for (u32 index = 0;
         index < object_file.include_directories.number_of_directories;
         ++index)
   {
      error = _jim_string_append(&command, "/I %s ", object_file.include_directories.directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "/Fo%s %s",
         object_file.target,
         object_file.source
         );
   IF_ERROR_RETURN(error);

   return (0);
}

   error
_jim_msvc_link(struct string command, struct jim_executable executable)
{
   _jim_string_append(&command,
         "link /nologo /out:%s%s ",
         executable.output_directory,
         executable.output_file
         );
   if (executable.debug)
   {
      _jim_string_append(&command,
            "/debug "
            );
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      if (executable.libraries[index].directory)
         _jim_string_append(&command, "/libpath:%s ", executable.libraries[index].directory);
   }
   for (u32 index = 0;
         index < executable.number_of_object_files;
         ++index)
   {
      _jim_string_append(
            &command,
            "%s ",
            executable.object_files[index].target);
   }
   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "%s.lib ", executable.libraries[index].name);
   }
   return (0);
}

   error
_jim_clang_link_dll(struct string command, struct jim_executable executable)
{
   _jim_string_append(&command,
         "clang -shared ");

   if (executable.debug)
   {
      _jim_string_append(&command,
            "-O0 -g "
            );
   }
   else
   {
      _jim_string_append(&command,
            "-O2 "
            );
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      if (executable.libraries[index].directory)
         _jim_string_append(&command, "-L%s ",  executable.libraries[index].directory);
   }

   _jim_string_append(&command,
         "-o %s%s ",
         executable.output_directory,
         executable.output_file);

   for (u32 index = 0;
         index < executable.number_of_object_files;
         ++index)
   {
      _jim_string_append(&command, "%s ", executable.object_files[index].target);
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "-l%s ",  executable.libraries[index].name);
   }

   return (0);
}

   error
_jim_clang_compile(
      struct string command,
      struct jim_object_file object_file)
{
   error error = 0;
   error = _jim_string_append(&command, "clang -Wall -Wextra -c %s ", object_file.debug ? "-g -gcodeview -O0 " : "-O2 ");
   IF_ERROR_RETURN(error);

   for (u32 index = 0;
         index < object_file.include_directories.number_of_directories;
         ++index)
   {
      error = _jim_string_append(&command, "-I %s ", object_file.include_directories.directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "-o%s %s",
         object_file.target,
         object_file.source
         );
   IF_ERROR_RETURN(error);

   return (0);
}

   error
_jim_gcc_compile(
      struct string command,
      struct jim_object_file object_file)
{
   error error = 0;
   error = _jim_string_append(&command, "gcc -Wall -Wextra -c %s ", object_file.debug ? "-g -O0 " : "-O2 ");
   IF_ERROR_RETURN(error);

   for (u32 index = 0;
         index < object_file.include_directories.number_of_directories;
         ++index)
   {
      error = _jim_string_append(&command, "-I %s ", object_file.include_directories.directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "-o%s %s",
         object_file.target,
         object_file.source
         );
   IF_ERROR_RETURN(error);

   return (0);
}

   error
_jim_clang_link(struct string command, struct jim_executable executable)
{
   _jim_string_append(&command,
         "clang ");

   if (executable.debug)
   {
      _jim_string_append(&command,
            "-O0 -g "
            );
   }
   else
   {
      _jim_string_append(&command,
            "-O2 "
            );
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      if (executable.libraries[index].directory)
         _jim_string_append(&command, "-L%s ",  executable.libraries[index].directory);
   }

   _jim_string_append(&command,
         "-o %s%s ",
         executable.output_directory,
         executable.output_file);

   for (u32 index = 0;
         index < executable.number_of_object_files;
         ++index)
   {
      _jim_string_append(&command, "%s ", executable.object_files[index].target);
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "-l%s ",  executable.libraries[index].name);
   }

   return (0);
}

   error
_jim_gcc_link(struct string command, struct jim_executable executable)
{
   _jim_string_append(&command,
         "gcc ");

   if (executable.debug)
   {
      _jim_string_append(&command,
            "-O0 -g "
            );
   }
   else
   {
      _jim_string_append(&command,
            "-O2 "
            );
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      if (executable.libraries[index].directory)
         _jim_string_append(&command, "-L%s ",  executable.libraries[index].directory);
   }

   _jim_string_append(&command,
         "-o %s%s ",
         executable.output_directory,
         executable.output_file);

   for (u32 index = 0;
         index < executable.number_of_object_files;
         ++index)
   {
      _jim_string_append(&command, "%s ", executable.object_files[index].target);
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "-l%s ",  executable.libraries[index].name);
   }

   return (0);
}


   error
_jim_clang_create_library(struct string command, struct jim_library library)
{
   if(_jim.on_windows)
   {
      _jim_string_append(&command,
            "lib /OUT:%s%s.lib ",
            library.directory,
            library.name
            );

      for (u32 index = 0;
            index < library.number_of_object_files;
            ++index)
      {
         _jim_string_append(&command,
               "%s ",
               library.object_files[index].target
               );
      }
   }
   else
   {
      _jim_string_append(&command,
            "ar rcs %s%s.lib ",
            library.directory,
            library.name
            );

      for (u32 index = 0;
            index < library.number_of_object_files;
            ++index)
      {
         _jim_string_append(&command,
               "%s ",
               library.object_files[index].target
               );
      }
   }

   return (0);
}

   error
_jim_gcc_create_library(struct string command, struct jim_library library)
{
   _jim_string_append(&command,
         "ar rcs %s%s.lib ",
         library.directory,
         library.name
         );

   for (u32 index = 0;
         index < library.number_of_object_files;
         ++index)
   {
      _jim_string_append(&command,
            "%s ",
            library.object_files[index].target
            );
   }

   return (0);
}

   error
_jim_msvc_create_library(struct string command, struct jim_library library)
{
   _jim_string_append(&command,
         "lib /OUT:%s%s.lib ",
         library.directory,
         library.name
         );

   for (u32 index = 0;
         index < library.number_of_object_files;
         ++index)
   {
      _jim_string_append(&command,
            "%s ",
            library.object_files[index].target
            );
   }

   return (0);
}


struct jim_compiler jim_gcc_compiler = {
   .compile        = _jim_gcc_compile,
   .link           = _jim_gcc_link,
   .create_library = _jim_gcc_create_library,
};


struct jim_compiler jim_msvc_compiler = {
   .compile        = _jim_msvc_compile,
   .link           = _jim_msvc_link,
   .create_library = _jim_msvc_create_library,
};

struct jim_compiler jim_clang_compiler = {
   .compile        = _jim_clang_compile,
   .link           = _jim_clang_link,
   .create_library = _jim_clang_create_library,
   .link_dll       = _jim_clang_link_dll,
};

// --------------------
// jim please api
// --------------------

   void
_jim_please_set_error_message(char *format, ...)
{
   va_list arg_list;
   va_start(arg_list, format);
   vsnprintf(_jim.error_message.start, _jim.error_message.length - 1, format, arg_list);
   va_end(arg_list);
}

   void
jim_please_use_msvc(void)
{
   _jim.default_compiler_set = 1;
   _jim.default_compiler = jim_msvc_compiler;
}


void
_jim_update_yourself(void);

#define jim_please_listen() \
   _jim_please_listen(__FILE__, __LINE__);
   void
_jim_please_listen(char *file, s32 line)
{
   file_create_directory(".jim");
   error error = 0;
   error = make_fixed_size_linear_allocator(&_jim.allocator, mebibytes(1));

#ifdef _WIN32
   _jim.on_windows = 1;
#else
   _jim.on_windows = 0;
#endif

   error = string__new(&_jim.error_message, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      return;
   }
   error = string__new(&_jim.compilation_result, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: compilation_result allocation failed\n", file, line);
      return;
   }

   if (!_jim.default_compiler_set)
   {
      _jim.default_compiler = jim_gcc_compiler;
      error = run_command("gcc --version", _jim.compilation_result.length, _jim.compilation_result.start);
      if (error)
      {
         error = run_command("cl", _jim.compilation_result.length, _jim.compilation_result.start);
         if (error)
         {
            error = run_command("clang --version", _jim.compilation_result.length, _jim.compilation_result.start);
            if (error)
            {
               _jim.error = error;
               _jim_please_set_error_message("./%s:%d:0: error: Couldn't use gcc or cl\n", file, line);
               return;
            }
            else
            {
               _jim.default_compiler = jim_clang_compiler;
            }
         }
         else
         {
            _jim.default_compiler = jim_msvc_compiler;
         }
      }
   }
   // check if we need to rebuild ourselves
   u64 last_write_time_jim        = 0;
   u64 last_write_time_jims_brain = 0;
   file_get_last_write_time(&last_write_time_jim, "jim.exe");
   file_get_last_write_time(&last_write_time_jims_brain, jims_brain);
   if (last_write_time_jim < last_write_time_jims_brain)
   {
      printf("I need to update myself\n");
      _jim_update_yourself();
      if (file_exists("./.jim/old_jim.exe"))
      {
         file_delete("./.jim/old_jim.exe");
      }
      exit(0);
   }
   if (file_exists("./.jim/old_jim.exe"))
   {
      file_delete("./.jim/old_jim.exe");
   }
}

#define jim_please_compile(Object_file)\
   _jim_please_compile(Object_file, __FILE__, __LINE__)

   void
_jim_please_compile(struct jim_object_file object_file, char *file, s32 line)
{
   if (_jim.error)
      return;

   struct string command = {0};
   error error = string__new(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Ran out of memory.", file, line);
      return;
   }

   error = _jim.default_compiler.compile(
         command,
         object_file);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Ran out of memory.", file, line);
      return;
   }

   if (!_jim.silent)
   {
      printf("jim:'%s'\n", command.start);
   }
   error = run_command(command.start, _jim.compilation_result.length, _jim.compilation_result.start);
   if (error)
   {
      _jim.error = error;
      string_view error_message = error_to_string_view(error);
      _jim_please_set_error_message("./%s:%d:0: error: %s\n%.*s\n%s",
            file, line,
            command.start,
            (int)error_message.length,
            error_message.start,
            _jim.compilation_result.start);
      return;
   }
   printf("%s", _jim.compilation_result.start);
}

   void
jim_please_link(struct jim_executable executable)
{
   if (_jim.error)
      return;

   struct string command = {0};
   error error = string__new(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link(%s) ran out of memory.");
      return;
   }

   error = _jim.default_compiler.link(command, executable);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_build_object_file(%s) ran out of memory.");
      return;
   }

   if (!_jim.silent)
   {
      printf("jim:'%s'\n", command.start);
   }
   error = run_command(command.start, _jim.compilation_result.length, _jim.compilation_result.start);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link(%s):\n\t%s\nFailed with %d\n\n\n%s",
            executable.output_file,
            command.start,
            error,
            _jim.compilation_result.start);
      return;
   }
}

   void
jim_please_link_dll(struct jim_executable executable)
{
   if (_jim.error)
      return;

   struct string command = {0};
   error error = string__new(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link_dll(%s) ran out of memory.");
      return;
   }

   error = _jim.default_compiler.link_dll(command, executable);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_build_object_file(%s) ran out of memory.");
      return;
   }

   if (!_jim.silent)
   {
      printf("jim:'%s'\n", command.start);
   }
   error = run_command(command.start, _jim.compilation_result.length, _jim.compilation_result.start);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link_dll(%s):\n\t%s\nFailed with %d\n\n\n%s",
            executable.output_file,
            command.start,
            error,
            _jim.compilation_result.start);
      return;
   }
}


#define jim_please_build_library(Name, Directory, Number_of_object_files, Object_files) \
   _jim_please_build_library(Name, Directory, Number_of_object_files, Object_files, __FILE__, __LINE__)

   struct jim_library
_jim_please_build_library(char *name, char *directory, u32 number_of_object_files, struct jim_object_file *object_files, char *file, s32 line)
{
   if (_jim.error)
      return (struct jim_library){0};

   struct jim_library library =
   {
      .number_of_object_files = number_of_object_files,
      .object_files = object_files,
      .name = name,
      .directory = directory,
   };


   struct string command = {0};
   error error = string__new(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: failed to allocate string command.", file, line);
      return (struct jim_library){0};
   }
   error = _jim.default_compiler.create_library(command, library);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Was unable to build library with %d",
            file,
            line,
            error);
      return (struct jim_library){0};
   }
   if (!_jim.silent)
   {
      printf("jim:'%s'\n", command.start);
   }
   error = run_command(command.start, _jim.compilation_result.length, _jim.compilation_result.start);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: %s\nFailed with %d\n\n\n%s",
            file,
            line,
            command.start,
            error,
            _jim.compilation_result.start);
      return (struct jim_library){0};
   }
   return (library);
}

#define jim_please_copy_if_newer(From, To) \
   _jim_please_copy_if_newer(From, To, __FILE__, __LINE__);

   void
_jim_please_copy_if_newer(char *from, char *to, char *file, s32 line)
{
   if (_jim.error)
      return;
   error error = file_copy_if_newer(from, to);
   if (error)
   {
      _jim.error = error;
      string_view error_message = error_to_string_view(error);
      _jim_please_set_error_message("./%s:%d:0 error: Failed to copy %s to %s: %s", file, line, from, to, (int)error_message.length, error_message.start);
      return;
   }
}

#define jim_please_copy(From, To) \
   _jim_please_copy(From, To, __FILE__, __LINE__)

   void
_jim_please_copy(char *from, char *to, char *file, s32 line)
{
   if (_jim.error)
      return;
   error error = file_copy(from, to);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Failed to copy %s to %s", file, line, from, to);
      return;
   }
}

#define jim_please_move(From, To) \
   _jim_please_move(From, To, __FILE__, __LINE__)

   void
_jim_please_move(char *from, char *to, char *file, s32 line)
{
   if (_jim.error)
      return;
   error error = file_move(from, to);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Failed to move %s to %s", file, line, from, to);
      return;
   }
}

#define jim_please_create_directory(Name) \
   _jim_please_create_directory(Name, __FILE__, __LINE__)

   void
_jim_please_create_directory(char *name, char *file, s32 line)
{
   if (_jim.error)
      return;
   error error = file_create_directory(name);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Failed to create directory %s", file, line, name);
      return;
   }
}

#define jim_please_delete(Name) \
   _jim_please_delete(Name, __FILE__, __LINE__)

   void
_jim_please_delete(char *name, char *file, s32 line)
{
   if (_jim.error)
      return;
   if (!file_exists(name)) return;
   error error = file_delete(name);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Failed to delete %s", file, line, name);
      return;
   }
}

#define jim_please_run(Command, Working_directory) \
   _jim_please_run(Command, Working_directory, __FILE__, __LINE__)

   void
_jim_please_run(char *command, char *working_directory, char *file, s32 line)
{
   if (_jim.error)
      return;
   if (!_jim.silent)
   {
      printf("jim:'%s'\n", command);
   }
   error error = run_command_at(command, working_directory,
         _jim.compilation_result.length, _jim.compilation_result.start);
   if (error)
   {
      _jim.error = error;
      string_view error_message = error_to_string_view(error);
      _jim_please_set_error_message("./%s:%d:0: error: Failed to run %s in directory %s with %.*s", file, line, command, working_directory, (int)error_message.length, error_message.start);
      printf("%s", _jim.compilation_result.start);
      return;
   }
   printf("%s", _jim.compilation_result.start);
}

   s32
jim_did_we_win(void)
{
   if (_jim.error)
   {
      printf("%s\n", _jim.error_message.start);
      return (_jim.error);
   }
   return (0);
}

   void
_jim_update_yourself(void)
{
   file_create_directory(".jim/");
   char *include_directory = "./";
   struct jim_include_directories include_directories =
   {
      .number_of_directories = 1,
      .directories = &include_directory,
   };
   struct jim_object_file self = {
      .target = "./.jim/jim.o",
      .debug = 0,
      .source = jims_brain,
      .include_directories = include_directories,
   };
   jim_please_compile(self);

   struct jim_executable jim_exe =
   {
      .output_directory = "./.jim/",
      .output_file = "new_jim.exe",
      .number_of_libraries = 0,
      .libraries = 0,
      .number_of_object_files = 1,
      .object_files = &self,
   };
   jim_please_link(jim_exe);
   error error = jim_did_we_win();
   if (!error)
   {
      file_delete("./.jim/old_jim.exe");
      error = file_move("jim.exe", "./.jim/old_jim.exe");
      if (error)
         printf("I couldn't call the new jim\n");
      error = file_move("./.jim/new_jim.exe", "jim.exe");
      if (error)
         printf("I couldn't call the new jim\n");
      error = process_new("jim.exe", "./", 0);
      if (error)
         printf("I couldn't call the new jim\n");
   }
}

char *_jim_default_compile_flags_txt
= ""
"-Isource\n"
"-I./\n";

char *_jim_default_jims_brain
= ""
"#include \"wings/jim/jim.c\"\n"
"\n"
"s32\n"
"main(void)\n"
"{\n"
"    jim_please_listen();\n"
"\n"
"char *include_directories[] = { \"./\" };\n"
"struct jim_object_file main =\n"
"{\n"
"   .name = \"main.o\",\n"
"   .directory = \".build/\",\n"
"   .source_file = \"main.c\",\n"
"   .source_file_directory = \"source/\",\n"
"   .number_of_include_directories = ARRAY_LENGTH(include_directories),\n"
"   .include_directories = include_directories,\n"
"};\n"
"    jim_please_compile(main);\n"
"    jim_please_link(\n"
"       \".build/\",\n"
"       \"main.exe\",\n"
"       \"0\",\n"
"       \"0,\n"
"       \"1,\n"
"       \"&main\");\n"
"\n"
"    jim_did_we_win();\n"
"}\n";

char *_jim_default_main_c
= ""
"#include <stdio.h>\n"
"\n"
"int\n"
"main(void)\n"
"{\n"
"    printf(\"Hello, worlor\\n\");\n"
"    return (0);\n"
"}\n";

#define jim_please_setup_a_new_project() \
   do                                    \
{                                     \
   jims_brain = __FILE__;             \
   printf("%s\n", jims_brain);        \
   _jim_please_setup_a_new_project(); \
}                                     \
while (0)

   void
_jim_please_setup_a_new_project(void)
{
   printf("\n");
   printf("********************************************************************************\n");
   printf("* ATTENTION                     PLEASE READ                          ATTENTION *\n");
   printf("********************************************************************************\n");
   printf("\n");
   printf("This will override the following files:\n");
   printf("\t - %s\n", jims_brain);
   printf("\t - create directory source/\n");
   printf("\t - create directory .build/\n");
   printf("\t - create directory executable/\n");
   printf("\t - override .clang-format\n");
   printf("\t - override compile_flags.txt\n");
   printf("\t - override source/main.c\n");
   printf("Are you sure sir (y/n)?\n");
   printf("\n");
   printf("********************************************************************************\n");
   printf("* ATTENTION                     PLEASE READ                          ATTENTION *\n");
   printf("********************************************************************************\n");
   printf("\n");
   char answer = 0;
   answer = getchar();
   if (answer == 'y')
   {
      file_create_directory("source");
      file_create_directory(".build");
      file_create_directory("executable");
      struct buffer buffer = { 0 };
      buffer.base          = (u8 *)_jim_default_jims_brain;
      buffer.used          = strlen(_jim_default_jims_brain);
      buffer.size          = buffer.used;
      file_write(buffer, jims_brain, 1);
      buffer.base = (u8 *)_jim_default_compile_flags_txt;
      buffer.used = strlen(_jim_default_compile_flags_txt);
      buffer.size = buffer.used;
      file_write(buffer, "compile_flags.txt", 1);
      buffer.base = (u8 *)_jim_default_main_c;
      buffer.used = strlen(_jim_default_main_c);
      buffer.size = buffer.used;
      file_write(buffer, "source/main.c", 1);
   }
   jim_please_listen();
}

#endif
