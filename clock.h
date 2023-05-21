#ifndef clock_h
#define clock_h

#include "types.h"

struct clock;

void  clock_initialise                (struct clock *clock);
b32   clock_enable_high_resolution    (void);
b32   clock_disable_high_resolution   (void);
void  clock_start                     (struct clock *clock);
s64   clock_get_tick                  (void);
f64   clock_elapsed_time_seconds      (struct clock *clock);
f64   clock_elapsed_time_milliseconds (struct clock *clock);

#endif
