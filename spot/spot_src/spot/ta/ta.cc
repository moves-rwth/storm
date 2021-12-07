// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2014, 2018 Laboratoire de Recherche et
// Developpement de l Epita (LRDE).
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
#include <spot/ta/ta.hh>

namespace spot
{


  scc_stack_ta::connected_component::connected_component(int i) noexcept
  {
    index = i;
    is_accepting = false;
    condition = {};
  }

  scc_stack_ta::connected_component&
  scc_stack_ta::top()
  {
    return s.front();
  }

  const scc_stack_ta::connected_component&
  scc_stack_ta::top() const
  {
    return s.front();
  }

  void
  scc_stack_ta::pop()
  {
    // assert(rem().empty());
    s.pop_front();
  }

  void
  scc_stack_ta::push(int index)
  {
    s.emplace_front(index);
  }

  std::list<const state*>&
  scc_stack_ta::rem()
  {
    return top().rem;
  }

  size_t
  scc_stack_ta::size() const
  {
    return s.size();
  }

  bool
  scc_stack_ta::empty() const
  {
    return s.empty();
  }

}
