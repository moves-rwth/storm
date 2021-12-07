// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2012, 2014-2016, 2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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
#include <cassert>
#include <spot/taalgos/reachiter.hh>

#include <iostream>
using namespace std;

namespace spot
{
  // ta_reachable_iterator
  //////////////////////////////////////////////////////////////////////

  ta_reachable_iterator::ta_reachable_iterator(const const_ta_ptr& a) :
    t_automata_(a)
  {
  }
  ta_reachable_iterator::~ta_reachable_iterator()
  {
    auto s = seen.begin();
    while (s != seen.end())
      {
        // Advance the iterator before deleting the "key" pointer.
        const state* ptr = s->first;
        ++s;
        t_automata_->free_state(ptr);
      }
  }

  void
  ta_reachable_iterator::run()
  {
    int n = 0;
    start();

    const spot::state* artificial_initial_state =
      t_automata_->get_artificial_initial_state();

    ta::const_states_set_t init_states_set;

    if (artificial_initial_state)
      {
        init_states_set.insert(artificial_initial_state);
      }
    else
      {
        init_states_set = t_automata_->get_initial_states_set();
      }

    for (auto init_state: init_states_set)
      {
        if (want_state(init_state))
          add_state(init_state);
        seen[init_state] = ++n;
      }

    const state* t;
    while ((t = next_state()))
      {
        assert(seen.find(t) != seen.end());
        int tn = seen[t];
        ta_succ_iterator* si = t_automata_->succ_iter(t);
        process_state(t, tn);
        for (si->first(); !si->done(); si->next())
          {
            const state* current = si->dst();
            auto s = seen.find(current);
            bool ws = want_state(current);
            if (s == seen.end())
              {
                seen[current] = ++n;
                if (ws)
                  {
                    add_state(current);
                    process_link(tn, n, si);
                  }
              }
            else
              {
                if (ws)
                  process_link(tn, s->second, si);
                t_automata_->free_state(current);
              }
          }
        delete si;
      }
    end();
  }

  bool
  ta_reachable_iterator::want_state(const state*) const
  {
    return true;
  }

  void
  ta_reachable_iterator::start()
  {
  }

  void
  ta_reachable_iterator::end()
  {
  }

  void
  ta_reachable_iterator::process_state(const state*, int)
  {
  }

  void
  ta_reachable_iterator::process_link(int, int, const ta_succ_iterator*)
  {
  }

  // ta_reachable_iterator_depth_first
  //////////////////////////////////////////////////////////////////////

  ta_reachable_iterator_depth_first::ta_reachable_iterator_depth_first(
      const const_ta_ptr& a) :
    ta_reachable_iterator(a)
  {
  }

  void
  ta_reachable_iterator_depth_first::add_state(const state* s)
  {
    todo.push(s);
  }

  const state*
  ta_reachable_iterator_depth_first::next_state()
  {
    if (todo.empty())
      return nullptr;
    const state* s = todo.top();
    todo.pop();
    return s;
  }

  // ta_reachable_iterator_breadth_first
  //////////////////////////////////////////////////////////////////////

  ta_reachable_iterator_breadth_first::ta_reachable_iterator_breadth_first(
      const const_ta_ptr& a) :
    ta_reachable_iterator(a)
  {
  }

  void
  ta_reachable_iterator_breadth_first::add_state(const state* s)
  {
    todo.emplace_back(s);
  }

  const state*
  ta_reachable_iterator_breadth_first::next_state()
  {
    if (todo.empty())
      return nullptr;
    const state* s = todo.front();
    todo.pop_front();
    return s;
  }

}
