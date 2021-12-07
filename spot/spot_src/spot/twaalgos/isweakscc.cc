// -*- coding: utf-8 -*-
// Copyright (C) 2012-2019 Laboratoire de Recherche et Développement
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

#include "config.h"
#include <spot/tl/formula.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/genem.hh>

namespace spot
{
  namespace
  {
    [[noreturn]] static void
    invalid_scc_number(const char* fn)
    {
      throw std::invalid_argument(std::string(fn) + "(): invalid SCC number");
    }
  }

  bool
  scc_has_rejecting_cycle(scc_info& map, unsigned scc)
  {
    if (SPOT_UNLIKELY(scc >= map.scc_count()))
      invalid_scc_number("scc_has_rejecting_cycle");
    acc_cond neg_acc = map.get_aut()->get_acceptance().complement();
    return !generic_emptiness_check_for_scc(map, scc, neg_acc);
  }

  bool
  is_inherently_weak_scc(scc_info& map, unsigned scc)
  {
    if (SPOT_UNLIKELY(scc >= map.scc_count()))
      invalid_scc_number("is_inherently_weak_scc");
     // Weak SCCs are inherently weak.
    if (is_weak_scc(map, scc))
      return true;
    // If we reach this place, we now the SCC has an accepting cycle.
    // The question is now to find whether is also contains a
    // rejecting cycle.
    return !scc_has_rejecting_cycle(map, scc);
  }

  bool
  is_weak_scc(scc_info& map, unsigned scc)
  {
    if (SPOT_UNLIKELY(scc >= map.scc_count()))
      invalid_scc_number("is_weak_scc");
    // Rejecting SCCs are weak.
    if (map.is_rejecting_scc(scc))
      return true;
    // If all transitions use the same acceptance set, the SCC is weak.
    return map.marks_of(scc).size() == 1;
  }

  bool
  is_complete_scc(scc_info& map, unsigned scc)
  {
    if (SPOT_UNLIKELY(scc >= map.scc_count()))
      invalid_scc_number("is_complete_scc");
    auto a = map.get_aut();
    for (auto s: map.states_of(scc))
      {
        bool has_succ = false;
        bdd sumall = bddfalse;
        for (auto& t: a->out(s))
          {
            has_succ = true;
            bool in = true;
            for (auto d: a->univ_dests(t.dst))
              if (map.scc_of(d) != scc)
                {
                  in = false;
                  break;
                }
            if (!in)
              continue;
            sumall |= t.cond;
            if (sumall == bddtrue)
              break;
          }
        if (!has_succ || sumall != bddtrue)
          return false;
      }
    return true;
  }

  bool
  is_terminal_scc(scc_info& map, unsigned scc)
  {
    if (SPOT_UNLIKELY(scc >= map.scc_count()))
      invalid_scc_number("is_terminal_scc");

    // If all transitions use all acceptance conditions, the SCC is weak.
    return (map.is_accepting_scc(scc)
            && map.marks_of(scc).size() == 1
            && is_complete_scc(map, scc));
  }
}
