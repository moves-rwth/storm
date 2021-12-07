// -*- coding: utf-8 -*-
// Copyright (C) 2017 Laboratoire de Recherche et Développement
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
  /// \brief Encode and print an automaton as an AIGER circuit.
  ///
  /// The circuit actually encodes the transition relation of the automaton, not
  /// its acceptance condition. Therefore, this function will reject automata
  /// whose acceptance condition is not trivial (i.e. true).
  /// States are encoded by latches (or registers) in the circuit. Atomic
  /// propositions are encoded as inputs and outputs of the circuit. To know
  /// which AP should be encoded as outputs, print_aiger() relies on the named
  /// property "synthesis-outputs", which is a bdd containing the conjunction of
  /// such output propositions. All other AP are encoded as inputs. If the named
  /// property is not set, all AP are encoded as inputs, and the circuit has no
  /// output.
  ///
  /// \param os           The output stream to print on.
  /// \param aut          The automaton to output.
  SPOT_API std::ostream&
  print_aiger(std::ostream& os, const const_twa_ptr& aut);
}
