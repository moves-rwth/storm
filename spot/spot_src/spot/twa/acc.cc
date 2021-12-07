// -*- coding: utf-8 -*-
// Copyright (C) 2015-2020 Laboratoire de Recherche et Développement
// de l'Epita.
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
#include <iostream>
#include <sstream>
#include <set>
#include <cctype>
#include <cstring>
#include <map>
#include <numeric>
#include <spot/twa/acc.hh>
#include "spot/priv/bddalloc.hh"
#include <spot/misc/minato.hh>
#include <spot/misc/random.hh>

using namespace std::string_literals;

namespace spot
{
  void acc_cond::report_too_many_sets()
  {
#define STR(x) #x
#define VALUESTR(x) STR(x)
    throw std::runtime_error("Too many acceptance sets used.  "
                             "The limit is " VALUESTR(SPOT_MAX_ACCSETS) ".");
  }

  std::ostream& operator<<(std::ostream& os, spot::acc_cond::mark_t m)
  {
    auto a = m;
    os << '{';
    unsigned level = 0;
    const char* comma = "";
    while (a)
      {
        if (a.has(0))
          {
            os << comma << level;
            comma = ",";
          }
        a >>= 1;
        ++level;
      }
    os << '}';
    return os;
  }

  std::ostream& operator<<(std::ostream& os, const acc_cond& acc)
  {
    return os << '(' << acc.num_sets() << ", " << acc.get_acceptance() << ')';
  }

  namespace
  {
    void default_set_printer(std::ostream& os, int v)
    {
      os << v;
    }

    enum code_output {HTML, TEXT, LATEX};

    template<enum code_output style>
    static void
    print_code(std::ostream& os,
               const acc_cond::acc_code& code, unsigned pos,
               std::function<void(std::ostream&, int)> set_printer)
    {
      const char* op_ = style == LATEX ? " \\lor " : " | ";
      auto& w = code[pos];
      const char* negated_pre = "";
      const char* negated_post = "";
      auto set_neg = [&]() {
        if (style == LATEX)
          {
            negated_pre = "\\overline{";
            negated_post = "}";
          }
        else
          {
            negated_pre = "!";
          }
      };
      bool top = pos == code.size() - 1;
      switch (w.sub.op)
        {
        case acc_cond::acc_op::And:
          switch (style)
            {
            case HTML:
              op_ = " &amp; ";
              break;
            case TEXT:
              op_ = " & ";
              break;
            case LATEX:
              op_ = " \\land ";
              break;
            }
          SPOT_FALLTHROUGH;
        case acc_cond::acc_op::Or:
          {
            unsigned sub = pos - w.sub.size;
            if (!top)
              os << '(';
            bool first = true;
            while (sub < pos)
              {
                --pos;
                if (first)
                  first = false;
                else
                  os << op_;
                print_code<style>(os, code, pos, set_printer);
                pos -= code[pos].sub.size;
              }
            if (!top)
              os << ')';
          }
          break;
        case acc_cond::acc_op::InfNeg:
          set_neg();
          SPOT_FALLTHROUGH;
        case acc_cond::acc_op::Inf:
          {
            auto a = code[pos - 1].mark;
            if (!a)
              {
                if (style == LATEX)
                  os << "\\mathsf{t}";
                else
                  os << 't';
              }
            else
              {
                if (!top)
                  // Avoid extra parentheses if there is only one set
                  top = code[pos - 1].mark.is_singleton();
                unsigned level = 0;
                const char* and_ = "";
                const char* and_next_ = []() {
                  // The lack of surrounding space in HTML and
                  // TEXT is on purpose: we want to
                  // distinguish those grouped "Inf"s from
                  // other terms that are ANDed together.
                  switch (style)
                    {
                    case HTML:
                      return "&amp;";
                    case TEXT:
                      return "&";
                    case LATEX:
                      return " \\land ";
                    }
                }();
                if (!top)
                  os << '(';
                const char* inf_ = (style == LATEX) ? "\\mathsf{Inf}(" : "Inf(";
                while (a)
                  {
                    if (a.has(0))
                      {
                        os << and_ << inf_ << negated_pre;
                        set_printer(os, level);
                        os << negated_post << ')';
                        and_ = and_next_;
                      }
                    a >>= 1;
                    ++level;
                  }
                if (!top)
                  os << ')';
              }
          }
          break;
        case acc_cond::acc_op::FinNeg:
          set_neg();
          SPOT_FALLTHROUGH;
        case acc_cond::acc_op::Fin:
          {
            auto a = code[pos - 1].mark;
            if (!a)
              {
                if (style == LATEX)
                  os << "\\mathsf{f}";
                else
                  os << 'f';
              }
            else
              {
                if (!top)
                  // Avoid extra parentheses if there is only one set
                  top = code[pos - 1].mark.is_singleton();
                unsigned level = 0;
                const char* or_ = "";
                if (!top)
                  os << '(';
                const char* fin_ = (style == LATEX) ? "\\mathsf{Fin}(" : "Fin(";
                while (a)
                  {
                    if (a.has(0))
                      {
                        os << or_ << fin_ << negated_pre;
                        set_printer(os, level);
                        os << negated_post << ')';
                        // The lack of surrounding space in HTML and
                        // TEXT is on purpose: we want to distinguish
                        // those grouped "Fin"s from other terms that
                        // are ORed together.
                        or_ = style == LATEX ? " \\lor " : "|";
                      }
                    a >>= 1;
                    ++level;
                  }
                if (!top)
                  os << ')';
              }
          }
          break;
        }
    }


    static bool
    eval(acc_cond::mark_t inf, const acc_cond::acc_word* pos)
    {
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            auto sub = pos - pos->sub.size;
            while (sub < pos)
              {
                --pos;
                if (!eval(inf, pos))
                  return false;
                pos -= pos->sub.size;
              }
            return true;
          }
        case acc_cond::acc_op::Or:
          {
            auto sub = pos - pos->sub.size;
            while (sub < pos)
              {
                --pos;
                if (eval(inf, pos))
                  return true;
                pos -= pos->sub.size;
              }
            return false;
          }
        case acc_cond::acc_op::Inf:
          return (pos[-1].mark & inf) == pos[-1].mark;
        case acc_cond::acc_op::Fin:
          return (pos[-1].mark & inf) != pos[-1].mark;
        case acc_cond::acc_op::FinNeg:
        case acc_cond::acc_op::InfNeg:
          SPOT_UNREACHABLE();
        }
      SPOT_UNREACHABLE();
      return false;
    }

    static trival
    partial_eval(acc_cond::mark_t infinitely_often,
                 acc_cond::mark_t always_present,
                 const acc_cond::acc_word* pos)
    {
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            auto sub = pos - pos->sub.size;
            trival res = true;
            while (sub < pos)
              {
                --pos;
                res = res &&
                  partial_eval(infinitely_often, always_present, pos);
                if (res.is_false())
                  return res;
                pos -= pos->sub.size;
              }
            return res;
          }
        case acc_cond::acc_op::Or:
          {
            auto sub = pos - pos->sub.size;
            trival res = false;
            while (sub < pos)
              {
                --pos;
                res = res ||
                  partial_eval(infinitely_often, always_present, pos);
                if (res.is_true())
                  return res;
                pos -= pos->sub.size;
              }
            return res;
          }
        case acc_cond::acc_op::Inf:
          return (pos[-1].mark & infinitely_often) == pos[-1].mark;
        case acc_cond::acc_op::Fin:
          if ((pos[-1].mark & always_present) == pos[-1].mark)
            return false;
          else if ((pos[-1].mark & infinitely_often) != pos[-1].mark)
            return true;
          else
            return trival::maybe();
        case acc_cond::acc_op::FinNeg:
        case acc_cond::acc_op::InfNeg:
          SPOT_UNREACHABLE();
        }
      SPOT_UNREACHABLE();
      return false;
    }

    static acc_cond::mark_t
    eval_sets(acc_cond::mark_t inf, const acc_cond::acc_word* pos)
    {
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            auto sub = pos - pos->sub.size;
            acc_cond::mark_t m = {};
            while (sub < pos)
              {
                --pos;
                if (auto s = eval_sets(inf, pos))
                  m |= s;
                else
                  return {};
                pos -= pos->sub.size;
              }
            return m;
          }
        case acc_cond::acc_op::Or:
          {
            auto sub = pos - pos->sub.size;
            while (sub < pos)
              {
                --pos;
                if (auto s = eval_sets(inf, pos))
                  return s;
                pos -= pos->sub.size;
              }
            return {};
          }
        case acc_cond::acc_op::Inf:
          if ((pos[-1].mark & inf) == pos[-1].mark)
            return pos[-1].mark;
          else
            return {};
        case acc_cond::acc_op::Fin:
        case acc_cond::acc_op::FinNeg:
        case acc_cond::acc_op::InfNeg:
          SPOT_UNREACHABLE();
        }
      SPOT_UNREACHABLE();
      return {};
    }
  }

  bool acc_cond::acc_code::accepting(mark_t inf) const
  {
    if (empty())
      return true;
    return eval(inf, &back());
  }

  trival acc_cond::acc_code::maybe_accepting(mark_t infinitely_often,
                                             mark_t always_present) const
  {
    if (empty())
      return true;
    return partial_eval(infinitely_often | always_present,
                        always_present, &back());
  }

  bool acc_cond::acc_code::inf_satisfiable(mark_t inf) const
  {
    return !maybe_accepting(inf, {}).is_false();
  }


  acc_cond::mark_t acc_cond::accepting_sets(mark_t inf) const
  {
    if (uses_fin_acceptance())
      throw std::runtime_error
        ("Fin acceptance is not supported by this code path.");
    if (code_.empty())
      return {};
    return eval_sets(inf, &code_.back());
  }

  bool acc_cond::check_fin_acceptance() const
  {
    if (code_.empty())
      return false;
    unsigned pos = code_.size();
    do
      {
        switch (code_[pos - 1].sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            pos -= 2;
            break;
          case acc_cond::acc_op::Fin:
            if (!code_[pos - 2].mark) // f
              {
                pos -= 2;
                break;
              }
            SPOT_FALLTHROUGH;
          case acc_cond::acc_op::FinNeg:
            return true;
          }
      }
    while (pos > 0);
    return false;
  }


  namespace
  {
    // Is Rabin or Streett, depending on highop and lowop.
    static bool
    is_rs(const acc_cond::acc_code& code,
          acc_cond::acc_op highop,
          acc_cond::acc_op lowop,
          acc_cond::mark_t all_sets)
    {
      unsigned s = code.back().sub.size;
      auto mainop = code.back().sub.op;
      if (mainop == highop)
        {
          // The size must be a multiple of 5.
          if ((s != code.size() - 1) || (s % 5))
            return false;
        }
      else                        // Single pair?
        {
          if (s != 4 || mainop != lowop)
            return false;
          // Pretend we were in a unary highop.
          s = 5;
        }
      acc_cond::mark_t seen_fin = {};
      acc_cond::mark_t seen_inf = {};
      while (s)
        {
          if (code[--s].sub.op != lowop)
            return false;
          auto o1 = code[--s].sub.op;
          auto m1 = code[--s].mark;
          auto o2 = code[--s].sub.op;
          auto m2 = code[--s].mark;

          // We assume
          //   Fin(n) lowop Inf(n+1)
          //   o1 (m1)       o2 (m2)
          // swap if it is the converse
          if (o2 == acc_cond::acc_op::Fin)
            {
              std::swap(o1, o2);
              std::swap(m1, m2);
            }
          if (o1 != acc_cond::acc_op::Fin
              || o2 != acc_cond::acc_op::Inf
              || !m1.is_singleton()
              || m2 != (m1 << 1))
            return false;
          seen_fin |= m1;
          seen_inf |= m2;
        }

      return (!(seen_fin & seen_inf)
              && (seen_fin | seen_inf) == all_sets);
    }

    // Is Rabin-like or Streett-like, depending on highop and lowop.
    static bool
    is_rs_like(const acc_cond::acc_code& code,
              acc_cond::acc_op highop,
              acc_cond::acc_op lowop,
              acc_cond::acc_op singleop,
              std::vector<acc_cond::rs_pair>& pairs)
    {
      assert(pairs.empty());
      unsigned s = code.back().sub.size;
      auto mainop = code.back().sub.op;

      if (mainop == acc_cond::acc_op::Fin || mainop == acc_cond::acc_op::Inf)
        {
          assert(code.size() == 2);

          auto m = code[0].mark;
          if (mainop == singleop && !m.is_singleton())
            return false;

          acc_cond::mark_t fin = {};
          acc_cond::mark_t inf = {};
          for (unsigned mark: m.sets())
            {
              if (mainop == acc_cond::acc_op::Fin)
                fin = {mark};
              else
                inf = {mark};

              pairs.emplace_back(fin, inf);
            }
          return true;
        }
      else if (mainop == lowop)      // Single pair?
        {
          if (s != 4)
            return false;
          // Pretend we were in a unary highop.
          s = 5;
        }
      else if (mainop != highop)
        {
          return false;
        }
      while (s)
        {
          auto op = code[--s].sub.op;
          auto size = code[s].sub.size;
          if (op == acc_cond::acc_op::Fin
              || op == acc_cond::acc_op::Inf)
            {
              auto m = code[--s].mark;
              acc_cond::mark_t fin = {};
              acc_cond::mark_t inf = {};

              if (op == singleop && !m.is_singleton())
                {
                  pairs.clear();
                  return false;
                }
              for (unsigned mark: m.sets())
                {
                  if (op == acc_cond::acc_op::Fin)
                    fin = {mark};
                  else //fin
                    inf = {mark};
                  pairs.emplace_back(fin, inf);
                }
            }
          else
            {
              if (op != lowop || size != 4)
                {
                  pairs.clear();
                  return false;
                }

              auto o1 = code[--s].sub.op;
              auto m1 = code[--s].mark;
              auto o2 = code[--s].sub.op;
              auto m2 = code[--s].mark;

              // We assume
              //   Fin(n) lowop Inf(n+1)
              //   o1 (m1)       o2 (m2)
              // swap if it is the converse
              if (o2 == acc_cond::acc_op::Fin)
                {
                  std::swap(o1, o2);
                  std::swap(m1, m2);
                }
              if (o1 != acc_cond::acc_op::Fin
                  || o2 != acc_cond::acc_op::Inf
                  || !m1.is_singleton()
                  || !m2.is_singleton())
                {
                  pairs.clear();
                  return false;
                }
              pairs.emplace_back(m1, m2);
            }
        }

      return true;
    }
  }

  int acc_cond::is_rabin() const
  {
    if (code_.is_f())
      return num_ == 0 ? 0 : -1;
    if ((num_ & 1) || code_.is_t())
      return -1;

    if (is_rs(code_, acc_op::Or, acc_op::And, all_sets()))
      return num_ / 2;
    else
      return -1;
  }

  int acc_cond::is_streett() const
  {
    if (code_.is_t())
      return num_ == 0 ? 0 : -1;
    if ((num_ & 1) || code_.is_f())
      return -1;

    if (is_rs(code_, acc_op::And, acc_op::Or, all_sets()))
      return num_ / 2;
    else
      return -1;
  }

  bool acc_cond::is_streett_like(std::vector<rs_pair>& pairs) const
  {
    pairs.clear();
    if (code_.is_t())
      return true;
    if (code_.is_f())
      {
        pairs.emplace_back(mark_t({}), mark_t({}));
        return true;
      }
    return is_rs_like(code_, acc_op::And, acc_op::Or, acc_op::Fin, pairs);
  }

  bool acc_cond::is_rabin_like(std::vector<rs_pair>& pairs) const
  {
    pairs.clear();
    if (code_.is_f())
      return true;
    if (code_.is_t())
      {
        pairs.emplace_back(mark_t({}), mark_t({}));
        return true;
      }
    return is_rs_like(code_, acc_op::Or, acc_op::And, acc_op::Inf, pairs);
  }


  // PAIRS contains the number of Inf in each pair.
  bool acc_cond::is_generalized_rabin(std::vector<unsigned>& pairs) const
  {
    pairs.clear();
    if (is_generalized_co_buchi())
      {
        pairs.resize(num_);
        return true;
      }
    // "Acceptance: 0 f" is caught by is_generalized_rabin() above.
    // Therefore is_f() below catches "Acceptance: n f" with n>0.
    if (code_.is_f() || code_.is_t())
      return false;

    auto s = code_.back().sub.size;
    acc_cond::mark_t seen_fin = {};
    acc_cond::mark_t seen_inf = {};

    // generalized Rabin should start with Or, unless
    // it has a single pair.
    bool singlepair = false;
    {
      auto topop = code_.back().sub.op;
      if (topop == acc_op::And)
        {
          // Probably single-pair generalized-Rabin.  Shift the source
          // by one position as if we had an invisible Or in front.
          ++s;
          singlepair = true;
        }
      else if (topop != acc_op::Or)
        {
          return false;
        }
    }

    // Each pair is the position of a Fin followed
    // by the number of Inf.
    std::map<unsigned, unsigned> p;
    while (s)
      {
        --s;
        if (code_[s].sub.op == acc_op::And)
          {
            auto o1 = code_[--s].sub.op;
            auto m1 = code_[--s].mark;
            auto o2 = code_[--s].sub.op;
            auto m2 = code_[--s].mark;

            // We assume
            //   Fin(n) & Inf({n+1,n+2,...,n+i})
            //   o1 (m1)       o2 (m2)
            // swap if it is the converse
            if (o2 == acc_cond::acc_op::Fin)
              {
                std::swap(o1, o2);
                std::swap(m1, m2);
              }

            if (o1 != acc_cond::acc_op::Fin
                || o2 != acc_cond::acc_op::Inf
                || !m1.is_singleton())
              return false;

            unsigned i = m2.count();
            // If we have seen this pair already, it must have the
            // same size.
            if (p.emplace(m1.max_set() - 1, i).first->second != i)
              return false;
            assert(i > 0);
            unsigned j = m1.max_set(); // == n+1
            do
              if (!m2.has(j++))
                return false;
            while (--i);
            seen_fin |= m1;
            seen_inf |= m2;
          }
        else if (code_[s].sub.op == acc_op::Fin)
          {
            auto m1 = code_[--s].mark;
            for (auto s: m1.sets())
              // If we have seen this pair already, it must have the
              // same size.
              {
                if (p.emplace(s, 0U).first->second != 0U)
                  return false;
              }
            seen_fin |= m1;
          }
        else
          {
            return false;
          }
      }
    for (auto i: p)
      pairs.emplace_back(i.second);
    return ((!singlepair || pairs.size() == 1)
            && !(seen_fin & seen_inf)
            && (seen_fin | seen_inf) == all_sets());
  }

  // PAIRS contains the number of Inf in each pair.
  bool acc_cond::is_generalized_streett(std::vector<unsigned>& pairs) const
  {
    pairs.clear();
    if (is_generalized_buchi())
      {
        pairs.resize(num_);
        return true;
      }
    // "Acceptance: 0 t" is caught by is_generalized_buchi() above.
    // Therefore is_t() below catches "Acceptance: n t" with n>0.
    if (code_.is_t() || code_.is_f())
      return false;

    auto s = code_.back().sub.size;
    acc_cond::mark_t seen_fin = {};
    acc_cond::mark_t seen_inf = {};

    // generalized Streett should start with And, unless
    // it has a single pair.
    bool singlepair = false;
    {
      auto topop = code_.back().sub.op;
      if (topop == acc_op::Or)
        {
          // Probably single-pair generalized-Streett.  Shift the source
          // by one position as if we had an invisible And in front.
          ++s;
          singlepair = true;
        }
      else if (topop != acc_op::And)
        {
          return false;
        }
    }


    // Each pairs is the position of a Inf followed
    // by the number of Fin.
    std::map<unsigned, unsigned> p;
    while (s)
      {
        --s;
        if (code_[s].sub.op == acc_op::Or)
          {
            auto o1 = code_[--s].sub.op;
            auto m1 = code_[--s].mark;
            auto o2 = code_[--s].sub.op;
            auto m2 = code_[--s].mark;

            // We assume
            //   Inf(n) | Fin({n+1,n+2,...,n+i})
            //   o1 (m1)       o2 (m2)
            // swap if it is the converse
            if (o2 == acc_cond::acc_op::Inf)
              {
                std::swap(o1, o2);
                std::swap(m1, m2);
              }

            if (o1 != acc_cond::acc_op::Inf
                || o2 != acc_cond::acc_op::Fin
                || !m1.is_singleton())
              return false;

            unsigned i = m2.count();
            // If we have seen this pair already, it must have the
            // same size.
            if (p.emplace(m1.max_set() - 1, i).first->second != i)
              return false;
            assert(i > 0);
            unsigned j = m1.max_set(); // == n+1
            do
              if (!m2.has(j++))
                return false;
            while (--i);

            seen_inf |= m1;
            seen_fin |= m2;
          }
        else if (code_[s].sub.op == acc_op::Inf)
          {
            auto m1 = code_[--s].mark;
            for (auto s: m1.sets())
              // If we have seen this pair already, it must have the
              // same size.
              if (p.emplace(s, 0U).first->second != 0U)
                return false;
            seen_inf |= m1;
          }
        else
          {
            return false;
          }
      }
    for (auto i: p)
      pairs.emplace_back(i.second);
    return ((!singlepair || pairs.size() == 1)
            && !(seen_inf & seen_fin)
            && (seen_inf | seen_fin) == all_sets());
  }

  acc_cond::acc_code
  acc_cond::acc_code::parity(bool max, bool odd, unsigned sets)
  {
    acc_cond::acc_code res;

    if (max)
      res = odd ? t() : f();
    else
      res = ((sets & 1) == odd) ? t() : f();

    if (sets == 0)
      return res;

    // When you look at something like
    //    acc-name: parity min even 5
    //    Acceptance: 5 Inf(0) | (Fin(1) & (Inf(2) | (Fin(3) & Inf(4))))
    // remember that we build it from right to left.
    int start = max ? 0 : sets - 1;
    int inc = max ? 1 : -1;
    int end = max ? sets : -1;
    for (int i = start; i != end; i += inc)
      {
        if ((i & 1) == odd)
          res |= inf({(unsigned)i});
        else
          res &= fin({(unsigned)i});
      }
    return res;
  }

  acc_cond::acc_code
  acc_cond::acc_code::random(unsigned n_accs, double reuse)
  {
    // With 0 acceptance sets, we always generate the true acceptance.
    // (Working with false is somehow pointless, and the formulas we
    // generate for n_accs>0 are always satisfiable, so it makes sense
    // that it should be satisfiable for n_accs=0 as well.)
    if (n_accs == 0)
      return {};

    std::vector<acc_cond::acc_code> codes;
    codes.reserve(n_accs);
    for (unsigned i = 0; i < n_accs; ++i)
      {
        codes.emplace_back(drand() < 0.5 ? inf({i}) : fin({i}));
        if (reuse > 0.0 && drand() < reuse)
          --i;
      }

    int s = codes.size();
    while (s > 1)
      {
        // Pick a random code and put it at the end
        int p1 = mrand(s--);
        if (p1 != s) // https://gcc.gnu.org/bugzilla//show_bug.cgi?id=59603
          std::swap(codes[p1], codes[s]);
        // and another one
        int p2 = mrand(s);

        if (drand() < 0.5)
          codes[p2] |= std::move(codes.back());
        else
          codes[p2] &= std::move(codes.back());

        codes.pop_back();
      }
    return codes[0];
  }

  namespace
  {
    bdd to_bdd_rec(const acc_cond::acc_word* c, const bdd* map)
    {
      auto sz = c->sub.size;
      auto start = c - sz - 1;
      auto op = c->sub.op;
      switch (op)
        {
        case acc_cond::acc_op::Or:
          {
            --c;
            bdd res = bddfalse;
            do
              {
                res |= to_bdd_rec(c, map);
                c -= c->sub.size + 1;
              }
            while (c > start);
            return res;
          }
        case acc_cond::acc_op::And:
          {
            --c;
            bdd res = bddtrue;
            do
              {
                res &= to_bdd_rec(c, map);
                c -= c->sub.size + 1;
              }
            while (c > start);
            return res;
          }
        case acc_cond::acc_op::Fin:
          {
            bdd res = bddfalse;
            for (auto i: c[-1].mark.sets())
              res |= !map[i];
            return res;
          }
        case acc_cond::acc_op::Inf:
          {
            bdd res = bddtrue;
            for (auto i: c[-1].mark.sets())
              res &= map[i];
            return res;
          }
        case acc_cond::acc_op::InfNeg:
        case acc_cond::acc_op::FinNeg:
          SPOT_UNREACHABLE();
          return bddfalse;
        }
      SPOT_UNREACHABLE();
      return bddfalse;
    }

    static bool
    equiv_codes(const acc_cond::acc_code& lhs,
                const acc_cond::acc_code& rhs)
    {
      auto used = lhs.used_sets() | rhs.used_sets();
      unsigned c = used.count();
      unsigned umax = used.max_set();

      bdd_allocator ba;
      int base = ba.allocate_variables(c);
      assert(base == 0);
      std::vector<bdd> r;
      for (unsigned i = 0; r.size() < umax; ++i)
        if (used.has(i))
          r.emplace_back(bdd_ithvar(base++));
        else
          r.emplace_back(bddfalse);
      return to_bdd_rec(&lhs.back(), &r[0]) == to_bdd_rec(&rhs.back(), &r[0]);
    }
  }

  std::vector<unsigned>
  acc_cond::acc_code::symmetries() const
  {
    auto used = used_sets();
    unsigned umax = used.max_set();

    bdd_allocator ba;
    int base = ba.allocate_variables(umax+2);
    assert(base == 0);
    std::vector<bdd> r;
    for (unsigned i = 0; r.size() < umax; ++i)
      r.emplace_back(bdd_ithvar(base++));
    bdd bddcode = to_bdd_rec(&back(), &r[0]);
    bdd tmp;

    std::vector<unsigned> res(umax);
    for (unsigned x = 0; x < umax; ++x)
      res[x] = x;

    for (unsigned x : used.sets())
      for (unsigned y : used.sets())
        {
          if (x >= y)
            continue;
          if (res[x] != x)
            continue;

          {
            bddPair* p = bdd_newpair();
            bdd_setpair(p, x, umax + 0);
            bdd_setpair(p, y, umax + 1);
            tmp = bdd_replace(bddcode, p);
            bdd_freepair(p);

            p = bdd_newpair();
            bdd_setpair(p, umax + 0, y);
            bdd_setpair(p, umax + 1, x);
            tmp = bdd_replace(tmp, p);
            bdd_freepair(p);
          }

          if (tmp == bddcode)
            res[y] = x;
        }

    return res;
  }

  namespace
  {
    bool
    has_parity_prefix_aux(acc_cond original,
                          acc_cond &new_cond,
                          std::vector<unsigned> &colors,
                          std::vector<acc_cond::acc_code> elements,
                          acc_cond::acc_op op)
    {
      if (elements.size() > 2)
        {
          new_cond = original;
          return false;
        }
      if (elements.size() == 2)
        {
          unsigned pos = (elements[1].back().sub.op == op
                          && elements[1][0].mark.is_singleton());
          if (!(elements[0].back().sub.op == op || pos))
            {
              new_cond = original;
              return false;
            }
          if ((elements[1 - pos].used_sets() & elements[pos][0].mark))
            {
              new_cond = original;
              return false;
            }
          if (!elements[pos][0].mark.is_singleton())
            {
              return false;
            }
          colors.push_back(elements[pos][0].mark.min_set() - 1);
          elements[1 - pos].has_parity_prefix(new_cond, colors);
          return true;
        }
      return false;
    }
  }

  bool
  acc_cond::acc_code::has_parity_prefix(acc_cond &new_cond,
                                        std::vector<unsigned> &colors) const
  {
    auto disj = top_disjuncts();
    if (!(has_parity_prefix_aux((*this), new_cond, colors,
            disj, acc_cond::acc_op::Inf) ||
          has_parity_prefix_aux((*this), new_cond, colors,
            top_conjuncts(), acc_cond::acc_op::Fin)))
      new_cond = acc_cond((*this));
    return disj.size() == 2;
  }

  bool
  acc_cond::has_parity_prefix(acc_cond& new_cond,
                              std::vector<unsigned>& colors) const
  {
    return code_.has_parity_prefix(new_cond, colors);
  }

  bool
  acc_cond::is_parity_max_equiv(std::vector<int>&permut, bool even) const
  {
    if (code_.used_once_sets() != code_.used_sets())
      return false;
    bool result = code_.is_parity_max_equiv(permut, 0, even);
    int max_value = *std::max_element(std::begin(permut), std::end(permut));
    for (unsigned i = 0; i < permut.size(); ++i)
      if (permut[i] != -1)
        permut[i] = max_value - permut[i];
      else
        permut[i] = i;
    return result;
  }

  bool acc_cond::is_parity(bool& max, bool& odd, bool equiv) const
  {
    unsigned sets = num_;
    if (sets == 0)
      {
        max = true;
        odd = is_t();
        return true;
      }
    acc_cond::mark_t u_inf;
    acc_cond::mark_t u_fin;
    std::tie(u_inf, u_fin) = code_.used_inf_fin_sets();

    odd = !u_inf.has(0);
    for (auto s: u_inf.sets())
      if ((s & 1) != odd)
        {
          max = false;             // just so the value is not uninitialized
          return false;
        }

    auto max_code = acc_code::parity(true, odd, sets);
    if (max_code == code_)
      {
        max = true;
        return true;
      }
    auto min_code = acc_code::parity(false, odd, sets);
    if (min_code == code_)
      {
        max = false;
        return true;
      }

    if (!equiv)
      {
        max = false;
        return false;
      }

    if (equiv_codes(code_, max_code))
      {
        max = true;
        return true;
      }
    if (equiv_codes(code_, min_code))
      {
        max = false;
        return true;
      }
    max = false;
    return false;
  }

  acc_cond::acc_code acc_cond::acc_code::to_dnf() const
  {
    if (empty() || size() == 2)
      return *this;

    auto used = acc_cond::acc_code::used_sets();
    unsigned c = used.count();
    unsigned max = used.max_set();

    bdd_allocator ba;
    int base = ba.allocate_variables(c);
    assert(base == 0);
    std::vector<bdd> r;
    std::vector<unsigned> sets(c);
    for (unsigned i = 0; r.size() < max; ++i)
      {
        if (used.has(i))
          {
            sets[base] = i;
            r.emplace_back(bdd_ithvar(base++));
          }
        else
          {
            r.emplace_back(bddfalse);
          }
      }

    bdd res = to_bdd_rec(&back(), &r[0]);

    if (res == bddtrue)
      return t();
    if (res == bddfalse)
      return f();

    minato_isop isop(res);
    bdd cube;
    acc_code rescode = f();
    while ((cube = isop.next()) != bddfalse)
      {
        mark_t i = {};
        acc_code f;
        while (cube != bddtrue)
          {
            // The acceptance set associated to this BDD variable
            mark_t s = {};
            s.set(sets[bdd_var(cube)]);

            bdd h = bdd_high(cube);
            if (h == bddfalse)        // Negative variable? -> Fin
              {
                cube = bdd_low(cube);
                // The strange order here make sure we can smaller set
                // numbers at the end of the acceptance code, i.e., at
                // the front of the output.
                f = fin(s) & f;
              }
            else                // Positive variable? -> Inf
              {
                i |= s;
                cube = h;
              }
          }
        // See comment above for the order.
        rescode = (inf(i) & f) | rescode;
      }

    return rescode;
  }

  acc_cond::acc_code acc_cond::acc_code::to_cnf() const
  {
    if (empty() || size() == 2)
      return *this;

    auto used = acc_cond::acc_code::used_sets();
    unsigned c = used.count();
    unsigned max = used.max_set();

    bdd_allocator ba;
    int base = ba.allocate_variables(c);
    assert(base == 0);
    std::vector<bdd> r;
    std::vector<unsigned> sets(c);
    for (unsigned i = 0; r.size() < max; ++i)
      {
        if (used.has(i))
          {
            sets[base] = i;
            r.emplace_back(bdd_ithvar(base++));
          }
        else
          {
            r.emplace_back(bddfalse);
          }
      }

    bdd res = to_bdd_rec(&back(), &r[0]);

    if (res == bddtrue)
      return t();
    if (res == bddfalse)
      return f();

    minato_isop isop(!res);
    bdd cube;
    acc_code rescode;
    while ((cube = isop.next()) != bddfalse)
      {
        mark_t m = {};
        acc_code i = f();
        while (cube != bddtrue)
          {
            // The acceptance set associated to this BDD variable
            mark_t s = {};
            s.set(sets[bdd_var(cube)]);

            bdd h = bdd_high(cube);
            if (h == bddfalse)        // Negative variable? -> Inf
              {
                cube = bdd_low(cube);
                // The strange order here make sure we can smaller set
                // numbers at the end of the acceptance code, i.e., at
                // the front of the output.
                i = inf(s) | i;
              }
            else                // Positive variable? -> Fin
              {
                m |= s;
                cube = h;
              }
          }
        // See comment above for the order.
        rescode = (fin(m) | i) & rescode;
      }
    return rescode;
  }

  bool
  acc_cond::acc_code::is_parity_max_equiv(std::vector<int>& permut,
                                          unsigned new_color,
                                          bool even) const
  {
    auto conj = top_conjuncts();
    auto disj = top_disjuncts();
    if (conj.size() == 1)
      {
        if (disj.size() == 1)
          {
            acc_cond::acc_code elem = conj[0];
            if ((even && elem.back().sub.op == acc_cond::acc_op::Inf)
                || (!even && elem.back().sub.op == acc_cond::acc_op::Fin))
              {
                for (auto color : disj[0][0].mark.sets())
                  {
                    if (permut[color] != -1
                        && ((unsigned) permut[color]) != new_color)
                      return false;
                    permut[color] = new_color;
                  }
                return true;
              }
            return false;
          }
        else
          {
            std::sort(disj.begin(), disj.end(),
                      [](acc_code c1, acc_code c2)
                      {
                        return (c1 != c2) &&
                          c1.back().sub.op == acc_cond::acc_op::Inf;
                      });
            unsigned i = 0;
            for (; i < disj.size() - 1; ++i)
              {
                if (disj[i].back().sub.op != acc_cond::acc_op::Inf
                    || !disj[i][0].mark.is_singleton())
                  return false;
                for (auto color : disj[i][0].mark.sets())
                  {
                    if (permut[color] != -1
                        && ((unsigned) permut[color]) != new_color)
                      return false;
                    permut[color] = new_color;
                  }
              }
            if (disj[i].back().sub.op == acc_cond::acc_op::Inf)
              {
                if (!even || !disj[i][0].mark.is_singleton())
                  return false;
                for (auto color : disj[i][0].mark.sets())
                  {
                    if (permut[color] != -1
                        && ((unsigned) permut[color]) != new_color)
                      return false;
                    permut[color] = new_color;
                  }
                return true;
              }
            return disj[i].is_parity_max_equiv(permut, new_color + 1, even);
          }
      }
    else
      {
        std::sort(conj.begin(), conj.end(),
                  [](acc_code c1, acc_code c2)
                  {
                    return (c1 != c2)
                      && c1.back().sub.op == acc_cond::acc_op::Fin;
                  });
        unsigned i = 0;
        for (; i < conj.size() - 1; i++)
          {
            if (conj[i].back().sub.op != acc_cond::acc_op::Fin
                || !conj[i][0].mark.is_singleton())
              return false;
            for (auto color : conj[i][0].mark.sets())
              {
                if (permut[color] != -1 && permut[color != new_color])
                  return false;
                permut[color] = new_color;
              }
          }
        if (conj[i].back().sub.op == acc_cond::acc_op::Fin)
          {
            if (even)
              return 0;
            if (!conj[i][0].mark.is_singleton())
              return false;
            for (auto color : conj[i][0].mark.sets())
              {
                if (permut[color] != -1 && permut[color != new_color])
                  return false;
                permut[color] = new_color;
              }
            return true;
          }

        return conj[i].is_parity_max_equiv(permut, new_color + 1, even);
      }
  }


  namespace
  {
    template<typename Fun>
    static void
    top_clauses(const acc_cond::acc_code& code,
                acc_cond::acc_op connect, acc_cond::acc_op inf_fin,
                Fun store)
    {
      if (!code.empty())
        {
          auto pos = &code.back();
          auto start = &code.front();
          assert(pos - pos->sub.size == start);
          if (pos->sub.op == connect)
            {
              do
                {
                  --pos;
                  if (pos->sub.op == inf_fin)
                    {
                      for (unsigned d: pos[-1].mark.sets())
                        {
                          acc_cond::acc_code tmp;
                          tmp.resize(2);
                          tmp[0].mark = {d};
                          tmp[1].sub.op = inf_fin;
                          tmp[1].sub.size = 1;
                          store(tmp);
                        }
                    }
                  else
                    {
                      store(pos);
                    }
                  pos -= pos->sub.size;
                }
              while (pos > start);
              return;
            }
          if (pos->sub.op == inf_fin)
            {
              for (unsigned d: pos[-1].mark.sets())
                {
                  acc_cond::acc_code tmp;
                  tmp.resize(2);
                  tmp[0].mark = {d};
                  tmp[1].sub.op = inf_fin;
                  tmp[1].sub.size = 1;
                  store(tmp);
                }
              return;
            }
        }
      store(code);
      return;
    }
  }

  std::vector<acc_cond::acc_code> acc_cond::acc_code::top_disjuncts() const
  {
    std::vector<acc_cond::acc_code> res;
    top_clauses(*this, acc_cond::acc_op::Or, acc_cond::acc_op::Fin,
                [&](const acc_cond::acc_code& c) { res.emplace_back(c); });
    return res;
  }

  std::vector<acc_cond::acc_code> acc_cond::acc_code::top_conjuncts() const
  {
    std::vector<acc_cond::acc_code> res;
    top_clauses(*this, acc_cond::acc_op::And, acc_cond::acc_op::Inf,
                [&](const acc_cond::acc_code& c) { res.emplace_back(c); });
    return res;
  }

  std::vector<acc_cond> acc_cond::top_disjuncts() const
  {
    std::vector<acc_cond> res;
    top_clauses(code_, acc_cond::acc_op::Or, acc_cond::acc_op::Fin,
                [&](const acc_cond::acc_code& c)
                { res.emplace_back(num_, c); });
    return res;
  }

  std::vector<acc_cond> acc_cond::top_conjuncts() const
  {
    std::vector<acc_cond> res;
    top_clauses(code_, acc_cond::acc_op::And, acc_cond::acc_op::Inf,
                [&](const acc_cond::acc_code& c)
                { res.emplace_back(num_, c); });
    return res;
  }

  std::pair<bool, acc_cond::mark_t>
  acc_cond::sat_unsat_mark(bool sat) const
  {
    if (sat)
      {
        if (is_f())
          return {false, mark_t({})};
        if (!uses_fin_acceptance())
          return {true, all_sets()};
      }
    else
      {
        if (is_t())
          return {false, mark_t({})};
        if (!uses_fin_acceptance())
          return {true, mark_t({})};
      }

    auto used = code_.used_sets();
    unsigned c = used.count();
    unsigned max = used.max_set();

    bdd_allocator ba;
    int base = ba.allocate_variables(c);
    assert(base == 0);
    std::vector<bdd> r;
    std::vector<unsigned> sets(c);
    for (unsigned i = 0; r.size() < max; ++i)
      {
        if (used.has(i))
          {
            sets[base] = i;
            r.emplace_back(bdd_ithvar(base++));
          }
        else
          {
            r.emplace_back(bddfalse);
          }
      }

    bdd res = to_bdd_rec(&code_.back(), &r[0]);

    if (res == bddtrue)
      return {sat, mark_t({})};
    if (res == bddfalse)
      return {!sat, mark_t({})};

    bdd cube = bdd_satone(sat ? res : !res);
    mark_t i = {};
    while (cube != bddtrue)
      {
        // The acceptance set associated to this BDD variable
        int s = sets[bdd_var(cube)];

        bdd h = bdd_high(cube);
        if (h == bddfalse)        // Negative variable? -> skip
          {
            cube = bdd_low(cube);
          }
        else                // Positive variable? -> Inf
          {
            i.set(s);
            cube = h;
          }
      }
    return {true, i};
  }

  std::vector<std::vector<int>>
  acc_cond::acc_code::missing(mark_t inf, bool accepting) const
  {
    if (empty())
      {
        if (accepting)
          return {};
        else
          return {
            {}
          };
      }
    auto used = acc_cond::acc_code::used_sets();
    unsigned c = used.count();
    unsigned max = used.max_set();

    bdd_allocator ba;
    int base = ba.allocate_variables(c);
    assert(base == 0);
    std::vector<bdd> r;
    std::vector<unsigned> sets(c);
    bdd known = bddtrue;
    for (unsigned i = 0; r.size() < max; ++i)
      {
        if (used.has(i))
          {
            sets[base] = i;
            bdd v = bdd_ithvar(base++);
            r.emplace_back(v);
            if (inf.has(i))
              known &= v;
          }
        else
          {
            r.emplace_back(bddfalse);
          }
      }

    bdd res = to_bdd_rec(&back(), &r[0]);

    res = bdd_restrict(res, known);

    if (accepting)
      res = !res;

    if (res == bddfalse)
      return {};

    minato_isop isop(res);
    bdd cube;
    std::vector<std::vector<int>> result;
    while ((cube = isop.next()) != bddfalse)
      {
        std::vector<int> partial;
        while (cube != bddtrue)
          {
            // The acceptance set associated to this BDD variable
            int s = sets[bdd_var(cube)];

            bdd h = bdd_high(cube);
            if (h == bddfalse)        // Negative variable
              {
                partial.emplace_back(s);
                cube = bdd_low(cube);
              }
            else                // Positive variable
              {
                partial.emplace_back(-s - 1);
                cube = h;
              }
          }
        result.emplace_back(std::move(partial));
      }
    return result;
  }

  bool acc_cond::acc_code::is_dnf() const
  {
    if (empty() || size() == 2)
      return true;
    auto pos = &back();
    auto start = &front();
    auto and_scope = pos + 1;
    if (pos->sub.op == acc_cond::acc_op::Or)
      --pos;
    while (pos > start)
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::Or:
            return false;
          case acc_cond::acc_op::And:
            and_scope = std::min(and_scope, pos - pos->sub.size);
            --pos;
            break;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::FinNeg:
            if (pos[-1].mark.has_many() && pos > and_scope)
              return false;
            SPOT_FALLTHROUGH;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            pos -= 2;
            break;
          }
      }
    return true;
  }

  bool acc_cond::acc_code::is_cnf() const
  {
    if (empty() || size() == 2)
      return true;
    auto pos = &back();
    auto start = &front();
    auto or_scope = pos + 1;
    if (pos->sub.op == acc_cond::acc_op::And)
      --pos;
    while (pos > start)
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
            return false;
          case acc_cond::acc_op::Or:
            or_scope = std::min(or_scope, pos - pos->sub.size);
            --pos;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            if (pos[-1].mark.has_many() && pos > or_scope)
              return false;
            SPOT_FALLTHROUGH;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::FinNeg:
            pos -= 2;
            break;
          }
      }
    return true;
  }

  namespace
  {
    acc_cond::acc_code complement_rec(const acc_cond::acc_word* pos)
    {
      auto start = pos - pos->sub.size;
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            --pos;
            auto res = acc_cond::acc_code::f();
            do
              {
                auto tmp = complement_rec(pos) | std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Or:
          {
            --pos;
            auto res = acc_cond::acc_code::t();
            do
              {
                auto tmp = complement_rec(pos) & std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Fin:
          return acc_cond::acc_code::inf(pos[-1].mark);
        case acc_cond::acc_op::Inf:
          return acc_cond::acc_code::fin(pos[-1].mark);
        case acc_cond::acc_op::FinNeg:
          return acc_cond::acc_code::inf_neg(pos[-1].mark);
        case acc_cond::acc_op::InfNeg:
          return acc_cond::acc_code::fin_neg(pos[-1].mark);
        }
      SPOT_UNREACHABLE();
      return {};
    }
  }

  acc_cond::acc_code acc_cond::acc_code::complement() const
  {
    if (is_t())
      return acc_cond::acc_code::f();
    return complement_rec(&back());
  }

  namespace
  {
    static acc_cond::acc_code
    strip_rec(const acc_cond::acc_word* pos, acc_cond::mark_t rem, bool missing,
              bool strip)
    {
      auto start = pos - pos->sub.size;
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            --pos;
            auto res = acc_cond::acc_code::t();
            do
              {
                auto tmp = strip_rec(pos, rem, missing, strip) & std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Or:
          {
            --pos;
            auto res = acc_cond::acc_code::f();
            do
              {
                auto tmp = strip_rec(pos, rem, missing, strip) | std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Fin:
          if (missing && (pos[-1].mark & rem))
            return acc_cond::acc_code::t();
          return acc_cond::acc_code::fin(strip
                                         ? pos[-1].mark.strip(rem)
                                         : pos[-1].mark - rem);
        case acc_cond::acc_op::Inf:
          if (missing && (pos[-1].mark & rem))
            return acc_cond::acc_code::f();
          return acc_cond::acc_code::inf(strip
                                         ? pos[-1].mark.strip(rem)
                                         : pos[-1].mark - rem);
        case acc_cond::acc_op::FinNeg:
        case acc_cond::acc_op::InfNeg:
          SPOT_UNREACHABLE();
          return {};
        }
      SPOT_UNREACHABLE();
      return {};
    }

    static acc_cond::acc_code
    force_inf_rec(const acc_cond::acc_word* pos, acc_cond::mark_t rem)
    {
      auto start = pos - pos->sub.size;
      switch (pos->sub.op)
        {
        case acc_cond::acc_op::And:
          {
            --pos;
            auto res = acc_cond::acc_code::t();
            do
              {
                auto tmp = force_inf_rec(pos, rem) & std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Or:
          {
            --pos;
            auto res = acc_cond::acc_code::f();
            do
              {
                auto tmp = force_inf_rec(pos, rem) | std::move(res);
                std::swap(tmp, res);
                pos -= pos->sub.size + 1;
              }
            while (pos > start);
            return res;
          }
        case acc_cond::acc_op::Fin:
          return acc_cond::acc_code::fin(pos[-1].mark - rem);
        case acc_cond::acc_op::Inf:
          return acc_cond::acc_code::inf(pos[-1].mark);
        case acc_cond::acc_op::FinNeg:
        case acc_cond::acc_op::InfNeg:
          SPOT_UNREACHABLE();
          return {};
        }
      SPOT_UNREACHABLE();
      return {};
    }
  }

  acc_cond::acc_code
  acc_cond::acc_code::strip(acc_cond::mark_t rem, bool missing) const
  {
    if (is_t() || is_f())
      return *this;
    return strip_rec(&back(), rem, missing, true);
  }

  acc_cond::acc_code
  acc_cond::acc_code::remove(acc_cond::mark_t rem, bool missing) const
  {
    if (is_t() || is_f())
      return *this;
    return strip_rec(&back(), rem, missing, false);
  }

  acc_cond::acc_code
  acc_cond::acc_code::force_inf(mark_t rem) const
  {
    if (is_t() || is_f())
      return *this;
    return force_inf_rec(&back(), rem);
  }

  acc_cond::mark_t
  acc_cond::acc_code::used_sets() const
  {
    if (is_t() || is_f())
      return {};
    acc_cond::mark_t used_in_cond = {};
    auto pos = &back();
    auto end = &front();
    while (pos > end)
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::FinNeg:
          case acc_cond::acc_op::InfNeg:
            used_in_cond |= pos[-1].mark;
            pos -= 2;
            break;
          }
      }
    return used_in_cond;
  }

  std::pair<acc_cond::mark_t, acc_cond::mark_t>
  acc_cond::acc_code::used_inf_fin_sets() const
  {
    if (is_t() || is_f())
      return {mark_t({}), mark_t({})};

    acc_cond::mark_t used_fin = {};
    acc_cond::mark_t used_inf = {};
    auto pos = &back();
    auto end = &front();
    while (pos > end)
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::FinNeg:
            used_fin |= pos[-1].mark;
            pos -= 2;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            used_inf |= pos[-1].mark;
            pos -= 2;
            break;
          }
      }
    return {used_inf, used_fin};
  }

  std::ostream&
  acc_cond::acc_code::to_html(std::ostream& os,
                              std::function<void(std::ostream&, int)>
                              set_printer) const
  {
    if (empty())
      os << 't';
    else
      print_code<HTML>(os, *this, size() - 1,
                       set_printer ? set_printer : default_set_printer);
    return os;
  }

  std::ostream&
  acc_cond::acc_code::to_text(std::ostream& os,
                              std::function<void(std::ostream&, int)>
                              set_printer) const
  {
    if (empty())
      os << 't';
    else
      print_code<TEXT>(os, *this, size() - 1,
                       set_printer ? set_printer : default_set_printer);
    return os;
  }

  std::ostream&
  acc_cond::acc_code::to_latex(std::ostream& os,
                              std::function<void(std::ostream&, int)>
                              set_printer) const
  {
    if (empty())
      os << "\\mathsf{t}";
    else
      print_code<LATEX>(os, *this, size() - 1,
                        set_printer ? set_printer : default_set_printer);
    return os;
  }


  std::ostream& operator<<(std::ostream& os,
                           const spot::acc_cond::acc_code& code)
  {
    return code.to_text(os);
  }


  /// Return the name of this acceptance condition, in the
  /// specified format.
  std::string acc_cond::name(const char* fmt) const
  {
    bool accentuated = false;
    bool no_extra_param = false;
    bool no_main_param = false;
    bool no_parity_param = false;
    bool abbreviate = false;
    bool like_names = false;
    bool other = false;

    if (fmt)
      while (*fmt)
        switch (char c = *fmt++)
          {
          case '0':               // no param style
            no_extra_param = no_main_param = no_parity_param = true;
            break;
          case 'a':
            accentuated = true;
            break;
          case 'b':
            abbreviate = true;
            break;
          case 'd':               // print_dot() style
            accentuated = no_extra_param = abbreviate = like_names = true;
            break;
          case 'g':
            no_extra_param = true;
            break;
          case 'l':
            like_names = true;
            break;
          case 'm':
            no_main_param = true;
            break;
          case 'p':
            no_parity_param = true;
            break;
          case 'o':
            other = true;
            break;
          case 's':
            other = no_extra_param = no_main_param = no_parity_param =
              like_names = true;
            break;
          default:
            throw std::runtime_error
              ("unknown option for acc_cond::name(): "s + c);
          }

    std::ostringstream os;

    auto gen = [abbreviate]()
      {
        return abbreviate ? "gen. " : "generalized-";
      };
    auto sets = [no_main_param, this]() -> std::string
      {
        return no_main_param ? "" : " " + std::to_string(num_sets());
      };

    if (is_generalized_buchi())
      {
        if (is_all())
          os << "all";
        else if (is_buchi())
          os << (accentuated ? "Büchi" : "Buchi");
        else
          os << gen() << (accentuated ? "Büchi" : "Buchi") << sets();
      }
    else if (is_generalized_co_buchi())
      {
        if (is_none())
          os << "none";
        else if (is_co_buchi())
          os << (accentuated ? "co-Büchi" : "co-Buchi");
        else
          os << gen() << (accentuated ? "co-Büchi" : "co-Buchi") << sets();
      }
    else
      {
        int r = is_rabin();
        assert(r != 0);
        if (r > 0)
          {
            os << "Rabin";
            if (!no_main_param)
              os << ' ' << r;
          }
        else
          {
            r = is_streett();
            assert(r != 0);
            if (r > 0)
              {
                os << "Streett";
                if (!no_main_param)
                  os << ' ' << r;
              }
            else
              {
                std::vector<unsigned> pairs;
                if (is_generalized_rabin(pairs))
                  {
                    os << gen() << "Rabin";
                    if (!no_main_param)
                      {
                        os << ' ' << pairs.size();
                        if (!no_extra_param)
                          for (auto p: pairs)
                            os << ' ' << p;
                      }
                  }
                else if (is_generalized_streett(pairs))
                  {
                    os << gen() << "Streett";
                    if (!no_main_param)
                      {
                        os << ' ' << pairs.size();
                        if (!no_extra_param)
                          for (auto p: pairs)
                            os << ' ' << p;
                      }
                  }
                else
                  {
                    bool max = false;
                    bool odd = false;
                    if (is_parity(max, odd))
                      {
                        os << "parity";
                        if (!no_parity_param)
                          os << (max ? " max" : " min")
                             << (odd ? " odd" : " even");
                        os << sets();
                      }
                    else if (like_names)
                      {
                        if (!uses_fin_acceptance())
                          {
                            os << "Fin-less" << sets();
                          }
                        else
                          {
                            std::vector<acc_cond::rs_pair> r_pairs;
                            bool r_like = is_rabin_like(r_pairs);
                            unsigned rsz = r_pairs.size();
                            std::vector<acc_cond::rs_pair> s_pairs;
                            bool s_like = is_streett_like(s_pairs);
                            unsigned ssz = s_pairs.size();
                            if (r_like && (!s_like || (rsz <= ssz)))
                              {
                                os << "Rabin-like";
                                if (!no_main_param)
                                  os << ' ' << rsz;
                              }
                            else if (s_like && (!r_like || (ssz < rsz)))
                              {
                                os << "Streett-like";
                                if (!no_main_param)
                                  os << ' ' << ssz;
                              }
                          }
                      }
                  }
              }
          }
      }
    std::string res = os.str();
    if (other && res.empty())
      res = "other" + sets();
    return res;
  }


  namespace
  {

    static void
    syntax_error(const char* input, const char* message)
    {
      std::ostringstream s;
      s << "syntax error at ";
      if (*input)
        s << '\'' << input << "': ";
      else
        s << "end of acceptance: ";
      s << message;
      throw parse_error(s.str());
    }

    /// acc ::= term | term "|" acc
    /// term := "t" | "f" | "Inf" "(" num ")"
    ///       | "Fin" "(" num ") " | "(" acc ")
    ///       | term "&" term

    static void skip_space(const char*& input)
    {
      while (std::isspace(*input))
        ++input;
    }

    // Eat c and remove the following spaces.
    // Complain if there is no c.
    void expect(const char*& input, char c)
    {
      if (*input != c)
        {
          char msg[20];
          sprintf(msg, "was expecting %c '.'", c);
          syntax_error(input, msg);
        }
      ++input;
      skip_space(input);
    }

    static acc_cond::acc_code parse_term(const char*& input);

    static acc_cond::acc_code parse_acc(const char*& input)
    {
      auto res = parse_term(input);
      skip_space(input);
      while (*input == '|')
        {
          ++input;
          skip_space(input);
          // Prepend instead of append, to preserve the input order.
          auto tmp = parse_term(input);
          std::swap(tmp, res);
          res |= std::move(tmp);
        }
      return res;
    }

    static unsigned parse_num(const char*& input)
    {

      errno = 0;
      char* end;
      unsigned long n = strtoul(input, &end, 10);
      unsigned num = n;
      if (errno || num != n)
        syntax_error(input, "invalid number.");
      input = end;
      return n;
    }

    static unsigned parse_range(const char*& str)
    {
      skip_space(str);
      int min;
      int max;
      char* end;
      errno = 0;
      min = strtol(str, &end, 10);
      if (end == str || errno)
        {
          // No leading number.  It's OK as long as '..' or ':' are next.
          if (errno || (*end != ':' && *end != '.'))
            syntax_error(str, "invalid range.");
          min = 1;
        }
      if (!*end || (*end != ':' && *end != '.'))
        {
          // Only one number.
          max = min;
        }
      else
        {
          // Skip : or ..
          if (end[0] == ':')
            ++end;
          else if (end[0] == '.' && end[1] == '.')
            end += 2;

          // Parse the next integer.
          char* end2;
          max = strtol(end, &end2, 10);
          if (end == end2 || errno)
            syntax_error(str, "invalid range (missing end?)");
          end = end2;
        }

      if (min < 0 || max < 0)
        syntax_error(str, "values in range must be positive.");

      str = end;

      if (min == max)
        return min;

      if (min > max)
        std::swap(min, max);
      return spot::rrand(min, max);
    }

    static unsigned parse_par_num(const char*& input)
    {
      skip_space(input);
      expect(input, '(');
      unsigned num = parse_num(input);
      skip_space(input);
      expect(input, ')');
      return num;
    }

    static double parse_proba(const char*& input)
    {
      errno = 0;
      char* end;
      double n = strtod(input, &end);
      if (errno)
        syntax_error(input, "cannot convert to double.");
      if (!(n >= 0.0 && n <= 1.0))
        syntax_error(input, "value should be between 0 and 1.");
      input = end;
      return n;
    }

    static acc_cond::acc_code parse_term(const char*& input)
    {
      acc_cond::acc_code res;
      if (*input == 't')
        {
          ++input;
          res = acc_cond::acc_code::t();
        }
      else if (*input == 'f')
        {
          ++input;
          res = acc_cond::acc_code::f();
        }
      else if (*input == '(')
        {
          ++input;
          skip_space(input);
          res = parse_acc(input);
          skip_space(input);
          expect(input, ')');
        }
      else if (!strncmp(input, "Inf", 3))
        {
          input += 3;
          res = acc_cond::acc_code::inf({parse_par_num(input)});
        }
      else if (!strncmp(input, "Fin", 3))
        {
          input += 3;
          res = acc_cond::acc_code::fin({parse_par_num(input)});
        }
      else
        {
          syntax_error(input, "unexpected character.");
        }

      skip_space(input);
      while (*input == '&')
        {
          ++input;
          skip_space(input);
          // Prepend instead of append, to preserve the input order.
          auto tmp = parse_term(input);
          std::swap(tmp, res);
          res &= std::move(tmp);
        }
      return res;
    }

    static bool max_or_min(const char*& input)
    {
      skip_space(input);
      if (!strncmp(input, "max", 3))
        {
          input += 3;
          return true;
        }
      if (!strncmp(input, "min", 3))
        {
          input += 3;
          return false;
        }
      if (!strncmp(input, "rand", 4))
        {
          input += 4;
          return drand() < 0.5;
        }
      if (!strncmp(input, "random", 6))
        {
          input += 6;
          return drand() < 0.5;
        }
      syntax_error(input, "expecting 'min', 'max', or 'rand'.");
      SPOT_UNREACHABLE();
      return false;
    }

    static bool odd_or_even(const char*& input)
    {
      skip_space(input);
      if (!strncmp(input, "odd", 3))
        {
          input += 3;
          return true;
        }
      if (!strncmp(input, "even", 4))
        {
          input += 4;
          return false;
        }
      if (!strncmp(input, "rand", 4))
        {
          input += 4;
          return drand() < 0.5;
        }
      if (!strncmp(input, "random", 6))
        {
          input += 6;
          return drand() < 0.5;
        }
      syntax_error(input, "expecting 'odd', 'even', or 'rand'.");
      SPOT_UNREACHABLE();
      return false;
    }

  }

  acc_cond::acc_code::acc_code(const char* input)
  {
    skip_space(input);
    acc_cond::acc_code c;
    if (!strncmp(input, "all", 3))
      {
        input += 3;
        c = acc_cond::acc_code::t();
      }
    else if (!strncmp(input, "none", 4))
      {
        input += 4;
        c = acc_cond::acc_code::f();
      }
    else if (!strncmp(input, "Buchi", 5))
      {
        input += 5;
        c = acc_cond::acc_code::buchi();
      }
    else if (!strncmp(input, "co-Buchi", 8))
      {
        input += 8;
        c = acc_cond::acc_code::cobuchi();
      }
    else if (!strncmp(input, "generalized-Buchi", 17))
      {
        input += 17;
        c = acc_cond::acc_code::generalized_buchi(parse_range(input));
      }
    else if (!strncmp(input, "generalized-co-Buchi", 20))
      {
        input += 20;
        c = acc_cond::acc_code::generalized_co_buchi(parse_range(input));
      }
    else if (!strncmp(input, "Rabin", 5))
      {
        input += 5;
        c = acc_cond::acc_code::rabin(parse_range(input));
      }
    else if (!strncmp(input, "Streett", 7))
      {
        input += 7;
        c = acc_cond::acc_code::streett(parse_range(input));
      }
    else if (!strncmp(input, "generalized-Rabin", 17))
      {
        input += 17;
        unsigned num = parse_num(input);
        std::vector<unsigned> v;
        v.reserve(num);
        while (num > 0)
          {
            v.emplace_back(parse_range(input));
            --num;
          }
        c = acc_cond::acc_code::generalized_rabin(v.begin(), v.end());
      }
    else if (!strncmp(input, "parity", 6))
      {
        input += 6;
        bool max = max_or_min(input);
        bool odd = odd_or_even(input);
        unsigned num = parse_range(input);
        c = acc_cond::acc_code::parity(max, odd, num);
      }
    else if (!strncmp(input, "random", 6))
      {
        input += 6;
        unsigned n = parse_range(input);
        skip_space(input);
        auto setreuse = input;
        double reuse = (*input) ? parse_proba(input) : 0.0;
        if (reuse >= 1.0)
          syntax_error(setreuse, "probability for set reuse should be <1.");
        c = acc_cond::acc_code::random(n, reuse);
      }
    else
      {
        c = parse_acc(input);
      }
    skip_space(input);
    if (*input)
      syntax_error(input, "unexpected character.");

    std::swap(c, *this);
  }

  int acc_cond::acc_code::fin_one() const
  {
    if (empty() || is_f())
      return -1;
    const acc_cond::acc_word* pos = &back();
    do
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
          case acc_cond::acc_op::FinNeg:
            pos -= 2;
            break;
          case acc_cond::acc_op::Fin:
            return pos[-1].mark.min_set() - 1;
          }
      }
    while (pos >= &front());
    return -1;
  }

  namespace
  {
    bool
    find_unit_clause(acc_cond::acc_code code, bool& conj, bool& fin,
                     acc_cond::mark_t& res)
    {
      res = {};
      bool found_one = false;
      conj = false;
      fin = false;
      if (code.empty() || code.is_f())
        return false;
      acc_cond::mark_t candidates = ~code.used_once_sets();
      if (!candidates)
        return false;
      const acc_cond::acc_word* pos = &code.back();
      conj = (pos->sub.op == acc_cond::acc_op::And);
      do
        {
          switch (pos->sub.op)
            {
            case acc_cond::acc_op::And:
              if (!conj)
                pos -= pos->sub.size + 1;
              else
                --pos;
              break;
            case acc_cond::acc_op::Or:
              if (conj)
                pos -= pos->sub.size + 1;
              else
                --pos;
              break;
            case acc_cond::acc_op::Inf:
            case acc_cond::acc_op::InfNeg:
            case acc_cond::acc_op::FinNeg:
              if (!fin)
                {
                  auto m = pos[-1].mark & candidates;
                  if (m && (conj || pos[-1].mark.is_singleton()))
                    {
                      found_one = true;
                      res |= m;
                    }
                }
              pos -= 2;
              break;
            case acc_cond::acc_op::Fin:
              if (!found_one || fin)
                {
                  auto m = pos[-1].mark & candidates;
                  if (m && (!conj || pos[-1].mark.is_singleton()))
                    {
                      found_one = true;
                      fin = true;
                      res |= m;
                    }
                }
              pos -= 2;
              break;
            }
        }
      while (pos >= &code.front());
      return !!res;
    }
  }

  acc_cond::acc_code
  acc_cond::acc_code::unit_propagation()
  {
    bool fin, conj;
    acc_cond::mark_t mark;
    acc_code result = (*this);
    acc_code code;
    if (result.size() > 1 &&
        (result.back().sub.op == acc_cond::acc_op::And
        || result.back().sub.op == acc_cond::acc_op::Or))
    {
      while (find_unit_clause(result, conj, fin, mark))
      {
        acc_code init_code;

        if (fin)
        {
          if (conj)
          {
            init_code = acc_code::t();
            for (unsigned col : mark.sets())
              init_code &= acc_code::fin({col});
          } else
          {
            init_code = acc_code::f();
            for (unsigned col : mark.sets())
              init_code |= acc_code::fin({col});
          }
        }
        else
        {
          if (conj)
          {
            init_code = acc_code::t();
            for (unsigned col : mark.sets())
              init_code &= acc_code::inf({col});
          }
          else
          {
            init_code = acc_code::f();
            for (unsigned col : mark.sets())
              init_code |= acc_code::inf({col});
          }
        }
        if (conj)
          result = init_code & result.remove(mark, fin == conj);
        else
          result = init_code | result.remove(mark, fin == conj);
      }

      if (result.is_t())
        return result;
      auto pos = &result.back();
      auto fo = pos->sub.op;
      bool is_and = (fo == acc_cond::acc_op::And);
      std::vector<acc_cond::acc_code> propagated;
      acc_cond::acc_code res;
      if (is_and)
        res = acc_cond::acc_code::t();
      else
        res = acc_cond::acc_code::f();
      if (is_and || fo == acc_cond::acc_op::Or)
      {
        --pos;
        while (pos >= &result.front())
        {
          propagated.push_back(acc_code(pos).unit_propagation());
          pos -= pos->sub.size + 1;
        }
        result = std::accumulate(propagated.rbegin(), propagated.rend(), res,
        [&](acc_cond::acc_code c1, acc_cond::acc_code c2)
        {
          if (is_and)
            return c1 & c2;
          return c1 | c2;
        });
      }
    }
    return result;
  }

  acc_cond::mark_t acc_cond::acc_code::fin_unit() const
  {
    mark_t res = {};
    if (empty() || is_f())
      return res;
    const acc_cond::acc_word* pos = &back();
    do
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
            --pos;
            break;
          case acc_cond::acc_op::Or:
            pos -= pos->sub.size + 1;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
          case acc_cond::acc_op::FinNeg:
            pos -= 2;
            break;
          case acc_cond::acc_op::Fin:
            auto m = pos[-1].mark;
            if (m.is_singleton())
              res |= m;
            pos -= 2;
            break;
          }
      }
    while (pos >= &front());
    return res;
  }

  acc_cond::mark_t acc_cond::acc_code::used_once_sets() const
  {
    mark_t seen = {};
    mark_t dups = {};
    if (empty())
      return {};
    const acc_cond::acc_word* pos = &back();
    do
      {
        switch (pos->sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
          case acc_cond::acc_op::FinNeg:
          case acc_cond::acc_op::Fin:
            auto m = pos[-1].mark;
            pos -= 2;
            dups |= m & seen;
            seen |= m;
            break;
          }
      }
    while (pos >= &front());
    return seen - dups;
  }
}
