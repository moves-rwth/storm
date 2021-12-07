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
  class SPOT_API remove_ap
  {
    std::set<formula> props_exist;
    std::set<formula> props_pos;
    std::set<formula> props_neg;
  public:
    void add_ap(const char* ap_csv);

    bool empty() const
    {
      return props_exist.empty() && props_pos.empty() && props_neg.empty();
    }

    twa_graph_ptr strip(const_twa_graph_ptr aut) const;
  };
}
