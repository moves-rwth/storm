// -*- coding: utf-8 -*-
// Copyright (C) 2016, 2018, 2019 Laboratoire de Recherche et
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
#include <spot/tl/ltlf.hh>

namespace spot
{
  namespace
  {
    formula from_ltlf_aux(formula f, formula alive)
    {
      auto t = [&alive] (formula f) { return from_ltlf_aux(f, alive); };
      switch (auto o = f.kind())
        {
        case op::strong_X:
          o = op::X;
          SPOT_FALLTHROUGH;
        case op::F:
          return formula::unop(o, formula::And({alive, t(f[0])}));
        case op::X:             // weak
        case op::G:
          return formula::unop(o, formula::Or({formula::Not(alive), t(f[0])}));
          // Note that the t() function given in the proof of Theorem 1 of
          // the IJCAI'13 paper by De Giacomo & Vardi has a typo.
          //  t(a U b) should be equal to t(a) U t(b & alive).
          // This typo is fixed in the Memocode'14 paper by Dutta & Vardi.
          //
          // (However beware that the translation given in the
          // Memocode'14 paper forgets to ensure that alive holds
          // initially, as required in the IJCAI'13 paper.)
        case op::U:
          return formula::U(t(f[0]), formula::And({alive, t(f[1])}));
        case op::R:
          return formula::R(t(f[0]),
                            formula::Or({formula::Not(alive), t(f[1])}));
        case op::M:
          return formula::M(formula::And({alive, t(f[0])}), t(f[1]));
        case op::W:
          return formula::W(formula::Or({formula::Not(alive), t(f[0])}),
                            t(f[1]));
        default:
          return f.map(t);
        }
    }
  }

  formula from_ltlf(formula f, const char* alive)
  {
    if (!f.is_ltl_formula())
      throw std::runtime_error("from_ltlf() only supports LTL formulas");
    auto al = ((*alive == '!')
               ? formula::Not(formula::ap(alive + 1))
               : formula::ap(alive));
    return formula::And({from_ltlf_aux(f, al), al,
                         formula::U(al, formula::G(formula::Not(al)))});
  }
}
