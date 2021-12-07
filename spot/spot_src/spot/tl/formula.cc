// -*- coding: utf-8 -*-
// Copyright (C) 2015-2019 Laboratoire de Recherche et Développement
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
#include <spot/misc/common.hh>
#include <spot/tl/formula.hh>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <cstring>
#include <algorithm>
#include <spot/misc/bareword.hh>
#include <spot/tl/print.hh>

#ifndef HAVE_STRVERSCMP
// If the libc does not have this, a version is compiled in lib/.
extern "C" int strverscmp(const char *s1, const char *s2);
#endif

namespace spot
{
  namespace
  {
    typedef std::vector<const fnode*> vec;

    // Compare two formulas, by looking at their operators and
    // children.  This does not use id for the top-level operator,
    // because it is used to decide whether to reuse an equal existing
    // formula.
    struct formula_cmp
    {
      bool operator()(const fnode* l, const fnode* r) const
      {
        op opl = l->kind();
        op opr = r->kind();
        if (opl != opr)
          return opl < opr;

        if (SPOT_UNLIKELY(opl == op::Star || opl == op::FStar))
          {
            {
              auto minl = l->min();
              auto minr = r->min();
              if (minl != minr)
                return minl < minr;
            }
            {
              auto maxl = l->max();
              auto maxr = r->max();
              if (maxl != maxr)
                return maxl < maxr;
            }
          }
        else
          {
            auto szl = l->size();
            auto szr = r->size();
            if (szl != szr)
              return szl < szr;
          }

        auto el = l->end();
        auto ir = r->begin();
        for (auto il = l->begin(); il != el; ++il, ++ir)
          if (*il != *ir)
            return (*il)->id() < (*ir)->id();

        return false;
      }
    };

    struct maps_t final
    {
      std::map<std::string, const fnode*> name2ap;
      std::map<size_t, std::string> ap2name;

      std::set<const fnode*, formula_cmp> uniq;
    };
    static maps_t m;

    static void
    gather_bool(vec& v, op o)
    {
      // Gather all boolean terms.
      vec b;
      vec::iterator i = v.begin();
      while (i != v.end())
        {
          if ((*i)->is_boolean())
            {
              b.emplace_back(*i);
              i = v.erase(i);
            }
          else
            {
              ++i;
            }
        }
      // - AndNLM(Exps1...,Bool1,Exps2...,Bool2,Exps3...) =
      //    AndNLM(And(Bool1,Bool2),Exps1...,Exps2...,Exps3...)
      // - AndRat(Exps1...,Bool1,Exps2...,Bool2,Exps3...) =
      //    AndRat(And(Bool1,Bool2),Exps1...,Exps2...,Exps3...)
      // - OrRat(Exps1...,Bool1,Exps2...,Bool2,Exps3...) =
      //    AndRat(Or(Bool1,Bool2),Exps1...,Exps2...,Exps3...)
      if (!b.empty())
        v.insert(v.begin(), fnode::multop(o, std::move(b)));
    }
  }

  const fnode* fnode::unique(fnode* f)
  {
    auto ires = m.uniq.emplace(f);
    if (!ires.second)
      {
        //(*ires.first)->dump(std::cerr << "UNI: ") << '\n';
        for (auto c: *f)
          c->destroy();
        f->~fnode();
        ::operator delete(f);
        return (*ires.first)->clone();
      }
    //f->dump(std::cerr << "INS: ") << '\n';
    return f;
  }

  void
  fnode::destroy_aux() const
  {
    if (SPOT_UNLIKELY(is(op::ap)))
      {
        auto i = m.ap2name.find(id());
        auto n = m.name2ap.erase(i->second);
        assert(n == 1);
        (void)n;
        m.ap2name.erase(i);
      }
    else
      {
        auto n = m.uniq.erase(this);
        assert(n == 1);
        (void)n;
        for (auto c: *this)
          c->destroy();
      }
    this->~fnode();
    ::operator delete(const_cast<fnode*>(this));
  }

  void
  fnode::report_non_existing_child()
  {
    throw std::runtime_error("access to non-existing child");
  }

  void
  fnode::report_too_many_children()
  {
    throw std::runtime_error("too many children for formula");
  }

  void
  fnode::report_get_child_of_expecting_single_child_node()
  {
    throw std::invalid_argument
      ("get_child_of() expecting single-child node");
  }

  void
  fnode::report_min_invalid_arg()
  {
    throw std::invalid_argument
      ("min() only works on Star and FStar nodes");
  }

  void
  fnode::report_max_invalid_arg()
  {
    throw std::invalid_argument
      ("min() only works on Star and FStar nodes");
  }

  void
  formula::report_ap_invalid_arg()
  {
    throw std::invalid_argument("atomic propositions cannot be "
                                "constructed from arbitrary formulas");
  }

  std::string fnode::kindstr() const
  {
    switch (op_)
      {
#define C(x)                                        \
        case op::x:                                \
          return #x;                                \
          break
        C(ff);
        C(tt);
        C(eword);
        C(ap);
        C(Not);
        C(X);
        C(F);
        C(G);
        C(Closure);
        C(NegClosure);
        C(NegClosureMarked);
        C(Xor);
        C(Implies);
        C(Equiv);
        C(U);
        C(R);
        C(W);
        C(M);
        C(EConcat);
        C(EConcatMarked);
        C(UConcat);
        C(Or);
        C(OrRat);
        C(And);
        C(AndRat);
        C(AndNLM);
        C(Concat);
        C(Fusion);
        C(Star);
        C(FStar);
        C(first_match);
        C(strong_X);
#undef C
      }
    SPOT_UNREACHABLE();
  }

  const fnode*
  fnode::multop(op o, vec v)
  {
    // Inline children of same kind.
    //
    // When we construct a formula such as Multop(Op,X,Multop(Op,Y,Z))
    // we will want to inline it as Multop(Op,X,Y,Z).
    //
    // At the same time, it's possible that vec contains some null
    // pointers we should remove.  We can do it in the same loop.
    //
    // It is simpler to construct a separate vector to do that, but that's
    // only needed if we have nested multops or null poiners.
    if (std::find_if(v.begin(), v.end(),
                     [o](const fnode* f) { return f == nullptr || f->is(o); })
        != v.end())
      {
        vec inlined;
        for (const fnode* f: v)
          {
            if (f == nullptr)
              continue;
            if (f->is(o))
              {
                unsigned ps = f->size();
                for (unsigned n = 0; n < ps; ++n)
                  inlined.emplace_back(f->nth(n)->clone());
                f->destroy();
              }
            else
              {
                inlined.emplace_back(f);
              }
          }
        v.swap(inlined);
      }
    if (o != op::Concat && o != op::Fusion)
      std::sort(v.begin(), v.end(), formula_ptr_less_than_bool_first());

    unsigned orig_size = v.size();

    const fnode* neutral;
    const fnode* neutral2;
    const fnode* abs;
    const fnode* abs2;
    const fnode* weak_abs;
    switch (o)
      {
      case op::And:
        neutral = tt();
        neutral2 = nullptr;
        abs = ff();
        abs2 = nullptr;
        weak_abs = nullptr;
        break;
      case op::AndRat:
        neutral = one_star();
        neutral2 = nullptr;
        abs = ff();
        abs2 = nullptr;
        weak_abs = eword();
        gather_bool(v, op::And);
        break;
      case op::AndNLM:
        neutral = eword();
        neutral2 = nullptr;
        abs = ff();
        abs2 = nullptr;
        weak_abs = tt();
        gather_bool(v, op::And);
        break;
      case op::Or:
        neutral = ff();
        neutral2 = nullptr;
        abs = tt();
        abs2 = nullptr;
        weak_abs = nullptr;
        break;
      case op::OrRat:
        neutral = ff();
        neutral2 = nullptr;
        abs = one_star();
        abs2 = nullptr;
        weak_abs = nullptr;
        gather_bool(v, op::Or);
        break;
      case op::Concat:
        neutral = eword();
        neutral2 = nullptr;
        abs = ff();
        abs2 = nullptr;
        weak_abs = nullptr;

        // - Concat(Exps1...,FExps2...,1[*],FExps3...,Exps4) =
        //     Concat(Exps1...,1[*],Exps4)
        // If FExps2... and FExps3 all accept [*0].
        {
          vec::iterator i = v.begin();
          const fnode* os = one_star();
          while (i != v.end())
            {
              while (i != v.end() && !(*i)->accepts_eword())
                ++i;
              if (i == v.end())
                break;
              vec::iterator b = i;
              // b is the first expressions that accepts [*0].
              // let's find more, and locate the position of
              // 1[*] at the same time.
              bool os_seen = false;
              do
                {
                  os_seen |= (*i == os);
                  ++i;
                }
              while (i != v.end() && (*i)->accepts_eword());

              if (os_seen) // [b..i) is a range that contains [*].
                {
                  // Place [*] at the start of the range, and erase
                  // all other formulae.
                  (*b)->destroy();
                  *b++ = os->clone();
                  for (vec::iterator c = b; c < i; ++c)
                    (*c)->destroy();
                  i = v.erase(b, i);
                }
            }
        }

        break;
      case op::Fusion:
        neutral = tt();
        neutral2 = nullptr;
        abs = ff();
        abs2 = eword();
        weak_abs = nullptr;

        // Make a first pass to group adjacent Boolean formulae.
        // - Fusion(Exps1...,BoolExp1...BoolExpN,Exps2,Exps3...) =
        //   Fusion(Exps1...,And(BoolExp1...BoolExpN),Exps2,Exps3...)
        {
          vec::iterator i = v.begin();
          while (i != v.end())
            {
              if ((*i)->is_boolean())
                {
                  vec::iterator first = i;
                  ++i;
                  if (i == v.end())
                    break;
                  if (!(*i)->is_boolean())
                    {
                      ++i;
                      continue;
                    }
                  do
                    ++i;
                  while (i != v.end() && (*i)->is_boolean());
                  // We have at least two adjacent Boolean formulae.
                  // Replace the first one by the conjunction of all.
                  // FIXME: Investigate the removal of the temporary
                  // vector, by allowing range to be passed to
                  // multop() instead of entire containers.  We
                  // should then able to do
                  //   *first = multop(op::And, first, i);
                  //   i = v.erase(first + 1, i);
                  vec b;
                  b.insert(b.begin(), first, i);
                  i = v.erase(first + 1, i);
                  *first = multop(op::And, b);
                }
              else
                {
                  ++i;
                }
            }
        }

        break;

      default:
        neutral = nullptr;
        neutral2 = nullptr;
        abs = nullptr;
        abs2 = nullptr;
        weak_abs = nullptr;
        break;
      }

    // Remove duplicates (except for Concat and Fusion).  We can't use
    // std::unique(), because we must destroy() any formula we drop.
    // Also ignore neutral elements and handle absorbent elements.
    {
      const fnode* last = nullptr;
      vec::iterator i = v.begin();
      bool weak_abs_seen = false;
      while (i != v.end())
        {
          if ((*i == neutral) || (*i == neutral2) || (*i == last))
            {
              (*i)->destroy();
              i = v.erase(i);
            }
          else if (*i == abs || *i == abs2)
            {
              for (i = v.begin(); i != v.end(); ++i)
                (*i)->destroy();
              assert(abs);
              return abs->clone();
            }
          else
            {
              weak_abs_seen |= (*i == weak_abs);
              if (o != op::Concat && o != op::Fusion)
                // Don't remove duplicates
                last = *i;
              ++i;
            }
        }

      if (weak_abs_seen)
        {
          if (o == op::AndRat)
            {
              // We have    a* && [*0] && c  = 0
              //     and    a* && [*0] && c* = [*0]
              // So if [*0] has been seen, check if alls term
              // recognize the empty word.
              bool acc_eword = true;
              for (i = v.begin(); i != v.end(); ++i)
                {
                  acc_eword &= (*i)->accepts_eword();
                  (*i)->destroy();
                }
              if (acc_eword)
                return weak_abs;
              else
                return abs;
            }
          else
            {
              // Similarly,  a* & 1 & (c;d) = c;d
              //             a* & 1 & c* = 1
              assert(o == op::AndNLM);
              vec tmp;
              for (auto i: v)
                {
                  if (i == weak_abs)
                    continue;
                  if (i->accepts_eword())
                    {
                      i->destroy();
                      continue;
                    }
                  tmp.emplace_back(i);
                }
              if (tmp.empty())
                tmp.emplace_back(weak_abs);
              v.swap(tmp);
            }
        }
      else if (o == op::Concat || o == op::Fusion)
        {
          // Perform an extra loop to merge starable items.
          //   f;f -> f[*2]
          //   f;f[*i..j] -> f[*i+1..j+1]
          //   f[*i..j];f -> f[*i+1..j+1]
          //   f[*i..j];f[*k..l] -> f[*i+k..j+l]
          // same for FStar:
          //   f:f -> f[:*2]
          //   f:f[*i..j] -> f[:*i+1..j+1]
          //   f[:*i..j];f -> f[:*i+1..j+1]
          //   f[:*i..j];f[:*k..l] -> f[:*i+k..j+l]
          op bop = o == op::Concat ? op::Star : op::FStar;
          i = v.begin();
          while (i != v.end())
            {
              vec::iterator fpos = i;
              const fnode* f;
              unsigned min;
              unsigned max;
              bool changed = false;
              if ((*i)->is(bop))
                {
                  f = (*i)->nth(0);
                  min = (*i)->min();
                  max = (*i)->max();
                }
              else
                {
                  f = *i;
                  min = max = 1;
                }

              ++i;
              while (i != v.end())
                {
                  const fnode* f2;
                  unsigned min2;
                  unsigned max2;
                  if ((*i)->is(bop))
                    {
                      f2 = (*i)->nth(0);
                      if (f2 != f)
                        break;
                      min2 = (*i)->min();
                      max2 = (*i)->max();
                    }
                  else
                    {
                      f2 = *i;
                      if (f2 != f)
                        break;
                      min2 = max2 = 1;
                    }
                  if (min2 == unbounded())
                    min = unbounded();
                  else if (min != unbounded())
                    min += min2;
                  if (max2 == unbounded())
                    max = unbounded();
                  else if (max != unbounded())
                    max += max2;
                  (*i)->destroy();
                  i = v.erase(i);
                  changed = true;
                }
              if (changed)
                {
                  const fnode* newfs = bunop(bop, f->clone(), min, max);
                  (*fpos)->destroy();
                  *fpos = newfs;
                }
            }
        }
    }

    vec::size_type s = v.size();
    if (s == 0)
      {
        assert(neutral != nullptr);
        return neutral->clone();
      }
    else if (s == 1)
      {
        // Simply replace Multop(Op,X) by X.
        // Except we should never reduce the
        // arguments of a Fusion operator to
        // a list with a single formula that
        // accepts [*0].
        const fnode* res = v[0];
        if (o != op::Fusion || orig_size == 1
            || !res->accepts_eword())
          return res;
        // If Fusion(f, ...) reduces to Fusion(f), emit Fusion(1,f).
        // to ensure that [*0] is not accepted.
        v.insert(v.begin(), tt());
      }

    auto mem = ::operator new(sizeof(fnode)
                              + (v.size() - 1)*sizeof(*children));
    return unique(new(mem) fnode(o, v.begin(), v.end()));
  }

  const fnode*
  fnode::bunop(op o, const fnode* child, uint8_t min, uint8_t max)
  {
    assert(min <= max);

    const fnode* neutral = nullptr;
    switch (o)
      {
      case op::Star:
        neutral = eword();
        break;
      case op::FStar:
        neutral = tt();
        break;
      default:
        SPOT_UNREACHABLE();
      }


    // common trivial simplifications

    //   - [*0][*min..max] = [*0]
    //   - [*0][:*0..max] = 1
    //   - [*0][:*min..max] = 0 if min > 0
    if (child->is_eword())
      switch (o)
        {
        case op::Star:
          return neutral;
        case op::FStar:
          if (min == 0)
            return neutral;
          else
            return ff();
        default:
          SPOT_UNREACHABLE();
        }

    //   - 0[*0..max] = [*0]
    //   - 0[*min..max] = 0 if min > 0
    //   - b[:*0..max] = 1
    //   - b[:*min..max] = 0 if min > 0
    if (child->is_ff()
        || (o == op::FStar && child->is_boolean()))
      {
        if (min == 0)
          {
            child->destroy();
            return neutral;
          }
        return child;
      }

    //   - Exp[*0] = [*0]
    //   - Exp[:*0] = 1
    if (max == 0)
      {
        child->destroy();
        return neutral;
      }

    //   - Exp[*1] = Exp
    //   - Exp[:*1] = Exp if Exp does not accept [*0]
    if (min == 1 && max == 1)
      if (o == op::Star || !child->accepts_eword())
        return child;

    //   - Exp[*i..j][*k..l] = Exp[*ik..jl] if i*(k+1)<=jk+1.
    //   - Exp[:*i..j][:*k..l] = Exp[:*ik..jl] if i*(k+1)<=jk+1.
    if (child->is(o))
      {
        unsigned i = child->min();
        unsigned j = child->max();

        // Exp has to be true between i*min and j*min
        //               then between i*(min+1) and j*(min+1)
        //               ...
        //            finally between i*max and j*max
        //
        // We can merge these intervals into [i*min..j*max] iff the
        // first are adjacent or overlap, i.e. iff
        //   i*(min+1) <= j*min+1.
        // (Because i<=j, this entails that the other intervals also
        // overlap).

        const fnode* exp = child->nth(0);
        if (j == unbounded())
          {
            min *= i;
            max = unbounded();

            // Exp[*min..max]
            exp->clone();
            child->destroy();
            child = exp;
          }
        else
          {
            if (i * (min + 1) <= (j * min) + 1)
              {
                min *= i;
                if (max != unbounded())
                  {
                    if (j == unbounded())
                      max = unbounded();
                    else
                      max *= j;
                  }
                exp->clone();
                child->destroy();
                child = exp;
              }
          }
      }

    return unique(new fnode(o, child, min, max));
  }

  const fnode*
  fnode::unop(op o, const fnode* f)
  {
    // Some trivial simplifications.
    switch (o)
      {
      case op::F:
      case op::G:
        {
          // F and G are idempotent.
          if (f->is(o))
            return f;

          // F(0) = G(0) = 0
          // F(1) = G(1) = 1
          if (f->is_ff() || f->is_tt())
            return f;

          assert(!f->is_eword());
        }
        break;

      case op::Not:
        {
          // !1 = 0
          if (f->is_tt())
            return ff();
          // !0 = 1
          if (f->is_ff())
            return tt();
          // ![*0] = 1[+]
          if (f->is_eword())
            return bunop(op::Star, tt(), 1);

          auto fop = f->kind();
          // "Not" is an involution.
          if (fop == o)
            {
              auto c = f->nth(0)->clone();
              f->destroy();
              return c;
            }
          // !Closure(Exp) = NegClosure(Exp)
          if (fop == op::Closure)
            {
              const fnode* c = unop(op::NegClosure, f->nth(0)->clone());
              f->destroy();
              return c;
            }
          // !NegClosure(Exp) = Closure(Exp)
          if (fop == op::NegClosure || fop == op::NegClosureMarked)
            {
              const fnode* c = unop(op::Closure, f->nth(0)->clone());
              f->destroy();
              return c;
            }
          break;
        }

      case op::X:
        // X(1) = 1
        if (f->is_tt())
          return f;
        // We do not have X(0)=0 because that
        // is not true with finite semantics.
        assert(!f->is_eword());
        break;
      case op::strong_X:
        // X[!](0) = 0
        if (f->is_ff())
          return f;
        // Note: with finite semantics X[!](1)≠1.
        assert(!f->is_eword());
        break;

      case op::Closure:
        // {0} = 0, {1} = 1,  {b} = b
        if (f->is_boolean())
          return f;
        // {[*0]} = 0
        if (f->is_eword())
          return ff();
        break;

      case op::NegClosure:
      case op::NegClosureMarked:
        // {1} = 0
        if (f->is_tt())
          return ff();
        // {0} = 1,  {[*0]} = 1
        if (f->is_ff() || f->is_eword())
          return tt();
        // {b} = !b
        if (f->is_boolean())
          return unop(op::Not, f);
        break;
      case op::first_match:
        // first_match(first_match(sere)) = first_match(sere);
        // first_match(b) = b
        if (f->is(o) || f->is_boolean())
          return f;
        // first_match(r*) = [*0]
        if (f->accepts_eword())
          {
            f->destroy();
            return eword();
          }
        break;
      default:
        SPOT_UNREACHABLE();
      }

    return unique(new fnode(o, {f}));
  }

  const fnode*
  fnode::binop(op o, const fnode* first, const fnode* second)
  {
    // Sort the operands of commutative operators, so that for
    // example the formula instance for 'a xor b' is the same as
    // that for 'b xor a'.

    // Trivial identities:
    switch (o)
      {
      case op::Xor:
        {
          // Xor is commutative: sort operands.
          formula_ptr_less_than_bool_first cmp;
          if (cmp(second, first))
            std::swap(second, first);
        }
        //   - (1 ^ Exp) = !Exp
        //   - (0 ^ Exp) = Exp
        if (first->is_tt())
          return unop(op::Not, second);
        if (first->is_ff())
          return second;
        if (first == second)
          {
            first->destroy();
            second->destroy();
            return ff();
          }
        // We expect constants to appear first, because they are
        // instantiated first.
        assert(!second->is_constant());
        break;
      case op::Equiv:
        {
          // Equiv is commutative: sort operands.
          formula_ptr_less_than_bool_first cmp;
          if (cmp(second, first))
            std::swap(second, first);
        }
        //   - (0 <=> Exp) = !Exp
        //   - (1 <=> Exp) = Exp
        //   - (Exp <=> Exp) = 1
        if (first->is_ff())
          return unop(op::Not, second);
        if (first->is_tt())
          return second;
        if (first == second)
          {
            first->destroy();
            second->destroy();
            return tt();
          }
        // We expect constants to appear first, because they are
        // instantiated first.
        assert(!second->is_constant());
        break;
      case op::Implies:
        //   - (1 => Exp) = Exp
        //   - (0 => Exp) = 1
        //   - (Exp => 1) = 1
        //   - (Exp => 0) = !Exp
        //   - (Exp => Exp) = 1
        if (first->is_tt())
          return second;
        if (first->is_ff())
          {
            second->destroy();
            return tt();
          }
        if (second->is_tt())
          {
            first->destroy();
            return second;
          }
        if (second->is_ff())
          return unop(op::Not, first);
        if (first == second)
          {
            first->destroy();
            second->destroy();
            return tt();
          }
        break;
      case op::U:
        //   - (Exp U 1) = 1
        //   - (Exp U 0) = 0
        //   - (0 U Exp) = Exp
        //   - (Exp U Exp) = Exp
        if (second->is_tt()
            || second->is_ff()
            || first->is_ff()
            || first == second)
          {
            first->destroy();
            return second;
          }
        break;
      case op::W:
        //   - (Exp W 1) = 1
        //   - (0 W Exp) = Exp
        //   - (1 W Exp) = 1
        //   - (Exp W Exp) = Exp
        if (second->is_tt()
            || first->is_ff()
            || first == second)
          {
            first->destroy();
            return second;
          }
        if (first->is_tt())
          {
            second->destroy();
            return first;
          }
        break;
      case op::R:
        //   - (Exp R 1) = 1
        //   - (Exp R 0) = 0
        //   - (1 R Exp) = Exp
        //   - (Exp R Exp) = Exp
        if (second->is_tt()
            || second->is_ff()
            || first->is_tt()
            || first == second)
          {
            first->destroy();
            return second;
          }
        break;
      case op::M:
        //   - (Exp M 0) = 0
        //   - (1 M Exp) = Exp
        //   - (0 M Exp) = 0
        //   - (Exp M Exp) = Exp
        if (second->is_ff()
            || first->is_tt()
            || first == second)
          {
            first->destroy();
            return second;
          }
        if (first->is_ff())
          {
            second->destroy();
            return first;
          }
        break;
      case op::EConcat:
      case op::EConcatMarked:
        //   - 0 <>-> Exp = 0
        //   - 1 <>-> Exp = Exp
        //   - [*0] <>-> Exp = 0
        //   - Exp <>-> 0 = 0
        //   - boolExp <>-> Exp = boolExp & Exp
        if (first->is_tt())
          return second;
        if (first->is_ff()
            || first->is_eword())
          {
            second->destroy();
            return ff();
          }
        if (second->is_ff())
          {
            first->destroy();
            return second;
          }
        if (first->is_boolean())
          return multop(op::And, {first, second});
        break;
      case op::UConcat:
        //   - 0 []-> Exp = 1
        //   - 1 []-> Exp = Exp
        //   - [*0] []-> Exp = 1
        //   - Exp []-> 1 = 1
        //   - boolExp []-> Exp = !boolExp | Exp
        if (first->is_tt())
          return second;
        if (first->is_ff()
            || first->is_eword())
          {
            second->destroy();
            return tt();
          }
        if (second->is_tt())
          {
            first->destroy();
            return second;
          }
        if (first->is_boolean())
          return multop(op::Or, {unop(op::Not, first), second});
        break;
      default:
        SPOT_UNREACHABLE();
      }

    auto mem = ::operator new(sizeof(fnode) + sizeof(*children));
    return unique(new(mem) fnode(o, {first, second}));
  }

  const fnode*
  fnode::ap(const std::string& name)
  {
    auto ires = m.name2ap.emplace(name, nullptr);
    if (!ires.second)
      return ires.first->second->clone();
    // Name the formula before creating it, because the constructor
    // will call ap_name().
    //
    // We plan to use next_id_ for identifier, but it is possible that
    // next_id_ has already wrapped around 0 and that next_id_ is
    // already the name of another atomic proposition.  In that
    // unlikely case, simply increment the id.
    while (SPOT_UNLIKELY(!m.ap2name.emplace(next_id_, name).second))
      bump_next_id();
    // next_id_ is incremented by setup_props(), called by the
    // constructor of fnode
    return ires.first->second = new fnode(op::ap, {});
  }

  const std::string&
  fnode::ap_name() const
  {
    if (op_ != op::ap)
      throw std::runtime_error("ap_name() called on non-AP formula");
    auto i = m.ap2name.find(id());
    assert(i != m.ap2name.end());
    return i->second;
  }

  size_t fnode::next_id_ = 0U;

  size_t fnode::bump_next_id()
  {
    size_t id = next_id_++;
    // If the counter of formulae ever loops, we want to skip the
    // first three values, because they are permanently associated
    // to constants, and it is convenient to have constants
    // smaller than all other formulas.
    if (SPOT_UNLIKELY(next_id_ == 0))
      next_id_ = 3;
    return id;
  }

  const fnode* fnode::ff_ = new fnode(op::ff, {});
  const fnode* fnode::tt_ = new fnode(op::tt, {});
  const fnode* fnode::ew_ = new fnode(op::eword, {});
  const fnode* fnode::one_star_ = nullptr; // Only built when necessary.

  void fnode::setup_props(op o)
  {
    id_ = bump_next_id();

    switch (o)
      {
      case op::ff:
      case op::tt:
        is_.boolean = true;
        is_.sugar_free_boolean = true;
        is_.in_nenoform = true;
        is_.syntactic_si = true; // for LTL (not PSL)
        is_.sugar_free_ltl = true;
        is_.ltl_formula = true;
        is_.psl_formula = true;
        is_.sere_formula = true;
        is_.finite = true;
        is_.eventual = true;
        is_.universal = true;
        is_.syntactic_safety = true;
        is_.syntactic_guarantee = true;
        is_.syntactic_obligation = true;
        is_.syntactic_recurrence = true;
        is_.syntactic_persistence = true;
        is_.not_marked = true;
        is_.accepting_eword = false;
        is_.lbt_atomic_props = true;
        is_.spin_atomic_props = true;
        break;
      case op::eword:
        is_.boolean = false;
        is_.sugar_free_boolean = false;
        is_.in_nenoform = true;
        is_.syntactic_si = true;
        is_.sugar_free_ltl = true;
        is_.ltl_formula = false;
        is_.psl_formula = false;
        is_.sere_formula = true;
        is_.finite = true;
        is_.eventual = false;
        is_.syntactic_safety = false;
        is_.syntactic_guarantee = false;
        is_.syntactic_obligation = false;
        is_.syntactic_recurrence = false;
        is_.syntactic_persistence = false;
        is_.universal = false;
        is_.not_marked = true;
        is_.accepting_eword = true;
        is_.lbt_atomic_props = true;
        is_.spin_atomic_props = true;
        break;
      case op::ap:
        is_.boolean = true;
        is_.sugar_free_boolean = true;
        is_.in_nenoform = true;
        is_.syntactic_si = true;        // Assuming LTL (for PSL, a Boolean
        // term that is not stared will be regarded as non-SI where
        // this matters.)
        is_.sugar_free_ltl = true;
        is_.ltl_formula = true;
        is_.psl_formula = true;
        is_.sere_formula = true;
        is_.finite = true;
        is_.eventual = false;
        is_.universal = false;
        is_.syntactic_safety = true;
        is_.syntactic_guarantee = true;
        is_.syntactic_obligation = true;
        is_.syntactic_recurrence = true;
        is_.syntactic_persistence = true;
        is_.not_marked = true;
        is_.accepting_eword = false;
        {
          // is_.lbt_atomic_props should be true if the name has the
          // form pNN where NN is any number of digit.
          const std::string& n = ap_name();
          std::string::const_iterator pos = n.begin();
          bool lbtap = (pos != n.end() && *pos++ == 'p');
          while (lbtap && pos != n.end())
            {
              char l = *pos++;
              lbtap = (l >= '0' && l <= '9');
            }
          is_.lbt_atomic_props = lbtap;
          is_.spin_atomic_props = lbtap || is_spin_ap(n.c_str());
        }
        break;
      case op::Not:
        props = children[0]->props;
        is_.not_marked = true;
        is_.eventual = children[0]->is_universal();
        is_.universal = children[0]->is_eventual();
        is_.in_nenoform = (children[0]->is(op::ap));
        is_.sere_formula = is_.boolean;

        is_.syntactic_safety = children[0]->is_syntactic_guarantee();
        is_.syntactic_guarantee = children[0]->is_syntactic_safety();
        // is_.syntactic_obligation inherited from child
        is_.syntactic_recurrence = children[0]->is_syntactic_persistence();
        is_.syntactic_persistence = children[0]->is_syntactic_recurrence();

        is_.accepting_eword = false;
        break;
      case op::X:
      case op::strong_X:
        props = children[0]->props;
        is_.not_marked = true;
        is_.boolean = false;
        is_.syntactic_si = false;
        is_.sere_formula = false;
        // is_.syntactic_safety inherited
        // is_.syntactic_guarantee inherited
        // is_.syntactic_obligation inherited
        // is_.syntactic_recurrence inherited
        // is_.syntactic_persistence inherited

        // is_.accepting_eword is currently unused outside SEREs, but
        // we could make sense of it if we start supporting LTL over
        // finite traces.
        is_.accepting_eword = false;
        break;
      case op::F:
        props = children[0]->props;
        is_.not_marked = true;
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.sugar_free_ltl = false;
        is_.eventual = true;
        is_.syntactic_safety = false;
        // is_.syntactic_guarantee inherited
        is_.syntactic_obligation = is_.syntactic_guarantee;
        is_.syntactic_recurrence = is_.syntactic_guarantee;
        // is_.syntactic_persistence inherited
        is_.accepting_eword = false;
        break;
      case op::G:
        props = children[0]->props;
        is_.not_marked = true;
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.sugar_free_ltl = false;
        is_.universal = true;
        // is_.syntactic_safety inherited
        is_.syntactic_guarantee = false;
        is_.syntactic_obligation = is_.syntactic_safety;
        // is_.syntactic_recurrence inherited
        is_.syntactic_persistence = is_.syntactic_safety;
        is_.accepting_eword = false;
        break;
      case op::NegClosure:
      case op::NegClosureMarked:
        props = children[0]->props;
        is_.not_marked = (op_ == op::NegClosure);
        is_.boolean = false;
        is_.ltl_formula = false;
        is_.psl_formula = true;
        is_.sere_formula = false;
        is_.syntactic_safety = is_.finite;
        is_.syntactic_guarantee = true;
        is_.syntactic_obligation = true;
        is_.syntactic_recurrence = true;
        is_.syntactic_persistence = true;
        is_.accepting_eword = false;
        assert(children[0]->is_sere_formula());
        assert(!children[0]->is_boolean());
        break;
      case op::Closure:
        props = children[0]->props;
        is_.not_marked = true;
        is_.boolean = false;
        is_.ltl_formula = false;
        is_.psl_formula = true;
        is_.sere_formula = false;
        is_.syntactic_safety = true;
        is_.syntactic_guarantee = is_.finite;
        is_.syntactic_obligation = true;
        is_.syntactic_recurrence = true;
        is_.syntactic_persistence = true;
        is_.accepting_eword = false;
        assert(children[0]->is_sere_formula());
        assert(!children[0]->is_boolean());
        break;
      case op::Xor:
      case op::Equiv:
        props = children[0]->props & children[1]->props;
        is_.eventual = false;
        is_.universal = false;
        is_.sere_formula = is_.boolean;
        is_.sugar_free_boolean = false;
        is_.in_nenoform = false;
        // is_.syntactic_obligation inherited;
        is_.accepting_eword = false;
        if (is_.syntactic_obligation)
          {
            // Only formula that are in the intersection of
            // guarantee and safety are closed by Xor and <=>.
            bool sg = is_.syntactic_safety && is_.syntactic_guarantee;
            is_.syntactic_safety = sg;
            is_.syntactic_guarantee = sg;
            assert(is_.syntactic_recurrence == true);
            assert(is_.syntactic_persistence == true);
          }
        else
          {
            is_.syntactic_safety = false;
            is_.syntactic_guarantee = false;
            is_.syntactic_recurrence = false;
            is_.syntactic_persistence = false;
          }
        break;
      case op::Implies:
        props = children[0]->props & children[1]->props;
        is_.eventual = false;
        is_.universal = false;
        is_.sere_formula = (children[0]->is_boolean()
                            && children[1]->is_sere_formula());
        is_.sugar_free_boolean = false;
        is_.in_nenoform = false;
        is_.syntactic_safety = (children[0]->is_syntactic_guarantee()
                                && children[1]->is_syntactic_safety());
        is_.syntactic_guarantee = (children[0]->is_syntactic_safety()
                                   && children[1]->is_syntactic_guarantee());
        // is_.syntactic_obligation inherited
        is_.syntactic_persistence = children[0]->is_syntactic_recurrence()
          && children[1]->is_syntactic_persistence();
        is_.syntactic_recurrence = children[0]->is_syntactic_persistence()
          && children[1]->is_syntactic_recurrence();
        is_.accepting_eword = false;
        break;
      case op::EConcatMarked:
      case op::EConcat:
        props = children[0]->props & children[1]->props;
        is_.not_marked = (op_ != op::EConcatMarked);
        is_.ltl_formula = false;
        is_.boolean = false;
        is_.sere_formula = false;
        is_.accepting_eword = false;
        is_.psl_formula = true;

        is_.syntactic_guarantee = children[1]->is_syntactic_guarantee();
        is_.syntactic_persistence = children[1]->is_syntactic_persistence();
        if (children[0]->is_finite())
          {
            is_.syntactic_safety = children[1]->is_syntactic_safety();
            is_.syntactic_obligation = children[1]->is_syntactic_obligation();
            is_.syntactic_recurrence = children[1]->is_syntactic_recurrence();
          }
        else
          {
            is_.syntactic_safety = false;
            bool g = children[1]->is_syntactic_guarantee();
            is_.syntactic_obligation = g;
            is_.syntactic_recurrence = g;
          }
        assert(children[0]->is_sere_formula());
        assert(children[1]->is_psl_formula());
        if (children[0]->is_boolean())
          is_.syntactic_si = false;
        break;
      case op::UConcat:
        props = children[0]->props & children[1]->props;
        is_.not_marked = true;
        is_.ltl_formula = false;
        is_.boolean = false;
        is_.sere_formula = false;
        is_.accepting_eword = false;
        is_.psl_formula = true;

        is_.syntactic_safety = children[1]->is_syntactic_safety();
        is_.syntactic_recurrence = children[1]->is_syntactic_recurrence();
        if (children[0]->is_finite())
          {
            is_.syntactic_guarantee = children[1]->is_syntactic_guarantee();
            is_.syntactic_obligation = children[1]->is_syntactic_obligation();
            is_.syntactic_persistence =
              children[1]->is_syntactic_persistence();
          }
        else
          {
            is_.syntactic_guarantee = false;
            bool s = children[1]->is_syntactic_safety();
            is_.syntactic_obligation = s;
            is_.syntactic_persistence = s;
          }
        assert(children[0]->is_sere_formula());
        assert(children[1]->is_psl_formula());
        if (children[0]->is_boolean())
          is_.syntactic_si = false;
        break;
      case op::U:
        // Beware: (f U g) is a pure eventuality if both operands
        // are pure eventualities, unlike in the proceedings of
        // Concur'00.  (The revision of the paper available at
        // http://www.bell-labs.com/project/TMP/ is fixed.)  See
        // also http://arxiv.org/abs/1011.4214v2 for a discussion
        // about this problem.  (Which we fixed in 2005 thanks
        // to LBTT.)
        // This means that we can use the following line to handle
        // all cases of (f U g), (f R g), (f W g), (f M g) for
        // universality and eventuality.
        props = children[0]->props & children[1]->props;
        // The matter can be further refined because:
        //  (f U g) is a pure eventuality if
        //                g is a pure eventuality (regardless of f),
        //             or f == 1
        //  (g M f) is a pure eventuality if f and g are,
        //                                or f == 1
        //  (g R f) is purely universal if
        //                f is purely universal (regardless of g)
        //               or g == 0
        //  (f W g) is purely universal if f and g are
        //                              or g == 0
        is_.not_marked = true;
        // f U g is universal if g is eventual, or if f == 1.
        is_.eventual = children[1]->is_eventual();
        is_.eventual |= children[0]->is_tt();
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.accepting_eword = false;

        is_.syntactic_safety = false;
        // is_.syntactic_guarantee = Guarantee U Guarantee
        is_.syntactic_obligation = // Obligation U Guarantee
          children[0]->is_syntactic_obligation()
          && children[1]->is_syntactic_guarantee();
        is_.syntactic_recurrence = // Recurrence U Guarantee
          children[0]->is_syntactic_recurrence()
          && children[1]->is_syntactic_guarantee();
        // is_.syntactic_persistence = Persistence U Persistance
        break;
      case op::W:
        // See comment for op::U.
        props = children[0]->props & children[1]->props;
        is_.not_marked = true;
        // f W g is universal if f and g are, or if g == 0.
        is_.universal |= children[1]->is_ff();
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.accepting_eword = false;

        // is_.syntactic_safety = Safety W Safety;
        is_.syntactic_guarantee = false;
        is_.syntactic_obligation = // Safety W Obligation
          children[0]->is_syntactic_safety()
          && children[1]->is_syntactic_obligation();
        // is_.syntactic_recurrence = Recurrence W Recurrence
        is_.syntactic_persistence = // Safety W Persistance
          children[0]->is_syntactic_safety()
          && children[1]->is_syntactic_persistence();

        break;
      case op::R:
        // See comment for op::U.
        props = children[0]->props & children[1]->props;
        is_.not_marked = true;
        // g R f is universal if f is universal, or if g == 0.
        is_.universal = children[1]->is_universal();
        is_.universal |= children[0]->is_ff();
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.accepting_eword = false;

        // is_.syntactic_safety = Safety R Safety;
        is_.syntactic_guarantee = false;
        is_.syntactic_obligation = // Obligation R Safety
          children[0]->is_syntactic_obligation()
          && children[1]->is_syntactic_safety();
        //is_.syntactic_recurrence = Recurrence R Recurrence
        is_.syntactic_persistence = // Persistence R Safety
          children[0]->is_syntactic_persistence()
          && children[1]->is_syntactic_safety();

        break;
      case op::M:
        // See comment for op::U.
        props = children[0]->props & children[1]->props;
        is_.not_marked = true;
        // g M f is eventual if both g and f are eventual, or if f == 1.
        is_.eventual |= children[1]->is_tt();
        is_.boolean = false;
        is_.sere_formula = false;
        is_.finite = false;
        is_.accepting_eword = false;

        is_.syntactic_safety = false;
        // is_.syntactic_guarantee = Guarantee M Guarantee
        is_.syntactic_obligation = // Guarantee M Obligation
          children[0]->is_syntactic_guarantee()
          && children[1]->is_syntactic_obligation();
        is_.syntactic_recurrence = // Guarantee M Recurrence
          children[0]->is_syntactic_guarantee()
          && children[1]->is_syntactic_recurrence();
        // is_.syntactic_persistence = Persistence M Persistance

        break;
      case op::Or:
        {
          props = children[0]->props;
          unsigned s = size_;
          bool ew = children[0]->accepts_eword();
          for (unsigned i = 1; i < s; ++i)
            {
              ew |= children[i]->accepts_eword();
              props &= children[i]->props;
            }
          is_.accepting_eword = ew;
          break;
        }
      case op::OrRat:
        {
          props = children[0]->props;
          unsigned s = size_;
          bool syntactic_si = is_.syntactic_si && !is_.boolean;
          // Note: OrRat(p1,p2) is a Boolean formula, but its is
          // actually rewritten as Or(p1,p2) by trivial identities
          // before this constructor is called.  So at this point,
          // AndNLM is always used with at most one Boolean argument,
          // and the result is therefore NOT Boolean.
          is_.boolean = false;
          is_.ltl_formula = false;
          is_.psl_formula = false;
          is_.eventual = false;
          is_.universal = false;

          bool ew = children[0]->accepts_eword();
          for (unsigned i = 1; i < s; ++i)
            {
              ew |= children[i]->accepts_eword();
              syntactic_si &= children[i]->is_syntactic_stutter_invariant()
                && !children[i]->is_boolean();
              props &= children[i]->props;
            }
          is_.accepting_eword = ew;
          is_.syntactic_si = syntactic_si;
          break;
        }
      case op::And:
        {
          props = children[0]->props;
          unsigned s = size_;
          for (unsigned i = 1; i < s; ++i)
            props &= children[i]->props;
          break;
        }
      case op::Fusion:
      case op::Concat:
      case op::AndNLM:
      case op::AndRat:
        {
          props = children[0]->props;
          unsigned s = size_;
          bool syntactic_si = is_.syntactic_si && !is_.boolean;
          // Note: AndNLM(p1,p2) and AndRat(p1,p2) are Boolean
          // formulae, but they are actually rewritten as And(p1,p2)
          // by trivial identities before this constructor is called.
          // So at this point, AndNLM/AndRat are always used with at
          // most one Boolean argument, and the result is therefore
          // NOT Boolean.
          is_.boolean = false;
          is_.ltl_formula = false;
          is_.psl_formula = false;
          is_.eventual = false;
          is_.universal = false;

          for (unsigned i = 1; i < s; ++i)
            {
              syntactic_si &= children[i]->is_syntactic_stutter_invariant()
                && !children[i]->is_boolean();
              props &= children[i]->props;
            }
          is_.syntactic_si = syntactic_si;
          if (op_ == op::Fusion)
            is_.accepting_eword = false;
          // A concatenation is an siSERE if looks like
          //    r;b*  or  b*;r
          // where b is Boolean and r is siSERE.  generalized to n-ary
          // concatenation, it means all arguments should be of the
          // form b*, except one that is siSERE (i.e., a sub-formula
          // that verify is_syntactic_stutter_invariant() and
          // !is_boolean()).   Since b* is siSERE, that means we
          // want at least s-1 operands of the form b*.
          if (op_ == op::Concat)
            {
              unsigned sb = 0; // stared Boolean formulas seen
              for (unsigned i = 0; i < s; ++i)
                {
                  auto ci = children[i];
                  if (ci->is_Kleene_star())
                    {
                      sb += ci->nth(0)->is_boolean();
                    }
                  else if (!ci->is_syntactic_stutter_invariant()
                           || ci->is_boolean())
                    {
                      sb = 0;
                      break;
                    }
                }
              is_.syntactic_si = sb >= s - 1;
            }
          break;
        }

      case op::Star:
      case op::FStar:
        {
          props = children[0]->props;
          assert(is_.sere_formula);
          is_.boolean = false;
          is_.ltl_formula = false;
          is_.psl_formula = false;
          is_.eventual = false;
          is_.universal = false;
          is_.syntactic_safety = false;
          is_.syntactic_guarantee = false;
          is_.syntactic_obligation = false;
          is_.syntactic_recurrence = false;
          is_.syntactic_persistence = false;

          switch (op_)
            {
            case op::Star:
              if (max_ == unbounded())
                {
                  is_.finite = false;
                  is_.syntactic_si = min_ <= 1 && children[0]->is_boolean();
                }
              else
                {
                  is_.syntactic_si = false;
                }
              if (min_ == 0)
                is_.accepting_eword = true;
              break;
            case op::FStar:
              is_.accepting_eword = false;
              is_.syntactic_si &= !children[0]->is_boolean();
              if (max_ == unbounded())
                is_.finite = false;
              if (min_ == 0)
                is_.syntactic_si = false;
              break;
            default:
              SPOT_UNREACHABLE();
            }
        }
        break;
      case op::first_match:
        props = children[0]->props;
        assert(is_.sere_formula);
        // {(a[+];b*);c*}<>->d is stutter invariant
        // {first_match(a[+];b*);c*}<>->d is NOT.
        is_.syntactic_si = false;
        is_.boolean = false;
        is_.ltl_formula = false;
        is_.psl_formula = false;
        is_.eventual = false;
        is_.universal = false;
        is_.syntactic_safety = false;
        is_.syntactic_guarantee = false;
        is_.syntactic_obligation = false;
        is_.syntactic_recurrence = false;
        is_.syntactic_persistence = false;
        break;
      }
  }

  const fnode*
  fnode::nested_unop_range(op uo, op bo, unsigned min, unsigned max,
                           const fnode* f)
  {
    const fnode* res = f;
    if (max < min)
      std::swap(min, max);
    if (max != unbounded())
      for (unsigned i = min; i < max; ++i)
        {
          const fnode* a = f->clone();
          res = fnode::multop(bo, {a, fnode::unop(uo, res)});
        }
    else
      res = fnode::unop(bo == op::Or ? op::F : op::G, res);
    for (unsigned i = 0; i < min; ++i)
      res = fnode::unop(uo, res);
    return res;
  }

  std::ostream& fnode::dump(std::ostream& os) const
  {
    os << kindstr() << "(@" << id_ << " #" << refs_;
    if (op_ == op::Star || op_ == op::FStar)
      {
        os << ' ' << +min() << "..";
        auto m = max();
        if (m != unbounded())
          os << +m;
      }
    if (op_ == op::ap)
      os << " \"" << ap_name() << '"';
    if (auto s = size())
      {
        os << " [";
        for (auto c: *this)
          {
            c->dump(os);
            if (--s)
              os << ", ";
          }
        os << ']';
      }
    return os << ')';
  }

  const fnode* fnode::all_but(unsigned i) const
  {
    switch (op o = kind())
      {
      case op::Or:
      case op::OrRat:
      case op::And:
      case op::AndRat:
      case op::AndNLM:
      case op::Concat:
      case op::Fusion:
        {
          unsigned s = size();
          assert(s > 1);
          vec v;
          v.reserve(s - 1);
          for (unsigned j = 0; j < s; ++j)
            if (i != j)
              v.emplace_back(nth(j)->clone());
          return multop(o, v);
        }
      default:
        throw
          std::runtime_error("all_but() is incompatible with this operator");
      }
    SPOT_UNREACHABLE();
  }

  const fnode* fnode::boolean_operands(unsigned* width) const
  {
    unsigned s = boolean_count();
    if (width)
      *width = s;
    if (s == 0)
      return nullptr;
    if (s == 1)
      return nth(0)->clone();
    vec v(children, children + s);
    for (auto c: v)
      c->clone();
    return multop(op_, v);
  }

  bool fnode::instances_check()
  {
    unsigned cnt = 0;
    for (auto i: m.uniq)
      if (i->id() > 3 && i != one_star_)
        {
          if (!cnt++)
            std::cerr << "*** m.uniq is not empty ***\n";
          i->dump(std::cerr) << '\n';
        }
    return cnt == 0;
  }

  formula formula::sugar_goto(const formula& b, uint8_t min, uint8_t max)
  {
    if (!b.is_boolean())
      throw
        std::runtime_error("sugar_goto() called with non-Boolean argument");
    // b[->min..max] is implemented as ((!b)[*];b)[*min..max]
    return Star(Concat({Star(Not(b)), b}), min, max);
  }

  formula formula::sugar_equal(const formula& b, uint8_t min, uint8_t max)
  {
    if (!b.is_boolean())
      throw
        std::runtime_error("sugar_equal() called with non-Boolean argument");

    // b[=0..] = 1[*]
    if (min == 0 && max == unbounded())
      return one_star();

    // b[=min..max] is implemented as ((!b)[*];b)[*min..max];(!b)[*]
    formula s = Star(Not(b));
    return Concat({Star(Concat({s, b}), min, max), s});
  }

  formula formula::sugar_delay(const formula& b,
                               unsigned min, unsigned max)
  {
    // ##[min:max] b = 1[*min:max];b
    return Concat({Star(tt(), min, max), b});
  }
  formula formula::sugar_delay(const formula& a, const formula& b,
                               unsigned min, unsigned max)
  {
    // If min>=1
    //   a ##[min:max] b  = a;1[*min-1:max-1];b
    // If min==0 we can use
    //   a ##[0:0] b = a:b
    //   a ##[0:max] b = a:(1[*0:max];b)  if a rejects [*0]
    //   a ##[0:max] b = (a;1[*0:max]):b  if b rejects [*0]
    //   a ##[0:max] b = (a:b)|(a;[*0:max-1];b)  else
    if (min > 0)
      {
        --min;
        if (max != unbounded())
          --max;
        return Concat({a, Star(tt(), min, max), b});
      }
    if (max == 0)
      return Fusion({a, b});
    if (!a.accepts_eword())
      return Fusion({a, Concat({Star(tt(), 0, max), b})});
    if (!b.accepts_eword())
      return Fusion({Concat({a, Star(tt(), 0, max)}), b});

    if (max != unbounded())
      --max;
    formula left = Fusion({a, b});
    formula right = Concat({a, Star(tt(), 0, max), b});
    return OrRat({left, right});
  }

  int atomic_prop_cmp(const fnode* f, const fnode* g)
  {
    return strverscmp(f->ap_name().c_str(), g->ap_name().c_str());
  }

#define printprops                                                        \
  proprint(is_boolean, "B", "Boolean formula");                                \
  proprint(is_sugar_free_boolean, "&", "without Boolean sugar");        \
  proprint(is_in_nenoform, "!", "in negative normal form");                \
  proprint(is_syntactic_stutter_invariant, "x",                                \
           "syntactic stutter invariant");                                \
  proprint(is_sugar_free_ltl, "f", "without LTL sugar");                \
  proprint(is_ltl_formula, "L", "LTL formula");                                \
  proprint(is_psl_formula, "P", "PSL formula");                                \
  proprint(is_sere_formula, "S", "SERE formula");                        \
  proprint(is_finite, "F", "finite");                                        \
  proprint(is_eventual, "e", "pure eventuality");                        \
  proprint(is_universal, "u", "purely universal");                        \
  proprint(is_syntactic_safety, "s", "syntactic safety");                \
  proprint(is_syntactic_guarantee, "g", "syntactic guarantee");                \
  proprint(is_syntactic_obligation, "o", "syntactic obligation");        \
  proprint(is_syntactic_persistence, "p", "syntactic persistence");        \
  proprint(is_syntactic_recurrence, "r", "syntactic recurrence");        \
  proprint(is_marked, "+", "marked");                                        \
  proprint(accepts_eword, "0", "accepts the empty word");                \
  proprint(has_lbt_atomic_props, "l",                                        \
           "has LBT-style atomic props");                                \
  proprint(has_spin_atomic_props, "a",                                        \
           "has Spin-style atomic props");


  std::list<std::string>
  list_formula_props(const formula& f)
  {
    std::list<std::string> res;
#define proprint(m, a, l)                        \
    if (f.m())                                        \
      res.emplace_back(l);
    printprops;
#undef proprint
    return res;
  }

  std::ostream&
  print_formula_props(std::ostream& out, const formula& f, bool abbr)
  {
    const char* comma = abbr ? "" : ", ";
    const char* sep = "";

#define proprint(m, a, l)                        \
    if (f.m())                                        \
      {                                                \
        out << sep; out << (abbr ? a : l);        \
        sep = comma;                                \
      }
    printprops;
#undef proprint

    return out;
  }

  std::ostream& operator<<(std::ostream& os, const formula& f)
  {
    return print_psl(os, f);
  }
}
