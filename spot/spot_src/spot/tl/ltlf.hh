// -*- coding: utf-8 -*-
// Copyright (C) 2016 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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
  /// \ingroup tl_rewriting
  /// \brief Convert an LTLf into an LTL formula.
  ///
  /// This is based on De Giacomo & Vardi (IJCAI'13) reduction from
  /// LTLf (finite-LTL) to LTL.
  ///
  /// In this reduction, finite words are extended into infinite words
  /// in which a new atomic proposition <code>alive</code> marks the
  /// prefix of the infinite word that corresponds to the original
  /// finite word.  The formula is rewritten to ensure that the
  /// eventualities occur during the "alive" portion.  For instance
  /// <code>a U b</code> becomes
  /// <code>alive&(a U (b & alive))&(alive U G!alive)</code>.
  ///
  /// The \a alive argument can be used to change the name of the
  /// atomic property used to introduce.  Additionally if \a alive is
  /// a string starting with and exclamation mark, e.g.,
  /// <code>!dead</code> then the atomic property will be built from
  /// the rest of the string, and its negation will be used in the
  /// transformation.  Using <code>!dead</code> rather than
  /// <code>alive</code> makes more sense if the state-space
  /// introduces a <code>dead</code> property on states representing
  /// the end of finite computations.
  SPOT_API formula from_ltlf(formula f, const char* alive = "alive");
}
