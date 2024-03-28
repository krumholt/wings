#ifndef WINGS_BASE_TESTING_C_
#define WINGS_BASE_TESTING_C_

#include "types.h"

#include "testing.h"

#include <stdio.h>
#include <string.h>

struct tests _tests = {0};

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


void
begin_test(const char *name)
{
   _tests.run += 1;
   _tests.test_failed_asserts = 0;
   _tests.test_asserts        = 0;
   _tests.name                = name;
   _tests.name_printed        = 0;
}

void
end_test(void)
{
   if (_tests.test_failed_asserts)
   {
      _tests.failed += 1;
      printf("   %d / %d asserts failed.\n",
             _tests.test_failed_asserts,
             _tests.test_asserts);
      printf("\n");
   }
   else
   {
      printf("✅ %s [%d asserts]\n",_tests.name, _tests.test_asserts);
   }
}

void
print_summary(void)
{
   printf("   %d/%d successful _tests\n", _tests.run - _tests.failed, _tests.run);
}

#endif
