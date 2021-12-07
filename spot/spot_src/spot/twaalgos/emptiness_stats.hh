// -*- coding: utf-8 -*-
// Copyright (C) 2015-2017 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <spot/misc/ltstr.hh>
#include <map>

namespace spot
{

  /// \addtogroup emptiness_check_stats
  /// @{

  struct unsigned_statistics
  {
    virtual
    ~unsigned_statistics()
    {
    }

    unsigned
    get(const char* str) const
    {
      auto i = stats.find(str);
      SPOT_ASSERT(i != stats.end());
      return (this->*i->second)();
    }

    typedef unsigned (unsigned_statistics::*unsigned_fun)() const;
    typedef std::map<const char*, unsigned_fun, char_ptr_less_than> stats_map;
    stats_map stats;
  };

  /// \brief Emptiness-check statistics
  ///
  /// Implementations of spot::emptiness_check may also implement
  /// this interface.  Try to dynamic_cast the spot::emptiness_check
  /// pointer to know whether these statistics are available.
  class ec_statistics: public unsigned_statistics
  {
  public :
    ec_statistics()
    : states_(0), transitions_(0), depth_(0), max_depth_(0)
    {
      stats["states"] =
        static_cast<unsigned_statistics::unsigned_fun>(&ec_statistics::states);
      stats["transitions"] =
        static_cast<unsigned_statistics::unsigned_fun>
          (&ec_statistics::transitions);
      stats["max. depth"] =
        static_cast<unsigned_statistics::unsigned_fun>
          (&ec_statistics::max_depth);
    }

    void
    set_states(unsigned n)
    {
      states_ = n;
    }

    void
    inc_states()
    {
      ++states_;
    }

    void
    inc_transitions()
    {
      ++transitions_;
    }

    void
    inc_depth(unsigned n = 1)
    {
      depth_ += n;
      if (depth_ > max_depth_)
        max_depth_ = depth_;
    }

    void
    dec_depth(unsigned n = 1)
    {
      SPOT_ASSERT(depth_ >= n);
      depth_ -= n;
    }

    unsigned
    states() const
    {
      return states_;
    }

    unsigned
    transitions() const
    {
      return transitions_;
    }

    unsigned
    max_depth() const
    {
      return max_depth_;
    }

    unsigned
    depth() const
    {
      return depth_;
    }

  private :
    unsigned states_;                /// number of disctint visited states
    unsigned transitions_;        /// number of visited transitions
    unsigned depth_;                /// maximal depth of the stack(s)
    unsigned max_depth_;        /// maximal depth of the stack(s)
  };

  /// \brief Accepting Run Search statistics.
  ///
  /// Implementations of spot::emptiness_check_result may also implement
  /// this interface.  Try to dynamic_cast the spot::emptiness_check_result
  /// pointer to know whether these statistics are available.
  class ars_statistics: public unsigned_statistics
  {
  public:
    ars_statistics()
      : prefix_states_(0), cycle_states_(0)
    {
      stats["(non unique) states for prefix"] =
        static_cast<unsigned_statistics::unsigned_fun>
          (&ars_statistics::ars_prefix_states);
      stats["(non unique) states for cycle"] =
        static_cast<unsigned_statistics::unsigned_fun>
          (&ars_statistics::ars_cycle_states);
    }

    void
    inc_ars_prefix_states()
    {
      ++prefix_states_;
    }

    unsigned
    ars_prefix_states() const
    {
      return prefix_states_;
    }

    void
    inc_ars_cycle_states()
    {
      ++cycle_states_;
    }

    unsigned
    ars_cycle_states() const
    {
      return cycle_states_;
    }

  private:
    unsigned prefix_states_;        /// states visited to construct the prefix
    unsigned cycle_states_;        /// states visited to construct the cycle
  };

  /// \brief Accepting Cycle Search Space statistics
  ///
  /// Implementations of spot::emptiness_check_result may also implement
  /// this interface.  Try to dynamic_cast the spot::emptiness_check_result
  /// pointer to know whether these statistics are available.
  class acss_statistics: public ars_statistics
  {
  public:
    acss_statistics()
    {
      stats["search space states"] =
        static_cast<unsigned_statistics::unsigned_fun>
          (&acss_statistics::acss_states);
    }

    virtual
    ~acss_statistics()
    {
    }

    /// Number of states in the search space for the accepting cycle.
    virtual unsigned acss_states() const = 0;
  };
  /// @}
}
