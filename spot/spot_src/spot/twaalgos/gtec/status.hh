// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2016 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#pragma once

#include <spot/twaalgos/gtec/sccstack.hh>
#include <spot/twa/twa.hh>
#include <iosfwd>

namespace spot
{
  /// \brief The status of the emptiness-check on success.
  ///
  /// This contains everything needed to construct a counter-example:
  /// the automata, the stack of SCCs traversed by the counter-example,
  /// and the heap of visited states with their indexes.
  class SPOT_API couvreur99_check_status
  {
  public:
    couvreur99_check_status(const const_twa_ptr& aut);

    ~couvreur99_check_status();

    const_twa_ptr aut;
    scc_stack root;

    state_map<int> h;

    const state* cycle_seed;

    /// Output statistics about this object.
    void print_stats(std::ostream& os) const;

    /// Return the number of states visited by the search
    int states() const;
  };
}
