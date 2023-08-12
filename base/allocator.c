#ifndef WINGS_BASE_ALLOCATOR_C_
#define WINGS_BASE_ALLOCATOR_C_

#include "wings/base/types.c"

#include "wings/profiler.c"
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

struct memory_interval
{
    u8 *from;
    u8 *to;
};

struct memory_partition
{
    u8 *from;
    u8 *to;
    u8 *current;
};

struct memory_partition_stack
{
    struct memory_partition_stack *previous;
    struct memory_partition        partition;
};

struct growing_linear_allocator
{
    struct memory_partition_stack *stack;

    u64 number_of_blocks;
    u64 min_block_size;
};

struct fixed_size_linear_allocator
{
    struct memory_partition_stack stack;
};

enum allocator_type
{
    allocator_type_growing_linear,
    allocator_type_fixed_size_linear,
    //    allocator_type_fixed_size_stack,
};

struct allocator
{
    enum allocator_type type;
    s32                 alignment;
    union
    {
        struct growing_linear_allocator    growing_linear_allocator;
        struct fixed_size_linear_allocator fixed_size_linear_allocator;
    };
};

struct memory_partition
make_memory_partition(u64 size)
{
    struct memory_partition partition = { 0 };

    partition.from    = calloc(size, 1);
    partition.to      = partition.from + size;
    partition.current = partition.from;
    return (partition);
}

struct allocator
make_growing_linear_allocator(u64 block_size)
{
    struct allocator allocator = {
        .type                     = allocator_type_growing_linear,
        .alignment                = 8,
        .growing_linear_allocator = {
                                     .stack          = 0,
                                     .min_block_size = block_size,
                                     },
    };
    return (allocator);
}

u8 *
linear_growing_allocator_allocate(struct growing_linear_allocator *allocator, u64 alignment, u64 size)
{
    size = (size + alignment - 1) & ~(alignment - 1);
    if (!allocator->stack
        || (allocator->stack->partition.current + size
            > allocator->stack->partition.to))
    {
        u64 new_block_size = max(size, allocator->min_block_size);

        struct memory_partition_stack *new_stack
            = calloc(1, sizeof(struct memory_partition_stack));
        new_stack->partition.from    = calloc(new_block_size, sizeof(u8));
        new_stack->partition.to      = new_stack->partition.from + new_block_size;
        new_stack->partition.current = new_stack->partition.from;

        new_stack->previous = allocator->stack;
        allocator->stack    = new_stack;
        allocator->number_of_blocks += 1;
    }

    u8 *memory = allocator->stack->partition.current;
    allocator->stack->partition.current += size;
    return (memory);
}

struct allocator
make_fixed_size_linear_allocator(u64 size)
{
    struct memory_partition memory    = make_memory_partition(size);
    struct allocator        allocator = {
               .type                                        = allocator_type_fixed_size_linear,
               .alignment                                   = 8,
               .fixed_size_linear_allocator.stack.partition = memory,
    };
    return (allocator);
}

u8 *
linear_fixed_size_allocator_allocate(struct fixed_size_linear_allocator *allocator, u64 alignment, u64 size)
{
    size            = (size + alignment - 1) & ~(alignment - 1);
    u64 memory_left = allocator->stack.partition.to
                      - allocator->stack.partition.current;
    if (memory_left < size)
        assert(0);

    u8 *memory = allocator->stack.partition.current;
    allocator->stack.partition.current += size;
    return (memory);
}

u8 *
allocate(struct allocator *allocator, u64 size)
{
    switch (allocator->type)
    {
    case allocator_type_growing_linear:
        return linear_growing_allocator_allocate(
            &allocator->growing_linear_allocator,
            allocator->alignment, size);
    case allocator_type_fixed_size_linear:
        return linear_fixed_size_allocator_allocate(
            &allocator->fixed_size_linear_allocator,
            allocator->alignment, size);
    }
}

inline void
linear_growing_allocator_free_top_block(struct growing_linear_allocator *allocator)
{
    struct memory_partition_stack *top = allocator->stack;
    allocator->stack                   = top->previous;
    allocator->number_of_blocks -= 1;
    free(top);
}

void
linear_growing_allocator_clear(struct growing_linear_allocator *allocator)
{
    while (allocator->stack)
    {
        linear_growing_allocator_free_top_block(allocator);
    }
}

void
linear_fixed_size_allocator_clear(struct fixed_size_linear_allocator *allocator)
{
    for (u8 *current = allocator->stack.partition.from;
         current <= allocator->stack.partition.current;
         ++current)
    {
        *current = 0;
    }
    allocator->stack.partition.current = allocator->stack.partition.from;
}

void
allocator_clear(struct allocator *allocator)
{
    switch (allocator->type)
    {
    case allocator_type_growing_linear:
    {
        linear_growing_allocator_clear(&allocator->growing_linear_allocator);
    }
    break;
    case allocator_type_fixed_size_linear:
    {
        linear_fixed_size_allocator_clear(&allocator->fixed_size_linear_allocator);
    }
    break;
    }
}

#endif
