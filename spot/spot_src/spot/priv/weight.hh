// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
// Copyright (C) 2004, 2014  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <iosfwd>
#include <map>
#include <bddx.h>
#include <spot/twa/acc.hh>

namespace spot
{
  /// \brief Manage for a given automaton a vector of counter indexed by
  /// its acceptance condition

  class weight
  {
  public:
    /// Construct a empty vector (all counters set to zero).
    weight(const acc_cond& acc);
    /// Increment by one the counters of each acceptance condition in \a a.
    weight& add(acc_cond::mark_t a);
    /// Decrement by one the counters of each acceptance condition in \a a.
    weight& sub(acc_cond::mark_t a);
    /// Return the set of each acceptance condition such that its counter is
    /// strictly greatest than the corresponding counter in w.
    ///
    /// \pre For each acceptance condition, its counter is greatest or equal to
    /// the corresponding counter in w.
    acc_cond::mark_t diff(const acc_cond& acc, const weight& w) const;
    friend std::ostream& operator<<(std::ostream& os,
                                    const weight& w);

  private:
    std::vector<int> m;
  };
}
