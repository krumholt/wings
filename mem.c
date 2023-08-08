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

struct linear_growing_allocator
{
    struct memory_block_stack *memory_block;

    u64 number_of_blocks;
    u64 min_block_size;
};

struct linear_fixed_size_allocator
{
    struct memory_block_stack memory_block;
};

enum allocator_type
{
    allocator_type_linear_growing_allocator,
    allocator_type_linear_fixed_size_allocator,
};

struct allocator
{
    enum allocator_type type;
    s32                 alignment;
    union
    {
        struct linear_growing_allocator    linear_growing_allocator;
        struct linear_fixed_size_allocator linear_fixed_size_allocator;
    } allocator;
};

struct allocator
make_linear_growing_allocator(void)
{
    struct allocator allocator = {
        .type                               = allocator_type_linear_growing_allocator,
        .alignment                          = 8,
        .allocator.linear_growing_allocator = {
                                               .memory_block   = 0,
                                               .min_block_size = 1024 * 1024,
                                               },
    };
    return (allocator);
}

u8 *
linear_growing_allocator_allocate(struct linear_growing_allocator *allocator, u64 alignment, u64 size)
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

        new_block->previous     = allocator->memory_block;
        allocator->memory_block = new_block;
        allocator->number_of_blocks += 1;
    }

    u8 *memory = allocator->memory_block->base + allocator->memory_block->used;
    allocator->memory_block->used += size;
    return (memory);
}

struct allocator
make_linear_fixed_size_allocator(u64 size)
{
    struct allocator allocator = {
        .type      = allocator_type_linear_fixed_size_allocator,
        .alignment = 8,
        .allocator.linear_fixed_size_allocator
            .memory_block.base
        = calloc(size, 1),
        .allocator.linear_fixed_size_allocator
            .memory_block.size
        = size,
    };
    return (allocator);
}

u8 *
linear_fixed_size_allocator_allocate(struct linear_fixed_size_allocator *allocator, u64 alignment, u64 size)
{
    size            = (size + alignment - 1) & ~(alignment - 1);
    u64 memory_left = allocator->memory_block.size - allocator->memory_block.used;
    assert(memory_left >= size);

    u8 *memory = allocator->memory_block.base
                 + allocator->memory_block.used;
    allocator->memory_block.used += size;
    return (memory);
}

u8 *
allocate(struct allocator *allocator, u64 size)
{
    switch (allocator->type)
    {
    case allocator_type_linear_growing_allocator:
        return linear_growing_allocator_allocate(
            &allocator->allocator.linear_growing_allocator,
            allocator->alignment, size);
    case allocator_type_linear_fixed_size_allocator:
        return linear_fixed_size_allocator_allocate(
            &allocator->allocator.linear_fixed_size_allocator,
            allocator->alignment, size);
    }
}

inline void
linear_growing_allocator_free_top_block(struct linear_growing_allocator *allocator)
{
    struct memory_block_stack *top = allocator->memory_block;
    allocator->memory_block        = top->previous;
    allocator->number_of_blocks -= 1;
    free(top);
}

void
linear_growing_allocator_clear(struct linear_growing_allocator *allocator)
{
    while (allocator->memory_block)
    {
        linear_growing_allocator_free_top_block(allocator);
    }
}

void
linear_fixed_size_allocator_clear(struct linear_fixed_size_allocator *allocator)
{
    for (s32 index = 0;
         index < allocator->memory_block.used;
         ++index)
    {
        allocator->memory_block.base[index] = 0;
    }
	allocator->memory_block.used = 0;
}

void
allocator_clear(struct allocator *allocator)
{
    switch (allocator->type)
    {
    case allocator_type_linear_growing_allocator:
    {
        linear_growing_allocator_clear(&allocator->allocator.linear_growing_allocator);
    }
    break;
    case allocator_type_linear_fixed_size_allocator:
    {
        linear_fixed_size_allocator_clear(&allocator->allocator.linear_fixed_size_allocator);
    }
    break;
    }
}

#endif
