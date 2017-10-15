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

#ifndef CR_TIME_H_
#define CR_TIME_H_
#include "blaze.h"

#ifdef WIN32
typedef int64_t     rt_time_t;
#else
typedef long long   rt_time_t;
#endif

#ifdef WIN32
struct timeval;
struct timespec;
/**
@param tv
@param tz
@return
*/
int         gettimeofday(struct timeval *tv,
                         struct timezone *tz);

#endif

namespace blaze {

/**
real time measurement initialization
*/
void        rt_init_timers();

/**
@param t
*/
void        rt_mark_time(rt_time_t *t);

/**
@param t1
@param t2
@return
*/
rt_time_t   rt_diff_time(rt_time_t t1,
                         rt_time_t t2);

/**
get nanosecond timestamp

@return
*/
uint64_t    get_timestamp();

/**
get nanosecond timestamp in timespec format (as pthread).

@return
*/
int         get_timestamp_ts(struct timespec *tv);

/**
@param msec
*/
void        mssleep(long msec);

}
#endif
