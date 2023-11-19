#ifndef WINGS_BASE_ALLOCATORS_C_
#define WINGS_BASE_ALLOCATORS_C_

#include "wings/base/types.h"
#include "wings/base/error_codes.c"
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
linear_growing_allocator_allocate(u8 **memory, struct allocator *general_allocator, u64 size)
{
   struct growing_linear_allocator *allocator = &general_allocator->growing_linear_allocator;

   size = (size + general_allocator->alignment - 1) & ~(general_allocator->alignment - 1);
   if (!allocator->stack.top
       || (allocator->stack.top->buffer.used + size
           > allocator->stack.top->buffer.size))
   {
      u64 new_block_size = 0;
      if (size + sizeof(struct memory_stack_node) > allocator->min_block_size)
      {
         new_block_size = size + sizeof(struct memory_stack_node);
      }
      else
      {
         new_block_size = allocator->min_block_size;
      }

      struct os_allocation allocation = { 0 };

      error error = os_reserve_and_commit_memory(
          &allocation,
          new_block_size);
      if (error)
         return (1);
      general_allocator->total_memory_allocated += allocation.size;
      struct memory_stack_node *new_node
          = (struct memory_stack_node *)allocation.base;
      new_node->allocation  = allocation;
      new_node->buffer.base = allocation.base;
      new_node->buffer.used = sizeof(struct memory_stack_node);
      new_node->buffer.size = allocation.size;

      new_node->previous   = allocator->stack.top;
      allocator->stack.top = new_node;
      allocator->stack.number_of_nodes += 1;
   }

   *memory = allocator->stack.top->buffer.base + allocator->stack.top->buffer.used;
   allocator->stack.top->buffer.used += size;
   general_allocator->total_memory_used += size;
   return (0);
}

error
make_fixed_size_linear_allocator(struct allocator *allocator_, u64 size)
{
   struct os_allocation allocation = { 0 };

   error error = os_reserve_and_commit_memory(&allocation,
                                              sizeof(struct memory_stack_node) + size);
   if (error)
      return (1);
   struct fixed_size_linear_allocator *allocator
       = &allocator_->fixed_size_linear_allocator;

   allocator->allocation              = allocation;
   allocator->buffer.base             = allocation.base;
   allocator->buffer.used             = 0;
   allocator->buffer.size             = allocation.size;
   allocator_->type                   = allocator_type_fixed_size_linear;
   allocator_->alignment              = 8;
   allocator_->total_memory_allocated = allocation.size;
   return (0);
}

error
linear_fixed_size_allocator_allocate(u8 **memory, struct allocator *allocator_, u64 size)
{
   struct fixed_size_linear_allocator *allocator = &allocator_->fixed_size_linear_allocator;

   size = (size + allocator_->alignment - 1) & ~(allocator_->alignment - 1);
   u64 memory_left = allocator->buffer.size - allocator->buffer.used;
   if (memory_left < size)
      return (ec_base_allocators__no_space_left);

   *memory = allocator->buffer.base + allocator->buffer.used;
   allocator->buffer.used += size;
   allocator_->total_memory_used += size;
   return (ec__no_error);
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
   return (1);
}

error
allocate_string(struct string *string, u64 size, struct allocator *allocator)
{
   if (!string)
   {
      error_code_set_message("allocate_string first parameter was 0");
      return ec_base_allocators__illegal_parameter;
   }
   error error = allocate((u8 **)&string->first, allocator, size * sizeof(char));
   if (error)
      return error;
   string->length = size;

   return (ec__no_error);
}

error
linear_growing_allocator_clear(struct allocator *allocator_)
{
   struct growing_linear_allocator *allocator = &allocator_->growing_linear_allocator;
   allocator_->total_memory_allocated         = 0;
   allocator_->total_memory_used              = 0;
   struct memory_stack_node *node             = allocator->stack.top;
   allocator->stack.top                       = 0;
   while (node)
   {
      struct memory_stack_node *previous
          = node->previous;
      node->buffer.base = 0;
      node->buffer.used = 0;
      node->buffer.size = 0;
      error error       = os_release_memory(node->allocation);
      if (error)
         return error;
      node = previous;
   }
   allocator->stack.number_of_nodes = 0;
   return (ec__no_error);
}

void
linear_fixed_size_allocator_clear(struct allocator *allocator_)
{
   //@TODO: figure out if it is not cleaner to get new pages from the os
   struct fixed_size_linear_allocator *allocator = &allocator_->fixed_size_linear_allocator;
   for (u8 *current = allocator->buffer.base;
        current < allocator->buffer.base + allocator->buffer.used;
        ++current)
   {
      *current = 0;
   }
   allocator->buffer.used = 0;
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
      return (ec__no_error);
   }
   break;
   }
   return (1);
}

error
make_buffer(struct buffer *buffer, u64 size, struct allocator *allocator)
{
   error error  = allocate_array(&buffer->base, allocator, size, u8);
   buffer->size = size;
   return (error);
}

#endif
