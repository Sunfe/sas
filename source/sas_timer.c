/******************************************************************************/
/******************************************************************************/
#include <time.h>
#ifndef _POSIX_TIMERS
# include <sys/time.h>
#endif

#include "sas_timer.h"
#include "sas_basic_type.h"

/* {{{ just sleep some msec (encaplulation of OS function) */
void sas_sleep(uint32 timeToSleepMS)
{
  struct timespec ts;
  ts.tv_sec = timeToSleepMS/1000;
  ts.tv_nsec = (timeToSleepMS-ts.tv_sec*1000)*1000000L;
  nanosleep(&ts, 0);
}


char *getCurrentTimeStr(char *s) 
{
#ifdef _POSIX_TIMERS
  struct timespec tv;
  if (clock_gettime(CLOCK_REALTIME, &tv) == 0)
#else
  struct timeval tv;
  struct timezone tz;
  if (gettimeofday(&tv, &tz) == 0)
#endif
  {
    struct tm tt; 
    localtime_r(&tv.tv_sec, &tt);
    strftime(s, 25, "%Y.%m.%d.%H:%M:%S ", &tt);
  }
  else
    *s = '\0';

  return s;
}


uint32 getCurrentTime(void) 
{
#ifdef _POSIX_TIMERS
  struct timespec tv;
  if (clock_gettime(CLOCK_REALTIME, &tv) == 0)
    return (tv.tv_sec*1000) + (tv.tv_nsec/1000000);
#else
  struct timeval tv;
  struct timezone tz;
  if (gettimeofday(&tv, &tz) == 0)
    return (tv.tv_sec*1000) + (tv.tv_usec/1000);
#endif
  else
    return 0;
}


