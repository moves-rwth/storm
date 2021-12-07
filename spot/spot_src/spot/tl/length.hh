// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2015, 2016 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/tl/formula.hh>

namespace spot
{
  /// \ingroup tl_misc
  /// \brief Compute the length of a formula.
  ///
  /// The length of a formula is the number of atomic propositions,
  /// constants, and operators (logical and temporal) occurring in
  /// the formula.  n-ary operators count for n-1; for instance
  /// <code>a | b | c</code> has length 5, even if there is only as
  /// single <code>|</code> node internally.
  SPOT_API
  int length(formula f);

  /// \ingroup tl_misc
  /// \brief Compute the length of a formula, squashing Boolean formulae
  ///
  /// This is similar to spot::length(), except all Boolean
  /// formulae are assumed to have length one.
  SPOT_API
  int length_boolone(formula f);
}
