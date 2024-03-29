#ifndef WINGS_BASE_ALLOCATORS_C_
#define WINGS_BASE_ALLOCATORS_C_

#include "types.h"
#include "errors.h"
#include "macros.h"
#include "os/memory.c"

#include "allocators.h"

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

static error
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

static error
linear_fixed_size_allocator_allocate(u8 **memory, struct allocator *allocator_, u64 size)
{
   struct fixed_size_linear_allocator *allocator = &allocator_->fixed_size_linear_allocator;

   size = (size + allocator_->alignment - 1) & ~(allocator_->alignment - 1);
   u64 memory_left = allocator->buffer.size - allocator->buffer.used;
   if (memory_left < size)
      return (make_error("Failed to allocate. Memory left %llu. Memory requested %llu", memory_left, size));

   *memory = allocator->buffer.base + allocator->buffer.used;
   allocator->buffer.used += size;
   allocator_->total_memory_used += size;
   return (0);
}

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


static error
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
   return (0);
}

static void
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
      return (0);
   }
   break;
   }
   return (1);
}

#endif
