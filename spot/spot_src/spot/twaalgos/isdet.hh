// -*- coding: utf-8 -*-
// Copyright (C) 2012-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \addtogroup twa_misc
  /// @{

  /// \brief Count the number of states with non-deterministic
  /// branching in \a aut.
  ///
  /// The automaton is universal if it has 0 states with
  /// non-deterministic branching but it is more efficient to call
  /// is_universal() if you do not care about the number of
  /// non-deterministic states.
  SPOT_API unsigned
  count_nondet_states(const const_twa_graph_ptr& aut);

  /// \brief Return true iff \a aut is universal.
  ///
  /// This function is more efficient than count_nondet_states() when
  /// the automaton is nondeterministic, because it can return before
  /// the entire automaton has been explored.
  ///
  /// In addition to returning the result as a Boolean, this will set
  /// the prop_universal() property of the automaton as a
  /// side-effect, so further calls will return in constant-time.
  SPOT_API bool
  is_universal(const const_twa_graph_ptr& aut);

  /// \brief Return true iff \a aut is deterministic.
  ///
  /// An automaton is called deterministic if it is both universal and
  /// existential.
  SPOT_API bool
  is_deterministic(const const_twa_graph_ptr& aut);

  /// \brief Highlight nondeterministic states
  ///
  /// A state is nondeterministic if it has two outgoing edges whose
  /// labels are not incompatibles.
  ///
  /// \param aut the automaton to process
  /// \param color the color to give to nondeterministic states.
  SPOT_API void
  highlight_nondet_states(twa_graph_ptr& aut, unsigned color);

  /// \brief Highlight nondeterministic edges
  ///
  /// An edge is nondeterministic if there exist another edge leaving
  /// the same source state, with a compatible label (i.e., the
  /// conjunction of the two labels is not false).
  ///
  /// \param aut the automaton to process
  /// \param color the color to give to nondeterministic edges.
  SPOT_API void
  highlight_nondet_edges(twa_graph_ptr& aut, unsigned color);
  /// @}

  /// \brief Highlight the deterministic part of the automaton
  ///
  /// In the case of a semideterministic automaton, highlights the
  /// states reachable from any accepting SCC.
  ///
  /// \param si the SCC information of the automaton to process
  /// \param color the color to give to states reachable from accepting SCCs.
  SPOT_API void
  highlight_semidet_sccs(scc_info& si, unsigned color);

  /// \brief Return true iff \a aut is complete.
  ///
  /// An automaton is complete if its translation relation is total,
  /// i.e., each state as a successor for any possible configuration.
  SPOT_API bool
  is_complete(const const_twa_graph_ptr& aut);

  /// \brief Return true iff \a aut is semi-deterministic.
  ///
  /// An automaton is semi-deterministic if the sub-automaton
  /// reachable from any accepting SCC is deterministic.
  SPOT_API bool
  is_semi_deterministic(const const_twa_graph_ptr& aut);

  /// \brief Whether an SCC is in the deterministic part of an automaton
  SPOT_API std::vector<bool>
  semidet_sccs(scc_info& si);

  /// \brief Set the deterministic and semi-deterministic properties
  /// appropriately.
  SPOT_API void check_determinism(twa_graph_ptr aut);

  // \brief Count states with some universal branching.
  //
  // This counts the number of states that have edges going to several
  // destinations at once (as reported by aut->is_univ_dest(...)).
  //
  // Note that nondeterministic automata (which include deterministic
  // automata) have 0 such state, but additionally they also have
  // "singleton" initial state (which this function does not check).
  //
  // \see count_univbranch_edges()
  SPOT_API unsigned
  count_univbranch_states(const const_twa_graph_ptr& aut);

  // \brief Count edges with universal branching.
  //
  // This counts the number of edges going to several destination at
  // once (as reported by aut->is_univ_dest(...)).
  //
  // If the automaton starts in multiple initial states at once, this
  // is considered as a universal "initial edge", and adds one to the
  // total count.
  //
  // Nondeterministic automata (which include deterministic automata)
  // have 0 edges with universal branching.
  //
  // \see count_univbranch_states()
  SPOT_API unsigned
  count_univbranch_edges(const const_twa_graph_ptr& aut);
}
