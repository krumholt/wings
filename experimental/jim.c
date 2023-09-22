#ifndef JIM_C_
#define JIM_C_

#include "wings/base/allocators.c"
#include "wings/base/types.c"
#include "wings/base/units.c"

#include <stdlib.h>

#define MAX_COMMAND_RESULT_SIZE (56 * 1024)
#define MAX_STRING_MEMORY_SIZE (56 * 1024)

struct bon_master
{
    u32   string_memory_used;
    u32   string_memory_size;
    char *string_memory;
    char *command_result;
} bon;

s32
main(s32 argument_count, char **argument_vector)
{
    struct allocator static_memory = make_growing_linear_allocator(4096 * 10);

    error error = NO_ERROR;
    if (error)
    {
        printf("Why u no giv memory?\n");
        exit(-1);
    }

    error = allocate_array(&bon.string_memory, &static_memory, MAX_STRING_MEMORY_SIZE, char);
    if (error)
    {
        printf("Why u no giv memory?\n");
        exit(-1);
    }
    error = allocate_array(&bon.command_result, &static_memory, MAX_COMMAND_RESULT_SIZE, char);
    if (error)
    {
        printf("Why u no giv memory?\n");
        exit(-1);
    }
    // check_available_compilers();
    // printf("I was build with %s,\n", COMPILED_WITH);
    // printf("%s", __BASE_FILE__);
    // compile("hello_world.c", "");
    printf("Hello wombat i got ");
	for (s32 index = 0; index < argument_count; ++index)
	{
		printf("%s", argument_vector[index]);
		if (index != argument_count-1)
			printf(", ");
	}
	printf("\n");
	printf(">>"__BASE_FILE__"\n");
}

#endif
