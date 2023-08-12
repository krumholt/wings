#ifndef LINUX_OS_MEMORY_C
#define LINUX_OS_MEMORY_C

#include "base/types.h"
#include "base/macros.c"

#include <stdio.h>
#include <sys/mman.h>

#ifndef OS_MEMORY_C_
enum memory_state
{
    memory_state_commited,
    memory_state_reserved,
    memory_state_free,
};

struct os_memory_block
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
os_reserve_memory(struct os_memory_block *block, u64 size)
{
	block->base = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block->base == MAP_FAILED)
	{
		return (1);
	}
	block->size = size;
    return (0);
}

error
os_commit_memory(struct os_memory_block block)
{
	int result = mprotect(block.base, block.size, PROT_READ | PROT_WRITE);
	printf("mprotect called\n");
    return (result == -1 ? 1 : 0);
}

error
os_release_memory(struct os_memory_block block)
{
	int result = munmap(block.base, block.size);
	error error = 0;
	if (result == -1)
		error = 1;

    return (error);
}

error
os_decommit_memory(struct os_memory_block block)
{
	int result = mprotect(block.base, block.size, PROT_NONE);
	error error = 0;
	if (result == -1)
		error = 1;
    return (error);
}

error
os_get_memory_info(struct memory_info *info, void *memory)
{
	UNUSED(info);
	UNUSED(memory);
	WARN("os_get_memory_info not implemented in linux");
	return(1);
}

#endif
