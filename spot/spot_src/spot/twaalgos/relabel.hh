// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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

#include <spot/tl/relabel.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \brief replace atomic propositions in an automaton
  ///
  /// The relabeling map \a relmap should have keys that are atomic
  /// propositions, and values that are Boolean formulas.
  ///
  /// This function is typically used with maps produced by relabel()
  /// or relabel_bse().
  SPOT_API void
  relabel_here(twa_graph_ptr& aut, relabeling_map* relmap);
}
