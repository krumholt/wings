#ifndef TEST_C_
#define TEST_C_

#include "types.h"

#include <stdio.h>
#include <string.h>

void print_header(char *text, s32 header_width)
{
    printf("x");
    for (s32 index = 0; index < header_width - 2; ++index)
    {
        printf("=");
    }
    printf("x\n");
    printf("| %s", text);
    s32 string_size = (s32)strlen(text);
    for (s32 index = 0; index < header_width - string_size - 3; ++index)
    {
        printf(" ");
    }
    printf("|\n");
    printf("x");
    for (s32 index = 0; index < header_width - 2; ++index)
    {
        printf("=");
    }
    printf("x\n");
}

#define init_test        \
    int test_count  = 0; \
    int error_count = 0;

#define begin_test(text)          \
    do                            \
    {                             \
        print_header((text), 80); \
        error_count = 0;          \
        start(&stopwatch);        \
    } while (0)

#define end_test()                                                                                \
    printf("\n");                                                                                 \
    if (error_count)                                                                              \
        printf(SET_CONSOLE_RED "%d / %d tests failed.\n" RESET_CONSOLE, error_count, test_count); \
    printf("took %f seconds.\n", time_in_seconds);                                                \
    printf("\n")

#define test(f)                                                     \
    test_count++;                                                   \
    printf("%d: " #f, test_count);                                  \
    if (f)                                                          \
    {                                                               \
        printf(SET_CONSOLE_GREEN " => SUCCESS" RESET_CONSOLE "\n"); \
    }                                                               \
    else                                                            \
    {                                                               \
        error_count++;                                              \
        printf(SET_CONSOLE_RED " => FAILED\n" RESET_CONSOLE);       \
    }

#endif
