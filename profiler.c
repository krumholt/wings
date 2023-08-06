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

struct profiler_block
{
    char *zone_id;
    u64   start_tick;
    u64   end_tick;
    u32   parent_id;
    u32   last_sub_block_id;
};

struct profiler_result_block
{
    char *zone_id;
    u64   start_tick;
    u64   end_tick;
    u32   parent_id;
    u32   id;

    u32 sub_block_count;
    u64 total_ticks;
    u64 self_ticks;
    b32 marked;
    u32 hit_count;
    u32 first_child_id;
    u32 last_child_id;
    u32 sibling_id;
    u32 depth;
};

struct profiler
{
    struct profiler_block *blocks;
    u32                    next_id;
    u32                    open_block_id;
    f64                    start_time;
} profiler = {
    .blocks        = 0,
    .next_id       = 0,
    .open_block_id = 0,
    .start_time    = 0,
};

#if PROFILING == 1
void start_profiling_zone(char *zone_id)
{
    u64 current_tick = GET_CPU_TICK();

    u32 id = profiler.next_id++;

    struct profiler_block *entry = profiler.blocks + id;
    entry->parent_id             = profiler.open_block_id;
    entry->zone_id               = zone_id;
    entry->start_tick            = current_tick;

    profiler.open_block_id = id;
}

void end_profiling_zone(void)
{
    u64 current_tick = GET_CPU_TICK();

    struct profiler_block *block = profiler.blocks + profiler.open_block_id;
    block->end_tick              = current_tick;
    block->last_sub_block_id     = profiler.next_id - 1;

    profiler.open_block_id = block->parent_id;
}

void start_profiling(void)
{
    profiler.blocks     = calloc(1024 * 1024, sizeof(struct profiler_block));
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

void _pbs_push(struct profiler_block_queue *queue, u32 id)
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

u32 _pbs_pop(struct profiler_block_queue *queue)
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

b32 _pbs_empty(struct profiler_block_queue *queue)
{
    return queue->front == 0;
}

void end_profiling(void)
{
    end_profiling_zone();
    f64 total_time  = seconds_to_nanoseconds(get_os_timer_in_seconds() - profiler.start_time);
    u64 total_ticks = profiler.blocks[0].end_tick - profiler.blocks[0].start_tick;

    struct profiler_result_block *blocks = calloc(profiler.next_id, sizeof(struct profiler_result_block));
    {
        for (s32 id = 0;
             id < profiler.next_id;
             ++id)
        {
            blocks[id].id              = id;
            blocks[id].start_tick      = profiler.blocks[id].start_tick;
            blocks[id].end_tick        = profiler.blocks[id].end_tick;
            blocks[id].parent_id       = profiler.blocks[id].parent_id;
            blocks[id].sub_block_count = profiler.blocks[id].last_sub_block_id - id;
            blocks[id].zone_id         = profiler.blocks[id].zone_id;
            blocks[id].total_ticks     = profiler.blocks[id].end_tick - profiler.blocks[id].start_tick;
            if (blocks[id].sub_block_count == 0)
            {
                blocks[id].first_child_id = 0;
                blocks[id].last_child_id  = 0;
            }
            else
            {
                blocks[id].first_child_id = id + 1;
                u32 last_child_id         = id + blocks[id].sub_block_count;
                while (profiler.blocks[last_child_id].parent_id != id)
                    last_child_id = profiler.blocks[last_child_id].parent_id;
                blocks[id].last_child_id = last_child_id;
            }
        }
    }

    { // setting siblings
        struct profiler_block_queue queue = { 0 };

        u32 call_depth = 0;
        s32 index      = 0;
        _pbs_push(&queue, 0);
        while (!_pbs_empty(&queue))
        {
            index = _pbs_pop(&queue);

            struct profiler_result_block *entry       = blocks + index;
            s32                           child_index = entry->first_child_id;
            call_depth                                = entry->depth;
            while (child_index != 0)
            {
                _pbs_push(&queue, child_index);
                struct profiler_result_block *child                = blocks + child_index;
                u32                           number_of_sub_blocks = child->sub_block_count;
                child_index += number_of_sub_blocks + 1;
                if (child_index > entry->id + entry->sub_block_count)
                    child_index = 0;
                child->sibling_id = child_index;
                child->depth      = entry->depth + 1;
            }
        }
    }
    { // merge loops
        for (s32 index = 0;
             index < profiler.next_id;
             ++index)
        {
            struct profiler_result_block *block = blocks + index;
            if (block->marked)
                continue;

            block->hit_count                           = 1;
            s32                           sibling_id   = block->sibling_id;
            struct profiler_result_block *last_sibling = block;
            while (sibling_id != 0)
            {
                struct profiler_result_block *sibling = blocks + sibling_id;
                sibling_id                            = sibling->sibling_id;
                if (sibling->marked)
                {
                    continue;
                }
                if (sibling->zone_id == block->zone_id)
                {
                    sibling->marked          = 1;
                    last_sibling->sibling_id = sibling->sibling_id;
                    block->hit_count += 1;
                    block->total_ticks += sibling->total_ticks;
                    if (block->last_child_id == 0)
                    {
                        block->first_child_id = sibling->first_child_id;
                    }
                    else
                    {
                        struct profiler_result_block *last_child = blocks + block->last_child_id;
                        assert(last_child->sibling_id == 0);
                        last_child->sibling_id = sibling->first_child_id;
                    }
                    block->last_child_id = sibling->last_child_id;
                }
                last_sibling = sibling;
            }
        }
    }
    { // printing
        for (s32 index = 0; index < profiler.next_id; ++index)
        {
            struct profiler_result_block *block = blocks + index;
            if (block->marked)
                continue;
            for (s32 index = 1; index < block->depth; ++index)
                printf("        ");
            printf("%6.2f%%  ", (100.0 * block->total_ticks) / (f64)blocks[0].total_ticks);
            printf("%s[%d] %zu\n", block->zone_id, block->hit_count, block->total_ticks);
        }
    }
    char *time_unit = set_to_closest_time_unit(&total_time);
    printf("Total time(%s): %f\n", time_unit, total_time);
}
#else

void start_profiling_zone(char *zone_id)
{
}

void end_profiling_zone(void)
{
}

void start_profiling(void)
{
    profiler.start_time = get_os_timer_in_seconds();
}

void end_profiling(void)
{
    f64   total_time = seconds_to_nanoseconds(get_os_timer_in_seconds() - profiler.start_time);
    char *time_unit  = set_to_closest_time_unit(&total_time);
    printf("Total time(%s): %f\n", time_unit, total_time);
}
#endif

#endif
