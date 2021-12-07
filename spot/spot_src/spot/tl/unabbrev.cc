// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2018-2020 Laboratoire de Recherche et
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
#include <spot/tl/unabbrev.hh>

using namespace std::string_literals;

namespace spot
{
  unabbreviator::unabbreviator(const char* opt)
  {
    while (*opt)
      switch (char c = *opt++)
        {
        case 'e':
          re_e_ = true;
          re_some_bool_ = true;
          break;
        case 'F':
          re_f_ = true;
          re_some_f_g_ = true;
          break;
        case 'G':
          re_g_ = true;
          re_some_f_g_ = true;
          break;
        case 'i':
          re_i_ = true;
          re_some_bool_ = true;
          break;
        case 'M':
          re_m_ = true;
          re_some_other_ = true;
          break;
        case 'R':
          re_r_ = true;
          re_some_other_ = true;
          break;
        case 'W':
          re_w_ = true;
          re_some_other_ = true;
          break;
        case '^':
          re_xor_ = true;
          re_some_bool_ = true;
          break;
        default:
          throw std::runtime_error("unknown unabbreviation option: "s + c);
        }
  }

  formula unabbreviator::run(formula in)
  {
    auto entry = cache_.emplace(in, nullptr);
    if (!entry.second)
      return entry.first->second;

    // Skip recursion whenever possible
    bool no_boolean_rewrite = !re_some_bool_ || in.is_sugar_free_boolean();
    bool no_f_g_rewrite = !re_some_f_g_ || in.is_sugar_free_ltl();
    if (no_boolean_rewrite
        && (in.is_boolean() || (no_f_g_rewrite && !re_some_other_)))
      return entry.first->second = in;

    auto rec = [this](formula f)
      {
        return this->run(f);
      };

    formula out = in;
    if (in.size() > 0)
      out = in.map(rec);

    switch (out.kind())
      {
      case op::ff:
      case op::tt:
      case op::eword:
      case op::ap:
      case op::Not:
      case op::X:
      case op::strong_X:
      case op::Closure:
      case op::NegClosure:
      case op::NegClosureMarked:
      case op::EConcat:
      case op::EConcatMarked:
      case op::UConcat:
      case op::U:
      case op::Or:
      case op::OrRat:
      case op::And:
      case op::AndRat:
      case op::AndNLM:
      case op::Concat:
      case op::Fusion:
      case op::Star:
      case op::FStar:
      case op::first_match:
        break;
      case op::F:
        //  F e = e    if e eventual
        //  F f = true U f
        if (!re_f_)
          break;
        if (out[0].is_eventual())
          {
            out = out[0];
            break;
          }
        out = formula::U(formula::tt(), out[0]);
        break;
      case op::G:
        //  G u = u   if u universal
        //  G f = false R f
        //  G f = f W false
        //  G f = !F!f
        //  G f = !(true U !f)
        if (!re_g_)
          break;
        if (out[0].is_universal())
          {
            out = out[0];
            break;
          }
        if (!re_r_)
          {
            out = formula::R(formula::ff(), out[0]);
            break;
          }
        if (!re_w_)
          {
            out = formula::W(out[0], formula::ff());
            break;
          }
        {
          auto nc = formula::Not(out[0]);
          if (!re_f_)
            {
              out = formula::Not(formula::F(nc));
              break;
            }
          out = formula::Not(formula::U(formula::tt(), nc));
          break;
        }
      case op::Xor:
        // f1 ^ f2  ==  !(f1 <-> f2)
        // f1 ^ f2  ==  (f1 & !f2) | (f2 & !f1)
        if (!re_xor_)
          break;
        {
          auto f1 = out[0];
          auto f2 = out[1];
          if (!re_e_)
            {
              out = formula::Not(formula::Equiv(f1, f2));
            }
          else
            {
              auto a = formula::And({f1, formula::Not(f2)});
              auto b = formula::And({f2, formula::Not(f1)});
              out = formula::Or({a, b});
            }
        }
        break;
      case op::Implies:
        // f1 => f2  ==  !f1 | f2
        if (!re_i_)
          break;
        out = formula::Or({formula::Not(out[0]), out[1]});
        break;
      case op::Equiv:
        // f1 <=> f2  ==  (f1 & f2) | (!f1 & !f2)
        if (!re_e_)
          break;
        {
          auto f1 = out[0];
          auto f2 = out[1];
          auto nf1 = formula::Not(f1);
          auto nf2 = formula::Not(f2);
          auto term1 = formula::And({f1, f2});
          auto term2 = formula::And({nf1, nf2});
          out = formula::Or({term1, term2});
          break;
        }
      case op::R:
        // f1 R u = u   if u universal
        // f1 R f2 = f2 W (f1 & f2)
        // f1 R f2 = f2 U ((f1 & f2) | Gf2)
        // f1 R f2 = f2 U ((f1 & f2) | !F!f2)
        // f1 R f2 = f2 U ((f1 & f2) | !(1 U !f2))
        if (!re_r_)
          break;
        {
          auto f2 = out[1];
          if (f2.is_universal())
            {
              out = f2;
              break;
            }
          auto f1 = out[0];
          auto f12 = formula::And({f1, f2});
          if (!re_w_)
            {
              out = formula::W(f2, f12);
              break;
            }
          auto gf2 = formula::G(f2);
          if (re_g_)
            gf2 = run(gf2);
          out = formula::U(f2, formula::Or({f12, gf2}));
          break;
        }
      case op::W:
        // f1 W u = G(f1 | u)   if u universal
        // f1 W f2 = f2 R (f2 | f1)
        // f1 W f2 = f1 U (f2 | G f1)
        // f1 W f2 = f1 U (f2 | !F !f1)
        // f1 W f2 = f1 U (f2 | !(1 U !f1))
        if (!re_w_)
          break;
        {
          auto f1 = out[0];
          auto f2 = out[1];
          if (f2.is_universal())
            {
              auto g = formula::G(formula::Or({f1, f2}));
              out = re_g_ ? run(g) : g;
              break;
            }
          if (!re_r_)
            {
              out = formula::R(f2, formula::Or({f1, f2}));
              break;
            }
          auto gf1 = formula::G(f1);
          if (re_g_)
            gf1 = run(gf1);
          out = formula::U(f1, formula::Or({f2, gf1}));
          break;
        }
      case op::M:
        // f1 M e = F(f1 & e)   if e eventual
        // f1 M f2 = f2 U (g2 & f1)
        if (!re_m_)
          break;
        {
          auto f1 = out[0];
          auto f2 = out[1];
          auto andf = formula::And({f1, f2});
          if (f2.is_eventual())
            {
              auto f = formula::F(andf);
              out = re_f_ ? run(f) : f;
              break;
            }
          out = formula::U(f2, andf);
          break;
        }
      }
    // The recursion may have invalidated the "entry" iterator, so do
    // not reuse it.
    return cache_[in] = out;
  }

  formula unabbreviate(formula in, const char* opt)
  {
    unabbreviator un(opt);
    return un.run(in);
  }
}
