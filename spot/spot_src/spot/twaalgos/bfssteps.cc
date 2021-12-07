// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015, 2018, 2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE)
// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "config.h"
#include <deque>
#include <utility>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/bfssteps.hh>

namespace spot
{

  bfs_steps::bfs_steps(const const_twa_ptr& a)
    : a_(a)
  {
  }

  bfs_steps::~bfs_steps()
  {
  }

  void
  bfs_steps::finalize(const std::map<const state*, twa_run::step,
                state_ptr_less_than>& father, const twa_run::step& s,
                const state* start, twa_run::steps& l)
  {
    twa_run::steps p;
    twa_run::step current = s;
    for (;;)
      {
        twa_run::step tmp = current;
        tmp.s = tmp.s->clone();
        p.push_front(tmp);
        if (current.s == start)
          break;
        std::map<const state*, twa_run::step,
            state_ptr_less_than>::const_iterator it = father.find(current.s);
        assert(it != father.end());
        current = it->second;
      }
    l.splice(l.end(), p);
  }

  const state*
  bfs_steps::search(const state* start, twa_run::steps& l)
  {
    // Records backlinks to parent state during the BFS.
    // (This also stores the propositions of this link.)
    std::map<const state*, twa_run::step,
      state_ptr_less_than> father;
    // BFS queue.
    std::deque<const state*> todo;
    // Initial state.
    todo.emplace_back(start);

    while (!todo.empty())
      {
        const state* src = todo.front();
        todo.pop_front();
        for (auto i: a_->succ(src))
          {
            // skip false transitions
            if (SPOT_UNLIKELY(i->cond() == bddfalse))
              continue;

            const state* dest = filter(i->dst());

            if (!dest)
              continue;

            bdd cond = i->cond();
            acc_cond::mark_t acc = i->acc();
            twa_run::step s = { src, cond, acc };

            if (match(s, dest))
              {
                // Found it!
                finalize(father, s, start, l);
                return dest;
              }

            // Common case: record backlinks and continue BFS
            // for unvisited states.
            if (father.find(dest) == father.end())
              {
                todo.emplace_back(dest);
                father[dest] = s;
              }
          }
      }
    return nullptr;
  }

}
