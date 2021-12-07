// -*- coding: utf-8 -*-
// Copyright (C) 2015 Laboratoire de Recherche et Développement de
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

#include <vector>
#include <spot/tl/formula.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  class SPOT_API exclusive_ap final
  {
    std::vector<std::vector<formula>> groups;
  public:
#ifndef SWIG
    void add_group(std::vector<formula> ap);
#endif
    void add_group(const char* ap_csv);

    bool empty() const
    {
      return groups.empty();
    }

    formula constrain(formula f) const;
    twa_graph_ptr constrain(const_twa_graph_ptr aut,
                               bool simplify_guards = false) const;
  };
}
