// -*- coding: utf-8 -*-
// Copyright (C) 2016, 2017 Laboratoire de Recherche et Développement
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

#include <vector>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \brief Identify states that recognize the same language.
  ///
  /// The returned vector is the same size as the automaton's number of state.
  /// The number of different values (ignoring occurences) in the vector is the
  /// total number of recognized languages, states recognizing the same
  /// language have the same value.
  ///
  /// The given automaton must be deterministic.
  SPOT_API std::vector<unsigned>
  language_map(const const_twa_graph_ptr& aut);

  /// \brief Color state that recognize identical language.
  ///
  /// State that recognize a unique language will not be colored.
  SPOT_API void
  highlight_languages(twa_graph_ptr& aut);
}
