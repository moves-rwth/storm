// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014  Laboratoire de Recherche et Développement de
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

#pragma once

#include <bddx.h>
#include <list>
#include <spot/twa/twa.hh>

namespace spot
{
  // A stack of Strongly-Connected Components, as needed by the
  // Tarjan-Couvreur algorithm.
  class SPOT_API scc_stack
  {
  public:
    struct connected_component
    {
    public:
      connected_component(int index = -1);

      /// Index of the SCC.
      int index;
      /// The union of all acceptance marks of transitions the
      /// connected component.
      acc_cond::mark_t condition;

      std::list<const state*> rem;
    };

    /// Stack a new SCC with index \a index.
    void push(int index);

    /// Access the top SCC.
    connected_component& top();

    /// Access the top SCC.
    const connected_component& top() const;

    /// Pop the top SCC.
    void pop();

    /// How many SCC are in stack.
    size_t size() const;

    /// The \c rem member of the top SCC.
    std::list<const state*>& rem();

    /// Purge all \c rem members.
    ///
    /// \return the number of elements cleared.
    unsigned clear_rem();

    /// Is the stack empty?
    bool empty() const;

    typedef std::list<connected_component> stack_type;
    stack_type s;
  };
}
