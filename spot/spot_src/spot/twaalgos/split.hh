// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement
// de l'Epita.
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
  /// \ingroup twa_misc
  /// \brief transform edges into transitions
  ///
  /// Create a new version of the automaton where all edges are split
  /// so that they are all labeled by a conjunction of all atomic
  /// propositions.  After this we can consider that each edge of the
  /// automate is a transition labeled by one letter.
  SPOT_API twa_graph_ptr split_edges(const const_twa_graph_ptr& aut);

  /// \brief make each transition a 2-step transition
  ///
  /// Given a set of atomic propositions I, split each transition
  ///     p -- cond --> q                cond in 2^2^AP
  /// into a set of transitions of the form
  ///     p -- {a} --> (p,a) -- o --> q
  /// for each a in cond \cap 2^2^I
  /// and where o = (cond & a) \cap 2^2^(AP - I)
  ///
  /// By definition, the states p are deterministic, only the states of the form
  /// (p,a) may be non-deterministic.
  /// This function is used to transform an automaton into a turn-based game in
  /// the context of LTL reactive synthesis.
  SPOT_API twa_graph_ptr
  split_2step(const const_twa_graph_ptr& aut, bdd input_bdd);

  /// \brief the reverse of split_2step
  SPOT_API twa_graph_ptr
  unsplit_2step(const const_twa_graph_ptr& aut);
}
