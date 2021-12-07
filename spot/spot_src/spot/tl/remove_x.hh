// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2015, 2019 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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
  /// \brief Rewrite a stutter-insensitive formula \a f without
  /// using the X operator.
  ///
  /// This function may also be applied to stutter-sensitive formulas,
  /// but in that case the resulting formula is not equivalent.
  ///
  /// \see \cite etessami.00.ipl
  SPOT_API
  formula remove_x(formula f);
}
