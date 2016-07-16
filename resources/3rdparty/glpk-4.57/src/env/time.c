/* time.c (standard time) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000-2013 Andrew Makhorin, Department for Applied
*  Informatics, Moscow Aviation Institute, Moscow, Russia. All rights
*  reserved. E-mail: <mao@gnu.org>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "env.h"
#include "jd.h"

/***********************************************************************
*  NAME
*
*  glp_time - determine current universal time
*
*  SYNOPSIS
*
*  double glp_time(void);
*
*  RETURNS
*
*  The routine glp_time returns the current universal time (UTC), in
*  milliseconds, elapsed since 00:00:00 GMT January 1, 1970. */

#define EPOCH 2440588 /* jday(1, 1, 1970) */

/* POSIX version ******************************************************/

#if defined(HAVE_SYS_TIME_H) && defined(HAVE_GETTIMEOFDAY)

#include <sys/time.h>
#include <time.h>

double glp_time(void)
{     struct timeval tv;
      struct tm *tm;
      int j;
      double t;
      gettimeofday(&tv, NULL);
      tm = gmtime(&tv.tv_sec);
      j = jday(tm->tm_mday, tm->tm_mon + 1, 1900 + tm->tm_year);
      xassert(j >= 0);
      t = ((((double)(j - EPOCH) * 24.0 + (double)tm->tm_hour) * 60.0 +
         (double)tm->tm_min) * 60.0 + (double)tm->tm_sec) * 1000.0 +
         (double)(tv.tv_usec / 1000);
      return t;
}

/* MS Windows version *************************************************/

#elif defined(__WOE__)

#include <windows.h>

double glp_time(void)
{     SYSTEMTIME st;
      int j;
      double t;
      GetSystemTime(&st);
      j = jday(st.wDay, st.wMonth, st.wYear);
      xassert(j >= 0);
      t = ((((double)(j - EPOCH) * 24.0 + (double)st.wHour) * 60.0 +
         (double)st.wMinute) * 60.0 + (double)st.wSecond) * 1000.0 +
         (double)st.wMilliseconds;
      return t;
}

/* portable ANSI C version ********************************************/

#else

#include <time.h>

double glp_time(void)
{     time_t timer;
      struct tm *tm;
      int j;
      double t;
      timer = time(NULL);
      tm = gmtime(&timer);
      j = jday(tm->tm_mday, tm->tm_mon + 1, 1900 + tm->tm_year);
      xassert(j >= 0);
      t = ((((double)(j - EPOCH) * 24.0 + (double)tm->tm_hour) * 60.0 +
         (double)tm->tm_min) * 60.0 + (double)tm->tm_sec) * 1000.0;
      return t;
}

#endif

/***********************************************************************
*  NAME
*
*  glp_difftime - compute difference between two time values
*
*  SYNOPSIS
*
*  double glp_difftime(double t1, double t0);
*
*  RETURNS
*
*  The routine glp_difftime returns the difference between two time
*  values t1 and t0, expressed in seconds. */

double glp_difftime(double t1, double t0)
{     return
         (t1 - t0) / 1000.0;
}

/**********************************************************************/

#ifdef GLP_TEST
#include <assert.h>

int main(void)
{     int ttt, ss, mm, hh, day, month, year;
      double t;
      t = glp_time();
      xprintf("t = %.f\n", t);
      assert(floor(t) == t);
      ttt = (int)fmod(t, 1000.0);
      t = (t - (double)ttt) / 1000.0;
      assert(floor(t) == t);
      ss = (int)fmod(t, 60.0);
      t = (t - (double)ss) / 60.0;
      assert(floor(t) == t);
      mm = (int)fmod(t, 60.0);
      t = (t - (double)mm) / 60.0;
      assert(floor(t) == t);
      hh = (int)fmod(t, 24.0);
      t = (t - (double)hh) / 24.0;
      assert(floor(t) == t);
      assert(jdate((int)t + EPOCH, &day, &month, &year) == 0);
      printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
         year, month, day, hh, mm, ss, ttt);
      return 0;
}
#endif

/* eof */
