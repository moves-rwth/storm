/* 
 * This file is part of the program ltl2dstar (http://www.ltl2dstar.de/).
 * Copyright (C) 2005-2007 Joachim Klein <j.klein@ltl2dstar.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

/** @file
 *  Provides class TimeKeeper
 */

#include "common/Exceptions.hpp"

#if (__WIN32__ || _WIN32)

#include <windows.h>

/**
 * TimeKeeper (win32 version). Like a stop watch, can be started multiple times
 * and will then record the time until it was stopped the same number of times 
 * (total time from first start to last stop).
 */
class TimeKeeper {
public:
  /** Constructor */
  TimeKeeper() {
    running=0;
    elapsed_total=0L;
  }
  
  /** Start the stop watch. */
  void start() {
    last_start_time=GetTickCount();
    running++;
  }

  /** Stop the stop watch, keeps tracking the time if there were more start()
   *  then stop */
  void stop() {
    if (running==0) {
      THROW_EXCEPTION(Exception, "Can't stop timekeeper which is not running");
    }
    running--;
    if (running>0) {
      return;
    }

    unsigned long stop_time=GetTickCount();
    unsigned long elapsed=stop_time-last_start_time;
    elapsed_total+=elapsed;
  }

  /** Get the elapsed time in milliseconds. May only be called if the stop
   *  watch is not running currently */
  unsigned long getElapsedMilliseconds() {
    if (running>0) {
      THROW_EXCEPTION(Exception, "Can't get time from still running timekeeper");
    }

    return elapsed_total;
  };
  
private:
  unsigned long last_start_time;
  unsigned long elapsed_total;
  unsigned int running;
};



#else 
// POSIX

#include <ctime>
#include <sys/time.h>

/**
 * TimeKeeper (posix version). Like a stop watch, can be started multiple times
 * and will then record the time until it was stopped the same number of times 
 * (total time from first start to last stop).
 */
class TimeKeeper {
public:
  /** Constructor */
  TimeKeeper() {
    running=0;
    elapsed_total=0L;
  }
  
  /** Start the stop watch. */
  void start() {
    gettimeofday(&last_start_time, NULL);
    running++;
  }

  /** Stop the stop watch, keeps tracking the time if there were more start()
   *  then stop */
  void stop() {
    if (running==0) {
      THROW_EXCEPTION(Exception, "Can't stop timekeeper which is not running");
    }
    running--;
    if (running>0) {
      return;
    }

    struct timeval stop_time;
    gettimeofday(&stop_time, NULL);

    //    std::cerr << "Start : " << last_start_time.tv_sec <<
    //      ":" << last_start_time.tv_usec << std::endl;

    //    std::cerr << "Stop : " << stop_time.tv_sec <<
    //      ":" << stop_time.tv_usec << std::endl;

    unsigned long elapsed=0;
    if (stop_time.tv_usec < last_start_time.tv_usec) {
      elapsed=1000000 + stop_time.tv_usec;
      elapsed=elapsed - last_start_time.tv_usec;
      --stop_time.tv_sec;
    } else {
      elapsed=stop_time.tv_usec - last_start_time.tv_usec;
    }
    elapsed=elapsed/1000; // milliseconds
    elapsed+=(stop_time.tv_sec - last_start_time.tv_sec)*1000;

    // std::cerr << "Elapsed: "<< elapsed << std::endl;

    elapsed_total+=elapsed;
  }

  /** Get the elapsed time in milliseconds. May only be called if the stop
   *  watch is not running currently */
  unsigned long getElapsedMilliseconds() {
    if (running>0) {
      THROW_EXCEPTION(Exception, "Can't get time from still running timekeeper");
    }

    return elapsed_total;
  };
  
private:
  struct timeval last_start_time;
  unsigned long elapsed_total;
  unsigned int running;
};



#endif

#endif
