#ifndef WINGS_BASE_ALLOCATORS_C_
#define WINGS_BASE_ALLOCATORS_C_

#include "wings/base/macros.c"
#include "wings/base/types.c"
#include "wings/os/memory.c"

struct buffer
{
    u8 *from;
    u8 *to;
    u8 *current;
};

struct buffer_stack
{
    struct buffer_stack *previous;
    struct buffer        buffer;
};

struct growing_linear_allocator
{
    struct buffer_stack *stack;

    u64 number_of_blocks;
    u64 min_block_size;
};

struct fixed_size_linear_allocator
{
    struct buffer_stack stack;
};

enum allocator_type
{
    allocator_type_growing_linear,
    allocator_type_fixed_size_linear,
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

error
allocate_buffer(struct buffer *buffer, u64 size)
{
    struct os_memory_block block = { 0 };

    error error = os_reserve_and_commit_memory(&block, size);
    if (error)
        return (1);
    buffer->from    = block.base;
    buffer->to      = block.base + block.size;
    buffer->current = block.base;
    return (0);
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

error
linear_growing_allocator_allocate(u8 **memory, struct growing_linear_allocator *allocator, u64 alignment, u64 size)
{
    size = (size + alignment - 1) & ~(alignment - 1);
    if (!allocator->stack
        || (allocator->stack->buffer.current + size
            > allocator->stack->buffer.to))
    {
        u64 new_block_size = max(size, allocator->min_block_size);

        struct os_memory_block block = { 0 };

        error error = os_reserve_and_commit_memory(&block,
                                                   sizeof(struct buffer_stack) + new_block_size);
        if (error)
            return (1);
        struct buffer_stack *new_stack = (struct buffer_stack *)block.base;
        new_stack->buffer.from         = block.base
                                 + sizeof(struct buffer_stack);
        new_stack->buffer.to      = new_stack->buffer.from + new_block_size;
        new_stack->buffer.current = new_stack->buffer.from;

        new_stack->previous = allocator->stack;
        allocator->stack    = new_stack;
        allocator->number_of_blocks += 1;
    }

    *memory = allocator->stack->buffer.current;
    allocator->stack->buffer.current += size;
    return (0);
}

error
make_fixed_size_linear_allocator(struct allocator *allocator, u64 size)
{
    allocator->type      = allocator_type_fixed_size_linear;
    allocator->alignment = 8;
    error error          = 0;
    error                = allocate_buffer(&allocator->fixed_size_linear_allocator.stack.buffer, size);
    return (error);
}

error
linear_fixed_size_allocator_allocate(u8 **memory, struct fixed_size_linear_allocator *allocator, u64 alignment, u64 size)
{
    size            = (size + alignment - 1) & ~(alignment - 1);
    u64 memory_left = allocator->stack.buffer.to
                      - allocator->stack.buffer.current;
    if (memory_left < size)
        return (1);

    *memory = allocator->stack.buffer.current;
    allocator->stack.buffer.current += size;
    return (0);
}

#define allocate_struct(pointer, allocator, type) \
    (*(pointer) = (type *)(0), allocate((u8 **)(pointer), (allocator), sizeof(type)))

#define allocate_array(pointer, allocator, n, type) \
    (*(pointer) = (type *)(0), allocate((u8 **)(pointer), (allocator), (n) * sizeof(type)))

error
allocate(u8 **memory, struct allocator *allocator, u64 size)
{
    switch (allocator->type)
    {
    case allocator_type_growing_linear:
        return linear_growing_allocator_allocate(
            memory,
            &allocator->growing_linear_allocator,
            allocator->alignment, size);
    case allocator_type_fixed_size_linear:
        return linear_fixed_size_allocator_allocate(
            memory,
            &allocator->fixed_size_linear_allocator,
            allocator->alignment, size);
    }
}

inline error
linear_growing_allocator_free_top_block(struct growing_linear_allocator *allocator)
{
    struct buffer_stack *top = allocator->stack;
    allocator->stack         = top->previous;
    allocator->number_of_blocks -= 1;
    struct os_memory_block block = { top->buffer.from, top->buffer.to - top->buffer.from };
    error error = os_release_memory(block);
    return (error);
}

error
linear_growing_allocator_clear(struct growing_linear_allocator *allocator)
{
    while (allocator->stack)
    {
        error error = linear_growing_allocator_free_top_block(allocator);
        if (error)
            return error;
    }
    return NO_ERROR;
}

void
linear_fixed_size_allocator_clear(struct fixed_size_linear_allocator *allocator)
{
    for (u8 *current = allocator->stack.buffer.from;
         current < allocator->stack.buffer.current;
         ++current)
    {
        *current = 0;
    }
    allocator->stack.buffer.current = allocator->stack.buffer.from;
}

error
allocator_clear(struct allocator *allocator)
{
    switch (allocator->type)
    {
    case allocator_type_growing_linear:
    {
        return linear_growing_allocator_clear(&allocator->growing_linear_allocator);
    }
    break;
    case allocator_type_fixed_size_linear:
    {
		linear_fixed_size_allocator_clear(&allocator->fixed_size_linear_allocator);
		return NO_ERROR;
    }
    break;
    }
}

#endif
