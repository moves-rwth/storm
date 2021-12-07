// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018, 2019 Laboratoire de Recherche et Développement
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

#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief Complement an automaton by dualizing it.
  ///
  /// Given an automaton \a aut of any type, produces the dual as output. The
  /// automaton will be completed if it isn't already. If it is deterministic
  /// and complete, complementing the automaton can be done by just
  /// complementing the acceptance condition.
  ///
  /// In particular, this implies that an input that use generalized Büchi will
  /// be output as generalized co-Büchi.
  ///
  /// Functions like to_generalized_buchi() or remove_fin() are frequently
  /// called on existential automata after dualize() to obtain an easier
  /// acceptance condition, but maybe at the cost of losing determinism.
  ///
  /// If the input automaton is deterministic, the output will be deterministic.
  /// If the input automaton is existential, the output will be universal.
  /// If the input automaton is universal, the output will be existential.
  /// Finally, if the input automaton is alternating, the result is alternating.
  /// More can be found on page 22 (Definition 1.6) of \cite loding.98.msc .
  SPOT_API twa_graph_ptr
  dualize(const const_twa_graph_ptr& aut);
}
