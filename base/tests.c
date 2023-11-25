#ifndef WINGS_BASE_TESTS_C_
#define WINGS_BASE_TESTS_C_

#include "wings/base/types.h"

#include <stdio.h>
#include <string.h>

void
print_header(const char *text)
{
   printf("%s\n", text);
   for (u32 index = 0; index < strlen(text); ++index)
   {
      printf("-");
   }
   printf("\n");
}

struct tests
{
   u32         run;
   u32         failed;
   u32         test_asserts;
   u32         test_failed_asserts;
   const char *name;
   u32         name_printed;
} tests = { 0 };

void
begin_test(const char *name)
{
   tests.run += 1;
   tests.test_failed_asserts = 0;
   tests.test_asserts        = 0;
   tests.name                = name;
   tests.name_printed        = 0;
}

void
end_test(void)
{
   if (tests.test_failed_asserts)
   {
      tests.failed += 1;
      printf("   %d / %d asserts failed.\n",
             tests.test_failed_asserts,
             tests.test_asserts);
      printf("\n");
   }
   else
   {
      printf("✅ %s\n",tests.name);
   }
}

#define assert_equal_cstrings(A, B)                      \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if (strcmp((A), (B)) != 0)\
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("'%s' != '%s'\n", A, B);                 \
      }                                                  \
   }                                                     \
   while (0)

#define assert_equal(A, B)                               \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if (!((A) == (B)))                                 \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("%s != %s\n", #A, #B);                   \
      }                                                  \
      else                                               \
      {                                                  \
      }                                                  \
   }                                                     \
   while (0)

#define assert_greater(A, B)                             \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if (!((A) > (B)))                                  \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("%s <= %s\n", #A, #B);                   \
      }                                                  \
      else                                               \
      {                                                  \
      }                                                  \
   }                                                     \
   while (0)

#define test(f)                                          \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if (!(f))                                          \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("   (%s) failed\n", #f);                 \
      }                                                  \
      else                                               \
      {                                                  \
      }                                                  \
   }                                                     \
   while (0)

void
print_summary(void)
{
   printf("   %d/%d successful tests\n", tests.run - tests.failed, tests.run);
}

#endif
