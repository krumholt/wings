#ifndef MEMORY_C
#define MEMORY_C

#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

struct memory
{
    u8  *start;
    u8  *current;
    u8  *end;
    u64  high_water_mark;
};

struct temporary_memory
{
    struct memory *memory;
    u8 *start;
};

struct temporary_memory
begin_temporary_memory(struct memory *memory)
{
    struct temporary_memory temporary_memory = {0};
    temporary_memory.memory = memory;
    temporary_memory.start  = memory->current;
    return(temporary_memory);
}

void
end_temporary_memory(struct temporary_memory temporary)
{
    struct memory *memory = temporary.memory;
    while (memory->current != temporary.start)
    {
        *memory->current = 0;
        --memory->current;
    }
    *memory->current = 0;
}

u64
align_to(u64 alignment, u64 value)
{
    u64 range = value + alignment - 1;
    return range - (range % alignment);
}



#define MEMORY_ALIGNMENT 8

#define allocate_array(block, size, type) \
    (type *)_allocate((block), (size) * sizeof(type))

#define allocate_array_packed(block, size, type) \
    (type *)_allocate_packed((block), (size) * sizeof(type))

#define allocate_struct(block, type) \
    (type *)_allocate((block), sizeof(type))

struct memory
make_memory(u64 size)
{
    size = align_to(MEMORY_ALIGNMENT, size);
    struct memory block = {0};
    u64 block_size = size;
    block.start = (u8 *)calloc(block_size, sizeof(u8));
    assert(block.start);
    block.current  = block.start;
    block.end      = block.start + block_size;
    return(block);
}

void
deallocate_block(struct memory *block)
{
    if (block->start)
        free(block->start);
    block->start = 0;
    block->current  = 0;
    block->end   = 0;
    block->high_water_mark = 0;
}

u64
get_memory_used(struct memory memory)
{
    return((u64)(memory.current - memory.start));
}


u64
get_memory_free(struct memory memory)
{
    return((u64)(memory.end - memory.current));
}


u64
get_memory_total(struct memory memory)
{
    return((u64)(memory.end - memory.start));
}


u8 *
_allocate(struct memory *block, u64 size)
{
    assert(block);
    assert(block->start);
    u64 aligned_size = (size + MEMORY_ALIGNMENT - 1) & ~(MEMORY_ALIGNMENT - 1);
    if (block->current + aligned_size > block->end)
    {
        f64 total_memory = (f64)(block->end - block->start);
        f64 memory_left  = (f64)(block->end - block->current);
        char *total_memory_unit = set_to_closest_memory_unit(&total_memory);
        char *memory_left_unit = set_to_closest_memory_unit(&memory_left);
        printf("[error] tried to allocate %zu from memory %0.2f %s but only %0.2f %s left.\n",
               size,
               total_memory,
               total_memory_unit,
               memory_left,
               memory_left_unit);
        assert(0);
        exit(-1);
    }

    u8 *new_memory = block->current;
    block->current += aligned_size;
    u64 memory_in_use = get_memory_used(*block);
    if (memory_in_use > block->high_water_mark)
        block->high_water_mark = memory_in_use;
    return(new_memory);
}


u8 *
_allocate_packed(struct memory *block, u64 size)
{
    assert(block);
    if (block->current + size > block->end)
    {
        printf("[error] tried to allocate %zu from memory(%zu) but not enough memory was left.\n",
				size, get_memory_free(*block));
        assert(0); // @TODO: cleanly communicate this
    }

    u8 *new_memory = block->current;
    block->current += size;
    u64 memory_in_use = get_memory_used(*block);
    if (memory_in_use > block->high_water_mark)
        block->high_water_mark = memory_in_use;
    return(new_memory);
}


u8 *
allocate(struct memory *block, u64 size)
{
    return(_allocate(block, size));
}

void
clear_and_dont_zero(struct memory *block)
{
    block->current = block->start;
}


void
clear(struct memory *block)
{
    while (block->current != block->start)
    {
        *block->current = 0;
        --block->current;
    }
    *block->current = 0;
}

void
copy_bytes(u8* destination, u64 destination_size,
     u8 *source, u64 number_of_elements, u64 element_size,
     u64 stride, u64 offset)
{
    assert(element_size <= stride);
    assert(offset < stride);

    assert(destination_size >= number_of_elements * stride );
    for (u64 index = 0;
         index < number_of_elements;
         ++index)
    {
        u64 target_position = offset + index * stride;
        u64 source_position = index * element_size;
        memcpy(destination + target_position,
               source + source_position,
               element_size);
    }
}

#endif
