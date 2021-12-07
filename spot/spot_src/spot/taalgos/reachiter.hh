// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2016 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
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

#include <spot/misc/hash.hh>
#include <spot/ta/ta.hh>
#include <stack>
#include <deque>

namespace spot
{
  /// \ingroup ta_generic
  /// \brief Iterate over all reachable states of a spot::ta.
  class SPOT_API ta_reachable_iterator
  {
  public:
    ta_reachable_iterator(const const_ta_ptr& a);
    virtual
    ~ta_reachable_iterator();

    /// \brief Iterate over all reachable states of a spot::ta.
    ///
    /// This is a template method that will call add_state(), next_state(),
    /// start(), end(), process_state(), and process_link(), while it
    /// iterates over states.
    void
    run();

    /// \name Todo list management.
    ///
    /// spot::ta_reachable_iterator_depth_first and
    /// spot::ta_reachable_iterator_breadth_first offer two precanned
    /// implementations for these functions.
    /// \{
    /// \brief Called by run() to register newly discovered states.
    virtual void
    add_state(const state* s) = 0;
    /// \brief Called by run() to obtain the next state to process.
    virtual const state*
    next_state() = 0;
    /// \}

    /// Called by add_state or next_states implementations to filter
    /// states.  Default implementation always return true.
    virtual bool
    want_state(const state* s) const;

    /// Called by run() before starting its iteration.
    virtual void
    start();
    /// Called by run() once all states have been explored.
    virtual void
    end();

    /// Called by run() to process a state.
    ///
    /// \param s The current state.
    /// \param n A unique number assigned to \a s.
    virtual void
    process_state(const state* s, int n);
    /// Called by run() to process a transition.
    ///
    /// \param in The source state number.
    /// \param out The destination state number.
    /// \param si The spot::twa_succ_iterator positionned on the current
    ///             transition.
    virtual void
    process_link(int in, int out, const ta_succ_iterator* si);

  protected:

    const_ta_ptr t_automata_; ///< The spot::ta to explore.

    state_map<int> seen; ///< States already seen.
  };

  /// \ingroup ta_generic
  /// \brief An implementation of spot::ta_reachable_iterator that browses
  /// states depth first.
  class SPOT_API ta_reachable_iterator_depth_first
    : public ta_reachable_iterator
  {
  public:
    ta_reachable_iterator_depth_first(const const_ta_ptr& a);

    virtual void add_state(const state* s) override;
    virtual const state* next_state() override;

  protected:
    std::stack<const state*> todo; ///< A stack of states yet to explore.
  };

  /// \ingroup ta_generic
  /// \brief An implementation of spot::ta_reachable_iterator that browses
  /// states breadth first.
  class SPOT_API ta_reachable_iterator_breadth_first
    : public ta_reachable_iterator
  {
  public:
    ta_reachable_iterator_breadth_first(const const_ta_ptr& a);

    virtual void add_state(const state* s) override;
    virtual const state* next_state() override;

  protected:
    std::deque<const state*> todo; ///< A queue of states yet to explore.
  };
}
