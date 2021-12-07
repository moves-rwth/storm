// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2015, 2019 Laboratoire de Recherche et
// Développement de l'Epita.
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

#include <set>
#include <vector>
#include <iosfwd>
#include <spot/twa/twagraph.hh>

namespace spot
{

  struct SPOT_API power_map
  {
    typedef std::set<unsigned> power_state;
    std::vector<power_state> map_;

    const power_state&
    states_of(unsigned s) const
    {
      return map_.at(s);
    }
  };

  /// \brief Helper object to specify when an algorithm
  /// should abort its construction.
  class SPOT_API output_aborter
  {
    unsigned max_states_;
    unsigned max_edges_;
    mutable bool reason_is_states_;
  public:
    output_aborter(unsigned max_states,
                   unsigned max_edges = ~0U)
      : max_states_(max_states), max_edges_(max_edges)
    {
    }

    unsigned max_states() const
    {
      return max_states_;
    }

    unsigned max_edges() const
    {
      return max_edges_;
    }

    bool too_large(const const_twa_graph_ptr& aut) const
    {
      bool too_many_states = aut->num_states() > max_states_;
      if (!too_many_states && (aut->num_edges() <= max_edges_))
        return false;
      // Only update the reason if we return true;
      reason_is_states_ = too_many_states;
      return true;
    }

    std::ostream& print_reason(std::ostream&) const;
  };


  /// \ingroup twa_misc
  /// \brief Build a deterministic automaton, ignoring acceptance conditions.
  ///
  /// This create a deterministic automaton that recognizes the
  /// same language as \a aut would if its acceptance conditions
  /// were ignored.  This is the classical powerset algorithm.
  ///
  /// If \a pm is supplied it will be filled with the set of original states
  /// associated to each state of the deterministic automaton.
  /// The \a merge argument can be set to false to prevent merging of
  /// transitions.
  ///
  /// If ab \a aborter is given, abort the construction whenever it
  /// would build an automaton that is too large, and return nullptr.
  //@{
  SPOT_API twa_graph_ptr
  tgba_powerset(const const_twa_graph_ptr& aut,
                power_map& pm, bool merge = true,
                const output_aborter* aborter = nullptr);
  SPOT_API twa_graph_ptr
  tgba_powerset(const const_twa_graph_ptr& aut,
                const output_aborter* aborter = nullptr);
  //@}


  /// \brief Determinize a TBA using the powerset construction.
  ///
  /// The input automaton should have at most one acceptance
  /// condition.  Beware that not all Büchi automata can be
  /// determinized, and this procedure does not ensure that the
  /// produced automaton is equivalent to \a aut.
  ///
  /// The construction is adapted from Section 3.2 of
  /// \cite dax.07.atva
  /// only adapted to work on TBA rather than BA.
  ///
  /// If \a threshold_states is non null, abort the construction
  /// whenever it would build an automaton that is more than \a
  /// threshold_states time bigger (in term of states) than the
  /// original automaton.
  ///
  /// If \a threshold_cycles is non null, abort the construction
  /// whenever an SCC of the constructed automaton has more than \a
  /// threshold_cycles cycles.
  SPOT_API twa_graph_ptr
  tba_determinize(const const_twa_graph_ptr& aut,
                  unsigned threshold_states = 0,
                  unsigned threshold_cycles = 0);

  /// \brief Determinize a TBA and make sure it is correct.
  ///
  /// Apply tba_determinize(), then check that the result is
  /// equivalent.  If it isn't, return the original automaton.
  ///
  /// Only one of \a f or \a neg_aut needs to be supplied.  If
  /// \a neg_aut is not given, it will be built from \a f.
  ///
  /// \param aut the automaton to minimize
  ///
  /// \param threshold_states if non null, abort the construction
  /// whenever it would build an automaton that is more than \a
  /// threshold time bigger (in term of states) than the original
  /// automaton.
  ///
  /// \param threshold_cycles can be used to abort the construction
  /// if the number of cycles in a SCC of the constructed automaton
  /// is bigger than the supplied value.
  ///
  /// \param f the formula represented by the original automaton
  ///
  /// \param neg_aut an automaton representing the negation of \a aut
  ///
  /// \return a new tgba if the automaton could be determinized, \a aut if
  /// the automaton cannot be determinized, 0 if we do not know if the
  /// determinization is correct because neither \a f nor \a neg_aut
  /// were supplied.
  SPOT_API twa_graph_ptr
  tba_determinize_check(const twa_graph_ptr& aut,
                        unsigned threshold_states = 0,
                        unsigned threshold_cycles = 0,
                        formula f = nullptr,
                        const_twa_graph_ptr neg_aut = nullptr);

}
