// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2015, 2017 Laboratoire de Recherche et
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
  /// \brief Complete a twa_graph in place.
  ///
  /// If the TωA has an acceptance condition that is a tautology,
  /// it will be changed into a Büchi automaton.
  SPOT_API void complete_here(twa_graph_ptr aut);

  /// \brief Clone a twa and complete it.
  ///
  /// If the twa has no acceptance set, one will be added.
  SPOT_API twa_graph_ptr complete(const const_twa_ptr& aut);
}
