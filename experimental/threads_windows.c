#ifndef THREADS_WINDOWS_C
#define THREADS_WINDOWS_C

#include "types.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct thread
{
	HANDLE handle;
};

b32
make_thread_and_run(struct thread *thread, unsigned long (*fn)(void *), void *args)
{
	thread->handle = CreateThread(0, 0, fn, args, 0, 0);
	if (!thread->handle)
		return(1);

	return(0);
}

b32
thread_running(struct thread thread)
{
	return( WaitForSingleObject(thread.handle, 0) != WAIT_OBJECT_0);
}

b32
thread_result(struct thread thread, u32 *result)
{
	b32 error = GetExitCodeThread(thread.handle, (unsigned long *)result);
	return (error == 0);
}

#endif
