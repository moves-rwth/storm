// -*- coding: utf-8 -*-
// Copyright (C) 2015-2016, 2019-2020 Laboratoire de Recherche et
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

#include <spot/twaalgos/powerset.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \ingroup twa_algorithms
  /// \brief Determinize a TGBA
  ///
  /// The main algorithm works only with automata using Büchi acceptance
  /// (preferably transition-based).  If generalized Büchi is input, it
  /// will be automatically degeneralized first.
  ///
  /// The output will be a deterministic automaton using parity acceptance.
  ///
  /// This procedure is based on an algorithm by Roman Redziejowski
  /// \cite redziejowski.12.fi .  Redziejowski's algorithm is similar
  /// to Piterman's improvement of Safra's algorithm, except it is
  /// presented on transition-based acceptance and use simpler
  /// notations.  We implement three additional optimizations (they
  /// can be individually disabled) based on
  ///
  ///   - knowledge about SCCs of the input automaton
  ///   - knowledge about simulation relations in the input automaton
  ///   - knowledge about stutter-invariance of the input automaton
  ///
  /// The last optimization is an idea implemented in
  /// [`ltl2dstar`](https://www.ltl2dstar.fr) \cite klein.07.ciaa .  In
  /// fact, `ltl2dstar` even has a finer version (letter-based
  /// stuttering) that is not implemented here.
  ///
  /// \param aut the automaton to determinize
  ///
  /// \param pretty_print whether to decorate states with names
  ///                     showing the paths they track (this only
  ///                     makes sense if the input has Büchi
  ///                     acceptance already, otherwise the input
  ///                     automaton will be degeneralized and the
  ///                     names will refer to the states in the
  ///                     degeneralized automaton).
  ///
  /// \param use_scc whether to simplify the construction based on
  ///                the SCCs in the input automaton.
  ///
  /// \param use_simulation whether to simplify the construction based
  ///                       on simulation relations between states in
  ///                       the original automaton.
  ///
  /// \param use_stutter whether to simplify the construction when the
  ///                    input automaton is known to be
  ///                    stutter-invariant.  (The stutter-invariant
  ///                    flag of the input automaton is used, so it
  ///                    might be worth to call
  ///                    spot::check_stutter_invariance() first if
  ///                    possible.)
  ///
  /// \param aborter abort the construction if the constructed
  ///                automaton would be too large.  Return nullptr
  ///                in this case.
  SPOT_API twa_graph_ptr
  tgba_determinize(const const_twa_graph_ptr& aut,
                   bool pretty_print = false,
                   bool use_scc = true,
                   bool use_simulation = true,
                   bool use_stutter = true,
                   const output_aborter* aborter = nullptr);
}
