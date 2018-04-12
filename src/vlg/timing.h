/*
 *
 * (C) 2017 - giuseppe.baccini@gmail.com
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
