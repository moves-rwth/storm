// -*- coding: utf-8 -*-
// Copyright (C) 2015-2019 Laboratoire de Recherche et
// Développement de l'Epita.
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

#include "config.h"
#include <spot/twaalgos/sepsets.hh>
#include <spot/twaalgos/sccinfo.hh>


namespace spot
{
  namespace
  {
    static acc_cond::mark_t common_sets(const const_twa_graph_ptr& aut)
    {
      auto p = aut->get_acceptance().used_inf_fin_sets();
      return p.first & p.second;
    }
  }

  bool has_separate_sets(const const_twa_graph_ptr& aut)
  {
    return !common_sets(aut);
  }

  twa_graph_ptr
  separate_sets_here(const twa_graph_ptr& aut)
  {
    auto common = common_sets(aut);
    if (!common)
      return aut;
    // Each Fin(first) should be replaced by Fin(second).
    std::vector<std::pair<acc_cond::mark_t, acc_cond::mark_t>> map;
    {
      unsigned base = aut->acc().add_sets(common.count());
      for (auto s: common.sets())
        map.emplace_back(acc_cond::mark_t({s}),
                         acc_cond::mark_t({base++}));
    }

    // Fix the acceptance condition
    auto& code = aut->acc().get_acceptance();
    // If code were empty, then common would have been 0.
    assert(!code.empty());
    acc_cond::acc_word* pos = &code.back();
    acc_cond::acc_word* start = &code.front();
    while (pos > start)
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::Or:
          case acc_cond::acc_op::And:
            --pos;
            break;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::FinNeg:
            if (pos[-1].mark & common)
              for (auto p: map)
                if (pos[-1].mark & p.first)
                  {
                    pos[-1].mark -= p.first;
                    pos[-1].mark |= p.second;
                  }
            SPOT_FALLTHROUGH;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            pos -= 2;
            break;
          }
      }

    // Fix the edges
    for (auto& t: aut->edges())
      {
        if (!(t.acc & common))
          continue;
        for (auto p: map)
          if (t.acc & p.first)
            t.acc |= p.second;
      }
    return aut;
  }



}
