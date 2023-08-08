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

struct memory_block_stack
{
    struct memory_block_stack *previous;

    u8 *base;
    u64 size;
    u64 used;
};

struct stack_allocator
{
    struct memory_block_stack *memory_block;

	u64 number_of_blocks;
    u64 min_block_size;
};

enum allocator_type
{
    allocator_type_stack_allocator,
};

struct allocator
{
    enum allocator_type type;
    s32                 alignment;
    union
    {
        struct stack_allocator stack_allocator;
    } allocator;
};

struct allocator
make_stack_allocator(void)
{
    struct allocator allocator = {
        .type            = allocator_type_stack_allocator,
        .alignment       = 8,
        .allocator.stack_allocator = {
			.memory_block = 0,
			.min_block_size = 1024 * 1024,
		},
    };
    return (allocator);
}

u8 *
stack_allocator_allocate(struct stack_allocator *allocator, u64 alignment, u64 size)
{
    size = (size + alignment - 1) & ~(alignment - 1);
    if (!allocator->memory_block
        || (allocator->memory_block->size - allocator->memory_block->used < size))
    {
        u64 new_block_size = max(size, allocator->min_block_size);

        struct memory_block_stack *new_block
            = calloc(1, sizeof(struct memory_block_stack));
		new_block->size = new_block_size;
		new_block->base = calloc(new_block_size, sizeof(u8));

		new_block->previous = allocator->memory_block;
		allocator->memory_block = new_block;
		allocator->number_of_blocks +=  1;
    }

    u8 *memory       = allocator->memory_block->base + allocator->memory_block->used;
    allocator->memory_block->used += size;
    return (memory);
}

u8 *
allocate(struct allocator *allocator, u64 size)
{
    switch (allocator->type)
    {
    case allocator_type_stack_allocator:
    {
        return stack_allocator_allocate(&allocator->allocator.stack_allocator,
                                        allocator->alignment, size);
    }
    break;
    }
}

inline void
stack_allocator_free_top_block(struct stack_allocator *allocator)
{
	struct memory_block_stack *top = allocator->memory_block;
	allocator->memory_block = top->previous;
	allocator->number_of_blocks -= 1;
	free(top);
}

void
stack_allocator_clear(struct stack_allocator *allocator)
{
	while(allocator->memory_block)
	{
		stack_allocator_free_top_block(allocator);
	}

}

void
allocator_clear(struct allocator *allocator)
{
    switch (allocator->type)
    {
    case allocator_type_stack_allocator:
    {
        stack_allocator_clear(&allocator->allocator.stack_allocator);
    }
    break;
    }
}

#endif
