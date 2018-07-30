/*
 * vulgaris
 * (C) 2018 - giuseppe.baccini@gmail.com
 *
 */

#include "timing.h"
#include <time.h>
#if defined WIN32 && defined _MSC_VER
#include <sys/timeb.h>
#else
#include <unistd.h>
#endif
#if defined(__MACH__) || defined(__APPLE__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#if defined WIN32 && defined _MSC_VER
#define DELTA_EPOCH_IN_MICROSECS        11644473600000000Ui64
#define DELTA_EPOCH_IN_MICROSECS10      116444736000000000Ui64
#define ONE_BILLION_64B                 1000000000i64
#else
#define DELTA_EPOCH_IN_MICROSECS        11644473600000000ULL
#define DELTA_EPOCH_IN_MICROSECS10      116444736000000000ULL
#define ONE_BILLION_64B                 1000000000LL
#endif

#define DEFAULT_MINTIME_INIT    999999999

#if defined WIN32 && defined _MSC_VER
#define RT_ONEBILLION           1000000000i64
#define RT_ONEMILLION           1000000i64
#define RT_ONEKAPPA             1000i64
#else
#define RT_ONEBILLION           1000000000LL
#define RT_ONEMILLION           1000000LL
#define RT_ONEKAPPA             1000LL
#endif

namespace vlg {

#if defined WIN32 && defined _MSC_VER
LARGE_INTEGER frequency;
LARGE_INTEGER global_start;
#else
rt_time_t rt_global_offset;
#endif

#if defined WIN32 && defined _MSC_VER
rt_time_t rt_diff_time(rt_time_t t1, rt_time_t t2)
{
    return (rt_time_t)((t2 - t1) * RT_ONEBILLION / frequency.QuadPart); //nsec
}
#else
rt_time_t rt_diff_time(rt_time_t t1, rt_time_t t2)
{
    return t2 - t1;
}

rt_time_t DIFFT2(rt_time_t t1, rt_time_t t2)
{
    return t2 - t1;
}
#endif

static bool rt_timers_init = false;
void rt_init_timers()
{
    if(rt_timers_init) {
        return;
    }
#if defined WIN32 && defined _MSC_VER
    QueryPerformanceFrequency(&frequency);
    global_start.QuadPart = 0;
#else
    rt_time_t res = 0;
    int i;
    struct timespec t0;
    struct timespec t2;
    for(i = 0; i < 100; i++) {
#if defined(__MACH__) || defined(__APPLE__)
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        t0.tv_sec = mts.tv_sec;
        t0.tv_nsec = mts.tv_nsec;
#elif defined(CLOCK_MONOTONIC_HR)
        clock_gettime(CLOCK_MONOTONIC_HR, &t0);
#else
        clock_gettime(CLOCK_MONOTONIC, &t0);
#endif
#if defined(__MACH__) || defined(__APPLE__)
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        t2.tv_sec = mts.tv_sec;
        t2.tv_nsec = mts.tv_nsec;
#elif defined(CLOCK_MONOTONIC_HR)
        clock_gettime(CLOCK_MONOTONIC_HR, &t2);
#else
        clock_gettime(CLOCK_MONOTONIC, &t2);
#endif
    }
    rt_global_offset = res;
#endif
    rt_timers_init = true;
}

void rt_mark_time(rt_time_t *T)
{
#if defined WIN32 && defined _MSC_VER
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    *T = (curTime.QuadPart);// + global_start.QuadPart);
#else
    struct timespec tspec;
#if defined(__MACH__) || defined(__APPLE__)
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tspec.tv_sec = mts.tv_sec;
    tspec.tv_nsec = mts.tv_nsec;
#elif defined(CLOCK_MONOTONIC_HR)
    clock_gettime(CLOCK_MONOTONIC_HR, &tspec);
#else
    clock_gettime(CLOCK_MONOTONIC, &tspec);
#endif
    *T = (rt_time_t)tspec.tv_sec * RT_ONEBILLION + (rt_time_t)tspec.tv_nsec + rt_global_offset;
#endif
}

}

#if defined WIN32 && defined _MSC_VER
class time_zone_win_ts {
    public:
        time_zone_win_ts();
        LARGE_INTEGER start_quad;
        LARGE_INTEGER frequency;
        int64_t global_offset;
};

time_zone_win_ts::time_zone_win_ts()
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_quad);
    GetSystemTimeAsFileTime((FILETIME *)&global_offset);
    global_offset = (global_offset - (DELTA_EPOCH_IN_MICROSECS10)) * 100;
}

static time_zone_win_ts tzutil_ts;

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};
#endif

namespace vlg {
#if defined WIN32 && defined _MSC_VER
/** get nanosecond timestamp. */
uint64_t get_timestamp()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (uint64_t)
           ((int64_t)((double)(now.QuadPart - tzutil_ts.start_quad.QuadPart) /
                      (double)tzutil_ts.frequency.QuadPart * ONE_BILLION_64B) +
            tzutil_ts.global_offset);
}

/** get nanosecond timestamp in timespec format (as pthread)*/
int get_timestamp_ts(struct timespec *tv)
{
    struct _timeb timebuffer;
    _ftime64_s(&timebuffer);
    tv->tv_sec = timebuffer.time;
    tv->tv_nsec = 1000000L * timebuffer.millitm;
    return 0;
}
#elif defined (__MACH__) || defined (__APPLE__)
/** get nanosecond timestamp. */
uint64_t get_timestamp()
{
    struct timespec        _ts;
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    _ts.tv_sec = mts.tv_sec;
    _ts.tv_nsec = mts.tv_nsec;
    return (uint64_t)((int64_t)_ts.tv_sec * 1000000000LL + (int64_t)_ts.tv_nsec);
}

/** get nanosecond timestamp in timespec format (as pthread). */
int get_timestamp_ts(struct timespec *tv)
{
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    tv->tv_sec = mts.tv_sec;
    tv->tv_nsec = mts.tv_nsec;
    return 0;
}
#else
/** get nanosecond timestamp. */
uint64_t get_timestamp()
{
    struct timespec _ts;
    clock_gettime(CLOCK_REALTIME, &_ts);
    return (uint64_t)((int64_t)_ts.tv_sec * 1000000000LL + (int64_t)_ts.tv_nsec);
}

/** get nanosecond timestamp in timespec format (as pthread). */
int get_timestamp_ts(struct timespec *tv)
{
    clock_gettime(CLOCK_REALTIME, tv);
    return 0;
}
#endif

void mssleep(long msec)
{
#if defined WIN32 && defined _MSC_VER
    Sleep(msec);
#else
    usleep((unsigned int)(msec * 1000));
#endif
}

}

#if defined WIN32 && defined _MSC_VER
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    static int tzflag = 0;
    FILETIME ft;
    if(nullptr != tv) {
        unsigned _int64 tmpres = 0;
        GetSystemTimeAsFileTime(&ft);
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;
        tmpres /= 10;  /*convert into microseconds*/
        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }
    if(nullptr != tz) {
        if(!tzflag) {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }
    return 0;
}
#endif
