#ifndef WINGS_BASE_PROFILER_C_
#define WINGS_BASE_PROFILER_C_

#include "wings/base/types.h"
#include "wings/base/macros.c"
#include "wings/base/units.c"
#include "wings/os/timer.c"

#include <assert.h>
#include <stdio.h>
#include <winuser.h>

#ifndef GET_CPU_TICK
#define GET_CPU_TICK get_cpu_timer
#endif

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
} profiler = {
   .anchors      = { { 0 } },
   .start_tick   = 0,
   .end_tick     = 0,
   .start_time   = 0,
   .end_time     = 0,
   .parent_index = 0,
};

#if NO_PROFILING == 0

#define add_to_profiling_zone(name) \
   _start_profiling_zone(#name, name.anchor_index, &name)

#define start_profiling_zone(name)     \
   struct profiler_block name = { 0 }; \
   _start_profiling_zone(#name, __COUNTER__ + 1, &name)

void
_start_profiling_zone(char                  *zone_id,
                      u32                    anchor_index,
                      struct profiler_block *block)
{
   block->parent_index = profiler.parent_index;
   block->anchor_index = anchor_index;
   block->zone_id      = zone_id;

   struct profiler_anchor *anchor = profiler.anchors + anchor_index;
   block->old_ticks_total         = anchor->ticks_total;
   profiler.parent_index          = anchor_index;
   block->start_tick              = GET_CPU_TICK();
   anchor->zone_id                = zone_id;
}

#define end_profiling_zone(name) \
   _end_profiling_zone(&name)

void
_end_profiling_zone(struct profiler_block *block)
{
   u64 current_tick      = GET_CPU_TICK();
   u64 elapsed           = current_tick - block->start_tick;
   profiler.parent_index = block->parent_index;

   struct profiler_anchor *parent = profiler.anchors
                                    + block->parent_index;
   struct profiler_anchor *anchor = profiler.anchors
                                    + block->anchor_index;
   parent->ticks_self -= elapsed;
   anchor->ticks_self += elapsed;
   anchor->ticks_total = block->old_ticks_total + elapsed;
   anchor->hit_count += 1;
}

void
start_profiling(void)
{
   profiler.start_time = get_os_timer_in_seconds();
   profiler.start_tick = GET_CPU_TICK();
}

void
profiler_print_anchor(struct profiler_anchor *anchor)
{
   u64 total_ticks   = profiler.end_tick - profiler.start_tick;
   f64 percent_self  = 100.0 * (f64)anchor->ticks_self / (f64)total_ticks;
   f64 percent_total = 100.0 * (f64)anchor->ticks_total / (f64)total_ticks;
   printf("%6.2f%% %s(%zu)",
          percent_total,
          anchor->zone_id,
          anchor->ticks_total);
   if (anchor->ticks_total != anchor->ticks_self)
   {
      printf(" %6.2f%%(%zu)",
             percent_self, anchor->ticks_self);
   }
   printf("\n");
}

void
end_profiling(void)
{
   profiler.end_tick = GET_CPU_TICK();
   f64 total_time    = seconds_to_nanoseconds(get_os_timer_in_seconds() - profiler.start_time);
   for (u32 index = 0;
        index < ARRAY_LENGTH(profiler.anchors);
        ++index)
   {
      struct profiler_anchor *anchor = profiler.anchors + index;
      if (anchor->ticks_total)
      {
         profiler_print_anchor(anchor);
      }
   }
   char *time_unit = set_to_closest_time_unit(&total_time);
   printf("Total time(%s): %f\n", time_unit, total_time);
}
#else

#define start_profiling_zone(name)
#define end_profiling_zone(name)
#define add_to_profiling_zone(name)

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
