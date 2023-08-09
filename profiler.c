#ifndef PROFILER_C
#define PROFILER_C

#include "os_timer.c"
#include "types.h"

#include <assert.h>
#include <stdio.h>
#include <winuser.h>

#ifndef GET_CPU_TICK
#define GET_CPU_TICK get_cpu_timer
#endif

struct profile_anchor
{
    u64   ticks_total;
    u64   ticks_self;
    u64   hit_count;
    char *zone_id;
};

struct profiler_block
{
    char *zone_id;
    u64   old_elapsed_inclusive;
    u64   start_tick;
    u32   parent_id;
    u32   anchor_id;
};

struct profiler
{
    struct profile_anchor anchors[1024];
    f64                   start_tick;
    f64                   end_tick;
    f64                   start_time;
    f64                   end_time;
    u32                   global_parent_id;
} profiler = {
    .anchors          = { 0 },
    .start_tick       = 0,
    .end_tick         = 0,
    .global_parent_id = 0,
};

#if PROFILING == 1
void
start_profiling_zone(char *zone_id, u32 anchor_id, struct profiler_block block)
{
    block.parent_id = profiler.global_parent_id;
    block.anchor_id = anchor_id;
    block.zone_id   = zone_id;

	struct profile_anchor *anchor = profiler.anchors+anchor_id;
    u64 current_tick
        = GET_CPU_TICK();

    struct profiler_block *entry = profiler.blocks + id;
    entry->parent_id             = profiler.open_block_id;
    entry->zone_id               = zone_id;
    entry->start_tick            = current_tick;

    profiler.open_block_id = id;
}

void
end_profiling_zone(struct profile_block block)
{
    u64 current_tick = GET_CPU_TICK();
    u64 elapsed      = current_tick

        struct profiler_block *block
        = profiler.blocks + profiler.open_block_id;
    block->end_tick          = current_tick;
    block->last_sub_block_id = profiler.next_id - 1;

    profiler.open_block_id = block->parent_id;
}

void
start_profiling(void)
{
    profiler.blocks     = calloc(1024 * 1024 * 1024, sizeof(struct profiler_block));
    profiler.start_time = get_os_timer_in_seconds();
    start_profiling_zone("Root");
}

struct profiler_block_queue_node
{
    struct profiler_block_queue_node *next;
    u32                               id;
};

struct profiler_block_queue
{
    struct profiler_block_queue_node *front;
    struct profiler_block_queue_node *back;
};

void
_pbs_push(struct profiler_block_queue *queue, u32 id)
{
    struct profiler_block_queue_node *new = calloc(1, sizeof(struct profiler_block_queue_node));

    new->id   = id;
    new->next = 0;
    if (!queue->front)
    {
        queue->front = queue->back = new;
    }
    else
    {
        queue->back->next = new;
        queue->back       = new;
    }
}

u32
_pbs_pop(struct profiler_block_queue *queue)
{
    struct profiler_block_queue_node *top = queue->front;

    u32 id = 0;
    if (top)
    {
        id           = top->id;
        queue->front = top->next;
        free(top);
    }
    return (id);
}

b32
_pbs_empty(struct profiler_block_queue *queue)
{
    return queue->front == 0;
}

void
end_profiling(void)
{
}
#else

void
start_profiling_zone(char *zone_id)
{
}

void
end_profiling_zone(void)
{
}

void
start_profiling(void)
{
    profiler.start_time = get_os_timer_in_seconds();
}

void
end_profiling(void)
{
    f64   total_time = seconds_to_nanoseconds(get_os_timer_in_seconds() - profiler.start_time);
    char *time_unit  = set_to_closest_time_unit(&total_time);
    printf("Total time(%s): %f\n", time_unit, total_time);
}
#endif

#endif
