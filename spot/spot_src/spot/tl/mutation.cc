// -*- coding: utf-8 -*-
// Copyright (C) 2014-2016, 2018-2019 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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
#include <set>
#include <algorithm>

#include <spot/tl/apcollect.hh>
#include <spot/tl/mutation.hh>
#include <spot/tl/length.hh>

#define And_(x, y) formula::And({(x), (y)})
#define AndRat_(x, y) formula::AndRat({(x), (y)})
#define AndNLM_(x) formula::AndNLM(x)
#define Concat_(x, y) formula::Concat({(x), (y)})
#define Not_(x) formula::Not(x)

namespace spot
{
  namespace
  {
    formula substitute_ap(formula f, formula ap_src, formula ap_dst)
    {
      return f.map([&](formula f)
                   {
                     if (f == ap_src)
                       return ap_dst;
                     else
                       return substitute_ap(f, ap_src, ap_dst);
                   });
    }

    typedef std::vector<formula> vec;
    class mutator final
    {
      int mutation_counter_ = 0;
      formula f_;
      unsigned opts_;
    public:
      mutator(formula f, unsigned opts) : f_(f), opts_(opts)
      {
      }

      formula mutate(formula f)
      {
        auto recurse = [this](formula f)
          {
            return this->mutate(f);
          };

        switch (f.kind())
          {
          case op::ff:
          case op::tt:
          case op::eword:
            return f;
          case op::ap:
            if (opts_ & Mut_Ap2Const)
              {
                if (mutation_counter_-- == 0)
                  return formula::tt();
                if (mutation_counter_-- == 0)
                  return formula::ff();
              }
            return f;
          case op::Not:
          case op::X:
          case op::strong_X:
          case op::F:
          case op::G:
          case op::first_match:
            if ((opts_ & Mut_Remove_Ops)
                && mutation_counter_-- == 0)
              return f[0];
            SPOT_FALLTHROUGH;
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
            if (mutation_counter_ < 0)
              return f;
            else
              return f.map(recurse);
          case op::Or:
          case op::OrRat:
          case op::And:
          case op::AndRat:
          case op::AndNLM:
          case op::Concat:
          case op::Fusion:
            {
              int mos = f.size();
              if (opts_ & Mut_Remove_Multop_Operands)
                {
                  for (int i = 0; i < mos; ++i)
                    if (mutation_counter_-- == 0)
                      return f.all_but(i);
                }

              if (opts_ & Mut_Split_Ops && f.is(op::AndNLM))
                {
                  if (mutation_counter_ >= 0
                      && mutation_counter_ < 2 * (mos - 1))
                    {
                      vec v1;
                      vec v2;
                      v1.emplace_back(f[0]);
                      bool reverse = false;
                      int i = 1;
                      while (i < mos)
                        {
                          if (mutation_counter_-- == 0)
                            break;
                          if (mutation_counter_-- == 0)
                            {
                              reverse = true;
                              break;
                            }
                          v1.emplace_back(f[i++]);
                        }
                      for (; i < mos; ++i)
                        v2.emplace_back(f[i]);
                      formula first = AndNLM_(v1);
                      formula second = AndNLM_(v2);
                      formula ost = formula::one_star();
                      if (!reverse)
                        return AndRat_(Concat_(first, ost), second);
                      else
                        return AndRat_(Concat_(second, ost), first);
                    }
                  else
                    {
                      mutation_counter_ -= 2 * (mos - 1);
                    }
                }

              if (mutation_counter_ < 0)
                return f;
              else
                return f.map(recurse);
            }
          case op::Xor:
          case op::Implies:
          case op::Equiv:
          case op::U:
          case op::R:
          case op::W:
          case op::M:
          case op::EConcat:
          case op::EConcatMarked:
          case op::UConcat:
            {
              formula first = f[0];
              formula second = f[1];
              op o = f.kind();
              bool left_is_sere = o == op::EConcat
                || o == op::EConcatMarked
                || o == op::UConcat;

              if (opts_ & Mut_Remove_Ops && mutation_counter_-- == 0)
                {
                  if (!left_is_sere)
                    return first;
                  else if (o == op::UConcat)
                    return formula::NegClosure(first);
                  else // EConcat or EConcatMarked
                    return formula::Closure(first);
                }
              if (opts_ & Mut_Remove_Ops && mutation_counter_-- == 0)
                return second;
              if (opts_ & Mut_Rewrite_Ops)
                {
                  switch (o)
                    {
                    case op::U:
                      if (mutation_counter_-- == 0)
                        return formula::W(first, second);
                      break;
                    case op::M:
                      if (mutation_counter_-- == 0)
                        return formula::R(first, second);
                      if (mutation_counter_-- == 0)
                        return formula::U(second, first);
                      break;
                    case op::R:
                      if (mutation_counter_-- == 0)
                        return formula::W(second, first);
                      break;
                    default:
                      break;
                    }
                }
              if (opts_ & Mut_Split_Ops)
                {
                  switch (o)
                    {
                    case op::Equiv:
                      if (mutation_counter_-- == 0)
                        return formula::Implies(first, second);
                      if (mutation_counter_-- == 0)
                        return formula::Implies(second, first);
                      if (mutation_counter_-- == 0)
                        return formula::And({first, second});
                      if (mutation_counter_-- == 0)
                        {
                          // Negate the two argument sequentially (in this
                          // case right before left, otherwise different
                          // compilers will make different choices.
                          auto right = formula::Not(second);
                          return formula::And({formula::Not(first), right});
                        }
                      break;
                    case op::Xor:
                      if (mutation_counter_-- == 0)
                        return formula::And({first, formula::Not(second)});
                      if (mutation_counter_-- == 0)
                        return formula::And({formula::Not(first), second});
                      break;
                    default:
                      break;
                    }
                }
              if (mutation_counter_ < 0)
                return f;
              else
                return f.map(recurse);
            }
          case op::Star:
          case op::FStar:
            {
              formula c = f[0];
              op o = f.kind();
              if (opts_ & Mut_Remove_Ops && mutation_counter_-- == 0)
                return c;
              if (opts_ & Mut_Simplify_Bounds)
                {
                  auto min = f.min();
                  auto max = f.max();
                  if (min > 0)
                    {
                      if (mutation_counter_-- == 0)
                        return formula::bunop(o, c, min - 1, max);
                      if (mutation_counter_-- == 0)
                        return formula::bunop(o, c, 0, max);
                    }
                  if (max != formula::unbounded())
                    {
                      if (max > min && mutation_counter_-- == 0)
                        return formula::bunop(o, c, min, max - 1);
                      if (mutation_counter_-- == 0)
                        return formula::bunop(o, c, min,
                                              formula::unbounded());
                    }
                }
              if (mutation_counter_ < 0)
                return f;
              else
                return f.map(recurse);
            }
          }
        SPOT_UNREACHABLE();
      }

      formula
        get_mutation(int n)
      {
        mutation_counter_ = n;
        formula mut = mutate(f_);
        if (mut == f_)
          return nullptr;
        return mut;
      }

    };

    bool
    formula_length_less_than(formula left, formula right)
    {
      assert(left != nullptr);
      assert(right != nullptr);
      if (left == right)
        return false;
      auto ll = length(left);
      auto lr = length(right);
      if (ll < lr)
        return true;
      if (ll > lr)
        return false;
      return left < right;
    }

    typedef std::set<formula> fset_t;

    void
    single_mutation_rec(formula f, fset_t& mutations, unsigned opts,
                        unsigned& n, unsigned m)
    {
      if (m == 0)
        {
          if (mutations.insert(f).second)
            --n;
        }
      else
        {
          formula mut;
          int i = 0;
          mutator mv(f, opts);
          while (n > 0 && ((mut = mv.get_mutation(i++)) != nullptr))
            single_mutation_rec(mut, mutations, opts, n, m - 1);
        }
    }

    void
    replace_ap_rec(formula f, fset_t& mutations, unsigned opts,
                   unsigned& n, unsigned m)
    {
      if (m == 0)
        {
          if (mutations.insert(f).second)
            --n;
        }
      else
        {
          if (!n)
            return;
          auto aps =
            std::unique_ptr<atomic_prop_set>(atomic_prop_collect(f));
          for (auto ap1: *aps)
            for (auto ap2: *aps)
              {
                if (ap1 == ap2)
                  continue;
                auto mut = substitute_ap(f, ap1, ap2);
                replace_ap_rec(mut, mutations, opts, n, m - 1);
                if (!n)
                  return;
              }
        }
    }
  }

  std::vector<formula>
  mutate(formula f, unsigned opts, unsigned max_output,
         unsigned mutation_count, bool sort)
  {
    fset_t mutations;
    single_mutation_rec(f, mutations, opts, max_output, mutation_count);
    if (opts & Mut_Remove_One_Ap)
      replace_ap_rec(f, mutations, opts, max_output, mutation_count);

    vec res(mutations.begin(), mutations.end());
    if (sort)
      std::sort(res.begin(), res.end(), formula_length_less_than);
    return res;
  }
}
