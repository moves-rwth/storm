// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011, 2012, 2013, 2014, 2015, 2016 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <spot/misc/common.hh>
#include <spot/misc/_config.h>
#include <cassert>
#include <iosfwd>
#include <string>
#include <map>
#include <chrono>
#if SPOT_HAVE_SYS_TIMES_H
# include <sys/times.h>
#endif
#include <ctime>


namespace spot
{
  /// \ingroup misc_tools
  /// @{


  /// \brief A simple stopwatch
  struct stopwatch
  {
  protected:
    typedef std::chrono::high_resolution_clock clock;
    clock::time_point start_;
  public:
    /// Marks the start if the measurement
    void start()
    {
      start_ = clock::now();
    }

    /// \brief Returns the elapsed duration in seconds.
    ///
    /// May be called multiple times, and will always return the
    /// duration since the last call to start().
    double stop()
    {
      auto t = clock::now();
      typedef std::chrono::duration<double> seconds;
      return std::chrono::duration_cast<seconds>(t - start_).count();
    }
  };

  /// A structure to record elapsed time in clock ticks.
  struct time_info
  {
    time_info()
      : utime(0), stime(0), cutime(0), cstime(0)
    {
    }
    clock_t utime;
    clock_t stime;
    clock_t cutime;
    clock_t cstime;
  };

  /// A timekeeper that accumulate interval of time in a more detailed way.
  /// For instance, you can get the time spent with or without children
  /// processes.
  class timer
  {
  public:
    timer()
      : running(false)
    {
    }

    /// Start a time interval.
    void
    start()
    {
      SPOT_ASSERT(!running);
      running = true;
#ifdef SPOT_HAVE_TIMES
      struct tms tmp;
      times(&tmp);
      start_.utime = tmp.tms_utime;
      start_.cutime = tmp.tms_cutime;
      start_.stime = tmp.tms_stime;
      start_.cstime = tmp.tms_cstime;
#else
      start_.utime = clock();
#endif
    }

    /// Stop a time interval and update the sum of all intervals.
    void
    stop()
    {
#ifdef SPOT_HAVE_TIMES
      struct tms tmp;
      times(&tmp);
      total_.utime += tmp.tms_utime - start_.utime;
      total_.cutime += tmp.tms_cutime - start_.cutime;
      total_.stime += tmp.tms_stime - start_.stime;
      total_.cstime += tmp.tms_cstime - start_.cstime;
#else
      total_.utime += clock() - start_.utime;
#endif
      SPOT_ASSERT(running);
      running = false;
    }

    /// \brief Return the user time of the current process (without children)
    /// of all accumulated interval.
    ///
    /// Any time interval that has been start()ed but not stop()ed
    /// will not be accounted for.
    clock_t
    utime() const
    {
      return total_.utime;
    }

    /// \brief Return the user time of children of all accumulated interval.
    ///
    /// Any time interval that has been start()ed but not stop()ed
    /// will not be accounted for.
    clock_t
    cutime() const
    {
      return total_.cutime;
    }

    /// \brief Return the system time of the current process (whithout children)
    /// of all accumulated interval.
    ///
    /// Any time interval that has been start()ed but not stop()ed
    /// will not be accounted for.
    clock_t
    stime() const
    {
      return total_.stime;
    }

    /// \brief Return the system time of children of all accumulated interval.
    ///
    /// Any time interval that has been start()ed but not stop()ed
    /// will not be accounted for.
    clock_t
    cstime() const
    {
      return total_.cstime;
    }

    clock_t get_uscp(bool user, bool system, bool children, bool parent) const
    {
      clock_t res = 0;

      if (user && parent)
        res += utime();

      if (user && children)
        res += cutime();

      if (system && parent)
        res += stime();

      if (system && children)
        res += cstime();

      return res;
    }

    /// \brief Whether the timer is running.
    bool
    is_running() const
    {
      return running;
    }

  protected:
    time_info start_;
    time_info total_;
    bool running;
  };

  // This function declared here must be implemented in each file
  // that includes this header, well, only if this operator is needed!
  inline std::ostream& operator<<(std::ostream& os, const timer& dt);

  /// \brief A map of timer, where each timer has a name.
  ///
  /// Timer_map also keeps track of the number of measures each timer
  /// has performed.
  class timer_map
  {
  public:

    /// \brief Start a timer with name \a name.
    ///
    /// The timer is created if it did not exist already.
    /// Once started, a timer should be either stop()ed or
    /// cancel()ed.
    void
    start(const std::string& name)
    {
      item_type& it = tm[name];
      it.first.start();
      ++it.second;
    }

    /// \brief Stop timer \a name.
    ///
    /// The timer must have been previously started with start().
    void
    stop(const std::string& name)
    {
      tm[name].first.stop();
    }

    /// \brief Cancel timer \a name.
    ///
    /// The timer must have been previously started with start().
    ///
    /// This cancel only the current measure.  (Previous measures
    /// recorded by the timer are preserved.)  When a timer that has
    /// not done any measure is canceled, it is removed from the map.
    void
    cancel(const std::string& name)
    {
      tm_type::iterator i = tm.find(name);
      if (SPOT_UNLIKELY(i == tm.end()))
        throw std::invalid_argument("timer_map::cancel(): unknown name");
      SPOT_ASSERT(0 < i->second.second);
      if (0 == --i->second.second)
        tm.erase(i);
    }

    /// Return the timer \a name.
    const spot::timer&
    timer(const std::string& name) const
    {
      tm_type::const_iterator i = tm.find(name);
      if (SPOT_UNLIKELY(i == tm.end()))
        throw std::invalid_argument("timer_map::timer(): unknown name");
      return i->second.first;
    }

    /// \brief Whether there is no timer in the map.
    ///
    /// If empty() return true, then either no timer where ever
    /// started, or all started timers were canceled without
    /// completing any measure.
    bool
    empty() const
    {
      return tm.empty();
    }

    /// Format information about all timers in a table.
    SPOT_API std::ostream&
    print(std::ostream& os) const;

    /// \brief Remove information about all timers.
    void
    reset_all()
    {
      tm.clear();
    }

  protected:
    typedef std::pair<spot::timer, int> item_type;
    typedef std::map<std::string, item_type> tm_type;
    tm_type tm;
  };

  /// \brief Struct used to start and stop both timer and stopwatch clocks.
  typedef struct process_timer
  {
    void start()
    {
      walltimer.start();
      cputimer.start();
    }
    // sw.stop() --> It always returns the duration since the last call to
    // start(). Therefore, it wont't stop timing, moreover, it can be called
    // multiple times.
    void stop()
    {
      walltime_lap_ = walltimer.stop();
      cputimer.stop();
    }

    double walltime() const
    {
      return walltime_lap_;
    }

    clock_t cputime(bool user, bool system, bool children, bool parent) const
    {
      return cputimer.get_uscp(user, system, children, parent);
    }

  private:
    spot::timer cputimer;
    spot::stopwatch walltimer;
    double walltime_lap_ = 0;
  } process_timer;

  /// @}
}
