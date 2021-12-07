// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011, 2013-2016, 2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <cassert>
#include <spot/twaalgos/reachiter.hh>

namespace spot
{
  // twa_reachable_iterator
  //////////////////////////////////////////////////////////////////////

  twa_reachable_iterator::twa_reachable_iterator(const const_twa_ptr& a)
    : aut_(a)
  {
  }

  twa_reachable_iterator::~twa_reachable_iterator()
  {
    auto s = seen.begin();
    while (s != seen.end())
      {
        // Advance the iterator before deleting the "key" pointer.
        const state* ptr = s->first;
        ++s;
        ptr->destroy();
      }
  }

  void
  twa_reachable_iterator::run()
  {
    int n = 0;
    start();
    const state* i = aut_->get_init_state();
    if (want_state(i))
      add_state(i);
    seen[i] = ++n;
    const state* t;
    while ((t = next_state()))
      {
        assert(seen.find(t) != seen.end());
        int tn = seen[t];
        twa_succ_iterator* si = aut_->succ_iter(t);
        process_state(t, tn, si);
        if (si->first())
          do
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
                      process_link(t, tn, current, n, si);
                    }
                }
              else
                {
                  if (ws)
                    process_link(t, tn, s->first, s->second, si);
                  current->destroy();
                }
            }
          while (si->next());
        aut_->release_iter(si);
      }
    end();
  }

  bool
  twa_reachable_iterator::want_state(const state*) const
  {
    return true;
  }

  void
  twa_reachable_iterator::start()
  {
  }

  void
  twa_reachable_iterator::end()
  {
  }

  void
  twa_reachable_iterator::process_state(const state*, int,
                                         twa_succ_iterator*)
  {
  }

  void
  twa_reachable_iterator::process_link(const state*, int,
                                        const state*, int,
                                        const twa_succ_iterator*)
  {
  }

  // twa_reachable_iterator_breadth_first
  //////////////////////////////////////////////////////////////////////

  twa_reachable_iterator_breadth_first::
    twa_reachable_iterator_breadth_first(const const_twa_ptr& a)
      : twa_reachable_iterator(a)
  {
  }

  void
  twa_reachable_iterator_breadth_first::add_state(const state* s)
  {
    todo.emplace_back(s);
  }

  const state*
  twa_reachable_iterator_breadth_first::next_state()
  {
    if (todo.empty())
      return nullptr;
    const state* s = todo.front();
    todo.pop_front();
    return s;
  }

  // twa_reachable_iterator_depth_first
  //////////////////////////////////////////////////////////////////////

  twa_reachable_iterator_depth_first::
    twa_reachable_iterator_depth_first(const const_twa_ptr& a)
      : aut_(a)
  {
  }

  twa_reachable_iterator_depth_first::~twa_reachable_iterator_depth_first()
  {
    auto s = seen.begin();
    while (s != seen.end())
      {
        // Advance the iterator before deleting the "key" pointer.
        const state* ptr = s->first;
        ++s;
        ptr->destroy();
      }
  }

  void
  twa_reachable_iterator_depth_first::push(const state* s, int sn)
  {
    twa_succ_iterator* si = aut_->succ_iter(s);
    process_state(s, sn, si);
    stack_item item = { s, sn, si };
    todo.emplace_back(item);
    si->first();
  }

  void
  twa_reachable_iterator_depth_first::pop()
  {
    aut_->release_iter(todo.back().it);
    todo.pop_back();
    if (!todo.empty())
      todo.back().it->next();
  }

  void
  twa_reachable_iterator_depth_first::run()
  {
    int n = 1;
    start();
    const state* i = aut_->get_init_state();
    if (want_state(i))
      push(i, n);
    seen[i] = n++;
    const state* dst;
    while (!todo.empty())
      {
        twa_succ_iterator* si = todo.back().it;
        if (si->done())
          {
            pop();
            continue;
          }

        dst = si->dst();
        auto res = seen.emplace(dst, n);
        if (!res.second)
          {
            // The state has already been seen.
            dst->destroy();
            // 0-numbered states are not wanted.
            if (res.first->second == 0)
              {
                si->next();
                continue;
              }
            dst = res.first->first;
          }
        else if (!want_state(dst))
          {
            // Mark this state as non-wanted in case we see it again.
            res.first->second = 0;
            si->next();
            continue;
          }
        else
          {
            ++n;
          }

        int dst_n = res.first->second;
        process_link(todo.back().src, todo.back().src_n, dst, dst_n, si);

        if (res.second)
          push(dst, dst_n);
        else
          si->next();
      }
    end();
  }

  bool
  twa_reachable_iterator_depth_first::want_state(const state*) const
  {
    return true;
  }

  void
  twa_reachable_iterator_depth_first::start()
  {
  }

  void
  twa_reachable_iterator_depth_first::end()
  {
  }

  void
  twa_reachable_iterator_depth_first::process_state(const state*, int,
                                                     twa_succ_iterator*)
  {
  }

  void
  twa_reachable_iterator_depth_first::process_link(const state*, int,
                                                    const state*, int,
                                                    const twa_succ_iterator*)
  {
  }

  // twa_reachable_iterator_depth_first_stack
  //////////////////////////////////////////////////////////////////////


  twa_reachable_iterator_depth_first_stack::
  twa_reachable_iterator_depth_first_stack(const const_twa_ptr& a)
    : twa_reachable_iterator_depth_first(a)
  {
  }

  void
  twa_reachable_iterator_depth_first_stack::push(const state* s, int sn)
  {
    stack_.insert(sn);
    this->twa_reachable_iterator_depth_first::push(s, sn);
  }

  void
  twa_reachable_iterator_depth_first_stack::pop()
  {
    stack_.erase(todo.back().src_n);
    this->twa_reachable_iterator_depth_first::pop();
  }

  bool
  twa_reachable_iterator_depth_first_stack::on_stack(int sn) const
  {
    return stack_.find(sn) != stack_.end();
  }


}
