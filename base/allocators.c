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
};

struct growing_linear_allocator
{
    struct memory_block_stack stack;

    u64 min_block_size;
};

struct fixed_size_linear_allocator
{
    struct memory_block block;
};

enum allocator_type
{
    allocator_type_growing_linear,
    allocator_type_fixed_size_linear,
};

struct allocator
{
    enum allocator_type type;
    union
    {
        struct growing_linear_allocator    growing_linear_allocator;
        struct fixed_size_linear_allocator fixed_size_linear_allocator;
    };
    u64 total_memory_used;
    u64 total_memory_allocated;
    s32 alignment;
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
linear_growing_allocator_allocate(u8 **memory, struct allocator *allocator_, u64 size)
{
    struct growing_linear_allocator *allocator = &allocator_->growing_linear_allocator;

    size = (size + allocator_->alignment - 1) & ~(allocator_->alignment - 1);
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
        allocator_->total_memory_allocated += allocation.size;
        struct memory_block_stack_node *new_node
            = (struct memory_block_stack_node *)allocation.base;
        new_node->block.allocation = allocation;
        new_node->block.used       = sizeof(struct memory_block_stack_node);

        new_node->previous   = allocator->stack.top;
        allocator->stack.top = new_node;
        allocator->stack.number_of_nodes += 1;
    }

    *memory = allocator->stack.top->block.allocation.base + allocator->stack.top->block.used;
    allocator->stack.top->block.used += size;
    allocator_->total_memory_used += size;
    return (0);
}

error
make_fixed_size_linear_allocator(struct allocator *allocator_, u64 size)
{
    struct os_allocation allocation = { 0 };

    error error = os_reserve_and_commit_memory(&allocation,
                                               sizeof(struct memory_block_stack_node) + size);
    if (error)
        return (1);
    struct fixed_size_linear_allocator *allocator
        = &allocator_->fixed_size_linear_allocator;

    allocator->block.allocation        = allocation;
    allocator->block.used              = 0;
    allocator_->type                   = allocator_type_fixed_size_linear;
    allocator_->alignment              = 8;
    allocator_->total_memory_allocated = allocation.size;
    return (0);
}

error
linear_fixed_size_allocator_allocate(u8 **memory, struct allocator *allocator_, u64 size)
{
    struct fixed_size_linear_allocator *allocator = &allocator_->fixed_size_linear_allocator;

    size            = (size + allocator_->alignment - 1) & ~(allocator_->alignment - 1);
    u64 memory_left = allocator->block.allocation.size
                      - allocator->block.used;
    if (memory_left < size)
        return (1);

    *memory = allocator->block.allocation.base;
    allocator->block.used += size;
    allocator_->total_memory_used += size;
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
        return linear_growing_allocator_allocate(memory, allocator, size);
    case allocator_type_fixed_size_linear:
        return linear_fixed_size_allocator_allocate(memory, allocator, size);
    }
}

error
linear_growing_allocator_clear(struct allocator *allocator_)
{
    struct growing_linear_allocator *allocator = &allocator_->growing_linear_allocator;
    allocator_->total_memory_allocated         = 0;
    allocator_->total_memory_used              = 0;
    struct memory_block_stack_node *node       = allocator->stack.top;
    while (node)
    {
        struct memory_block_stack_node *previous
            = node->previous;
        error error = os_release_memory(node->block.allocation);
        if (error)
            return error;
        node = previous;
    }
    return NO_ERROR;
}

void
linear_fixed_size_allocator_clear(struct allocator *allocator_)
{
    struct fixed_size_linear_allocator *allocator = &allocator_->fixed_size_linear_allocator;
    for (u8 *current = allocator->block.allocation.base;
         current < allocator->block.allocation.base + allocator->block.used;
         ++current)
    {
        *current = 0;
    }
    allocator->block.used = 0;
}

error
allocator_clear(struct allocator *allocator)
{
    switch (allocator->type)
    {
    case allocator_type_growing_linear:
    {
        return linear_growing_allocator_clear(allocator);
    }
    break;
    case allocator_type_fixed_size_linear:
    {
        linear_fixed_size_allocator_clear(allocator);
        return NO_ERROR;
    }
    break;
    }
}

#endif
