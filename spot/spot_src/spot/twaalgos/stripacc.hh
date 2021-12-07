// -*- coding: utf-8 -*-
// Copyright (C) 2012-2014, 2017 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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
  /// \brief Remove all acceptance sets from a twa_graph.
  ///
  /// This will also set the acceptance condition to true, and mark
  /// the automaton as weak.  Doing so obviously makes all recognized
  /// infinite runs accepting.
  SPOT_API void
  strip_acceptance_here(twa_graph_ptr a);
}
