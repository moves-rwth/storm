// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2015, 2018 Laboratoire de Recherche et Developpement
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

#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \addtogroup twa_misc
  /// @{

  /// \brief Whether the automaton \a aut is unambiguous.
  ///
  /// An automaton is unambiguous if each accepted word is
  /// recognized by only one path.
  ///
  /// We check unambiguousity by synchronizing the automaton with
  /// itself, and then making sure that the co-reachable part of the
  /// squared automaton has the same size as the co-reachable part of
  /// the original automaton.
  SPOT_API bool
  is_unambiguous(const const_twa_graph_ptr& aut);

  /// Like is_unambiguous(), but also sets the property in the twa.
  SPOT_API bool
  check_unambiguous(const twa_graph_ptr& aut);
  /// @}
}
