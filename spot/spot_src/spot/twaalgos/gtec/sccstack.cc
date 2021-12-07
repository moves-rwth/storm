// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2018 Laboratoire de Recherche et Developpement de
// l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <spot/twaalgos/gtec/sccstack.hh>

namespace spot
{
  scc_stack::connected_component::connected_component(int i)
  {
    index = i;
    condition = acc_cond::mark_t({});
  }

  scc_stack::connected_component&
  scc_stack::top()
  {
    return s.front();
  }

  const scc_stack::connected_component&
  scc_stack::top() const
  {
    return s.front();
  }

  void
  scc_stack::pop()
  {
    // assert(rem().empty());
    s.pop_front();
  }

  void
  scc_stack::push(int index)
  {
    s.emplace_front(index);
  }

  std::list<const state*>&
  scc_stack::rem()
  {
    return top().rem;
  }


  size_t
  scc_stack::size() const
  {
    return s.size();
  }

  bool
  scc_stack::empty() const
  {
    return s.empty();
  }

  unsigned
  scc_stack::clear_rem()
  {
    unsigned n = 0;
    for (stack_type::iterator i = s.begin(); i != s.end(); ++i)
      {
        n += i->rem.size();
        i->rem.clear();
      }
    return n;
  }


}
