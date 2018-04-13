/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#pragma once
#include "vlg.h"

#if defined WIN32 && defined _MSC_VER
typedef int64_t     rt_time_t;
#else
typedef long long   rt_time_t;
#endif

#if defined WIN32 && defined _MSC_VER
struct timeval;
struct timespec;

int gettimeofday(struct timeval *tv, struct timezone *tz);
#else
#include <sys/time.h>
#endif

namespace vlg {

void rt_init_timers();
void rt_mark_time(rt_time_t *t);
rt_time_t rt_diff_time(rt_time_t t1, rt_time_t t2);
uint64_t get_timestamp();
int get_timestamp_ts(struct timespec *tv);
void mssleep(long msec);

}
