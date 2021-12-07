// -*- coding: utf-8 -*-
// Copyright (C) 2012-2017 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003-2005 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief Build an explicit automaton from all states of \a aut,
  ///
  /// This function was deprecated in Spot 2.4.  Use the
  /// function make_twa_graph() instead.
  SPOT_DEPRECATED("use make_twa_graph() instead")
  SPOT_API twa_graph_ptr
  inline copy(const const_twa_ptr& aut, twa::prop_set p,
              bool preserve_names = false, unsigned max_states = -1U)
  {
    return make_twa_graph(aut, p, preserve_names, max_states);
  }
}
