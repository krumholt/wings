#ifndef WINGS_OS_WINDOWS_MEMORY_C_
#define WINGS_OS_WINDOWS_MEMORY_C_

#include "wings/base/types.c"
#include "wings/base/macros.c"
#include "wings/base/error_codes.c"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
// #define NOCOMM
#endif
#include <Windows.h>

#ifndef WINGS_OS_MEMORY_C_
enum memory_state
{
   memory_state_commited,
   memory_state_reserved,
   memory_state_free,
};

struct os_allocation
{
   u8 *base;
   u64 size;
};

struct memory_info
{
   void             *base_adress;
   void             *allocation_adress;
   u64               region_size;
   enum memory_state state;
};

#endif

error
os_reserve_memory(struct os_allocation *block, u64 size)
{
   SYSTEM_INFO info = { 0 };
   GetSystemInfo(&info);
   u32 allocation_size = info.dwAllocationGranularity;
   u32 actual_size     = ((size + allocation_size - 1) / allocation_size) * allocation_size;
   block->base         = (u8 *)VirtualAlloc(0,
                                            actual_size,
                                            MEM_RESERVE,
                                            PAGE_READWRITE);
   if (block->base == 0)
      return (ec_os_memory__failed_to_allocate);
   block->size = actual_size;

   return (NO_ERROR);
}

error
os_commit_memory(struct os_allocation block)
{
   void *result = VirtualAlloc(block.base,
                               block.size,
                               MEM_COMMIT,
                               PAGE_READWRITE);
   return (block.base == result ? NO_ERROR : ec_os_memory__failed_to_allocate);
}

error
os_reserve_and_commit_memory(struct os_allocation *block, u64 size)
{
   error error = 0;
   error       = os_reserve_memory(block, size);
   if (error)
      return error;

   error = os_commit_memory(*block);
   if (error)
      return error;

   return (NO_ERROR);
}

error
os_release_memory(struct os_allocation block)
{
   b32 result = VirtualFree(block.base, 0, MEM_RELEASE);
   return (result == 0 ? ec_os_memory__failed_to_allocate : NO_ERROR);
}

error
os_decommit_memory(struct os_allocation block)
{
   b32 result = VirtualFree(block.base, block.size, MEM_DECOMMIT);
   return (result == 0 ? ec_os_memory__failed_to_allocate : NO_ERROR);
}

u64
os_get_page_size(void)
{
   SYSTEM_INFO info = { 0 };
   GetSystemInfo(&info);
   u32 page_size = info.dwPageSize;
   return (page_size);
}

u64
os_get_allocation_size(void)
{
   SYSTEM_INFO info = { 0 };
   GetSystemInfo(&info);
   u32 allocation_size = info.dwAllocationGranularity;
   return (allocation_size);
}

error
os_get_memory_info(struct memory_info *info, struct os_allocation block)
{
   MEMORY_BASIC_INFORMATION basic_info = { 0 };

   u64 query_size = VirtualQuery(block.base, &basic_info, sizeof(MEMORY_BASIC_INFORMATION));
   if (query_size != sizeof(MEMORY_BASIC_INFORMATION))
      return (ec_os_memory__query_failed);

   enum memory_state state = memory_state_free;
   switch (basic_info.State)
   {
   case MEM_RESERVE:
      state = memory_state_reserved;
      break;
   case MEM_FREE:
      state = memory_state_free;
      break;
   case MEM_COMMIT:
      state = memory_state_commited;
      break;
   default:
      return (ec_os_memory__query_unknown_state);
   }
   info->state             = state;
   info->base_adress       = basic_info.BaseAddress;
   info->allocation_adress = basic_info.AllocationBase;
   info->region_size       = basic_info.RegionSize;

   return (NO_ERROR);
}

#endif
