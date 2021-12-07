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

#include <spot/tl/formula.hh>
#include <vector>

namespace spot
{
  enum mut_opts
    {
      Mut_Ap2Const = 1U<<0,
      Mut_Simplify_Bounds = 1U<<1,
      Mut_Remove_Multop_Operands = 1U<<2,
      Mut_Remove_Ops = 1U<<3,
      Mut_Split_Ops = 1U<<4,
      Mut_Rewrite_Ops = 1U<<5,
      Mut_Remove_One_Ap = 1U<<6,
      Mut_All = -1U
    };

  SPOT_API
  std::vector<formula> mutate(formula f,
                              unsigned opts = Mut_All,
                              unsigned max_output = -1U,
                              unsigned mutation_count = 1,
                              bool sort = true);
}
