// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2014, 2016, 2017, 2018 Laboratoire de Recherche
// et Developpement de l'Epita.
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
#include <cassert>
#include <ostream>
#include <spot/priv/weight.hh>

namespace spot
{
  weight::weight(const acc_cond& acc):
    m(acc.num_sets())
  {
  }

  weight& weight::add(acc_cond::mark_t a)
  {
    for (auto s: a.sets())
      ++m[s];
    return *this;
  }

  weight& weight::sub(acc_cond::mark_t a)
  {
    for (auto s: a.sets())
      if (m[s] > 0)
        --m[s];
    return *this;
  }

  acc_cond::mark_t weight::diff(const acc_cond& acc, const weight& w) const
  {
    unsigned max = acc.num_sets();
    std::vector<unsigned> res;
    for (unsigned n = 0; n < max; ++n)
      if (m[n] > w.m[n])
        res.emplace_back(n);
    return acc_cond::mark_t(res.begin(), res.end());
  }

  std::ostream& operator<<(std::ostream& os, const weight& w)
  {
    unsigned s = w.m.size();
    for (unsigned n = 0; n < s; ++n)
      os << '(' << n << ',' << w.m[n] << ')';
    return os;
  }
}
