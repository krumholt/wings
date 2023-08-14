#ifndef WINGS_BASE_ALLOCATORS_C_
#define WINGS_BASE_ALLOCATORS_C_

#include "wings/base/macros.c"
#include "wings/base/types.c"
#include "wings/os/memory.c"

struct memory_block
{
    struct os_allocation allocation;
    u64                  used;
};

struct memory_block_stack_node
{
    struct memory_block_stack_node *previous;
    struct memory_block             block;
};

struct memory_block_stack
{
    struct memory_block_stack_node
        *top;
    u32  number_of_nodes;
    u64  total_memory_allocated;
};

struct growing_linear_allocator
{
    struct memory_block_stack stack;

    u64 min_block_size;
};

struct fixed_size_linear_allocator
{
    struct memory_block_stack stack;
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
_allocate(struct memory_block *allocation, u64 size)
{
    error error = os_reserve_and_commit_memory(&allocation->allocation, size);
    if (error)
        return (1);
    allocation->used = 0;
    return (0);
}

struct allocator
make_growing_linear_allocator(u64 block_size)
{
    struct allocator allocator = {
        .type                     = allocator_type_growing_linear,
        .alignment                = 8,
        .growing_linear_allocator = {
                                     .min_block_size = block_size,
                                     },
    };
    return (allocator);
}

error
linear_growing_allocator_allocate(u8 **memory, struct growing_linear_allocator *allocator, u64 alignment, u64 size)
{
    size = (size + alignment - 1) & ~(alignment - 1);
    if (!allocator->stack.top
        || (allocator->stack.top->block.used + size
            > allocator->stack.top->block.allocation.size))
    {
        u64 new_block_size = max(size, allocator->min_block_size);

        struct os_allocation allocation = { 0 };

        error error = os_reserve_and_commit_memory(&allocation,
                                                   sizeof(struct memory_block_stack) + new_block_size);
        if (error)
            return (1);
        struct memory_block_stack_node *new_node
            = (struct memory_block_stack_node *)allocation.base;
        new_node->block.allocation = allocation;
        new_node->block.used       = sizeof(struct memory_block_stack_node);

        new_node->previous = allocator->stack.top;
        allocator->stack.top   = new_node;
        allocator->stack.number_of_nodes += 1;
    }

    *memory = allocator->stack.top->block.allocation.base + allocator->stack.top->block.used;
    allocator->stack.top->block.used += size;
    return (0);
}

error
make_fixed_size_linear_allocator(struct allocator *allocator, u64 size)
{
	struct os_allocation allocation = { 0 };
	error error = os_reserve_and_commit_memory(&allocation,
			sizeof(struct memory_block_stack_node) + size);
	if (error)
		return (1);
	struct memory_block_stack_node *new_node
		= (struct memory_block_stack_node *)allocation.base;
	new_node->block.allocation = allocation;
	new_node->block.used       = sizeof(struct memory_block_stack_node);
    allocator->type      = allocator_type_fixed_size_linear;
    allocator->alignment = 8;
    allocator->fixed_size_linear_allocator.stack.top = new_node;
    return (0);
}

error
linear_fixed_size_allocator_allocate(u8 **memory, struct fixed_size_linear_allocator *allocator, u64 alignment, u64 size)
{
    size            = (size + alignment - 1) & ~(alignment - 1);
    u64 memory_left = allocator->stack.top->block.allocation.size
                      - allocator->stack.top->block.used;
    if (memory_left < size)
        return (1);

    *memory = allocator->stack.top->block.allocation.base;
    allocator->stack.top->block.used += size;
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
    struct memory_block_stack *top = allocator->stack;
    allocator->stack               = top->previous;
    allocator->number_of_blocks -= 1;
    struct os_allocation block = { top->allocation.from, top->allocation.to - top->allocation.from };
    error                error = os_release_memory(block);
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
    for (u8 *current = allocator->stack.allocation.from;
         current < allocator->stack.allocation.current;
         ++current)
    {
        *current = 0;
    }
    allocator->stack.allocation.current = allocator->stack.allocation.from;
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
