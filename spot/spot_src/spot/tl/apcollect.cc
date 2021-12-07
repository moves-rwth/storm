// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014, 2015, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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
#include <spot/tl/apcollect.hh>
#include <spot/twa/twa.hh>
#include <spot/twa/bdddict.hh>

namespace spot
{
  atomic_prop_set create_atomic_prop_set(unsigned n)
  {
    atomic_prop_set res;
    for (unsigned i = 0; i < n; ++i)
      {
        std::ostringstream p;
        p << 'p' << i;
        res.insert(formula::ap(p.str()));
      }
    return res;
  }

  atomic_prop_set*
  atomic_prop_collect(formula f, atomic_prop_set* s)
  {
    if (!s)
      s = new atomic_prop_set;
    f.traverse([&](const formula& f)
               {
                 if (f.is(op::ap))
                   s->insert(f);
                 return false;
               });
    return s;
  }

  bdd
  atomic_prop_collect_as_bdd(formula f, const twa_ptr& a)
  {
    spot::atomic_prop_set aps;
    atomic_prop_collect(f, &aps);
    bdd res = bddtrue;
    for (auto f: aps)
      res &= bdd_ithvar(a->register_ap(f));
    return res;
  }
}
