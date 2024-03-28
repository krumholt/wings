#ifndef WINGS_BASE_TESTING_H_
#define WINGS_BASE_TESTING_H_

#include "types.h"

#include <stdio.h>
#include <string.h>

struct tests
{
   u32         run;
   u32         failed;
   u32         test_asserts;
   u32         test_failed_asserts;
   const char *name;
   u32         name_printed;
};

void
print_header(const char *text);

void
print_summary(void);

void
begin_test(const char *name);

void
end_test(void);

#define assert_equal_cstrings(A, B)                      \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if (strcmp((A), (B)) != 0)                         \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("'%s' != '%s'\n", #A, #B);               \
         printf("'%s' != '%s'\n", A, B);                 \
      }                                                  \
   }                                                     \
   while (0)

#define assert_equal_u64(A, B)                           \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if ((A) != (B))                                    \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("'%s' != '%s'\n", #A, #B);               \
         printf("'%llu' != '%llu'\n", A, B);             \
      }                                                  \
   }                                                     \
   while (0)
#define assert_equal_int(A, B)                           \
   tests.test_asserts += 1;                              \
   do                                                    \
   {                                                     \
      if ((A) != (B))                                    \
      {                                                  \
         printf("❌ %s\n",tests.name);                   \
         tests.test_failed_asserts += 1;                 \
         printf("%s:%d:0: error: ", __FILE__, __LINE__); \
         printf("'%s' != '%s'\n", #A, #B);               \
         printf("'%d' != '%d'\n", A, B);                 \
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

#define assert_true(f)                                   \
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

#endif
