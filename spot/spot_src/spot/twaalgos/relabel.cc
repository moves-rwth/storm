// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018, 2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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
#include <spot/twaalgos/relabel.hh>
#include <spot/twa/formula2bdd.hh>

namespace spot
{
  void
  relabel_here(twa_graph_ptr& aut, relabeling_map* relmap)
  {
    bddPair* pairs = bdd_newpair();
    auto d = aut->get_dict();
    std::vector<int> vars;
    std::set<int> newvars;
    vars.reserve(relmap->size());
    bool bool_subst = false;

    for (auto& p: *relmap)
      {
        if (!p.first.is(op::ap))
          throw std::runtime_error
            ("relabel_here: old labels should be atomic propositions");
        if (!p.second.is_boolean())
          throw std::runtime_error
            ("relabel_here: new labels should be Boolean formulas");

        int oldv = aut->register_ap(p.first);
        vars.emplace_back(oldv);
        if (p.second.is(op::ap))
          {
            int newv = aut->register_ap(p.second);
            newvars.insert(newv);
            bdd_setpair(pairs, oldv, newv);
          }
        else
          {
            p.second.traverse([&](const formula& f)
                              {
                                if (f.is(op::ap))
                                  newvars.insert(aut->register_ap(f));
                                return false;
                              });
            bdd newb = formula_to_bdd(p.second, d, aut);
            bdd_setbddpair(pairs, oldv, newb);
            bool_subst = true;
          }
      }

    bool need_cleanup = false;
    typedef bdd (*op_t)(const bdd&, bddPair*);
    op_t op = bool_subst ?
      static_cast<op_t>(bdd_veccompose) : static_cast<op_t>(bdd_replace);
    for (auto& t: aut->edges())
      {
        bdd c = (*op)(t.cond, pairs);
        t.cond = c;
        if (c == bddfalse)
          need_cleanup = true;
      }

    // Erase all the old variables that are not reused in the new set.
    // (E.g., if we relabel a&p0 into p0&p1 we should not unregister
    // p0)
    for (auto v: vars)
      if (newvars.find(v) == newvars.end())
        aut->unregister_ap(v);

    // If some of the edges were relabeled false, we need to clean the
    // automaton.
    if (need_cleanup)
      {
        aut->merge_edges();     // remove any edge labeled by 0
        aut->purge_dead_states(); // remove useless states
      }
  }
}
