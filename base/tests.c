#ifndef WINGS_BASE_TESTS_C_
#define WINGS_BASE_TESTS_C_

#include "wings/base/types.c"

#include <stdio.h>
#include <string.h>

void
print_header(char *text)
{
    printf("%s\n", text);
    for (u32 index = 0; index < strlen(text); ++index)
    {
        printf("-");
    }
	printf("\n");
}

#define init_tests()     \
    int test_count  = 0; \
    int error_count = 0

#define begin_test(text)          \
    do                            \
    {                             \
        print_header(text); \
        error_count = 0;          \
    } while (0)

#define end_test()                        \
    printf("\n");                         \
    if (error_count)                      \
        printf("%d / %d tests failed.\n", \
               error_count, test_count);  \
    printf("\n")

#define test(f)                       \
    test_count++;                     \
    printf("%d: %s", test_count, #f); \
    if ((f))                          \
    {                                 \
        printf(" => SUCCESS\n");      \
    }                                 \
    else                              \
    {                                 \
        error_count++;                \
        printf(" => FAILED\n");       \
    }

#define summary() \
    printf("Ran %d tests with %d failed\n", test_count, error_count)

#endif
