#ifndef MEM_C
#define MEM_C

#include "types.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buffer
{
    u8 *base;
    u64 size;
};

struct stack_allocator
{
    u64           used;
    struct buffer buffer;
};

enum allocator_type
{
    allocator_type_stack_allocator,
};

struct allocator
{
    enum allocator_type    type;
	union
	{
		struct stack_allocator stack_allocator;
	};
};

u8 *
stack_allocator_allocate(struct stack_allocator *allocator, u64 size)
{
    u8 *memory = allocator->buffer.base + allocator->used;
    allocator->used += size;
    return (memory);
}

u8 *
allocate(struct allocator *allocator, u64 size)
{
    switch (allocator->type)
    {
    case allocator_type_stack_allocator:
    {
		return stack_allocator_allocate(&allocator->stack_allocator, size);
    }
    break;
    }
}


#endif
