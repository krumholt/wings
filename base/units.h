#ifndef WINGS_BASE_UNITS_H_
#define WINGS_BASE_UNITS_H_

#include "wings/base/types.h"

u64
kibibytes(u64 n);

u64
mebibytes(u64 n);

u64
gibibytes(u64 n);

f64
bytes_to_mebibytes(u64 n);

const char *
set_to_closest_memory_unit(f64 *size);

const char *
set_to_closest_time_unit(f64 *time_in_ns);

f64
seconds_to_nanoseconds(f64 time);

#endif
