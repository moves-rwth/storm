// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2015, 2018 Laboratoire de Recherche et
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
#include <spot/tl/simplify.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/remove_x.hh>

namespace spot
{
  namespace
  {
    static formula
    remove_x_rec(formula f, atomic_prop_set& aps)
    {
      if (f.is_syntactic_stutter_invariant())
        return f;

      auto rec = [&aps](formula f)
        {
          return remove_x_rec(f, aps);
        };

      if (!f.is(op::X))
        return f.map(rec);

      formula c = rec(f[0]);

      std::vector<formula> vo;
      for (auto i: aps)
        {
          // First line
          std::vector<formula> va1;
          formula npi = formula::Not(i);
          va1.emplace_back(i);
          va1.emplace_back(formula::U(i, formula::And({npi, c})));

          for (auto j: aps)
            if (j != i)
              {
                // make sure the arguments of OR are created in a
                // deterministic order
                auto tmp = formula::U(formula::Not(j), npi);
                va1.emplace_back(formula::Or({formula::U(j, npi), tmp}));
              }
          vo.emplace_back(formula::And(va1));
          // Second line
          std::vector<formula> va2;
          va2.emplace_back(npi);
          va2.emplace_back(formula::U(npi, formula::And({i, c})));
          for (auto j: aps)
            if (j != i)
              {
                // make sure the arguments of OR are created in a
                // deterministic order
                auto tmp = formula::U(formula::Not(j), i);
                va2.emplace_back(formula::Or({formula::U(j, i), tmp}));
              }
          vo.emplace_back(formula::And(va2));
        }
      // Third line
      std::vector<formula> va3;
      for (auto i: aps)
        {
          // make sure the arguments of OR are created in a
          // deterministic order
          auto tmp = formula::G(formula::Not(i));
          va3.emplace_back(formula::Or({formula::G(i), tmp}));
        }
      va3.emplace_back(c);
      vo.emplace_back(formula::And(va3));
      return formula::Or(vo);
    }
  }

  formula remove_x(formula f)
  {
    if (f.is_syntactic_stutter_invariant())
      return f;
    atomic_prop_set aps;
    atomic_prop_collect(f, &aps);
    return remove_x_rec(f, aps);
  }
}
