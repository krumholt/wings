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

} _jim = { 0 };

error
_jim_string_append_vaargs(
    struct string *string,
    char          *format,
    va_list        arg_list)
{
   s32 chars_written = vsnprintf(string->first,
         string->length - 1,
         format,
         arg_list);
   if (chars_written > string->length)
   {
      return (1);
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
   error = _jim_string_append(&command, "cl /c %s /nologo ", object_file.debug ? "/Zi /Od" : "/O2");
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

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
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
_jim_gcc_compile(
      struct string command,
      struct jim_object_file object_file)
{
   error error = 0;
   error = _jim_string_append(&command, "gcc -c %s ", object_file.debug ? "-g -Od " : "-O2 ");
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
_jim_gcc_link(struct string command, struct jim_executable executable)
{
   _jim_string_append(&command,
         "gcc -O2 ");

   for (u32 index = 0;
         index < executable.number_of_libraries;
         ++index)
   {
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
   .compile = _jim_gcc_compile,
   .link = _jim_gcc_link,
   .create_library = _jim_gcc_create_library,
};


struct jim_compiler jim_msvc_compiler = {
   .compile = _jim_msvc_compile,
   .link = _jim_msvc_link,
   .create_library = _jim_msvc_create_library,
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

void
jim_please_listen(void)
{
   error error                        = 0;
   _jim.allocator                     = make_growing_linear_allocator(mebibytes(1));

   error = make_string(&_jim.compilation_result, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[internal error(sry)] Failed to jim_please_listen():%d\n", __LINE__);
      return;
   }
   error = make_string(&_jim.error_message, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[internal error(sry)] Failed to jim_please_listen():%d\n", __LINE__);
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
            _jim.error = error;
            _jim_please_set_error_message("[ERROR] Couldn't use gcc or cl\n");
            return;
         }
         _jim.default_compiler = jim_msvc_compiler;
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

void
jim_please_compile(struct jim_object_file object_file)
{
   if (_jim.error)
      return;

   struct string command = {0};
   error error = make_string(&command, 4096 * 10, &_jim.allocator);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_compile(%s) ran out of memory.");
   }

   error = _jim.default_compiler.compile(
         command,
         object_file);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_compile(%s) ran out of memory.");
   }

   if (!_jim.silent)
   {
      printf("%s\n", command.first);
   }
   error = run_command(command.first, _jim.compilation_result.first, _jim.compilation_result.length);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_compile(%s, %s):\n\t%s\nFailed with %d\n\n\n%s",
                                    object_file.source_file, object_file.name,
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
      _jim_please_set_error_message("[ERROR] jim_please_create_executable(%s):\n\t%s\nFailed with %d\n\n\n%s",
                                    filename,
                                    command.first,
                                    error,
                                    _jim.compilation_result.first);
      return;
   }
}


struct jim_library
jim_please_build_library(char *name, char *directory, u32 number_of_object_files, struct jim_object_file *object_files)
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
      _jim_please_set_error_message("[ERROR] jim_please_build_library(%s) ran out of memory.");
   }
   error = _jim.default_compiler.create_library(command, library);
   if (error)
   {
      _jim.error = error;
      _jim_please_set_error_message("[ERROR] jim_please_build(%s). Was unable to build library with %d",
                                    name,
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
      _jim_please_set_error_message("[ERROR] jim_please_build_library(%s):\n\t%s\nFailed with %d\n\n\n%s",
                                    name,
                                    command.first,
                                    error,
                                    _jim.compilation_result.first);
      return (struct jim_library){0};
   }

   return (library);
}

void
jim_please_copy(char *from, char *to)
{
   if (_jim.error)
      return;
    error error = file_copy(from, to);
    if (error)
    {
       _jim.error = error;
       _jim_please_set_error_message("Failed to copy %s to %s", from, to);
    }
}

void
jim_please_move(char *from, char *to)
{
   if (_jim.error)
      return;
    error error = file_move(from, to);
    if (error)
    {
       _jim.error = error;
       _jim_please_set_error_message("Failed to move %s to %s", from, to);
    }
}

void
jim_please_delete(char *filename)
{
   if (_jim.error)
      return;
    error error = file_delete(filename);
    if (error)
    {
       _jim.error = error;
       _jim_please_set_error_message("Failed to delete %s", filename);
    }
}

void
jim_please_run(char *command, char *working_directory)
{
   if (_jim.error)
      return;
   error error = process_new(command, working_directory);
    if (error)
    {
       _jim.error = error;
       _jim_please_set_error_message("Failed to run %s in directory %s", command, working_directory);
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
   jim_please_link(
         "./",
         "new_jim.exe",
         0,
         0,
         1,
         &self
         );
   error error = jim_did_we_win();
   if (!error)
   {
      file_delete("old_jim.exe");
      file_move("jim.exe", "old_jim.exe");
      file_move("new_jim.exe", "jim.exe");
      error = process_new("jim.exe", 0);
      if (error)
         printf("I couldn't call the new jim\n");
   }
}

char *_jim_default_compile_flags_txt
    = ""
      "-Isource\n"
      "-I./\n";

char *_jim_default_clang_format
    = ""
      "---\n"
      "Language:        Cpp\n"
      "# BasedOnStyle:  WebKit\n"
      "AccessModifierOffset: -4\n"
      "AlignAfterOpenBracket: Align\n"
      "AlignArrayOfStructures: Left\n"
      "AlignConsecutiveAssignments:\n"
      "  Enabled:         true\n"
      "  AcrossEmptyLines: false\n"
      "  AcrossComments:  false\n"
      "  AlignCompound:   false\n"
      "  PadOperators:    true\n"
      "AlignConsecutiveBitFields:\n"
      "  Enabled:         false\n"
      "  AcrossEmptyLines: false\n"
      "  AcrossComments:  false\n"
      "  AlignCompound:   false\n"
      "  PadOperators:    false\n"
      "AlignConsecutiveDeclarations:\n"
      "  Enabled:         true\n"
      "  AcrossEmptyLines: false\n"
      "  AcrossComments:  false\n"
      "  AlignCompound:   true\n"
      "  PadOperators:    true\n"
      "AlignConsecutiveMacros:\n"
      "  Enabled:         false\n"
      "  AcrossEmptyLines: false\n"
      "  AcrossComments:  false\n"
      "  AlignCompound:   true\n"
      "  PadOperators:    true\n"
      "AlignEscapedNewlines: Right\n"
      "AlignOperands:   Align\n"
      "AlignTrailingComments: false\n"
      "AllowAllArgumentsOnNextLine: true\n"
      "AllowAllParametersOfDeclarationOnNextLine: true\n"
      "AllowShortEnumsOnASingleLine: true\n"
      "AllowShortBlocksOnASingleLine: Empty\n"
      "AllowShortCaseLabelsOnASingleLine: false\n"
      "AllowShortFunctionsOnASingleLine: All\n"
      "AllowShortLambdasOnASingleLine: All\n"
      "AllowShortIfStatementsOnASingleLine: Never\n"
      "AllowShortLoopsOnASingleLine: false\n"
      "AlwaysBreakAfterDefinitionReturnType: All\n"
      "AlwaysBreakAfterReturnType: None\n"
      "AlwaysBreakBeforeMultilineStrings: false\n"
      "AlwaysBreakTemplateDeclarations: MultiLine\n"
      "AttributeMacros:\n"
      "  - __capability\n"
      "BinPackArguments: true\n"
      "BinPackParameters: true\n"
      "BraceWrapping:\n"
      "  AfterCaseLabel:  true\n"
      "  AfterClass:      true\n"
      "  AfterControlStatement: Always\n"
      "  AfterEnum:       true\n"
      "  AfterFunction:   true\n"
      "  AfterNamespace:  false\n"
      "  AfterObjCDeclaration: false\n"
      "  AfterStruct:     true\n"
      "  AfterUnion:      true\n"
      "  AfterExternBlock: false\n"
      "  BeforeCatch:     false\n"
      "  BeforeElse:      true\n"
      "  BeforeLambdaBody: false\n"
      "  BeforeWhile:     true\n"
      "  IndentBraces:    false\n"
      "  SplitEmptyFunction: true\n"
      "  SplitEmptyRecord: true\n"
      "  SplitEmptyNamespace: true\n"
      "BreakBeforeBinaryOperators: All\n"
      "BreakBeforeConceptDeclarations: Always\n"
      "BreakBeforeBraces: Custom\n"
      "BreakBeforeInheritanceComma: false\n"
      "BreakInheritanceList: BeforeColon\n"
      "BreakBeforeTernaryOperators: true\n"
      "BreakConstructorInitializersBeforeComma: true\n"
      "BreakConstructorInitializers: BeforeComma\n"
      "BreakAfterJavaFieldAnnotations: false\n"
      "BreakStringLiterals: true\n"
      "ColumnLimit:     0\n"
      "CommentPragmas:  '^ IWYU pragma:'\n"
      "QualifierAlignment: Leave\n"
      "CompactNamespaces: false\n"
      "ConstructorInitializerIndentWidth: 4\n"
      "ContinuationIndentWidth: 4\n"
      "Cpp11BracedListStyle: false\n"
      "DeriveLineEnding: true\n"
      "DerivePointerAlignment: false\n"
      "DisableFormat:   false\n"
      "EmptyLineAfterAccessModifier: Never\n"
      "EmptyLineBeforeAccessModifier: LogicalBlock\n"
      "ExperimentalAutoDetectBinPacking: false\n"
      "PackConstructorInitializers: BinPack\n"
      "BasedOnStyle:    ''\n"
      "ConstructorInitializerAllOnOneLineOrOnePerLine: false\n"
      "AllowAllConstructorInitializersOnNextLine: true\n"
      "FixNamespaceComments: false\n"
      "ForEachMacros:\n"
      "  - foreach\n"
      "  - Q_FOREACH\n"
      "  - BOOST_FOREACH\n"
      "IfMacros:\n"
      "  - KJ_IF_MAYBE\n"
      "IncludeBlocks:   Preserve\n"
      "IncludeCategories:\n"
      "  - Regex:           '^\"(llvm|llvm-c|clang|clang-c)/'\n"
      "    Priority:        2\n"
      "    SortPriority:    0\n"
      "    CaseSensitive:   false\n"
      "  - Regex:           '^(<|\"(gtest|gmock|isl|json)/)'\n"
      "    Priority:        3\n"
      "    SortPriority:    0\n"
      "    CaseSensitive:   false\n"
      "  - Regex:           '.*'\n"
      "    Priority:        1\n"
      "    SortPriority:    0\n"
      "    CaseSensitive:   false\n"
      "IncludeIsMainRegex: '(Test)?$'\n"
      "IncludeIsMainSourceRegex: ''\n"
      "IndentAccessModifiers: false\n"
      "IndentCaseLabels: false\n"
      "IndentCaseBlocks: false\n"
      "IndentGotoLabels: true\n"
      "IndentPPDirectives: None\n"
      "IndentExternBlock: AfterExternBlock\n"
      "IndentRequiresClause: true\n"
      "IndentWidth:     4\n"
      "IndentWrappedFunctionNames: false\n"
      "InsertBraces:    false\n"
      "InsertTrailingCommas: None\n"
      "JavaScriptQuotes: Leave\n"
      "JavaScriptWrapImports: true\n"
      "KeepEmptyLinesAtTheStartOfBlocks: true\n"
      "LambdaBodyIndentation: Signature\n"
      "MacroBlockBegin: ''\n"
      "MacroBlockEnd:   ''\n"
      "MaxEmptyLinesToKeep: 1\n"
      "NamespaceIndentation: Inner\n"
      "ObjCBinPackProtocolList: Auto\n"
      "ObjCBlockIndentWidth: 4\n"
      "ObjCBreakBeforeNestedBlockParam: true\n"
      "ObjCSpaceAfterProperty: true\n"
      "ObjCSpaceBeforeProtocolList: true\n"
      "PenaltyBreakAssignment: 2\n"
      "PenaltyBreakBeforeFirstCallParameter: 19\n"
      "PenaltyBreakComment: 300\n"
      "PenaltyBreakFirstLessLess: 120\n"
      "PenaltyBreakOpenParenthesis: 0\n"
      "PenaltyBreakString: 1000\n"
      "PenaltyBreakTemplateDeclaration: 10\n"
      "PenaltyExcessCharacter: 1000000\n"
      "PenaltyReturnTypeOnItsOwnLine: 60\n"
      "PenaltyIndentedWhitespace: 0\n"
      "PointerAlignment: Right\n"
      "PPIndentWidth:   -1\n"
      "ReferenceAlignment: Pointer\n"
      "ReflowComments:  true\n"
      "RemoveBracesLLVM: false\n"
      "RequiresClausePosition: OwnLine\n"
      "SeparateDefinitionBlocks: Leave\n"
      "ShortNamespaceLines: 1\n"
      "SortIncludes: false\n"
      "SortJavaStaticImport: Before\n"
      "SortUsingDeclarations: true\n"
      "SpaceAfterCStyleCast: false\n"
      "SpaceAfterLogicalNot: false\n"
      "SpaceAfterTemplateKeyword: true\n"
      "SpaceBeforeAssignmentOperators: true\n"
      "SpaceBeforeCaseColon: false\n"
      "SpaceBeforeCpp11BracedList: true\n"
      "SpaceBeforeCtorInitializerColon: true\n"
      "SpaceBeforeInheritanceColon: true\n"
      "SpaceBeforeParens: ControlStatements\n"
      "SpaceBeforeParensOptions:\n"
      "  AfterControlStatements: true\n"
      "  AfterForeachMacros: true\n"
      "  AfterFunctionDefinitionName: false\n"
      "  AfterFunctionDeclarationName: false\n"
      "  AfterIfMacros:   true\n"
      "  AfterOverloadedOperator: false\n"
      "  AfterRequiresInClause: false\n"
      "  AfterRequiresInExpression: false\n"
      "  BeforeNonEmptyParentheses: false\n"
      "SpaceAroundPointerQualifiers: Default\n"
      "SpaceBeforeRangeBasedForLoopColon: true\n"
      "SpaceInEmptyBlock: true\n"
      "SpaceInEmptyParentheses: false\n"
      "SpacesBeforeTrailingComments: 1\n"
      "SpacesInAngles:  Never\n"
      "SpacesInConditionalStatement: false\n"
      "SpacesInContainerLiterals: true\n"
      "SpacesInCStyleCastParentheses: false\n"
      "SpacesInLineCommentPrefix:\n"
      "  Minimum:         1\n"
      "  Maximum:         -1\n"
      "SpacesInParentheses: false\n"
      "SpacesInSquareBrackets: false\n"
      "SpaceBeforeSquareBrackets: false\n"
      "BitFieldColonSpacing: Both\n"
      "Standard:        Latest\n"
      "StatementAttributeLikeMacros:\n"
      "  - Q_EMIT\n"
      "StatementMacros:\n"
      "  - Q_UNUSED\n"
      "  - QT_REQUIRE_VERSION\n"
      "TabWidth:        8\n"
      "UseCRLF:         false\n"
      "UseTab:          Never\n"
      "WhitespaceSensitiveMacros:\n"
      "  - STRINGIZE\n"
      "  - PP_STRINGIZE\n"
      "  - BOOST_PP_STRINGIZE\n"
      "  - NS_SWIFT_NAME\n"
      "  - CF_SWIFT_NAME\n"
      "...\n";

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
      buffer.base = (u8 *)_jim_default_clang_format;
      buffer.used = strlen(_jim_default_clang_format);
      buffer.size = buffer.used;
      file_write(buffer, ".clang-format", 1);
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
