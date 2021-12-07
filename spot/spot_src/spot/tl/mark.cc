// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2012, 2014-2015, 2018-2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
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
#include <spot/tl/mark.hh>
#include <cassert>
#include <algorithm>
#include <set>
#include <vector>

namespace spot
{
  formula
  mark_tools::mark_concat_ops(formula f)
  {
    f2f_map::iterator i = markops_.find(f);
    if (i != markops_.end())
      return i->second;

    formula res;
    switch (f.kind())
      {
      case op::ff:
      case op::tt:
      case op::eword:
      case op::ap:
      case op::Not:
      case op::X:
      case op::strong_X:
      case op::F:
      case op::G:
      case op::Closure:
      case op::NegClosureMarked:
      case op::OrRat:
      case op::AndRat:
      case op::AndNLM:
      case op::Star:
      case op::FStar:
      case op::U:
      case op::R:
      case op::W:
      case op::M:
      case op::EConcatMarked:
      case op::UConcat:
      case op::Concat:
      case op::Fusion:
      case op::first_match:
        res = f;
        break;
      case op::NegClosure:
        res = formula::NegClosureMarked(f[0]);
        break;
      case op::EConcat:
        res = formula::EConcatMarked(f[0], f[1]);
        break;
      case op::Or:
      case op::And:
        res = f.map([this](formula f)
                    {
                      return this->mark_concat_ops(f);
                    });
        break;
      case op::Xor:
      case op::Implies:
      case op::Equiv:
        SPOT_UNIMPLEMENTED();
      }

    markops_[f] = res;
    return res;
  }

  formula
  mark_tools::simplify_mark(formula f)
  {
    if (!f.is_marked())
      return f;

    f2f_map::iterator i = simpmark_.find(f);
    if (i != simpmark_.end())
      return i->second;

    auto recurse = [this](formula f)
      {
        return this->simplify_mark(f);
      };

    formula res;
    switch (f.kind())
      {
      case op::ff:
      case op::tt:
      case op::eword:
      case op::ap:
      case op::Not:
      case op::X:
      case op::strong_X:
      case op::F:
      case op::G:
      case op::Closure:
      case op::NegClosure:
      case op::NegClosureMarked:
      case op::U:
      case op::R:
      case op::W:
      case op::M:
      case op::EConcat:
      case op::EConcatMarked:
      case op::UConcat:
      case op::first_match:
        res = f;
        break;
      case op::Or:
        res = f.map(recurse);
        break;
      case op::And:
        {
          std::set<std::pair<formula, formula>> empairs;
          std::set<formula> nmset;
          std::vector<formula> elist;
          std::vector<formula> nlist;
          std::vector<formula> v;

          for (auto c: f)
            {
              if (c.is(op::EConcatMarked))
                {
                  empairs.emplace(c[0], c[1]);
                  v.emplace_back(c.map(recurse));
                }
              else if (c.is(op::EConcat))
                {
                  elist.emplace_back(c);
                }
              else if (c.is(op::NegClosureMarked))
                {
                  nmset.insert(c[0]);
                  v.emplace_back(c.map(recurse));
                }
              else if (c.is(op::NegClosure))
                {
                  nlist.emplace_back(c);
                }
              else
                {
                  v.emplace_back(c);
                }
            }
          // Keep only the non-marked EConcat for which we
          // have not seen a similar EConcatMarked.
          for (auto e:  elist)
            if (empairs.find(std::make_pair(e[0], e[1]))
                == empairs.end())
              v.emplace_back(e);
          // Keep only the non-marked NegClosure for which we
          // have not seen a similar NegClosureMarked.
          for (auto n: nlist)
            if (nmset.find(n[0]) == nmset.end())
              v.emplace_back(n);
          res = formula::And(v);
        }
        break;
      case op::Xor:
      case op::Implies:
      case op::Equiv:
      case op::OrRat:
      case op::AndRat:
      case op::AndNLM:
      case op::Star:
      case op::FStar:
      case op::Concat:
      case op::Fusion:
        SPOT_UNIMPLEMENTED();
      }

    simpmark_[f] = res;
    return res;
  }
}
