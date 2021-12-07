// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015 Laboratoire de Recherche et Développement
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

#include <iosfwd>
#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \ingroup twa_io
  /// \brief Print reachable states in Hanoi Omega Automata format.
  ///
  /// \param os The output stream to print on.
  /// \param g The automaton to output.
  /// \param opt a set of characters each corresponding to a possible
  ///        option: (i) implicit labels for complete and
  ///        deterministic automata, (k) state labels when possible,
  ///        (s) state-based acceptance when possible, (t)
  ///        transition-based acceptance, (m) mixed acceptance, (l)
  ///        single-line output, (v) verbose properties, (1.1) use
  ///        version 1.1 of the HOA format.
  SPOT_API std::ostream&
  print_hoa(std::ostream& os,
            const const_twa_ptr& g,
            const char* opt = nullptr);
}
