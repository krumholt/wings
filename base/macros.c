#ifndef MACROS_C_
#define MACROS_C_

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(x) \
    (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef UNUSED
#define UNUSED(x) \
    (void)(x)
#endif


#endif
