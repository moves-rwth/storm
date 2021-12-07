// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2014, 2016, 2018 Laboratoire de Recherche et
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
#include <spot/ta/ta.hh>
#include <spot/taalgos/statessetbuilder.hh>
#include <spot/taalgos/reachiter.hh>

namespace spot
{
  namespace
  {
    class states_set_builder_bfs final:
      public ta_reachable_iterator_breadth_first
    {
    public:
      states_set_builder_bfs(const const_ta_ptr& a) :
        ta_reachable_iterator_breadth_first(a)
      {
      }

      void
      process_state(const state* s, int) override
      {
        states_set_.insert(s);
      }

      void
      process_link(int, int, const ta_succ_iterator*) override
      {
      }

      std::set<const state*>
      get_states_set()
      {
          return states_set_;
      }

    private:
      std::set<const state*>  states_set_;
    };
  } // anonymous



  std::set<const state*>
  get_states_set(const const_ta_ptr& t)
  {
    states_set_builder_bfs d(t);
    d.run();
    return d.get_states_set();
  }
}
