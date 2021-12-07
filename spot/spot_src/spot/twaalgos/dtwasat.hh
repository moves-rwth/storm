// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2015, 2018 Laboratoire de Recherche et
// Développement de l'Epita.
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

#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \brief Attempt to synthetize an equivalent deterministic TωA
  /// with a SAT solver.
  ///
  /// \param a the input TωA.  It should be a deterministic TωA.
  ///
  /// \param target_acc_number is the number of acceptance sets wanted
  /// in the result.
  ///
  /// \param target_acc the target acceptance condition
  ///
  /// \param target_state_number is the desired number of states in
  /// the result.  The output may have less than \a
  /// target_state_number reachable states.
  ///
  /// \param state_based set to true to force all outgoing transitions
  /// of a state to share the same acceptance conditions.
  ///
  /// \param colored if true, force all transitions to belong to
  /// exactly one acceptance set.
  ///
  /// This functions attempts to find a TωA with \a target_acc_number
  /// acceptance sets and target_state_number states that is
  /// equivalent to \a a.  If no such TωA is found, a null pointer is
  /// returned.
  SPOT_API twa_graph_ptr
  dtwa_sat_synthetize(const const_twa_graph_ptr& a,
                      unsigned target_acc_number,
                      const acc_cond::acc_code& target_acc,
                      int target_state_number,
                      bool state_based = false,
                      bool colored = false);

  /// \brief Attempt to minimize a deterministic TωA with a SAT solver.
  ///
  /// This calls dtwa_sat_synthetize() in a loop, with a decreasing
  /// number of states, and returns the last successfully built TGBA.
  ///
  /// If no smaller TGBA exists, this returns a null pointer.
  SPOT_API twa_graph_ptr
  dtwa_sat_minimize(const const_twa_graph_ptr& a,
                    unsigned target_acc_number,
                    const acc_cond::acc_code& target_acc,
                    bool state_based = false,
                    int max_states = -1,
                    bool colored = false);

  /// \brief Attempt to minimize a deterministic TωA with a SAT solver.
  ///
  /// This calls dtwa_sat_synthetize() in a loop, but attempting to
  /// find the minimum number of states using a binary search.
  //
  /// If no smaller TBA exist, this returns a null pointer.
  SPOT_API twa_graph_ptr
  dtwa_sat_minimize_dichotomy(const const_twa_graph_ptr& a,
                              unsigned target_acc_number,
                              const acc_cond::acc_code& target_acc,
                              bool state_based = false,
                              bool langmap = false,
                              int max_states = -1,
                              bool colored = false);

  /// \brief Attempt to minimize a deterministic TωA with a SAT solver.
  ///
  /// It acts like dtwa_sat_synthetisze() and obtains a first minimized
  /// automaton. Then, incrementally, it encodes and solves the deletion of one
  /// state as many time as param value.
  /// If param >= 0, this process is fully repeated until the minimal automaton
  /// is found. Otherwise, it continues to delete states one by one
  /// incrementally until the minimal automaton is found.
  ///
  /// If no smaller TGBA exists, this returns a null pointer.
  SPOT_API twa_graph_ptr
  dtwa_sat_minimize_incr(const const_twa_graph_ptr& a,
                         unsigned target_acc_number,
                         const acc_cond::acc_code& target_acc,
                         bool state_based = false,
                         int max_states = -1,
                         bool colored = false,
                         int param = 2);

  /// \brief Attempt to minimize a deterministic TωA with a SAT solver.
  ///
  /// This acts like dtba_sat_synthetize() and obtains a first minimized
  /// automaton. Then, it adds <param> assumptions, such that each assumption
  /// removes a new state and implies the previous assumptions. A first
  /// resolution is attempted assuming the last assumption (thus involving all
  /// the previous ones). If the problem is SAT several stages have just been
  /// won and all this process is restarted. Otherwise, it is known that the
  /// minimal automaton can be obtained with fewer assumption. This
  /// automaton is found dichotomously.
  ///
  /// If no smaller TGBA exists, this returns a null pointer.
  SPOT_API twa_graph_ptr
  dtwa_sat_minimize_assume(const const_twa_graph_ptr& a,
                           unsigned target_acc_number,
                           const acc_cond::acc_code& target_acc,
                           bool state_based = false,
                           int max_states = -1,
                           bool colored = false,
                           int param = 6);

  /// \brief High-level interface to SAT-based minimization
  ///
  /// Minimize the automaton \a aut, using options \a opt.
  /// These options are given as a comma-separated list of
  /// assignments of the form:
  ///
  ///   states = 10      // synthetize automaton with fixed number of states
  ///   max-states = 20  // minimize starting from this upper bound
  ///   acc = "generalized-Buchi 2"
  ///   acc = "Rabin 3"
  ///   acc = "same" /* default */
  ///   dichotomy = 1    // use dichotomy
  ///   incr = 1         // use satsolver incrementally to attempt to delete a
  ///                       fixed number of states before starting from scratch
  ///   incr < 0         // use satsolver incrementally, never restart
  ///   colored = 1      // build a colored TωA
  ///   log = "filename"
  ///
  SPOT_API twa_graph_ptr
  sat_minimize(twa_graph_ptr aut, const char* opt, bool state_based = false);
}
