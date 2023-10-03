#ifndef WINGS_BASE_MACROS_C_
#define WINGS_BASE_MACROS_C_

#define ARRAY_LENGTH(x) \
    (sizeof(x) / sizeof((x)[0]))

#define UNUSED(x) \
    (void)(x)

#define WARN(x) \
    printf("[WARNING] %s\n", x)

#define IF_ERROR_RETURN(x) \
    if ((x))            \
        return x;

#ifndef ASSERT
#ifdef NO_ASSERTS
#define ASSERT(argument)
#else
#include <assert.h>
#define ASSERT(argument) assert(argument)
#endif
#endif

#define NAME_AND_COMMA(Token) Token,
#define NAME_AS_STRING_AND_COMMA(Token) #Token,

#endif
