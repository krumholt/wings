#ifndef WINGS_BASE_ALLOCATORS_H_
#define WINGS_BASE_ALLOCATORS_H_

#include "wings/base/types.h"
#include "wings/base/error_codes.h"
#include "wings/base/macros.c"
#include "wings/os/memory.c"

struct memory_stack_node
{
   struct memory_stack_node *previous;
   struct buffer             buffer;
   struct os_allocation      allocation;
};

struct memory_stack
{
   struct memory_stack_node *top;
   u32                       number_of_nodes;
};

struct growing_linear_allocator
{
   struct memory_stack stack;
   u64                 min_block_size;
};

struct fixed_size_linear_allocator
{
   struct buffer        buffer;
   struct os_allocation allocation;
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

typedef struct allocator allocator;


struct allocator
make_growing_linear_allocator(u64 block_size);

error
make_fixed_size_linear_allocator(struct allocator *allocator_, u64 size);

error
allocate(u8 **memory, struct allocator *allocator, u64 size);

#define allocate_struct(pointer, allocator, type) \
   (*(pointer) = (type *)(0), allocate((u8 **)(pointer), (allocator), sizeof(type)))

#define allocate_array(pointer, allocator, n, type) \
   (*(pointer) = (type *)(0), allocate((u8 **)(pointer), (allocator), (n) * sizeof(type)))

error
allocator_clear(struct allocator *allocator);

#endif
