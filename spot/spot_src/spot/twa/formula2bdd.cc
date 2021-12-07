// -*- coding: utf-8 -*-
// Copyright (C) 2009-2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <spot/twa/formula2bdd.hh>
#include <spot/misc/minato.hh>

namespace spot
{
  namespace
  {
    // Convert a BDD which is known to be a conjonction into a formula.
    static formula
    conj_to_formula(bdd b, const bdd_dict_ptr d)
    {
      if (b == bddfalse)
        return formula::ff();
      std::vector<formula> v;
      while (b != bddtrue)
        {
          int var = bdd_var(b);
          const bdd_dict::bdd_info& i = d->bdd_map[var];
          if (SPOT_UNLIKELY(i.type != bdd_dict::var))
            throw std::runtime_error("bdd_to_formula() was passed a bdd"
                                     " with a variable that is not in "
                                     "the dictionary");
          formula res = i.f;

          bdd high = bdd_high(b);
          if (high == bddfalse)
            {
              res = formula::Not(res);
              b = bdd_low(b);
            }
          else
            {
              // If bdd_low is not false, then b was not a conjunction.
              assert(bdd_low(b) == bddfalse);
              b = high;
            }
          assert(b != bddfalse);
          v.emplace_back(res);
        }
      return formula::And(v);
    }
  } // anonymous

  bdd
  formula_to_bdd(formula f, const bdd_dict_ptr& d, void* owner)
  {
    auto recurse = [&d, owner](formula f)
      {
        return formula_to_bdd(f, d, owner);
      };
    switch (f.kind())
      {
      case op::ff:
        return bddfalse;
      case op::tt:
        return bddtrue;
      case op::eword:
      case op::Star:
      case op::FStar:
      case op::F:
      case op::G:
      case op::X:
      case op::strong_X:
      case op::Closure:
      case op::NegClosure:
      case op::NegClosureMarked:
      case op::U:
      case op::R:
      case op::W:
      case op::M:
      case op::UConcat:
      case op::EConcat:
      case op::EConcatMarked:
      case op::Concat:
      case op::Fusion:
      case op::AndNLM:
      case op::OrRat:
      case op::AndRat:
      case op::first_match:
        SPOT_UNIMPLEMENTED();
      case op::ap:
        return bdd_ithvar(d->register_proposition(f, owner));
      case op::Not:
        return bdd_not(recurse(f[0]));
      case op::Xor:
        return bdd_apply(recurse(f[0]), recurse(f[1]), bddop_xor);
      case op::Implies:
        return bdd_apply(recurse(f[0]), recurse(f[1]), bddop_imp);
      case op::Equiv:
        return bdd_apply(recurse(f[0]), recurse(f[1]), bddop_biimp);
      case op::And:
      case op::Or:
        {
          int o = bddop_and;
          bdd res = bddtrue;
          if (f.is(op::Or))
            {
              o = bddop_or;
              res = bddfalse;
            }
          unsigned s = f.size();
          for (unsigned n = 0; n < s; ++n)
            res = bdd_apply(res, recurse(f[n]), o);
          return res;
        }
      }
    SPOT_UNREACHABLE();
    return bddfalse;
  }

  formula
  bdd_to_formula(bdd f, const bdd_dict_ptr d)
  {
    if (f == bddfalse)
      return formula::ff();

    std::vector<formula> v;

    minato_isop isop(f);
    bdd cube;
    while ((cube = isop.next()) != bddfalse)
      v.emplace_back(conj_to_formula(cube, d));
    return formula::Or(std::move(v));
  }
}
