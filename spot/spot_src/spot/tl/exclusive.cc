// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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
#include <spot/tl/exclusive.hh>
#include <spot/twaalgos/mask.hh>
#include <spot/misc/casts.hh>
#include <spot/misc/minato.hh>
#include <spot/tl/apcollect.hh>

namespace spot
{
  namespace
  {
    static const std::vector<formula>
    split_aps(const char* arg)
    {
      std::vector<formula> group;
      auto start = arg;
      while (*start)
        {
          while (*start == ' ' || *start == '\t')
            ++start;
          if (!*start)
            break;
          if (*start == ',')
            {
              std::string s = "unexpected ',' in ";
              s += arg;
              throw std::invalid_argument(s);
            }
          if (*start == '"')
            {
              ++start;
              auto end = start;
              while (*end && *end != '"')
                {
                  if (*end == '\\')
                    ++end;
                  ++end;
                }
              if (!*end)
                {
                  std::string s = "missing closing '\"' in ";
                  s += arg;
                  throw std::invalid_argument(s);
                }
              std::string ap(start, end - start);
              group.emplace_back(formula::ap(ap));
              do
                ++end;
              while (*end == ' ' || *end == '\t');
              if (*end && *end != ',')
                {
                  std::string s = "unexpected character '";
                  s += *end;
                  s += "' in ";
                  s += arg;
                  throw std::invalid_argument(s);
                }
              if (*end == ',')
                ++end;
              start = end;
            }
          else
            {
              auto end = start;
              while (*end && *end != ',')
                ++end;
              auto rend = end;
              while (rend > start && (rend[-1] == ' ' || rend[-1] == '\t'))
                --rend;
              std::string ap(start, rend - start);
              group.emplace_back(formula::ap(ap));
              if (*end == ',')
                start = end + 1;
              else
                break;
            }
        }
      return group;
    }
  }

  void exclusive_ap::add_group(const char* ap_csv)
  {
    add_group(split_aps(ap_csv));
  }

  void exclusive_ap::add_group(std::vector<formula> ap)
  {
    groups.emplace_back(ap);
  }

  namespace
  {
    formula
    nand(formula lhs, formula rhs)
    {
      return formula::Not(formula::And({lhs, rhs}));
    }
  }

  formula
  exclusive_ap::constrain(formula f) const
  {
    auto* s = atomic_prop_collect(f);

    std::vector<formula> group;
    std::vector<formula> v;

    for (auto& g: groups)
      {
        group.clear();

        for (auto ap: g)
          if (s->find(ap) != s->end())
            group.emplace_back(ap);

        unsigned s = group.size();
        for (unsigned j = 0; j < s; ++j)
          for (unsigned k = j + 1; k < s; ++k)
            v.emplace_back(nand(group[j], group[k]));
      };

    delete s;
    return formula::And({f, formula::G(formula::And(v))});
  }

  twa_graph_ptr exclusive_ap::constrain(const_twa_graph_ptr aut,
                                           bool simplify_guards) const
  {
    // Compute the support of the automaton.
    bdd support = bddtrue;
    {
      std::set<int> bdd_seen;
      for (auto& t: aut->edges())
        if (bdd_seen.insert(t.cond.id()).second)
          support &= bdd_support(t.cond);
    }

    bdd restrict_ = bddtrue;
    auto d = aut->get_dict();

    std::vector<bdd> group;
    for (auto& g: groups)
      {
        group.clear();

        for (auto ap: g)
          {
            int v = d->has_registered_proposition(ap, aut);
            if (v >= 0)
              group.emplace_back(bdd_nithvar(v));
          }

        unsigned s = group.size();
        for (unsigned j = 0; j < s; ++j)
          for (unsigned k = j + 1; k < s; ++k)
            restrict_ &= group[j] | group[k];
      }

    twa_graph_ptr res = make_twa_graph(aut->get_dict());
    res->copy_ap_of(aut);
    res->prop_copy(aut, { true, true, false, false, false, true });
    res->copy_acceptance_of(aut);
    if (simplify_guards)
      {
        transform_accessible(aut, res, [&](unsigned, bdd& cond,
                                           acc_cond::mark_t&, unsigned)
                             {
                               minato_isop isop(cond & restrict_,
                                                cond | !restrict_,
                                                true);
                               bdd res = bddfalse;
                               bdd cube = bddfalse;
                               while ((cube = isop.next()) != bddfalse)
                                 res |= cube;
                               cond = res;
                             });
        res->remove_unused_ap();
      }
    else
      {
        transform_accessible(aut, res, [&](unsigned, bdd& cond,
                                           acc_cond::mark_t&, unsigned)
                             {
                               cond &= restrict_;
                             });
      }
    return res;
  }
}
