// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2018 Laboratoire de Recherche et Développement
// de l'Epita.
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
  /// \brief Whether the Inf and Fin numbers are disjoints
  SPOT_API bool
  has_separate_sets(const const_twa_graph_ptr& aut);

  /// \ingroup twa_acc_transform
  /// \brief Separate the Fin and Inf sets used by an automaton
  ///
  /// This makes sure that the numbers used a Fin and Inf are
  /// disjoints.
  SPOT_API twa_graph_ptr
  separate_sets_here(const twa_graph_ptr& aut);
}
