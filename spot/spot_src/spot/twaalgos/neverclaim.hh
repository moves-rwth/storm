// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011, 2012, 2013, 2014, 2015 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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
#include <spot/twa/fwd.hh>
#include <spot/misc/common.hh>

namespace spot
{
  /// \ingroup twa_io
  /// \brief Print reachable states in Spin never claim format.
  ///
  /// \param os The output stream to print on.
  /// \param g The (state-based degeneralized) automaton to output.
  ///          There should be only one acceptance condition, and
  ///          all the transitions of a state should be either all accepting
  ///          or all unaccepting.  If your automaton does not satisfies
  ///          these requirements, call degeneralize() first.
  /// \param opt a string of option: 'c' to comment each state
  SPOT_API std::ostream&
  print_never_claim(std::ostream& os,
                        const const_twa_ptr& g,
                        const char* opt = nullptr);
}
