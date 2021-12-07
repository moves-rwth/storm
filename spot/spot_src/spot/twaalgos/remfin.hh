// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017, 2018, 2020 Laboratoire de Recherche et
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
  /// \ingroup twa_acc_transform
  /// \brief Check if \a aut is Rablin-like and Büchi-realizable.
  ///
  /// This is inspired from rabin_to_buchi_maybe()'s algorithm. The
  /// main difference is that here, no automaton is built.
  ///
  /// If the input is non-deterministic, this algorithm may fail
  /// to detect Büchi-realizability (false-negative).
  SPOT_API bool
  rabin_is_buchi_realizable(const const_twa_graph_ptr& aut);

  /// \ingroup twa_acc_transform
  /// \brief Convert a Rabin-like automaton into a Büchi automaton only
  /// when it can be done without changing the automaton structure.
  ///
  /// If \a aut is a Rabin-like automaton that is not Büchi-realizable,
  /// this returns a Büchi automaton equivalent to \a aut that use
  /// exactly the same transition structure (the order of edges is
  /// even preserved).  In particular, determinism is preserved.
  ///
  /// If \a aut is not a Rabin-like automaton or is not
  /// Büchi-realizable, return `nullptr`.
  ///
  /// If the input is non-deterministic, this algorithm may fail
  /// to detect Büchi-realizability (false-negative).
  SPOT_API twa_graph_ptr
  rabin_to_buchi_if_realizable(const const_twa_graph_ptr& aut);

  /// \ingroup twa_acc_transform
  ///
  /// \brief Convert a Rabin-like automaton into a Büchi automaton,
  /// preserving determinism when possible.
  ///
  /// Return nullptr if the input is not a Rabin (or Rabin-like)
  /// automaton.
  ///
  /// This essentially applies the algorithm from "Deterministic
  /// ω-automata vis-a-vis Deterministic Büchi Automata", S. Krishnan,
  /// A. Puri, and R. Brayton (ISAAC'94), but SCC-wise.
  ///
  /// Unless you know what you are doing, you are probably better off
  /// calling remove_fin() instead, as this will call more specialized
  /// algorithms (e.g., for weak automata) when appropriate, and will
  /// deal with more than just Rabin-like automata.
  SPOT_API twa_graph_ptr
  rabin_to_buchi_maybe(const const_twa_graph_ptr& aut);

  /// \ingroup twa_acc_transform
  /// \brief Rewrite an automaton without Fin or f acceptance.
  ///
  /// This algorithm dispatches between many strategies.  It has
  /// dedicated algorithms for weak automata, automata with Rabin-like
  /// acceptance, automata with Streett-like acceptance, and some
  /// generic code that will work on any kind of acceptance condition.
  ///
  /// In Spot "f" acceptance is not considered Fin-less, because
  /// it can be seen as a case of generalized co-Büchi with 0 sets.
  /// Just like "t" corresponds generalized Büchi with 0 sets.)
  SPOT_API twa_graph_ptr
  remove_fin(const const_twa_graph_ptr& aut);
}
