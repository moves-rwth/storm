// -*- coding: utf-8 -*-
// Copyright (C) 2015-2016, 2018-2019 Laboratoire de Recherche et
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

#include <unordered_map>

namespace spot
{
  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return
  /// an equivalent Generalized Büchi automaton.
  ///
  /// This dispatch between many algorithms.  If the input has Streett
  /// (or Streett-like) acceptance,
  /// spot::streett_to_generalized_buchi() is called right away and
  /// produces a TGBA.  Otherwise, it calls spot::remove_in() which
  /// returns a TBA.
  SPOT_API twa_graph_ptr
  to_generalized_buchi(const const_twa_graph_ptr& aut);

  /// \ingroup twa_acc_transform
  /// \brief Convert Streett acceptance into generalized Büchi
  /// acceptance.
  SPOT_API twa_graph_ptr
  streett_to_generalized_buchi(const const_twa_graph_ptr& in);

  /// \ingroup twa_acc_transform
  /// \brief Convert Streett acceptance into generalized Büchi
  ///
  /// This version only works SPOT_STREET_CONF_MIN is set to a number
  /// of pairs less than the number of pairs used by \a in.  The
  /// default is 3.  It returns nullptr of that condition is not met,
  /// or if the input automaton does not have Streett-like acceptance.
  SPOT_API twa_graph_ptr
  streett_to_generalized_buchi_maybe(const const_twa_graph_ptr& in);

  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return
  /// an equivalent Generalized Rabin automaton.
  ///
  /// This works by putting the acceptance condition in disjunctive
  /// normal form, and then merging all the
  /// Fin(x1)&Fin(x2)&...&Fin(xn) that may occur in clauses into a
  /// single Fin(X).
  ///
  /// The acceptance-set numbers used by Inf may appear in
  /// multiple clauses if \a share_inf is set.
  SPOT_API twa_graph_ptr
  to_generalized_rabin(const const_twa_graph_ptr& aut,
                       bool share_inf = false);

  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return
  /// an equivalent Generalized Streett automaton.
  ///
  /// This works by putting the acceptance condition in cunjunctive
  /// normal form, and then merging all the
  /// Inf(x1)|Inf(x2)|...|Inf(xn) that may occur in clauses into a
  /// single Inf(X).
  ///
  /// The acceptance-set numbers used by Fin may appear in
  /// multiple clauses if \a share_fin is set.
  SPOT_API twa_graph_ptr
  to_generalized_streett(const const_twa_graph_ptr& aut,
                         bool share_fin = false);

  /// \ingroup twa_acc_transform
  /// \brief Converts any DNF acceptance condition into Streett-like.
  ///
  /// This function is an optimized version of the construction described
  /// by Lemma 4 and 5 of \cite boker.2011.fossacs .
  ///
  /// In the described construction, as many copies as there are minterms in
  /// the acceptance condition are made and the union of all those copies is
  /// returned.
  /// Instead of cloning the automaton for each minterm and end up with many
  /// rejecting and useless SCC, we construct the automaton SCC by SCC. Each SCC
  /// is copied at most as many times as there are minterms for which it is not
  /// rejecting and at least one time if it is always rejecting (to be
  /// consistent with the recognized language).
  ///
  /// \a aut The automaton to convert.
  /// \a original_states Enable mapping between each state of the resulting
  /// automaton and the original state of the input automaton. This is stored
  /// in the "original-states" named property of the produced automaton. Call
  /// `aut->get_named_prop<std::vector<unsigned>>("original-states")`
  /// to retrieve it.  Additionally, the correspondence between each created
  /// state and the associated DNF clause is recorded in the
  /// "original-clauses" property (also a vector of unsigned).
  SPOT_API twa_graph_ptr
  dnf_to_streett(const const_twa_graph_ptr& aut, bool original_states = false);
}
