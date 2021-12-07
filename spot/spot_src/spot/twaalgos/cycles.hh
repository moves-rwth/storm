// -*- coding: utf-8 -*-
// Copyright (C) 2012-2015, 2018-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#include <spot/twaalgos/sccinfo.hh>
#include <spot/misc/hash.hh>
#include <deque>

namespace spot
{
  /// \brief Enumerate elementary cycles in a SCC.
  ///
  /// This class implements a non-recursive version of the algorithm
  /// on page 170 of \cite loizou.82.is .
  /// (the additional preprocessings described later in that paper are
  /// not implemented).
  ///
  /// It should be noted that although the above paper does not
  /// consider multiple arcs and self-loops in its definitions, the
  /// algorithm they present works as expected in these cases.
  ///
  /// For our purpose an elementary cycle is a sequence of transitions
  /// that form a cycle and that visit a state at most once.  We may
  /// have two cycles that visit the same states in the same order if
  /// some pair of states are connected by several transitions.  Also
  /// A cycle may visit only one state if it is a self-loop.
  ///
  /// We represent a cycle by a sequence of succ_iterator objects
  /// positioned on the transition contributing to the cycle.  These
  /// succ_itertor are stored, along with their source state, in the
  /// dfs_ stack.  Only the last portion of this stack may form a
  /// cycle.
  ///
  /// Calling <code>run(n)</code> will enumerate all elementary cycles
  /// in SCC <code>n</code>.  Each time an SCC is found, the method
  /// cycle_found(s) is called with the initial state s of the cycle:
  /// the cycle is constituted from all the states that are on the \c
  /// dfs_ stack after \c s (including \c s).
  ///
  /// You should inherit from this class and redefine the
  /// cycle_found() method to perform any work you would like to do on
  /// the enumerated cycles.  If cycle_found() returns false, the
  /// run() method will terminate.  If it returns true, the run()
  /// method will search for the next elementary cycle and call
  /// cycle_found() again if it finds another cycle.
  class SPOT_API enumerate_cycles
  {
  protected:
    // Extra information required for the algorithm for each state.
    struct state_info
    {
      state_info(unsigned num)
        : seen(false), reach(false), mark(false), del(num)
      {
      }
      bool seen;
      // Whether the state has already left the stack at least once.
      bool reach;
      // set to true when the state current state w is stacked, and
      // reset either when the state is unstacked after having
      // contributed to a cycle, or when some state z that (1) w could
      // reach (even indirectly) without discovering a cycle, and (2)
      // that a contributed to a contributed to a cycle.
      bool mark;
      // Deleted successors (in the paper, states deleted from A(x))
      std::vector<bool> del;
      // Predecessors of the current states, that could not yet
      // contribute to a cycle.
      std::vector<unsigned> b;
    };

    // The automaton we are working on.
    const_twa_graph_ptr aut_;
    // Store the state_info for all visited states.
    std::vector<state_info> info_;
    // The SCC map built for aut_.
    const scc_info& sm_;

    // The DFS stack.  Each entry contains a state, an iterator on the
    // transitions leaving that state, and a Boolean f indicating
    // whether this state as already contributed to a cycle (f is
    // updated when backtracking, so it should not be used by
    // cycle_found()).
    struct dfs_entry
    {
      unsigned s;
      unsigned succ = 0U;
      bool f = false;
      dfs_entry(unsigned s) noexcept
        : s(s)
      {
      }
    };
    typedef std::vector<dfs_entry> dfs_stack;
    dfs_stack dfs_;

  public:
    enumerate_cycles(const scc_info& map);
    virtual ~enumerate_cycles() {}

    /// \brief Run in SCC scc, and call \a cycle_found() for any new
    /// elementary cycle found.
    ///
    /// It is safe to call this method multiple times, for instance to
    /// enumerate the cycle of each SCC.
    void run(unsigned scc);


    /// \brief Called whenever a cycle was found.
    ///
    /// The cycle uses all the states from the dfs stack, starting
    /// from the one labeled \a start.  The iterators in the DFS stack
    /// are all pointing to the transition considered for the cycle.
    ///
    /// This method is not const so you can modify private variables
    /// to your subclass, but it should definitely NOT modify the dfs
    /// stack or the tags map.
    ///
    /// The default implementation, not very useful, will print the
    /// states in the cycle on std::cout.
    ///
    /// This method method should return false iff no more cycles need
    /// should be enumerated by run().
    virtual bool cycle_found(unsigned start);

  private:
    // add a new state to the dfs_ stack
    void push_state(unsigned s);
    // block the edge (x,y) because it cannot contribute to a new
    // cycle currently (sub-procedure from the paper)
    void nocycle(unsigned x, unsigned y);
    // unmark the state y (sub-procedure from the paper)
    void unmark(unsigned y);
  };
}
