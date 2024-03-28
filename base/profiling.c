#ifndef WINGS_BASE_PROFILING_C_
#define WINGS_BASE_PROFILING_C_

#include "types.h"
#include "macros.h"
#include "units.h"
#include "os/timer.c"

#include "profiling.h"

#include <stdio.h>
#include <winuser.h>

#ifndef GET_CPU_TICK
#define GET_CPU_TICK get_cpu_timer
#endif

struct profiler profiler =
{
   .anchors      = { { 0 } },
   .start_tick   = 0,
   .end_tick     = 0,
   .start_time   = 0,
   .end_time     = 0,
   .parent_index = 0,
};


void
_start_profiling_zone(
      char  *zone_id,
      u32    anchor_index,
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
         profiler_print_anchor(anchor, total_time);
      }
   }
   const char *time_unit = set_to_closest_time_unit(&total_time);
   printf("Total time(%s): %f\n", time_unit, total_time);
}

void
profiler_print_anchor(struct profiler_anchor *anchor, f64 time_total)
{
   u64 total_ticks   = profiler.end_tick - profiler.start_tick;
   f64 percent_self  = 100.0 * (f64)anchor->ticks_self / (f64)total_ticks;
   f64 percent_total = 100.0 * (f64)anchor->ticks_total / (f64)total_ticks;
   const char *time_unit = set_to_closest_time_unit(&time_total);
   printf("%6.2f%%(%.2f%s) %s(%zu)",
         percent_total,
         time_total * (percent_self / 100.0),
         time_unit,
         anchor->zone_id,
         anchor->ticks_total);
   if (anchor->ticks_total != anchor->ticks_self)
   {
      printf(" %6.2f%%(%zu)",
            percent_self, anchor->ticks_self);
   }
   printf("\n");
}

#endif
