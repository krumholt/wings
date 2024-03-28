#ifndef WINGS_BASE_PROFILING_H_
#define WINGS_BASE_PROFILING_H_

#include "types.h"
#include "macros.h"
#include "units.h"
#include "os/timer.c"

#include <assert.h>
#include <stdio.h>
#include <winuser.h>

struct profiler_anchor
{
   u64   ticks_total;
   u64   ticks_self;
   u64   hit_count;
   char *zone_id;
};

struct profiler_block
{
   char *zone_id;
   u64   old_ticks_total;
   u64   start_tick;
   u32   parent_index;
   u32   anchor_index;
};

struct profiler
{
   struct profiler_anchor anchors[1024];
   f64                    start_tick;
   f64                    end_tick;
   f64                    start_time;
   f64                    end_time;
   u32                    parent_index;
};


#define add_to_profiling_zone(name) \
   _start_profiling_zone(#name, name.anchor_index, &name)

#define start_profiling_zone(name)     \
   struct profiler_block name = { 0 }; \
   _start_profiling_zone(#name, __COUNTER__ + 1, &name)

#define end_profiling_zone(name) \
   _end_profiling_zone(&name)

void
start_profiling(void);

void
end_profiling(void);

void
profiler_print_anchor(struct profiler_anchor *anchor, f64 time_total);

#endif
