// -*- coding: utf-8 -*-
// Copyright (C) 2013-2015, 2017, 2019 Laboratoire de Recherche et
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
#include <spot/twaalgos/powerset.hh>

namespace spot
{
  /// \brief Complement a deterministic TωA
  ///
  /// The automaton \a aut should be deterministic.  It will be
  /// completed if it isn't already.  In these conditions,
  /// complementing the automaton can be done by just complementing
  /// the acceptance condition.
  ///
  /// In particular, this implies that an input that use
  /// generalized Büchi will be output as generalized co-Büchi.
  ///
  /// Functions like to_generalized_buchi() or remove_fin() are
  /// frequently called after dtwa_complement() to obtain an easier
  /// acceptance condition (maybe at the cost of losing determinism.)
  ///
  /// This function was deprecated in spot 2.4. Call the function
  /// spot::dualize() instead, that is able to complement any input
  /// automaton, not only deterministic ones.
  SPOT_DEPRECATED("use spot::dualize() instead")
  SPOT_API twa_graph_ptr
  dtwa_complement(const const_twa_graph_ptr& aut);

  /// \brief Complement a semideterministic TωA
  ///
  /// The automaton \a aut should be semideterministic.
  ///
  /// Uses the NCSB algorithm described by F. Blahoudek, M. Heizmann,
  /// S. Schewe, J. Strejček, and MH. Tsai (TACAS'16).
  SPOT_API twa_graph_ptr
  complement_semidet(const const_twa_graph_ptr& aut, bool show_names = false);


  /// \brief Complement a TωA
  ///
  /// This employs different complementation strategies depending
  /// on the type of the automaton.
  ///
  /// If the input is alternating, the output may be alternating and
  /// is simply the result of calling dualize().
  ///
  /// If the input is not alternating, the output will not be
  /// alternating, but could have any acceptance condition.
  /// - deterministic inputs are passed to dualize()
  /// - very weak automata are also dualized, and then
  ///   passed to remove_alternation() to obtain a TGBA
  /// - any other type of input is determized before
  ///   complementation.
  ///
  /// If an output_aborter is supplied, it is used to
  /// abort the construction of larger automata.
  ///
  /// complement_semidet() is not yet used.
  SPOT_API twa_graph_ptr
  complement(const const_twa_graph_ptr& aut,
             const output_aborter* aborter = nullptr);

}
