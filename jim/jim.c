#ifndef JIM_C_
#define JIM_C_

#include "wings/base/macros.c"
#include "wings/base/types.c"
#include "wings/base/units.c"
#include "wings/base/error_codes.c"
#include "wings/base/allocators.c"
#include "wings/base/strings.c"
#include "wings/os/process.c"
#include "wings/os/file.c"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define JIM_MAX_OBJECT_FILES_PER_LIBRARY 100
#define JIM_MAX_INCLUDE_DIRECTORIES 100
#define JIM_MAX_LIBRARY_DIRECTORIES 100
#define JIM_MAX_LIBRARIES 100
#define JIM_MAX_OBJECT_FILES 1000

char *jims_brain = "jims_brain.c";


struct jim_object_file
{
   char  *name;
   char  *directory;
   char  *source_file;
   char  *source_file_directory;
   u32    number_of_include_directories;
   char **include_directories;
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
      string->first,
      string->length - 1,
      format,
      arg_list);
   if (chars_written < 0)
      return (ec_jim__string_append_failed);
   if ((u32)chars_written > string->length)
   {
      return (ec_jim__string_append_failed);
   }
   string->length -= chars_written;
   string->first += chars_written;

   return (ec__no_error);
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
         index < object_file.number_of_include_directories;
         ++index)
   {
      error = _jim_string_append(&command, "/I %s ", object_file.include_directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "/Fo%s%s %s%s",
         object_file.directory,
         object_file.name,
         object_file.source_file_directory,
         object_file.source_file
         );
   IF_ERROR_RETURN(error);

   return (ec__no_error);
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
            "%s%s ",
            executable.object_files[index].directory,
            executable.object_files[index].name);
   }
   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "%s.lib ", executable.libraries[index].name);
   }
   return (ec__no_error);
}

error
_jim_clang_compile(
      struct string command,
      struct jim_object_file object_file)
{
   error error = 0;
   error = _jim_string_append(&command, "clang -Wall -Wextra -c %s ", object_file.debug ? "-g -O0 " : "-O2 ");
   IF_ERROR_RETURN(error);

   for (u32 index = 0;
         index < object_file.number_of_include_directories;
         ++index)
   {
      error = _jim_string_append(&command, "-I %s ", object_file.include_directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "-o%s%s %s%s",
         object_file.directory,
         object_file.name,
         object_file.source_file_directory,
         object_file.source_file
         );
   IF_ERROR_RETURN(error);

   return (ec__no_error);
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
         index < object_file.number_of_include_directories;
         ++index)
   {
      error = _jim_string_append(&command, "-I %s ", object_file.include_directories[index]);
      IF_ERROR_RETURN(error);
   }
   error = _jim_string_append(
         &command,
         "-o%s%s %s%s",
         object_file.directory,
         object_file.name,
         object_file.source_file_directory,
         object_file.source_file
         );
   IF_ERROR_RETURN(error);

   return (ec__no_error);
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
      _jim_string_append(&command, "%s%s ", executable.object_files[index].directory, executable.object_files[index].name);
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "-l%s ",  executable.libraries[index].name);
   }

   return (ec__no_error);
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
      _jim_string_append(&command, "%s%s ", executable.object_files[index].directory, executable.object_files[index].name);
   }

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
      _jim_string_append(&command, "-l%s ",  executable.libraries[index].name);
   }

   return (ec__no_error);
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
                            "%s%s ",
                            library.object_files[index].directory,
                            library.object_files[index].name
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
                            "%s%s ",
                            library.object_files[index].directory,
                            library.object_files[index].name
                           );
      }
   }

   return (ec__no_error);
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
            "%s%s ",
            library.object_files[index].directory,
            library.object_files[index].name
            );
   }

   return (ec__no_error);
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
            "%s%s ",
            library.object_files[index].directory,
            library.object_files[index].name
            );
   }

   return (ec__no_error);
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
};

// --------------------
// jim please api
// --------------------

void
_jim_please_set_error_message(char *format, ...)
{
   va_list arg_list;
   va_start(arg_list, format);
   vsnprintf(_jim.error_message.first, _jim.error_message.length - 1, format, arg_list);
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
   error error                        = 0;
   _jim.allocator                     = make_growing_linear_allocator(mebibytes(1));

#ifdef _WIN32
   _jim.on_windows = 1;
#else
   _jim.on_windows = 0;
#endif

   error = make_string(&_jim.error_message, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      return;
   }
   error = make_string(&_jim.compilation_result, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: compilation_result allocation failed\n", file, line);
      return;
   }

   if (!_jim.default_compiler_set)
   {
      _jim.default_compiler = jim_gcc_compiler;
      error = run_command("gcc --version", _jim.compilation_result.first, _jim.compilation_result.length);
      if (error)
      {
         error = run_command("cl", _jim.compilation_result.first, _jim.compilation_result.length);
         if (error)
         {
            error = run_command("clang --version", _jim.compilation_result.first, _jim.compilation_result.length);
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
      if (file_exists("old_jim.exe"))
      {
         file_delete("old_jim.exe");
      }
      exit(0);
   }
   if (file_exists("old_jim.exe"))
   {
      file_delete("old_jim.exe");
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
   error error = make_string(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Ran out of memory.", file, line);
   }

   error = _jim.default_compiler.compile(
         command,
         object_file);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Ran out of memory.", file, line);
   }

   if (!_jim.silent)
   {
      printf("%s\n", command.first);
   }
   error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: \n\t%s\nFailed with %d\n\n\n%s",
                                    file, line,
                                    command.first,
                                    error,
                                    _jim.compilation_result.first);
      return;
   }
}

void
jim_please_link(struct jim_executable executable)
{
   if (_jim.error)
      return;

   struct string command = {0};
   error error = make_string(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link(%s) ran out of memory.");
   }

   error = _jim.default_compiler.link(command, executable);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_build_object_file(%s) ran out of memory.");
   }

   if (!_jim.silent)
   {
      printf("%s\n", command.first);
   }
   error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_link(%s):\n\t%s\nFailed with %d\n\n\n%s",
                                    executable.output_file,
                                    command.first,
                                    error,
                                    _jim.compilation_result.first);
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
   error error = make_string(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: failed to allocate string command.", file, line);
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
      printf("%s\n", command.first);
   }
   error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error:\n\t%s\nFailed with %d\n\n\n%s",
                                    file,
                                    line,
                                    command.first,
                                    error,
                                    _jim.compilation_result.first);
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
       _jim_please_set_error_message("./%s:%d:0 error: Failed to copy %s to %s: %s", file, line, from, to, error_code_as_text[error]);
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
    }
}

#define jim_please_delete(Name) \
   _jim_please_delete(Name, __FILE__, __LINE__)

void
_jim_please_delete(char *name, char *file, s32 line)
{
   if (_jim.error)
      return;
    error error = file_delete(name);
    if (error)
    {
        if (error == ec_os_file__not_found) return;
       _jim.error = error;
       _jim_please_set_error_message("./%s:%d:0: error: Failed to delete %s", file, line, name);
    }
}

#define jim_please_run(Command, Working_directory, New_console) \
   _jim_please_run(Command, Working_directory, New_console, __FILE__, __LINE__)

void
_jim_please_run(char *command, char *working_directory, b32 new_console, char *file, s32 line)
{
   if (!_jim.silent)
      printf("%s\n", command);
   if (_jim.error)
      return;
   error error = process_new(command, working_directory, new_console);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("./%s:%d:0: error: Failed to run %s in directory %s", file, line, command, working_directory);
   }
}

s32
jim_did_we_win(void)
{
   if (_jim.error)
   {
      printf("I'm sorry sir. We failed because\n");
      printf("%s\n", _jim.error_message.first);
      return (_jim.error);
   }
   return (ec__no_error);
}

void
_jim_update_yourself(void)
{
   char *include_directory = "./";
   struct jim_object_file self = {
      .directory = "./",
      .name = "jim.o",
      .debug = 0,
      .source_file = jims_brain,
      .source_file_directory = "./",
      .number_of_include_directories = 1,
      .include_directories = &include_directory,
   };
   jim_please_compile(self);

   struct jim_executable jim_exe =
   {
      .output_directory = "./",
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
      file_delete("old_jim.exe");
      file_move("jim.exe", "old_jim.exe");
      file_move("new_jim.exe", "jim.exe");
      error = process_new("jim.exe", 0, 1);
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
      "#include \"wings/base/error_codes.c\"\n"
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
      "    jim_please_copy(\".build/main.exe\", \"executable/main.exe\");\n"
      "\n"
      "    return (jim_did_we_win());\n"
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
