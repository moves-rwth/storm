// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2015, 2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/tl/apcollect.hh>
#include <spot/tl/defaultenv.hh>
#include <spot/twa/bdddict.hh>
#include <spot/twa/acc.hh>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief Construct a twa randomly.
  ///
  /// \param n The number of states wanted in the automata (>0).  All states
  ///          will be connected, and there will be no dead state.
  /// \param d The density of the automata.  This is the probability
  ///          (between 0.0 and 1.0), to add a transition between two
  ///          states.  All states have at least one outgoing transition,
  ///          so \a d is considered only when adding the remaining transition.
  ///          A density of 1 means all states will be connected to each other.
  /// \param ap The list of atomic property that should label the transition.
  /// \param dict The bdd_dict to used for this automata.
  /// \param n_accs The number of acceptance sets to use.
  ///          If this number is non null, then there is no guarantee
  ///          that the generated graph contains an accepting cycle (raise
  ///          the value of \a a to improve the chances).
  /// \param a The probability (between 0.0 and 1.0) that a transition belongs
  ///          to an acceptance set.
  /// \param t The probability (between 0.0 and 1.0) that an atomic proposition
  ///          is true.
  /// \param deterministic build a complete and deterministic automaton
  /// \param state_acc build an automaton with state-based acceptance
  /// \param colored build an automaton in which each transition (or state)
  ///          belongs to a single acceptance set.
  ///
  /// This algorithms is adapted from the one in Fig 6.2 page 48 of
  /// \cite tauriainen.00.tr .
  ///
  /// Although the intent is similar, there are some differences
  /// between the above published algorithm and this implementation.
  /// First labels are on transitions, and acceptance conditions are
  /// generated too.  Second, the number of successors of a node is
  /// chosen in \f$[1,n]\f$ following a normal distribution with mean
  /// \f$1+(n-1)d\f$ and variance \f$(n-1)d(1-d)\f$.  (This is less
  /// accurate, but faster than considering all possible \a n
  /// successors one by one.)
  ///
  /// Note that while this constructs an automaton with random
  /// acceptance sets, this does not set the acceptance condition.
  SPOT_API twa_graph_ptr
  random_graph(int n, float d,
               const atomic_prop_set* ap, const bdd_dict_ptr& dict,
               unsigned n_accs = 0, float a = 0.1, float t = 0.5,
               bool deterministic = false, bool state_acc = false,
               bool colored = false);

  /// Build a random acceptance where each acceptance sets is used once.
  SPOT_API acc_cond::acc_code random_acceptance(unsigned n_accs);
}
