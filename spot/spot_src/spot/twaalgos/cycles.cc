// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014-2018 Laboratoire de Recherche et
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
#include <iostream>
#include <spot/twaalgos/cycles.hh>

namespace spot
{
  enumerate_cycles::enumerate_cycles(const scc_info& map)
    : aut_(map.get_aut()),
      info_(aut_->num_states(), aut_->num_states()),
      sm_(map)
  {
    if (!aut_->is_existential())
      throw std::runtime_error
        ("enumerate_cycles does not support alternation");
  }

  void
  enumerate_cycles::nocycle(unsigned x, unsigned y)
  {
    // insert x in B(y)
    info_[y].b.emplace_back(x);
    // remove y from A(x)
    info_[x].del[y] = true;
  }

  void
  enumerate_cycles::unmark(unsigned y)
  {
    std::vector<unsigned> q;
    q.emplace_back(y);

    while (!q.empty())
      {
        y = q.back();
        q.pop_back();

        info_[y].mark = false;
        for (auto x: info_[y].b)
          {
            assert(info_[x].seen);
            // insert y in A(x)
            info_[x].del[y] = false;
            // unmark x recursively if marked
            if (info_[x].mark)
              q.emplace_back(x);
          }
        // empty B(y)
        info_[y].b.clear();
      }
  }

  void
  enumerate_cycles::push_state(unsigned s)
  {
    info_[s].mark = true;
    dfs_.emplace_back(s);
  }

  void
  enumerate_cycles::run(unsigned scc)
  {
    bool keep_going = true;

    {
      unsigned s = sm_.one_state_of(scc);
      info_[s].seen = true;
      push_state(s);
    }

    while (keep_going && !dfs_.empty())
      {
        dfs_entry& cur = dfs_.back();
        if (cur.succ == 0)
          cur.succ = aut_->get_graph().state_storage(cur.s).succ;
        else
          cur.succ = aut_->edge_storage(cur.succ).next_succ;
        if (cur.succ)
          {
            // Explore one successor.

            // Ignore those that are not on the SCC, or destination
            // that have been "virtually" deleted from A(v).
            unsigned s = aut_->edge_storage(cur.succ).dst;

            if ((sm_.scc_of(s) != scc) || (info_[cur.s].del[s]))
              continue;

            info_[s].seen = true;
            if (!info_[s].mark)
              {
                push_state(s);
              }
            else if (!info_[s].reach)
              {
                keep_going = cycle_found(s);
                cur.f = true;
              }
            else
              {
                nocycle(cur.s, s);
              }
          }
        else
          {
            // No more successors.
            bool f = cur.f;
            unsigned v = cur.s;

            dfs_.pop_back();
            if (f)
              unmark(v);
            info_[v].reach = true;

            // Update the predecessor in the stack if there is one.
            if (!dfs_.empty())
              {
                if (f)
                  dfs_.back().f = true;
                else
                  nocycle(dfs_.back().s, v);
              }
          }
      }

    // Purge the dfs_ stack, in case we aborted because cycle_found()
    // returned false.
    dfs_.clear();
  }

  bool
  enumerate_cycles::cycle_found(unsigned start)
  {
    dfs_stack::const_iterator i = dfs_.begin();
    while (i->s != start)
      ++i;
    do
      {
        std::cout << i->s << ' ';
        ++i;
      }
    while (i != dfs_.end());
    std::cout << '\n';
    return true;
  }


}
