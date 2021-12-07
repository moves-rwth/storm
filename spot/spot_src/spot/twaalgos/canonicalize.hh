// -*- coding: utf-8 -*-
// Copyright (C) 2014 Laboratoire de Recherche et
// Developpement de l Epita (LRDE).
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
  /// \brief Reorder the states and transitions of aut in a way that will be the
  /// same for every isomorphic automata.
  SPOT_API twa_graph_ptr canonicalize(twa_graph_ptr aut);
}
