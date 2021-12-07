// -*- coding: utf-8 -*-
// Copyright (C) 2016 Laboratoire de Recherche et Developpement de l'EPITA.
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

#include <spot/twaalgos/emptiness.hh>

namespace spot
{
  /// \brief A rewritten version of the Couvreur emptiness check.
  ///
  /// It is optimized to run on explicit automata (avoiding the memory
  /// allocations of the virtual, abstract interface.
  /// It also has specializations for weak and terminal automata.
  SPOT_API
  emptiness_check_ptr
  get_couvreur99_new(const const_twa_ptr& a, option_map o);

  /// \brief Same as above, but always uses the abstract interface.
  ///
  /// This function is provided to test the efficiency of specializing our
  /// algorithms for explicit automata. It uses the same optimizations for
  /// weak and terminal automata as the one above.
  SPOT_API
  emptiness_check_ptr
  get_couvreur99_new_abstract(const const_twa_ptr& a, option_map o);

  /// \brief A shortcut to run the optimized emptiness check directly.
  ///
  /// This is the same as get_couvreur99_new(a, {})->check().
  SPOT_API
  emptiness_check_result_ptr
  couvreur99_new_check(const const_twa_ptr& a);
}
