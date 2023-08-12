#ifndef WINGS_BASE_MACROS_C_
#define WINGS_BASE_MACROS_C_

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x) \
    (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef UNUSED
#define UNUSED(x) \
    (void)(x)
#endif

#ifndef WARN
#define WARN(x) \
    printf("[WARNING] %s\n", x)
#endif

#ifndef ASSERT
#ifdef NO_ASSERTS
#define ASSERT(argument)
#else
#include <assert.h>
#define ASSERT(argument) assert(argument)
#endif
#endif


#endif
