// -*- coding: utf-8 -*-
// Copyright (C) 2011-2020 Laboratoire de Recherche et Developpement
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
#include <iostream>
//#define TRACE
#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

#include <spot/tl/simplify.hh>
#include <spot/priv/robin_hood.hh>
#include <spot/tl/contain.hh>
#include <spot/tl/print.hh>
#include <spot/tl/snf.hh>
#include <spot/tl/length.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/misc/minato.hh>
#include <cassert>
#include <memory>
#include <unordered_set>
#include <map>



namespace spot
{
  typedef std::vector<formula> vec;

  // The name of this class is public, but not its contents.
  class tl_simplifier_cache final
  {
    typedef robin_hood::unordered_map<formula, formula> f2f_map;
    typedef robin_hood::unordered_map<formula, bdd> f2b_map;
    typedef robin_hood::unordered_map<int, formula> b2f_map;
    typedef std::pair<formula, formula> pairf;
    typedef std::map<pairf, bool> syntimpl_cache_t;
  public:
    bdd_dict_ptr dict;
    tl_simplifier_options options;
    language_containment_checker lcc;

    ~tl_simplifier_cache()
    {
      dict->unregister_all_my_variables(this);
    }

    tl_simplifier_cache(const bdd_dict_ptr& d)
      : dict(d), lcc(d, true, true, false, false)
    {
    }

    tl_simplifier_cache(const bdd_dict_ptr& d,
                         const tl_simplifier_options& opt)
      : dict(d), options(opt), lcc(d, true, true, false, false)
    {
      options.containment_checks |= options.containment_checks_stronger;
      options.event_univ |= options.favor_event_univ;
    }

    void
    print_stats(std::ostream& os) const
    {
      os << "simplified formulae:    " << simplified_.size() << " entries\n"
         << "negative normal form:   " << nenoform_.size() << " entries\n"
         << "syntactic implications: " << syntimpl_.size() << " entries\n"
         << "boolean to bdd:         " << as_bdd_.size() << " entries\n"
         << "star normal form:       " << snf_cache_.size() << " entries\n"
         << "boolean isop:           " << bool_isop_.size() << " entries\n"
         << "as dnf:                 " << as_dnf_.size() << " entries\n"
         << "as cnf:                 " << as_cnf_.size() << " entries\n";
    }

    void
    clear_as_bdd_cache()
    {
      as_bdd_.clear();
      for (auto p: bdd_to_f_)
        dict->unregister_variable(p.first, this);
      bdd_to_f_.clear();
    }

    // Convert a Boolean formula into a BDD for easier comparison.
    bdd
    as_bdd(formula f)
    {
      // Lookup the result in case it has already been computed.
      f2b_map::const_iterator it = as_bdd_.find(f);
      if (it != as_bdd_.end())
        return it->second;

      bdd result = bddfalse;

      switch (f.kind())
        {
        case op::tt:
          result = bddtrue;
          break;
        case op::ff:
          result = bddfalse;
          break;
        case op::ap:
          result = bdd_ithvar(dict->register_proposition(f, this));
          break;
        case op::Not:
          result = !as_bdd(f[0]);
          break;
        case op::Xor:
          result = bdd_apply(as_bdd(f[0]), as_bdd(f[1]), bddop_xor);
          break;
        case op::Implies:
          result = bdd_apply(as_bdd(f[0]), as_bdd(f[1]), bddop_imp);
          break;
        case op::Equiv:
          result = bdd_apply(as_bdd(f[0]), as_bdd(f[1]), bddop_biimp);
          break;
        case op::And:
          {
            result = bddtrue;
            for (auto c: f)
              result &= as_bdd(c);
            break;
          }
        case op::Or:
          {
            result = bddfalse;
            for (auto c: f)
              result |= as_bdd(c);
            break;
          }
        default:
          {
            unsigned var = dict->register_anonymous_variables(1, this);
            bdd_to_f_[var] = f;
            result = bdd_ithvar(var);
            break;
          }
        }

      // Cache the result before returning.
      as_bdd_[f] = result;
      return result;
    }

    formula as_xnf(formula f, bool cnf)
    {
      bdd in = as_bdd(f);
      if (cnf)
        in = !in;
      minato_isop isop(in);
      bdd cube;
      vec clauses;
      while ((cube = isop.next()) != bddfalse)
        {
          vec literals;
          while (cube != bddtrue)
            {
              int var = bdd_var(cube);
              const bdd_dict::bdd_info& i = dict->bdd_map[var];
              formula res;
              if (i.type == bdd_dict::var)
                {
                  res = i.f;
                }
              else
                {
                  res = bdd_to_f_[var];
                  assert(res);
                }
              bdd high = bdd_high(cube);
              if (high == bddfalse)
                {
                  if (!cnf)
                    res = formula::Not(res);
                  cube = bdd_low(cube);
                }
              else
                {
                  if (cnf)
                    res = formula::Not(res);
                  // If bdd_low is not false, then cube was not a
                  // conjunction.
                  assert(bdd_low(cube) == bddfalse);
                  cube = high;
                }
              assert(cube != bddfalse);
              literals.emplace_back(res);
            }
          if (cnf)
            clauses.emplace_back(formula::Or(literals));
          else
            clauses.emplace_back(formula::And(literals));
        }
      if (cnf)
        return formula::And(clauses);
      else
        return formula::Or(clauses);
    }

    formula as_dnf(formula f)
    {
      auto i = as_dnf_.find(f);
      if (i != as_dnf_.end())
        return i->second;
      formula r = as_xnf(f, false);
      as_dnf_[f] = r;
      return r;
    }

    formula as_cnf(formula f)
    {
      auto i = as_cnf_.find(f);
      if (i != as_cnf_.end())
        return i->second;
      formula r = as_xnf(f, true);
      as_cnf_[f] = r;
      return r;
    }



    formula
    lookup_nenoform(formula f)
    {
      f2f_map::const_iterator i = nenoform_.find(f);
      if (i == nenoform_.end())
        return nullptr;
      return i->second;
    }

    void
    cache_nenoform(formula orig, formula nenoform)
    {
      nenoform_[orig] = nenoform;
    }

    // Return true iff the option set (syntactic implication
    // or containment checks) allow to prove that f1 => f2.
    bool
    implication(formula f1, formula f2)
    {
      trace << "[->] does " << str_psl(f1) << " implies "
            << str_psl(f2) << " ?" << std::endl;
      if ((options.synt_impl && syntactic_implication(f1, f2))
          || (options.containment_checks && contained(f1, f2)))
        {
          trace << "[->] Yes" << std::endl;
          return true;
        }
      trace << "[->] No" << std::endl;
      return false;
    }

    // Return true if f1 => f2 syntactically
    bool
    syntactic_implication(formula f1, formula f2);
    bool
    syntactic_implication_aux(formula f1, formula f2);

    // Return true if f1 => f2
    bool
    contained(formula f1, formula f2)
    {
      if (!f1.is_psl_formula() || !f2.is_psl_formula())
        return false;
      return lcc.contained(f1, f2);
    }

    // If right==false, true if !f1 => f2, false otherwise.
    // If right==true, true if f1 => !f2, false otherwise.
    bool
    syntactic_implication_neg(formula f1, formula f2,
                              bool right);

    // Return true if f1 => !f2
    bool contained_neg(formula f1, formula f2)
    {
      if (!f1.is_psl_formula() || !f2.is_psl_formula())
        return false;
      trace << "[CN] Does (" << str_psl(f1) << ") imply !("
            << str_psl(f2) << ") ?" << std::endl;
      if (lcc.contained_neg(f1, f2))
        {
          trace << "[CN] Yes" << std::endl;
          return true;
        }
      else
        {
          trace << "[CN] No" << std::endl;
          return false;
        }
    }

    // Return true if !f1 => f2
    bool neg_contained(formula f1, formula f2)
    {
      if (!f1.is_psl_formula() || !f2.is_psl_formula())
        return false;
      trace << "[NC] Does !(" << str_psl(f1) << ") imply ("
            << str_psl(f2) << ") ?" << std::endl;
      if (lcc.neg_contained(f1, f2))
        {
          trace << "[NC] Yes" << std::endl;
          return true;
        }
      else
        {
          trace << "[NC] No" << std::endl;
          return false;
        }
    }

    // Return true iff the option set (syntactic implication
    // or containment checks) allow to prove that
    //   - !f1 => f2   (case where right=false)
    //   - f1 => !f2   (case where right=true)
    bool
    implication_neg(formula f1, formula f2, bool right)
    {
      trace << "[IN] Does " << (right ? "(" : "!(")
            << str_psl(f1) << ") imply "
            << (right ? "!(" : "(") << str_psl(f2) << ") ?"
            << std::endl;
      if ((options.synt_impl && syntactic_implication_neg(f1, f2, right))
          || (options.containment_checks && right && contained_neg(f1, f2))
          || (options.containment_checks && !right && neg_contained(f1, f2)))
        {
          trace << "[IN] Yes" << std::endl;
          return true;
        }
      else
        {
          trace << "[IN] No" << std::endl;
          return false;
        }
    }

    formula
    lookup_simplified(formula f)
    {
      f2f_map::const_iterator i = simplified_.find(f);
      if (i == simplified_.end())
        return nullptr;
      return i->second;
    }

    void
    cache_simplified(formula orig, formula simplified)
    {
      simplified_[orig] = simplified;
    }

    formula
    star_normal_form(formula f)
    {
      return spot::star_normal_form(f, &snf_cache_);
    }

    formula
    star_normal_form_bounded(formula f)
    {
      return spot::star_normal_form_bounded(f, &snfb_cache_);
    }


    formula
    boolean_to_isop(formula f)
    {
      f2f_map::const_iterator it = bool_isop_.find(f);
      if (it != bool_isop_.end())
        return it->second;

      assert(f.is_boolean());
      formula res = bdd_to_formula(as_bdd(f), dict);
      bool_isop_[f] = res;
      return res;
    }

  private:
    f2b_map as_bdd_;
    b2f_map bdd_to_f_;
    f2f_map simplified_;
    f2f_map as_dnf_;
    f2f_map as_cnf_;
    f2f_map nenoform_;
    syntimpl_cache_t syntimpl_;
    snf_cache snf_cache_;
    snf_cache snfb_cache_;
    f2f_map bool_isop_;
  };


  namespace
  {
    //////////////////////////////////////////////////////////////////////
    //
    //  NEGATIVE_NORMAL_FORM
    //
    //////////////////////////////////////////////////////////////////////

    formula
    nenoform_rec(formula f, bool negated, tl_simplifier_cache* c,
                 bool deep);

    formula equiv_or_xor(bool equiv, formula f1, formula f2,
                         tl_simplifier_cache* c, bool deep)
    {
      auto rec = [c, deep](formula f, bool negated)
        {
          return nenoform_rec(f, negated, c, deep);
        };

      if (equiv)
        {
          // Rewrite a<=>b as (a&b)|(!a&!b)
          auto recurse_f1_false = rec(f1, false);
          auto recurse_f2_false = rec(f2, false);
          if (!deep && c->options.keep_top_xor)
            return formula::Equiv(recurse_f1_false, recurse_f2_false);
          auto recurse_f1_true = rec(f1, true);
          auto recurse_f2_true = rec(f2, true);
          auto left = formula::And({recurse_f1_false, recurse_f2_false});
          auto right = formula::And({recurse_f1_true, recurse_f2_true});
          return formula::Or({left, right});
        }
      else
        {
          // Rewrite a^b as (a&!b)|(!a&b)
          auto recurse_f1_false = rec(f1, false);
          auto recurse_f2_false = rec(f2, false);
          if (!deep && c->options.keep_top_xor)
            return formula::Xor(recurse_f1_false, recurse_f2_false);
          auto recurse_f1_true = rec(f1, true);
          auto recurse_f2_true = rec(f2, true);
          auto left = formula::And({recurse_f1_false, recurse_f2_true});
          auto right = formula::And({recurse_f1_true, recurse_f2_false});
          return formula::Or({left, right});
        }
    }

    // The deep argument indicate whether we are under a temporal
    // operator (except X).
    formula
    nenoform_rec(formula f, bool negated, tl_simplifier_cache* c,
                 bool deep)
    {
      if (f.is(op::Not))
        {
          negated = !negated;
          f = f[0];
        }

      formula key = f;
      if (negated)
        key = formula::Not(f);
      formula result = c->lookup_nenoform(key);
      if (result)
        return result;

      if (key.is_in_nenoform()
          || (c->options.nenoform_stop_on_boolean && key.is_boolean()))
        {
          result = key;
        }
      else
        {
          auto rec = [c, &deep](formula f, bool neg)
            {
              return nenoform_rec(f, neg, c, deep);
            };

          switch (op o = f.kind())
            {
            case op::ff:
            case op::tt:
              // Negation of constants is taken care of in the
              // constructor of unop::Not, so these cases should be
              // caught by nenoform_recursively().
              assert(!negated);
              result = f;
              break;
            case op::ap:
              result = negated ? formula::Not(f) : f;
              break;
            case op::X:
            case op::strong_X:
              // Currently we don't distinguish between weak and
              // strong semantics, so we treat the two operators
              // identically.
              //
              //   !Xa == X!a
              //   !X[!]a = X!a
              result = formula::X(rec(f[0], negated));
              break;
            case op::F:
              // !Fa == G!a
              deep = true;
              result = formula::unop(negated ? op::G : op::F,
                                     rec(f[0], negated));
              break;
            case op::G:
              // !Ga == F!a
              deep = true;
              result = formula::unop(negated ? op::F : op::G,
                                     rec(f[0], negated));
              break;
            case op::Closure:
              deep = true;
              result = formula::unop(negated ?
                                     op::NegClosure : op::Closure,
                                     rec(f[0], false));
              break;
            case op::NegClosure:
            case op::NegClosureMarked:
              deep = true;
              result = formula::unop(negated ? op::Closure : o,
                                     rec(f[0], false));
              break;

            case op::Implies:
              if (negated)
                // !(a => b) == a & !b
                {
                  auto f2 = rec(f[1], true);
                  result = formula::And({rec(f[0], false), f2});
                }
              else // a => b == !a | b
                {
                  auto f2 = rec(f[1], false);
                  result = formula::Or({rec(f[0], true), f2});
                }
              break;
            case op::Xor:
              {
                // !(a ^ b) == a <=> b
                result = equiv_or_xor(negated, f[0], f[1], c, deep);
                break;
              }
            case op::Equiv:
              {
                // !(a <=> b) == a ^ b
                result = equiv_or_xor(!negated, f[0], f[1], c, deep);
                break;
              }
            case op::U:
              {
                deep = true;
                // !(a U b) == !a R !b
                auto f1 = rec(f[0], negated);
                auto f2 = rec(f[1], negated);
                result = formula::binop(negated ? op::R : op::U, f1, f2);
                break;
              }
            case op::R:
              {
                deep = true;
                // !(a R b) == !a U !b
                auto f1 = rec(f[0], negated);
                auto f2 = rec(f[1], negated);
                result = formula::binop(negated ? op::U : op::R, f1, f2);
                break;
              }
            case op::W:
              {
                deep = true;
                // !(a W b) == !a M !b
                auto f1 = rec(f[0], negated);
                auto f2 = rec(f[1], negated);
                result = formula::binop(negated ? op::M : op::W, f1, f2);
                break;
              }
            case op::M:
              {
                deep = true;
                // !(a M b) == !a W !b
                auto f1 = rec(f[0], negated);
                auto f2 = rec(f[1], negated);
                result = formula::binop(negated ? op::W : op::M, f1, f2);
                break;
              }
            case op::Or:
            case op::And:
              {
                unsigned mos = f.size();
                vec v;
                for (unsigned i = 0; i < mos; ++i)
                  v.emplace_back(rec(f[i], negated));
                op on = o;
                if (negated)
                  on = o == op::Or ? op::And : op::Or;
                result = formula::multop(on, v);
                break;
              }
            case op::OrRat:
            case op::AndRat:
            case op::AndNLM:
            case op::Concat:
            case op::Fusion:
            case op::Star:
            case op::FStar:
            case op::first_match:
              // !(a*) etc. should never occur.
              {
                assert(!negated);
                result = f.map([c, deep](formula f)
                               {
                                 return nenoform_rec(f, false, c, deep);
                               });
                break;
              }
            case op::EConcat:
            case op::EConcatMarked:
              {
                deep = true;
                // !(a <>-> b) == a[]-> !b
                auto f1 = f[0];
                auto f2 = f[1];
                result = formula::binop(negated ? op::UConcat : o,
                                        rec(f1, false), rec(f2, negated));
                break;
              }
            case op::UConcat:
              {
                deep = true;
                // !(a []-> b) == a<>-> !b
                auto f1 = f[0];
                auto f2 = f[1];
                result = formula::binop(negated ? op::EConcat : op::UConcat,
                                        rec(f1, false), rec(f2, negated));
                break;
              }
            case op::eword:
            case op::Not:
              SPOT_UNREACHABLE();
            }
        }

      c->cache_nenoform(key, result);
      return result;
    }

    //////////////////////////////////////////////////////////////////////
    //
    //  SIMPLIFY_VISITOR
    //
    //////////////////////////////////////////////////////////////////////

    // Forward declaration.
    formula
    simplify_recursively(formula f, tl_simplifier_cache* c);

    // X(a) R b   or   X(a) M b
    // This returns a.
    formula
    is_XRM(formula f)
    {
      if (!f.is(op::R, op::M))
        return nullptr;
      auto left = f[0];
      if (!left.is(op::X))
        return nullptr;
      return left[0];
    }

    // X(a) W b   or   X(a) U b
    // This returns a.
    formula
    is_XWU(formula f)
    {
      if (!f.is(op::W, op::U))
        return nullptr;
      auto left = f[0];
      if (!left.is(op::X))
        return nullptr;
      return left[0];
    }

    // b & X(b W a)  or   b & X(b U a)
    // This returns (b W a) or (b U a).
    formula
    is_bXbWU(formula f)
    {
      if (!f.is(op::And))
        return nullptr;
      unsigned s = f.size();
      for (unsigned pos = 0; pos < s; ++pos)
        {
          auto p = f[pos];
          if (!(p.is(op::X)))
            continue;
          auto c = p[0];
          if (!c.is(op::U, op::W))
            continue;
          formula b = f.all_but(pos);
          if (b == c[0])
            return c;
        }
      return nullptr;
    }

    // b | X(b R a)  or   b | X(b M a)
    // This returns (b R a) or (b M a).
    formula
    is_bXbRM(formula f)
    {
      if (!f.is(op::Or))
        return nullptr;
      unsigned s = f.size();
      for (unsigned pos = 0; pos < s; ++pos)
        {
          auto p = f[pos];
          if (!(p.is(op::X)))
            continue;
          auto c = p[0];
          if (!c.is(op::R, op::M))
            continue;
          formula b = f.all_but(pos);
          if (b == c[0])
            return c;
        }
      return nullptr;
    }

    formula
    unop_multop(op uop, op mop, vec v)
    {
      formula f = formula::unop(uop, formula::multop(mop, v));
      if (f.is(op::X) && f[0].is_ff())
        return formula::ff();
      return f;
    }

    formula
    unop_unop_multop(op uop1, op uop2, op mop, vec v)
    {
      return formula::unop(uop1, unop_multop(uop2, mop, v));
    }

    formula
    unop_unop(op uop1, op uop2, formula f)
    {
      return formula::unop(uop1, formula::unop(uop2, f));
    }

    struct mospliter
    {
      enum what { Split_GF = (1 << 0),
                  Strip_GF = (1 << 1) | (1 << 0),
                  Split_FG = (1 << 2),
                  Strip_FG = (1 << 3) | (1 << 2),
                  Split_F = (1 << 4),
                  Strip_F = (1 << 5) | (1 << 4),
                  Split_G = (1 << 6),
                  Strip_G = (1 << 7) | (1 << 6),
                  Strip_X = (1 << 8),
                  Split_U_or_W = (1 << 9),
                  Split_R_or_M = (1 << 10),
                  Split_EventUniv = (1 << 11),
                  Split_Event = (1 << 12),
                  Split_Univ = (1 << 13),
                  Split_Bool = (1 << 14)
      };

    private:
      mospliter(unsigned split, tl_simplifier_cache* cache)
        : split_(split), c_(cache),
          res_GF{(split_ & Split_GF) ? new vec : nullptr},
          res_FG{(split_ & Split_FG) ? new vec : nullptr},
          res_F{(split_ & Split_F) ? new vec : nullptr},
          res_G{(split_ & Split_G) ? new vec : nullptr},
          res_X{(split_ & Strip_X) ? new vec : nullptr},
          res_U_or_W{(split_ & Split_U_or_W) ? new vec : nullptr},
          res_R_or_M{(split_ & Split_R_or_M) ? new vec : nullptr},
          res_Event{(split_ & Split_Event) ? new vec : nullptr},
          res_Univ{(split_ & Split_Univ) ? new vec : nullptr},
          res_EventUniv{(split_ & Split_EventUniv) ? new vec : nullptr},
          res_Bool{(split_ & Split_Bool) ? new vec : nullptr},
          res_other{new vec}
      {
      }

    public:
      mospliter(unsigned split, vec v, tl_simplifier_cache* cache)
        : mospliter(split, cache)
      {
        for (auto f: v)
          {
            if (f) // skip null pointers left by previous simplifications
              process(f);
          }
      }

      mospliter(unsigned split, formula mo,
                tl_simplifier_cache* cache)
        : mospliter(split, cache)
      {
        unsigned mos = mo.size();
        for (unsigned i = 0; i < mos; ++i)
          {
            formula f = simplify_recursively(mo[i], cache);
            process(f);
          }
      }

      void process(formula f)
      {
        bool e = f.is_eventual();
        bool u = f.is_universal();
        bool eu = res_EventUniv && e & u && c_->options.event_univ;
        switch (f.kind())
          {
          case op::X:
          case op::strong_X:
            if (res_X && !eu)
              {
                res_X->emplace_back(f[0]);
                return;
              }
            break;
          case op::F:
            {
              formula c = f[0];
              if (res_FG && u && c.is(op::G))
                {
                  res_FG->emplace_back(((split_ & Strip_FG) == Strip_FG
                                     ? c[0] : f));
                  return;
                }
              if (res_F && !eu)
                {
                  res_F->emplace_back(((split_ & Strip_F) == Strip_F
                                    ? c : f));
                  return;
                }
              break;
            }
          case op::G:
            {
              formula c = f[0];
              if (res_GF && e && c.is(op::F))
                {
                  res_GF->emplace_back(((split_ & Strip_GF) == Strip_GF
                                     ? c[0] : f));
                  return;
                }
              if (res_G && !eu)
                {
                  res_G->emplace_back(((split_ & Strip_G) == Strip_G
                                    ? c : f));
                  return;
                }
              break;
            }
          case op::U:
          case op::W:
            if (res_U_or_W)
              {
                res_U_or_W->emplace_back(f);
                return;
              }
            break;
          case op::R:
          case op::M:
            if (res_R_or_M)
              {
                res_R_or_M->emplace_back(f);
                return;
              }
            break;
          default:
            if (res_Bool && f.is_boolean())
              {
                res_Bool->emplace_back(f);
                return;
              }
            break;
          }
        if (c_->options.event_univ)
          {
            if (res_EventUniv && e && u)
              {
                res_EventUniv->emplace_back(f);
                return;
              }
            if (res_Event && e)
              {
                res_Event->emplace_back(f);
                return;
              }
            if (res_Univ && u)
              {
                res_Univ->emplace_back(f);
                return;
              }
          }
        res_other->emplace_back(f);
      }

      unsigned split_;
      tl_simplifier_cache* c_;
      std::unique_ptr<vec> res_GF;
      std::unique_ptr<vec> res_FG;
      std::unique_ptr<vec> res_F;
      std::unique_ptr<vec> res_G;
      std::unique_ptr<vec> res_X;
      std::unique_ptr<vec> res_U_or_W;
      std::unique_ptr<vec> res_R_or_M;
      std::unique_ptr<vec> res_Event;
      std::unique_ptr<vec> res_Univ;
      std::unique_ptr<vec> res_EventUniv;
      std::unique_ptr<vec> res_Bool;
      std::unique_ptr<vec> res_other;
    };

    class simplify_visitor final
    {
    public:

      simplify_visitor(tl_simplifier_cache* cache)
        : c_(cache), opt_(cache->options)
        {
        }

      // if !neg build c&X(c&X(...&X(tail))) with n occurences of c
      // if neg build !c|X(!c|X(...|X(tail))).
      formula
        dup_b_x_tail(bool neg, formula c, formula tail, unsigned n)
      {
        op mop;
        if (neg)
          {
            c = formula::Not(c);
            mop = op::Or;
          }
        else
          {
            mop = op::And;
          }
        while (n--)
          tail = // b&X(tail) or !b|X(tail)
            formula::multop(mop, {c, formula::X(tail)});
        return tail;
      }

      formula
        visit(formula f)
      {
        formula result = f;

        auto recurse = [this](formula f)
          {
            return simplify_recursively(f, c_);
          };

        f = f.map(recurse);

        switch (op o = f.kind())
          {
          case op::ff:
          case op::tt:
          case op::eword:
          case op::ap:
          case op::Not:
          case op::FStar:
            return f;
          case op::X:
          case op::strong_X:
            {
              formula c = f[0];
              // The following rules are not trivial simplifications,
              // because they are not true in LTLf.
              //   X(0)=0
              //   X[!](1)=1
              if (c.is_constant())
                return c;
              // Xf = f if f is both eventual and universal.
              if (c.is_universal() && c.is_eventual())
                {
                  if (opt_.event_univ)
                    return c;
                  // If EventUniv simplification is disabled, use
                  // only the following basic rewriting rules:
                  //   XGF(f) = GF(f) and XFG(f) = FG(f)
                  // The former comes from Somenzi&Bloem (CAV'00).
                  // It's not clear why they do not list the second.
                  if (opt_.reduce_basics &&
                      (c.is({op::G, op::F}) || c.is({op::F, op::G})))
                    return c;
                }

              // If Xa = a, keep only a.
              if (opt_.containment_checks_stronger
                  && c_->lcc.equal(f, c))
                return c;

              // X(f1 & GF(f2)) = X(f1) & GF(f2)
              // X(f1 | GF(f2)) = X(f1) | GF(f2)
              // X(f1 & FG(f2)) = X(f1) & FG(f2)
              // X(f1 | FG(f2)) = X(f1) | FG(f2)
              //
              // The above usually make more sense when reversed (see
              // them in the And and Or rewritings), except when we
              // try to maximaze the size of subformula that do not
              // have EventUniv formulae.
              if (opt_.favor_event_univ)
                if (c.is(op::Or, op::And))
                  {
                    mospliter s(mospliter::Split_EventUniv, c, c_);
                    op oc = c.kind();
                    s.res_EventUniv->
                      emplace_back(unop_multop(op::X, oc,
                                            std::move(*s.res_other)));
                    formula result =
                      formula::multop(oc,
                                      std::move(*s.res_EventUniv));
                    if (result != f)
                      result = recurse(result);
                    return result;
                  }
              return f;
            }
          case op::F:
            {
              formula c = f[0];
              // If f is a pure eventuality formula then F(f)=f.
              if (opt_.event_univ && c.is_eventual())
                return c;

              auto g_in_f = [this](formula g, std::vector<formula>* to,
                                   std::vector<formula>* eventual = nullptr)
                {
                  if (g[0].is(op::Or))
                    {
                      mospliter s2(mospliter::Split_Univ |
                                   (eventual ? mospliter::Split_Event : 0),
                                   g[0], c_);
                      for (formula e: *s2.res_Univ)
                        to->push_back(e.is(op::X) ? e[0] : e);
                      to->push_back
                      (unop_multop(op::G, op::Or,
                                   std::move(*s2.res_other)));
                      if (eventual)
                        std::swap(*s2.res_Event, *eventual);
                    }
                  else
                    {
                      to->push_back(g);
                    }
                };

              if (opt_.reduce_basics)
                {
                  // F(a U b) = F(b)
                  if (c.is(op::U))
                    return recurse(formula::F(c[1]));

                  // F(a M b) = F(a & b)
                  if (c.is(op::M))
                    return recurse(unop_multop(op::F, op::And,
                                               {c[0], c[1]}));

                  // FX(a) = XF(a)
                  // FXX(a) = XXF(a) ...
                  // FXG(a) = XFG(a) = FG(a) ...
                  if (c.is(op::X))
                    return recurse(unop_unop(op::X, op::F, c[0]));

                  // F(G(a | Gb)) = F(Ga | Gb)
                  // F(G(a | Fb)) = FGa | GFb // opt_.favor_event_univ
                  if (c.is({op::G, op::Or}))
                    {
                      std::vector<formula> toadd, eventual;
                      g_in_f(c, &toadd,
                             opt_.favor_event_univ ? &eventual : nullptr);
                      formula res = unop_multop(op::F, op::Or,
                                                std::move(toadd));
                      if (!eventual.empty())
                        {
                          formula ev = unop_multop(op::G, op::Or,
                                                   std::move(eventual));
                          res = formula::Or({res, ev});
                        }
                      if (res != f)
                        return recurse(res);
                    }

                  // F(G(a & Fb) = FGa & GFb // !opt_.reduce_size_strictly
                  if (c.is({op::G, op::And}) && !opt_.reduce_size_strictly)
                    {
                      mospliter s2(mospliter::Split_Event, c[0], c_);
                      for (formula& e: *s2.res_Event)
                        while (e.is(op::X))
                          e = e[0];
                      formula fg = unop_unop_multop(op::F, op::G, op::And,
                                                    std::move(*s2.res_other));
                      formula gf = unop_multop(op::G, op::And,
                                               std::move(*s2.res_Event));
                      formula res = formula::And({fg, gf});
                      if (res != f)
                        return recurse(res);
                    }

                  // FG(a) = FG(dnf(a)) if a is not Boolean
                  // and contains some | above non-Boolean subformulas.
                  if (c.is(op::G) && !c[0].is_boolean())
                    {
                      formula m = c[0];
                      bool want_cnf = m.is(op::And);
                      if (!want_cnf && m.is(op::Or))
                        for (auto cc : m)
                          if (cc.is(op::And))
                            {
                              want_cnf = true;
                              break;
                            }
                      if (want_cnf && !opt_.reduce_size_strictly)
                        m = c_->as_cnf(m);
                      // FG(a & Xb) = FG(a & b)
                      // FG(a & Gb) = FG(a & b)
                      if (m.is(op::And))
                        m = m.map([](formula f)
                                  {
                                    if (f.is(op::X, op::G))
                                      return f[0];
                                    return f;
                                  });
                      if (c[0] != m)
                        return recurse(unop_unop(op::F, op::G, m));
                    }
                }
              // if Fa => a, keep a.
              if (opt_.containment_checks_stronger
                  && c_->lcc.contained(f, c))
                return c;

              // Disabled by default:
              //     F(f1 & GF(f2)) = F(f1) & GF(f2)
              //
              // As is, these two formulae are translated into
              // equivalent Büchi automata so the rewriting is
              // useless.
              //
              // However when taken in a larger formula such
              // as F(f1 & GF(f2)) | F(a & GF(b)), this
              // rewriting used to produce (F(f1) & GF(f2)) |
              // (F(a) & GF(b)), missing the opportunity to
              // apply the F(E1)|F(E2) = F(E1|E2) rule which
              // really helps the translation. F((f1 & GF(f2))
              // | (a & GF(b))) is indeed easier to translate.
              //
              // So we do not consider this rewriting rule by
              // default.  However if favor_event_univ is set,
              // we want to move the GF out of the F.
              //
              // Also if this appears inside a G, we want to
              // reduce it:
              //     GF(f1 & GF(f2)) = G(F(f1) & GF(f2))
              //                     = G(F(f1) & F(f2))
              // But this is handled by the G case.
              if (opt_.favor_event_univ)
                // F(f1&f2&FG(f3)&FG(f4)&f5&f6) =
                //                     F(f1&f2) & FG(f3&f4) & f5 & f6
                // if f5 and f6 are both eventual and universal.
                if (c.is(op::And))
                  {
                    mospliter s(mospliter::Strip_FG |
                                mospliter::Split_EventUniv,
                                c, c_);
                    s.res_EventUniv->
                      emplace_back(unop_multop(op::F, op::And,
                                            std::move(*s.res_other)));
                    s.res_EventUniv->
                      emplace_back(unop_unop_multop(op::F, op::G, op::And,
                                                 std::move(*s.res_FG)));
                    formula res =
                      formula::And(std::move(*s.res_EventUniv));
                    if (res != f)
                      return recurse(res);
                  }
              // If u3 and u4 are universal formulae and h is not:
              // F(f1 | f2 | Fu3 | u4 | FGg | Fh | Xu5 | G(f6 | Xu7 | u8))
              //    = F(f1 | f2 | u3 | u4 | Gg | h | u5 | Gf6 | u7 | u8)
              // or
              // F(f1 | f2 | Fu3 | u4 | FGg | Fh | Xu5)
              //    = F(f1 | f2 | h) | F(u3 | u4 | Gg | u5 | Gf6 | u7 | u8)
              // depending on whether favor_event_univ is set.
              if (c.is(op::Or))
                {
                  int w = mospliter::Strip_F
                    | mospliter::Strip_X | mospliter::Split_G;
                  if (opt_.favor_event_univ)
                    w |= mospliter::Split_Univ;
                  mospliter s(w, c, c_);
                  s.res_other->insert(s.res_other->end(),
                                      s.res_F->begin(), s.res_F->end());
                  for (formula f: *s.res_X)
                    if (f.is_universal())
                      s.res_other->push_back(f);
                    else
                      s.res_other->push_back(formula::X(f));
                  std::vector<formula>* to = opt_.favor_event_univ ?
                    s.res_Univ.get() : s.res_other.get();
                  for (formula g: *s.res_G)
                    g_in_f(g, to);
                  formula res = unop_multop(op::F, op::Or,
                                            std::move(*s.res_other));
                  if (s.res_Univ)
                    {
                      std::vector<formula> toadd;
                      for (auto& g: *s.res_Univ)
                        // Strip any F or X
                        if (g.is(op::F, op::X))
                          g = g[0];
                      s.res_Univ->insert(s.res_Univ->end(),
                                         toadd.begin(), toadd.end());
                      formula fu = unop_multop(op::F, op::Or,
                                               std::move(*s.res_Univ));
                      res = formula::Or({res, fu});
                    }
                  if (res != f)
                    return recurse(res);
                }
            }
            return f;
          case op::G:
            {
              formula c = f[0];
              // If f is a pure universality formula then G(f)=f.
              if (opt_.event_univ && c.is_universal())
                return c;

              auto f_in_g = [this](formula f, std::vector<formula>* to,
                                   std::vector<formula>* univ = nullptr)
                {
                  if (f[0].is(op::And))
                    {
                      mospliter s2(mospliter::Split_Event |
                                   (univ ? mospliter::Split_Univ : 0),
                                   f[0], c_);
                      for (formula e: *s2.res_Event)
                        to->push_back(e.is(op::X) ? e[0] : e);
                      to->push_back
                      (unop_multop(op::F, op::And,
                                   std::move(*s2.res_other)));
                      if (univ)
                        std::swap(*s2.res_Univ, *univ);
                    }
                  else
                    {
                      to->push_back(f);
                    }
                };

              if (opt_.reduce_basics)
                {
                  // G(a R b) = G(b)
                  if (c.is(op::R))
                    return recurse(formula::G(c[1]));

                  // G(a W b) = G(a | b)
                  if (c.is(op::W))
                    return recurse(unop_multop(op::G, op::Or,
                                               {c[0], c[1]}));

                  // GX(a) = XG(a)
                  // GXX(a) = XXG(a) ...
                  // GXF(a) = XGF(a) = GF(a) ...
                  if (c.is(op::X))
                    return recurse(unop_unop(op::X, op::G, c[0]));

                  // G(F(a & Fb)) = G(Fa & Fb)
                  // G(F(a & Gb)) = GFa & FGb // !opt_.reduce_size_strictly
                  if (c.is({op::F, op::And}))
                    {
                      std::vector<formula> toadd, univ;
                      f_in_g(c, &toadd,
                             opt_.reduce_size_strictly ? nullptr : &univ);
                      formula res = unop_multop(op::G, op::And,
                                                std::move(toadd));
                      if (!univ.empty())
                        {
                          formula un = unop_multop(op::F, op::And,
                                                   std::move(univ));
                          res = formula::And({res, un});
                        }
                      if (res != f)
                        return recurse(res);
                    }

                  // G(F(a | Gb)) = GFa | FGb   // opt_.favor_event_univ
                  if (c.is({op::F, op::Or}) && opt_.favor_event_univ)
                    {
                      mospliter s2(mospliter::Split_Univ, c[0], c_);
                      for (formula& u: *s2.res_Univ)
                        while (u.is(op::X))
                          u = u[0];
                      formula gf = unop_unop_multop(op::G, op::F, op::Or,
                                                    std::move(*s2.res_other));
                      formula fg = unop_multop(op::F, op::Or,
                                               std::move(*s2.res_Univ));
                      formula res = formula::Or({gf, fg});
                      if (res != f)
                        return recurse(res);
                    }

                  // G(f1|f2|GF(f3)|GF(f4)|f5|f6) =
                  //                        G(f1|f2) | GF(f3|f4) | f5 | f6
                  // if f5 and f6 are both eventual and universal.
                  if (c.is(op::Or))
                    {
                      mospliter s(mospliter::Strip_GF |
                                  mospliter::Split_EventUniv,
                                  c, c_);
                      s.res_EventUniv->
                        emplace_back(unop_multop(op::G, op::Or,
                                              std::move(*s.res_other)));
                      s.res_EventUniv->
                        emplace_back(unop_unop_multop(op::G, op::F, op::Or,
                                                   std::move(*s.res_GF)));
                      formula res =
                        formula::Or(std::move(*s.res_EventUniv));

                      if (res != f)
                        return recurse(res);
                    }
                  // If e3 and e4 are eventual formulae and h is not:
                  // G(f1 & f2 & Ge3 & e4 & GFg & Gh & Xe5 & F(f6 & Xe7 & e8))
                  //    = G(f1 & f2 & e3 & e4 & Fg & h & e5 & Ff6 & e7 & e8)
                  // or
                  // G(f1 & f2 & Ge3 & e4 & GFg & Gh & Xe5 & F(f6 & Xe7 & e8))
                  //    = G(f1 & f2 & h) & G(e3 & e4 & Fg & e5 & Ff6 & e7 & e8)
                  // depending on whether favor_event_univ is set.
                  else if (c.is(op::And))
                    {
                      int w = mospliter::Strip_G |
                        mospliter::Strip_X | mospliter::Split_F;
                      if (opt_.favor_event_univ)
                        w |= mospliter::Split_Event;
                      mospliter s(w, c, c_);
                      s.res_other->insert(s.res_other->end(),
                                          s.res_G->begin(), s.res_G->end());
                      for (formula f: *s.res_X)
                        if (f.is_eventual())
                          s.res_other->push_back(f);
                        else
                          s.res_other->push_back(formula::X(f));
                      std::vector<formula>* to = opt_.favor_event_univ ?
                        s.res_Event.get() : s.res_other.get();
                      for (formula f: *s.res_F)
                        f_in_g(f, to);

                      formula res = unop_multop(op::G, op::And,
                                                std::move(*s.res_other));
                      if (s.res_Event)
                        {
                          std::vector<formula> toadd;
                          // Strip any G or X
                          for (auto& g: *s.res_Event)
                            if (g.is(op::G, op::X))
                              {
                                g = g[0];
                              }
                          s.res_Event->insert(s.res_Event->end(),
                                              toadd.begin(), toadd.end());
                          formula ge =
                            unop_multop(op::G, op::And,
                                        std::move(*s.res_Event));
                          res = formula::And({res, ge});
                        }
                      if (res != f)
                        return recurse(res);
                    }


                  // GF(a) = GF(dnf(a)) if a is not Boolean
                  // and contains some | above non-Boolean subformulas.
                  if (c.is(op::F) && !c[0].is_boolean())
                    {
                      formula m = c[0];
                      bool want_dnf = m.is(op::Or);
                      if (!want_dnf && m.is(op::And))
                        for (auto cc : m)
                          if (cc.is(op::Or))
                            {
                              want_dnf = true;
                              break;
                            }
                      if (want_dnf && !opt_.reduce_size_strictly)
                        m = c_->as_dnf(m);
                      // GF(a | Xb) = GF(a | b)
                      // GF(a | Fb) = GF(a | b)
                      if (m.is(op::Or))
                        m = m.map([](formula f)
                                  {
                                    if (f.is(op::X, op::F))
                                      return f[0];
                                    return f;
                                  });
                      if (c[0] != m)
                        return recurse(unop_unop(op::G, op::F, m));
                    }
                  // GF(f1 & f2 & eu1 & eu2) = G(F(f1 & f2) & eu1 & eu2
                  if (opt_.event_univ && c.is({op::F, op::And}))
                    {
                      mospliter s(mospliter::Split_EventUniv,
                                  c[0], c_);
                      s.res_EventUniv->
                        emplace_back(unop_multop(op::F, op::And,
                                              std::move(*s.res_other)));
                      formula res =
                        formula::G(formula::And(std::move(*s.res_EventUniv)));
                      if (res != f)
                        return recurse(res);
                    }
                }
              // if a => Ga, keep a.
              if (opt_.containment_checks_stronger
                  && c_->lcc.contained(c, f))
                return c;
            }
            return f;
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
            {
              formula c = f[0];
              // {e[*]} = {e}
              // !{e[*]} = !{e}
              if (c.accepts_eword() && c.is(op::Star))
                return recurse(formula::unop(o, c[0]));

              if (!opt_.reduce_size_strictly)
                if (c.is(op::OrRat))
                  {
                    //  {a₁|a₂} =  {a₁}| {a₂}
                    // !{a₁|a₂} = !{a₁}&!{a₂}
                    unsigned s = c.size();
                    vec v;
                    for (unsigned n = 0; n < s; ++n)
                      v.emplace_back(formula::unop(o, c[n]));
                    return recurse(formula::multop(o == op::Closure
                                                   ? op::Or : op::And, v));
                  }
              if (c.is(op::Concat))
                {
                  if (c.accepts_eword())
                    {
                      if (opt_.reduce_size_strictly)
                        return f;
                      // If all terms accept the empty word, we have
                      // {e₁;e₂;e₃} =  {e₁}|{e₂}|{e₃}
                      // !{e₁;e₂;e₃} = !{e₁}&!{e₂}&!{e₃}
                      vec v;
                      unsigned end = c.size();
                      v.reserve(end);
                      for (unsigned i = 0; i < end; ++i)
                        v.emplace_back(formula::unop(o, c[i]));
                      return recurse(formula::multop(o == op::Closure ?
                                                     op::Or : op::And, v));
                    }

                  // Some term does not accept the empty word.
                  unsigned end = c.size() - 1;

                  // {r;1} = 1 if r accepts [*0], else {r}
                  // !{r;1} = 0 if r accepts [*0], else !{r}
                  if (c[end].is_tt())
                    {
                      formula rest = c.all_but(end);
                      if (rest.accepts_eword())
                        return o == op::Closure ? formula::tt() : formula::ff();
                      return recurse(formula::unop(o, rest));
                    }

                  // {b₁;b₂;e₁;f₁;e₂;f₂;e₂;e₃;e₄}
                  //    = b₁&X(b₂&X({e₁;f₁;e₂;f₂}))
                  // !{b₁;b₂;e₁;f₁;e₂;f₂;e₂;e₃;e₄}
                  //    = !b₁|X(!b₂|X(!{e₁;f₁;e₂;f₂}))
                  // if e denotes a term that accepts [*0]
                  // and b denotes a Boolean formula.
                  //
                  // if reduce_size_strictly is set, we simply remove
                  // the trailing e2;e3;e4.
                  while (c[end].accepts_eword())
                    --end;
                  unsigned start = 0;
                  while (start <= end)
                    {
                      formula r = c[start];
                      if (r.is_boolean() && !opt_.reduce_size_strictly)
                        ++start;
                      else
                        break;
                    }
                  unsigned s = end + 1 - start;
                  if (s != c.size())
                    {
                      bool doneg = o != op::Closure;
                      formula tail;
                      if (s > 0)
                        {
                          vec v;
                          v.reserve(s);
                          for (unsigned n = start; n <= end; ++n)
                            v.emplace_back(c[n]);
                          tail = formula::Concat(v);
                          tail = formula::unop(o, tail);
                        }
                      else
                        {
                          tail = doneg ? formula::ff() : formula::tt();
                        }

                      for (unsigned n = start; n > 0;)
                        {
                          --n;
                          formula e = c[n];
                          // {b;f} = b & X{f}
                          // !{b;f} = !b | X!{f}
                          if (e.is_boolean())
                            {
                              tail = formula::X(tail);
                              if (doneg)
                                tail = formula::Or({formula::Not(e), tail});
                              else
                                tail = formula::And({e, tail});
                            }
                        }
                      return recurse(tail);
                    }

                  // {b[*i..j];c} = b&X(b&X(... b&X{b[*0..j-i];c}))
                  // !{b[*i..j];c} = !b&X(!b&X(... !b&X!{b[*0..j-i];c}))
                  if (!opt_.reduce_size_strictly)
                    if (c[0].is(op::Star))
                      {
                        formula s = c[0];
                        formula sc = s[0];
                        unsigned min = s.min();
                        if (sc.is_boolean() && min > 0)
                          {
                            unsigned max = s.max();
                            if (max != formula::unbounded())
                              max -= min;
                            unsigned ss = c.size();
                            vec v;
                            v.reserve(ss);
                            v.emplace_back(formula::Star(sc, 0, max));
                            for (unsigned n = 1; n < ss; ++n)
                              v.emplace_back(c[n]);
                            formula tail = formula::Concat(v);
                            tail = // {b[*0..j-i]} or !{b[*0..j-i]}
                              formula::unop(o, tail);
                            tail =
                              dup_b_x_tail(o != op::Closure,
                                           sc, tail, min);
                            return recurse(tail);
                          }
                      }
                }
              // {b[*i..j]} = b&X(b&X(... b))  with i occurences of b
              // !{b[*i..j]} = !b&X(!b&X(... !b))
              if (!opt_.reduce_size_strictly)
                if (c.is(op::Star))
                  {
                    formula cs = c[0];
                    if (cs.is_boolean())
                      {
                        unsigned min = c.min();
                        assert(min > 0);
                        formula tail;
                        if (o == op::Closure)
                          tail = dup_b_x_tail(false, cs,
                                              formula::tt(), min);
                        else
                          tail = dup_b_x_tail(true, cs,
                                              formula::ff(), min);
                        return recurse(tail);
                      }
                  }
              return f;
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
            return visit_binop(f);
          case op::Or:
          case op::OrRat:
          case op::And:
          case op::AndRat:
          case op::AndNLM:
          case op::Concat:
            return visit_multop(f);
          case op::Fusion:
            return f;
          case op::Star:
            {
              if (!f.accepts_eword())
                return f;
              formula h = f[0];
              if (f.max() == 1 && h.accepts_eword())
                return h;
              auto min = 0;
              if (f.max() == formula::unbounded())
                {
                  h = c_->star_normal_form(h);
                }
              else
                {
                  h = c_->star_normal_form_bounded(h);
                  if (h.accepts_eword())
                    min = 1;
                }
              return formula::Star(h, min, f.max());
            }
          case op::first_match:
            {
              formula h = f[0];
              if (h.is(op::Star))
                {
                  // first_match(b[*i..j]) = b[*i]
                  // first_match(f[*i..j]) = first_match(f[*i])
                  unsigned m = h.min();
                  if (m < h.max())
                    {
                      formula s = formula::Star(h[0], m, m);
                      s = h[0].is_boolean() ? s : formula::first_match(s);
                      return recurse(s);
                    }
                }
              else if (h.is(op::FStar))
                {
                  // first_match(f[:*i..j]) = first_match(f[:*i])
                  unsigned m = h.min();
                  if (m < h.max())
                    {
                      formula s = formula::FStar(h[0], m, m);
                      return recurse(formula::first_match(s));
                    }
                }
              else if (h.is(op::Concat))
                {
                  // If b is Boolean and e accepts [*0], we have
                  // 1. first_match(b;f) ≡ b;first_match(f)
                  // 2. first_match(f;e) ≡ first_match(f)
                  // 3. first_match(first_match(f);g) =
                  //      first_match(f);first_match(g)
                  // 4. first_match(f;g[*i:j]) ≡ first_match(f;g[*i])
                  // 5. first_match(f;g[:*i..j]) = first_match(f;g[:*i])
                  // 6. first_match(b[*i:j];f) ≡ b[*i];first_match(b[*0:j-i];f)
                  // Rules 1-3 can be repeated, so we will loop
                  // to save the recursion and intermediate caching.

                  // Extract Boolean formulas at the beginning.
                  int i = 0;
                  int n = h.size();
                  while (i < n
                         && (h[i].is_boolean() || h[i].is(op::first_match)))
                    ++i;
                  vec prefix;
                  // +1 to append first_match(suffix), +1 to for rule 6.
                  prefix.reserve(i + 2);
                  for (int ii = 0; ii < i; ++ii)
                    prefix.push_back(h[ii]);
                  // Extract suffix, minus trailing formulas that accept [*0].
                  // "i" is the start of the suffix.
                  do
                    --n;
                  while (i <= n && h[n].accepts_eword());
                  vec suffix;
                  suffix.reserve(n + 1 - i);
                  for (int ii = i; ii <= n; ++ii)
                    suffix.push_back(h[ii]);
                  // Rules 4-5
                  if (!suffix.empty() && suffix.back().is(op::Star, op::FStar))
                    {
                      formula s = suffix.back();
                      unsigned smin = s.min();
                      suffix.back() = formula::bunop(s.kind(),
                                                     s[0], smin, smin);
                    }
                  // Rule 6
                  if (!suffix.empty() && suffix.front().is(op::Star))
                    {
                      formula s = suffix.front();
                      unsigned smin = s.min();
                      if (smin > 0 && s[0].is_boolean())
                        {
                          prefix.push_back(formula::Star(s[0], smin, smin));
                          suffix.front() =
                            formula::Star(s[0], 0, s.max() - smin);
                        }
                    }
                  prefix.push_back(formula::first_match
                                   (formula::Concat(suffix)));
                  formula res = formula::Concat(prefix);
                  if (res != f)
                    return recurse(res);
                }
              else if (h.is(op::Fusion))
                {
                  // 1. first_match(b:f) = b:first_match(f) if f rejects [*0]
                  // (not implemented, because first_match will build a DFA
                  // and limiting its choices is good)
                  // 2. first_match(b[*i..j]:f) =
                  //       b[*i-1];first_match(b[*1..j-i+1]:f) if i>1
                  if (h[0].is(op::Star))
                    {
                      formula s = h[0];
                      unsigned smin = s.min();
                      if (smin > 1 && s[0].is_boolean())
                        {
                          --smin;
                          unsigned smax = s.max();
                          if (smax != formula::unbounded())
                            smax -= smin;
                          formula s2 = formula::Star(s[0], 1, smax);
                          formula in = formula::Fusion({s2, h.all_but(0)});
                          in = formula::first_match(in);
                          formula s3 = formula::Star(s[0], smin, smin);
                          return recurse(formula::Concat({s3, in}));
                        }
                    }
                  // 3. first_match(first_match(f):g) =
                  //       first_match(f):first_match(1:g)
                  if (h[0].is(op::first_match))
                    {
                      formula rest = h.all_but(0);
                      if (rest.accepts_eword())
                        rest = formula::Fusion({formula::tt(), rest});
                      rest = formula::first_match(rest);
                      return recurse(formula::Fusion({h[0], rest}));
                    }
                  // 4. first_match(f:g[*i..j]) = first_match(f:g[*max(1,i)])
                  // 5. first_match(f:g[:*i..j]) = first_match(f:g[:*i])
                  unsigned last = h.size() - 1;
                  formula tail = h[last];
                  if (tail.is(op::Star, op::FStar))
                    {
                      unsigned smin = tail.min();
                      if (smin < tail.max())
                        {
                          if (smin == 0 && tail.is(op::Star))
                            ++smin;
                          formula s2 =
                            formula::bunop(tail.kind(), tail[0], smin, smin);
                          formula in = formula::Fusion({h.all_but(last), s2});
                          return recurse(formula::first_match(in));
                        }
                    }
                }
              return f;
            }
          }
        SPOT_UNREACHABLE();
      }

      formula reduce_sere_ltl(formula orig)
      {
        op bindop = orig.kind();
        formula a = orig[0];
        formula b = orig[1];

        auto recurse = [this](formula f)
          {
            return simplify_recursively(f, c_);
          };

        // All this function is documented assuming bindop ==
        // UConcat, but by changing the following variables it can
        // perform the rules for EConcat as well.
        op op_g;
        op op_w;
        op op_r;
        op op_and;
        bool doneg;
        if (bindop == op::UConcat)
          {
            op_g = op::G;
            op_w = op::W;
            op_r = op::R;
            op_and = op::And;
            doneg = true;
          }
        else // EConcat & EConcatMarked
          {
            op_g = op::F;
            op_w = op::M;
            op_r = op::U;
            op_and = op::Or;
            doneg = false;
          }

        if (!opt_.reduce_basics)
          return orig;
        if (a.is(op::Star))
          {
            // {[*]}[]->b = Gb
            if (a == formula::one_star())
              return recurse(formula::unop(op_g, b));

            formula s = a[0];
            unsigned min = a.min();
            unsigned max = a.max();
            // {s[*]}[]->b = b W !s   if s is Boolean.
            // {s[+]}[]->b = b W !s   if s is Boolean.
            if (s.is_boolean() && max == formula::unbounded() && min <= 1)
              {
                formula ns = doneg ? formula::Not(s) : s;
                // b W !s
                return recurse(formula::binop(op_w, b, ns));
              }
            // {s[*0..j]}[]->b = {s[*1..j]}[]->b
            // {s[*0..j]}<>->b = {s[*1..j]}<>->b
            if (min == 0)
              return recurse(formula::binop(bindop,
                                            formula::Star(s, 1, max), b));

            if (opt_.reduce_size_strictly)
              return orig;
            // {s[*i..j]}[]->b = {s;s;...;s[*1..j-i+1]}[]->b
            // = {s}[]->X({s}[]->X(...[]->X({s[*1..j-i+1]}[]->b)))
            // if i>0 and s does not accept the empty word
            assert(min > 0);
            if (s.accepts_eword())
              return orig;
            --min;
            if (max != formula::unbounded())
              max -= min; // j-i+1
            // Don't rewrite s[1..].
            if (min == 0)
              return orig;
            formula tail = // {s[*1..j-i]}[]->b
              formula::binop(bindop, formula::Star(s, 1, max), b);
            for (unsigned n = 0; n < min; ++n)
              tail = // {s}[]->X(tail)
                formula::binop(bindop, s, formula::X(tail));
            return recurse(tail);
          }
        else if (a.is(op::Concat))
          {
            unsigned s = a.size() - 1;
            formula last = a[s];
            // {r;[*]}[]->b = {r}[]->Gb
            if (last == formula::one_star())
              return recurse(formula::binop(bindop, a.all_but(s),
                                            formula::unop(op_g, b)));

            formula first = a[0];
            // {[*];r}[]->b = G({r}[]->b)
            if (first == formula::one_star())
              return recurse(formula::unop(op_g,
                                           formula::binop(bindop,
                                                          a.all_but(0), b)));

            if (opt_.reduce_size_strictly)
              return orig;

            // {r;s[*]}[]->b = {r}[]->(b & X(b W !s))
            // if s is Boolean and r does not accept [*0];
            if (last.is_Kleene_star()) // l = s[*]
              if (last[0].is_boolean())
                {
                  formula r = a.all_but(s);
                  if (!r.accepts_eword())
                    {
                      formula ns = // !s
                        doneg ? formula::Not(last[0]) : last[0];
                      formula w = // b W !s
                        formula::binop(op_w, b, ns);
                      formula x = // X(b W !s)
                        formula::X(w);
                      formula d = // b & X(b W !s)
                        formula::multop(op_and, {b, x});
                      // {r}[]->(b & X(b W !s))
                      return recurse(formula::binop(bindop, r, d));
                    }
                }
            // {s[*];r}[]->b = !s R ({r}[]->b)
            // if s is Boolean and r does not accept [*0];
            if (first.is_Kleene_star())
              if (first[0].is_boolean())
                {
                  formula r = a.all_but(0);
                  if (!r.accepts_eword())
                    {
                      formula ns = // !s
                        doneg
                        ? formula::Not(first[0])
                        : first[0];
                      formula u = // {r}[]->b
                        formula::binop(bindop, r, b);
                      // !s R ({r}[]->b)
                      return recurse(formula::binop(op_r, ns, u));
                    }
                }

            // {r₁;r₂;r₃}[]->b = {r₁}[]->X({r₂}[]->X({r₃}[]->b))
            // if r₁, r₂, r₃ do not accept [*0].
            if (!a.accepts_eword())
              {
                unsigned count = 0;
                for (unsigned n = 0; n <= s; ++n)
                  count += !a[n].accepts_eword();
                assert(count > 0);
                if (count == 1)
                  return orig;
                // Let e denote a term that accepts [*0]
                // and let f denote a term that do not.
                // A formula such as {e₁;f₁;e₂;e₃;f₂;e₄}[]->b
                // in which count==2 will be grouped
                // as follows:  r₁ = e₁;f₁;e₂;e₃
                //              r₂ = f₂;e₄
                // this way we have
                // {e₁;f₁;e₂;e₃;f₂;e₄}[]->b = {r₁;r₂;r₃}[]->b
                // where r₁ and r₂ do not accept [*0].
                unsigned pos = s + 1;

                // We compute the r formulas from the right
                // (i.e., r₂ before r₁.)
                vec r;
                do
                  r.insert(r.begin(), a[--pos]);
                while (r.front().accepts_eword());
                formula tail = // {r₂}[]->b
                  formula::binop(bindop, formula::Concat(r), b);
                while (--count)
                  {
                    vec r;
                    do
                      r.insert(r.begin(), a[--pos]);
                    while (r.front().accepts_eword());
                    // If it's the last block, take all leading
                    // formulae as well.
                    if (count == 1)
                      while (pos > 0)
                        {
                          r.insert(r.begin(), a[--pos]);
                          assert(r.front().accepts_eword());
                        }

                    tail = // X({r₂}[]->b)
                      formula::X(tail);
                    tail = // {r₁}[]->X({r₂}[]->b)
                      formula::binop(bindop, formula::Concat(r), tail);
                  }
                return recurse(tail);
              }
          }
        else if (opt_.reduce_size_strictly)
          {
            return orig;
          }
        else if (a.is(op::Fusion))
          {
            // {r₁:r₂:r₃}[]->b = {r₁}[]->({r₂}[]->({r₃}[]->b))
            unsigned s = a.size();
            formula tail = b;
            do
              {
                --s;
                tail = formula::binop(bindop, a[s], tail);
              }
            while (s != 0);
            return recurse(tail);
          }
        else if (a.is(op::OrRat))
          {
            // {r₁|r₂|r₃}[]->b = ({r₁}[]->b)&({r₂}[]->b)&({r₃}[]->b)
            unsigned s = a.size();
            vec v;
            for (unsigned n = 0; n < s; ++n)
              // {r₁}[]->b
              v.emplace_back(formula::binop(bindop, a[n], b));
            return recurse(formula::multop(op_and, v));
          }
        return orig;
      }

      formula
        visit_binop(formula bo)
      {
        auto recurse = [this](formula f)
          {
            return simplify_recursively(f, c_);
          };
        op o = bo.kind();
        formula b = bo[1];
        if (opt_.event_univ)
          {
            trace << "bo: trying eventuniv rules" << std::endl;
            /* If b is a pure eventuality formula then a U b = b.
               If b is a pure universality formula a R b = b. */
            if ((b.is_eventual() && bo.is(op::U))
                || (b.is_universal() && bo.is(op::R)))
              return b;
          }

        formula a = bo[0];
        if (opt_.event_univ)
          {
            /* If a is a pure eventuality formula then a M b = a & b.
               If a is a pure universality formula a W b = a | b. */
            if (a.is_eventual() && bo.is(op::M))
              return recurse(formula::And({a, b}));
            if (a.is_universal() && bo.is(op::W))
              return recurse(formula::Or({a, b}));

            // (q R Xf) = X(q R f)
            // (q U Xf) = X(q U f)
            if (a.is_eventual() && a.is_universal()
                && bo.is(op::R, op::U) && b.is(op::X))
              return recurse(formula::X(formula::binop(o, a, b[0])));

            // e₁ W e₂ = Ge₁ | e₂
            // u₁ M u₂ = Fu₁ & u₂
            //
            // The above formulas are actually true if e₁ and u₁ are
            // unconstrained, however there are many cases were such a
            // more generic reduction rule will actually produce more
            // states once the resulting formula is translated.
            if (!opt_.reduce_size_strictly)
              {
                if (bo.is(op::W) && a.is_eventual() && b.is_eventual())
                  return recurse(formula::Or({formula::G(a), b}));
                if (bo.is(op::M) && a.is_universal() && b.is_universal())
                  return recurse(formula::And({formula::F(a), b}));
              }

            // In the following rewritings we assume that
            // - e is a pure eventuality
            // - u is purely universal
            // - q is purely universal pure eventuality
            // (a U (b|e)) = (a U b)|e
            // (a W (b|e)) = (a W b)|e
            // (a U (b&q)) = (a U b)&q
            // ((a&q) M b) = (a M b)&q
            // (a R (b&u)) = (a R b)&u
            // (a M (b&u)) = (a M b)&u
            if (opt_.favor_event_univ)
              {
                if (bo.is(op::U, op::W))
                  if (b.is(op::Or))
                    {
                      mospliter s(mospliter::Split_Event, b, c_);
                      formula b2 = formula::Or(std::move(*s.res_other));
                      if (b2 != b)
                        {
                          s.res_Event->emplace_back(formula::binop(o, a, b2));
                          return recurse
                            (formula::Or(std::move(*s.res_Event)));
                        }
                    }
                if (bo.is(op::U))
                  if (b.is(op::And))
                    {
                      mospliter s(mospliter::Split_EventUniv, b, c_);
                      formula b2 = formula::And(std::move(*s.res_other));
                      if (b2 != b)
                        {
                          s.res_EventUniv->emplace_back(formula::binop(o,
                                                                    a, b2));
                          return recurse
                            (formula::And(std::move(*s.res_EventUniv)));
                        }
                    }
                if (bo.is(op::M))
                  if (a.is(op::And))
                    {
                      mospliter s(mospliter::Split_EventUniv, a, c_);
                      formula a2 = formula::And(std::move(*s.res_other));
                      if (a2 != a)
                        {
                          s.res_EventUniv->emplace_back(formula::binop(o,
                                                                    a2, b));
                          return recurse
                            (formula::And(std::move(*s.res_EventUniv)));
                        }
                    }
                if (bo.is(op::R, op::M))
                  if (b.is(op::And))
                    {
                      mospliter s(mospliter::Split_Univ, b, c_);
                      formula b2 = formula::And(std::move(*s.res_other));
                      if (b2 != b)
                        {
                          s.res_Univ->emplace_back(formula::binop(o, a, b2));
                          return recurse
                            (formula::And(std::move(*s.res_Univ)));
                        }
                    }
              }
            trace << "bo: no eventuniv rule matched" << std::endl;
          }

        // Inclusion-based rules
        if (opt_.synt_impl | opt_.containment_checks)
          {
            trace << "bo: trying inclusion-based rules" << std::endl;
            switch (o)
              {
              case op::Equiv:
                {
                  if (c_->implication(a, b))
                    return recurse(formula::Implies(b, a));
                  if (c_->implication(b, a))
                    return recurse(formula::Implies(a, b));
                  break;
                }
              case op::Implies:
                {
                  if (c_->implication(a, b))
                    return formula::tt();
                  break;
                }
              case op::Xor:
                {
                  // if (!a)->b then a xor b = b->!a = a->!b
                  if (c_->implication_neg(a, b, false))
                    {
                      if (b.is(op::Not))
                        return recurse(formula::Implies(a, b[0]));
                      return recurse(formula::Implies(b, formula::Not(a)));
                    }
                  // if a->!b then a xor b = (!b)->a = (!a)->b
                  if (c_->implication_neg(a, b, true))
                    {
                      if (b.is(op::Not))
                        return recurse(formula::Implies(b[0], a));
                      return recurse(formula::Implies(formula::Not(a), b));
                    }
                  break;
                }
              case op::U:
                // if a => b, then a U b = b
                if (c_->implication(a, b))
                  {
                    // but if also b => a, pick the smallest one.
                    if ((length_boolone(a) < length_boolone(b))
                        && c_->implication(b, a))
                      return a;
                    return b;
                  }
                // if (a U b) => b, then a U b = b (for stronger containment)
                if (opt_.containment_checks_stronger
                    && c_->contained(bo, b))
                  return b;
                // if !a => b, then a U b = Fb
                // if a eventual && b => a, then a U b = Fb
                if (c_->implication_neg(a, b, false)
                    || (a.is_eventual() && c_->implication(b, a)))
                  return recurse(formula::F(b));
                // if a => b, then a U (b U c) = (b U c)
                // if a => b, then a U (b W c) = (b W c)
                if (b.is(op::U, op::W) && c_->implication(a, b[0]))
                  return b;
                // if b => a, then a U (b U c) = (a U c)
                if (b.is(op::U) && c_->implication(b[0], a))
                  return recurse(formula::U(a, b[1]));
                // if a => c, then a U (b R (c U d)) = (b R (c U d))
                // if a => c, then a U (b R (c W d)) = (b R (c W d))
                // if a => c, then a U (b M (c U d)) = (b M (c U d))
                // if a => c, then a U (b M (c W d)) = (b M (c W d))
                if (b.is(op::R, op::M))
                  {
                    auto c1 = b[1];
                    if (c1.is(op::U, op::W) && c_->implication(a, c1[0]))
                      return b;
                  }
                // if a => b, then (a U c) U b = c U b
                // if a => b, then (a W c) U b = c U b
                if (a.is(op::U, op::W) && c_->implication(a[0], b))
                  return recurse(formula::U(a[1], b));
                // if c => b, then (a U c) U b = (a U c) | b
                if (a.is(op::U) && c_->implication(a[1], b))
                  return recurse(formula::Or({a, b}));
                // if g => h, then (f|g) U h = f U h
                if (a.is(op::Or))
                  {
                    unsigned n = a.size();
                    for (unsigned child = 0; child < n; ++child)
                      if (c_->implication(a[child], b))
                        return recurse(formula::U(a.all_but(child), b));
                  }
                // a U (b & e) = F(b & e) if !b => a
                if (b.is(op::And))
                  for (formula c: b)
                    if (c.is_eventual())
                      {
                        // We know there is one pure eventuality
                        // formula but we might have more.  So lets
                        // extract everything else.
                        vec rest;
                        rest.reserve(c.size());
                        for (formula cc: b)
                          if (!cc.is_eventual())
                            rest.emplace_back(cc);
                        if (c_->implication_neg(formula::And(rest), a, false))
                          return recurse(formula::F(b));
                        break;
                      }
                break;

              case op::R:
                // if b => a, then a R b = b
                if (c_->implication(b, a))
                  {
                    // but if also a => b, pick the smallest one.
                    if ((length_boolone(a) < length_boolone(b))
                        && c_->implication(a, b))
                      return a;
                    return b;
                  }
                // if b => !a, then a R b = Gb
                // if a universal && a => b, then a R b = Gb
                if (c_->implication_neg(b, a, true)
                    || (a.is_universal() && c_->implication(a, b)))
                  return recurse(formula::G(b));
                // if b => a, then a R (b R c) = b R c
                // if b => a, then a R (b M c) = b M c
                if (b.is(op::R, op::M) && c_->implication(b[0], a))
                  return b;
                // if a => b, then a R (b R c) = a R c
                if (b.is(op::R) && c_->implication(a, b[0]))
                  return recurse(formula::R(a, b[1]));
                // if b => a, then (a R c) R b = c R b
                // if b => a, then (a M c) R b = c R b
                // if c => b, then (a R c) R b = (a & c) R b
                // if c => b, then (a M c) R b = (a & c) R b
                if (a.is(op::R, op::M))
                  {
                    if (c_->implication(b, a[0]))
                      return recurse(formula::R(a[1], b));
                    if (c_->implication(a[1], b))
                      {
                        formula ac = formula::And({a[0], a[1]});
                        return recurse(formula::R(ac, b));
                      }
                  }
                // if h => g, then (f&g) R h = f R h
                if (a.is(op::And))
                  {
                    unsigned n = a.size();
                    for (unsigned child = 0; child < n; ++child)
                      if (c_->implication(b, a[child]))
                        return recurse(formula::R(a.all_but(child), b));
                  }
                // a R (b | u) = G(b | u) if b => !a
                if (b.is(op::Or))
                  for (formula c: b)
                    if (c.is_universal())
                      {
                        // We know there is one purely universal
                        // formula but we might have more.  So lets
                        // extract everything else.
                        vec rest;
                        rest.reserve(c.size());
                        for (formula cc: b)
                          if (!cc.is_universal())
                            rest.emplace_back(cc);
                        if (c_->implication_neg(formula::Or(rest), a, true))
                          return recurse(formula::G(b));
                        break;
                      }
                break;

              case op::W:
                // if !a => b then a W b = 1
                if (c_->implication_neg(a, b, false))
                  return formula::tt();
                // if a => b, then a W b = b
                if (c_->implication(a, b))
                  {
                    // but if also b => a, pick the smallest one.
                    if ((length_boolone(a) < length_boolone(b))
                        && c_->implication(b, a))
                      return a;
                    return b;
                  }

                // if a W b => b, then a W b = b (for stronger containment)
                if (opt_.containment_checks_stronger
                    && c_->contained(bo, b))
                  return b;
                // if a => b, then a W (b W c) = b W c
                // (Beware: even if a => b we do not have a W (b U c) = b U c)
                if (b.is(op::W) && c_->implication(a, b[0]))
                  return b;
                // if b => a, then a W (b U c) = a W c
                // if b => a, then a W (b W c) = a W c
                if (b.is(op::U, op::W) && c_->implication(b[0], a))
                  return recurse(formula::W(a, b[1]));
                // if a => b, then (a U c) W b = c W b
                // if a => b, then (a W c) W b = c W b
                if (a.is(op::U, op::W) && c_->implication(a[0], b))
                  return recurse(formula::W(a[1], b));
                // if c => b, then (a W c) W b = (a W c) | b
                // if c => b, then (a U c) W b = (a U c) | b
                if (a.is(op::U, op::W) && c_->implication(a[1], b))
                  return recurse(formula::Or({a, b}));
                // if g => h, then (f|g) W h = f M h
                if (a.is(op::Or))
                  {
                    unsigned n = a.size();
                    for (unsigned child = 0; child < n; ++child)
                      if (c_->implication(a[child], b))
                        return recurse(formula::W(a.all_but(child), b));
                  }
                break;

              case op::M:
                // if b => !a, then a M b = 0
                if (c_->implication_neg(b, a, true))
                  return formula::ff();
                // if b => a, then a M b = b
                if (c_->implication(b, a))
                  {
                    // but if also a => b, pick the smallest one.
                    if ((length_boolone(a) < length_boolone(b))
                        && c_->implication(a, b))
                      return a;
                    return b;
                  }
                // if b => a, then a M (b M c) = b M c
                if (b.is(op::M) && c_->implication(b[0], a))
                  return b;
                // if a => b, then a M (b M c) = a M c
                // if a => b, then a M (b R c) = a M c
                if (b.is(op::M, op::R) && c_->implication(a, b[0]))
                  return recurse(formula::M(a, b[1]));
                // if b => a, then (a R c) M b = c M b
                // if b => a, then (a M c) M b = c M b
                if (a.is(op::R, op::M) && c_->implication(b, a[0]))
                  return recurse(formula::M(a[1], b));
                // if c => b, then (a M c) M b = (a & c) M b
                if (a.is(op::M) && c_->implication(a[1], b))
                  return
                    recurse(formula::M(formula::And({a[0], a[1]}),
                                       b));
                // if h => g, then (f&g) M h = f M h
                if (a.is(op::And))
                  {
                    unsigned n = a.size();
                    for (unsigned child = 0; child < n; ++child)
                      if (c_->implication(b, a[child]))
                        return recurse(formula::M(a.all_but(child), b));
                  }
                break;

              default:
                break;
              }
            trace << "bo: no inclusion-based rules matched" << std::endl;
          }

        if (!opt_.reduce_basics)
          {
            trace << "bo: basic reductions disabled" << std::endl;
            return bo;
          }

        trace << "bo: trying basic reductions" << std::endl;
        // Rewrite U,R,W,M as F or G when possible.
        // true U b == F(b)
        if (bo.is(op::U) && a.is_tt())
          return recurse(formula::F(b));
        // false R b == G(b)
        if (bo.is(op::R) && a.is_ff())
          return recurse(formula::G(b));
        // a W false == G(a)
        if (bo.is(op::W) && b.is_ff())
          return recurse(formula::G(a));
        // a M true == F(a)
        if (bo.is(op::M) && b.is_tt())
          return recurse(formula::F(a));

        if (bo.is(op::W, op::M) || bo.is(op::U, op::R))
          {
            // X(a) U X(b) = X(a U b)
            // X(a) R X(b) = X(a R b)
            // X(a) W X(b) = X(a W b)
            // X(a) M X(b) = X(a M b)
            if (a.is(op::X) && b.is(op::X))
              return recurse(formula::X(formula::binop(o,
                                                       a[0], b[0])));

            if (bo.is(op::U, op::W))
              {
                // a U Ga = Ga
                // a W Ga = Ga
                if (b.is(op::G) && a == b[0])
                  return b;
                // a U (b | c | G(a)) = a W (b | c)
                // a W (b | c | G(a)) = a W (b | c)
                if (b.is(op::Or))
                  for (int i = 0, s = b.size(); i < s; ++i)
                    {
                      formula c = b[i];
                      if (c.is(op::G) && c[0] == a)
                        return recurse(formula::W(a, b.all_but(i)));
                    }
                // a U (b & a & c) == (b & c) M a
                // a W (b & a & c) == (b & c) R a
                if (b.is(op::And))
                  for (int i = 0, s = b.size(); i < s; ++i)
                    if (b[i] == a)
                      return recurse(formula::binop(o == op::U ?
                                                    op::M : op::R,
                                                    b.all_but(i), a));
                // If b is Boolean:
                // (Xc) U b = b | X(b M c)
                // (Xc) W b = b | X(b R c)
                if (!opt_.reduce_size_strictly
                    && a.is(op::X) && b.is_boolean())
                  {
                    formula x = formula::X(formula::binop(o == op::U ?
                                                          op::M : op::R,
                                                          b, a[0]));
                    return recurse(formula::Or({b, x}));
                  }
              }
            else if (bo.is(op::M, op::R))
              {
                // a R Fa = Fa
                // a M Fa = Fa
                if (b.is(op::F) && b[0] == a)
                  return b;

                // a R (b & c & F(a)) = a M (b & c)
                // a M (b & c & F(a)) = a M (b & c)
                if (b.is(op::And))
                  for (int i = 0, s = b.size(); i < s; ++i)
                    {
                      formula c = b[i];
                      if (c.is(op::F) && c[0] == a)
                        return recurse(formula::M(a, b.all_but(i)));
                    }
                // a M (b | a | c) == (b | c) U a
                // a R (b | a | c) == (b | c) W a
                if (b.is(op::Or))
                  for (int i = 0, s = b.size(); i < s; ++i)
                    if (b[i] == a)
                      return recurse(formula::binop(o == op::M ?
                                                    op::U : op::W,
                                                    b.all_but(i), a));
                // If b is Boolean:
                // (Xc) R b = b & X(b W c)
                // (Xc) M b = b & X(b U c)
                if (!opt_.reduce_size_strictly
                    && a.is(op::X) && b.is_boolean())
                  {
                    formula x =
                      formula::X(formula::binop(o == op::M ? op::U : op::W,
                                                b, a[0]));
                    return recurse(formula::And({b, x}));
                  }
              }
          }
        if (bo.is(op::UConcat) || bo.is(op::EConcat, op::EConcatMarked))
          return reduce_sere_ltl(bo);
        return bo;
      }

      formula
        visit_multop(formula mo)
      {
        auto recurse = [this](formula f)
          {
            return simplify_recursively(f, c_);
          };

        unsigned mos = mo.size();

        if ((opt_.synt_impl | opt_.containment_checks)
            && mo.is(op::Or, op::And))
          {
            // Do not merge these two loops, as rewritings from the
            // second loop could prevent rewritings from the first one
            // to trigger.
            for (unsigned i = 0; i < mos; ++i)
              {
                formula fi = mo[i];
                formula fo = mo.all_but(i);
                // if fi => !fo, then fi & fo = false
                // if fo => !fi, then fi & fo = false
                // if !fi => fo, then fi | fo = true
                // if !fo => fi, then fi | fo = true
                bool is_and = mo.is(op::And);
                if (c_->implication_neg(fi, fo, is_and)
                    || c_->implication_neg(fo, fi, is_and))
                  return recurse(is_and ? formula::ff() : formula::tt());
              }
            for (unsigned i = 0; i < mos; ++i)
              {
                formula fi = mo[i];
                formula fo = mo.all_but(i);
                // if fi => fo, then fi | fo = fo
                // if fo => fi, then fi & fo = fo
                if ((mo.is(op::Or) && c_->implication(fi, fo))
                    || (mo.is(op::And) && c_->implication(fo, fi)))
                  {
                    // We are about to pick fo, but hold on!
                    // Maybe we actually have fi <=> fo, in
                    // which case we could decide to work on fi or fo.
                    //
                    // As a heuristic, let's return the smallest
                    // subformula.  So we only need to check this
                    // other implication if fi is smaller than fo,
                    // otherwise we don't care.
                    if ((length_boolone(fi) < length_boolone(fo))
                        && ((mo.is(op::Or) && c_->implication(fo, fi))
                            || (mo.is(op::And) && c_->implication(fi, fo))))
                      return recurse(fi);
                    else
                      return recurse(fo);
                  }
              }
          }

        vec res;
        res.reserve(mos);
        for (auto f: mo)
          res.emplace_back(f);
        op o = mo.kind();

        // basics reduction do not concern Boolean formulas,
        // so don't waste time trying to apply them.
        if (opt_.reduce_basics && !mo.is_boolean())
          {
            switch (o)
              {
              case op::And:
                assert(!mo.is_sere_formula());
                {
                  // a & X(G(a&b...) & c...) = Ga & X(G(b...) & c...)
                  // a & (Xa W b) = b R a
                  // a & (Xa U b) = b M a
                  // a & (b | X(b R a)) = b R a
                  // a & (b | X(b M a)) = b M a
                  if (!mo.is_syntactic_stutter_invariant()) // Skip if no X.
                    {
                      typedef std::unordered_set<formula> fset_t;
                      typedef robin_hood::unordered_node_map
                        <formula, std::set<unsigned>> fmap_t;
                      fset_t xgset; // XG(...)
                      fset_t xset;  // X(...)
                      fmap_t wuset; // (X...)W(...) or (X...)U(...)

                      std::vector<bool> tokill(mos);

                      // Make a pass to search for subterms
                      // of the form XGa or  X(... & G(...&a&...) & ...)
                      for (unsigned n = 0; n < mos; ++n)
                        {
                          if (!res[n])
                            continue;
                          if (res[n].is_syntactic_stutter_invariant())
                            continue;

                          if (formula xarg = is_XWU(res[n]))
                            {
                              wuset[xarg].insert(n);
                              continue;
                            }

                          // Now we are looking for
                          // - X(...)
                          // - b | X(b R ...)
                          // - b | X(b M ...)
                          if (formula barg = is_bXbRM(res[n]))
                            {
                              wuset[barg[1]].insert(n);
                              continue;
                            }

                          if (!res[n].is(op::X))
                            continue;

                          formula c = res[n][0];
                          auto handle_G = [&xgset](formula c)
                            {
                              formula a2 = c[0];
                              if (a2.is(op::And))
                                for (auto c: a2)
                                  xgset.insert(c);
                              else
                                xgset.insert(a2);
                            };

                          if (c.is(op::G))
                            {
                              handle_G(c);
                            }
                          else if (c.is(op::And))
                            {
                              for (auto cc: c)
                                if (cc.is(op::G))
                                  handle_G(cc);
                                else
                                  xset.insert(cc);
                            }
                          else
                            {
                              xset.insert(c);
                            }
                          res[n] = nullptr;
                        }
                      // Make a second pass to check if the "a"
                      // terms can be used to simplify "Xa W b",
                      // "Xa U b", "b | X(b R a)", or "b | X(b M a)".
                      vec resorig(res);
                      for (unsigned n = 0; n < mos; ++n)
                        {
                          formula x = resorig[n];
                          if (!x)
                            continue;
                          fmap_t::const_iterator gs = wuset.find(x);
                          if (gs == wuset.end())
                            continue;

                          for (unsigned pos: gs->second)
                            {
                              formula wu = resorig[pos];
                              if (wu.is(op::W, op::U))
                                {
                                  // a & (Xa W b) = b R a
                                  // a & (Xa U b) = b M a
                                  op t = wu.is(op::U) ? op::M : op::R;
                                  assert(wu[0].is(op::X));
                                  formula a = wu[0][0];
                                  formula b = wu[1];
                                  res[pos] = formula::binop(t, b, a);
                                }
                              else
                                {
                                  // a & (b | X(b R a)) = b R a
                                  // a & (b | X(b M a)) = b M a
                                  wu = is_bXbRM(resorig[pos]);
                                  assert(wu);
                                  res[pos] = wu;
                                }
                              // Remember to kill "a".
                              tokill[n] = true;
                            }
                        }

                      // Make third pass to search for terms 'a'
                      // that also appears as 'XGa'.  Replace them
                      // by 'Ga' and delete XGa.
                      for (unsigned n = 0; n < mos; ++n)
                        {
                          formula x = res[n];
                          if (!x)
                            continue;
                          fset_t::const_iterator g = xgset.find(x);
                          if (g != xgset.end())
                            {
                              // x can appear only once.
                              formula gf = *g;
                              xgset.erase(g);
                              res[n] = formula::G(x);
                            }
                          else if (tokill[n])
                            {
                              res[n] = nullptr;
                            }
                        }

                      vec xv;
                      unsigned xgs = xgset.size();
                      xv.reserve(xset.size() + 1);
                      if (xgs > 0)
                        {
                          vec xgv;
                          xgv.reserve(xgs);
                          for (auto f: xgset)
                            xgv.emplace_back(f);
                          xv.emplace_back(unop_multop(op::G, op::And, xgv));
                        }
                      for (auto f: xset)
                        xv.emplace_back(f);
                      res.emplace_back(unop_multop(op::X, op::And, xv));
                    }

                  // Gather all operands by type.
                  mospliter s(mospliter::Strip_X |
                              mospliter::Strip_FG |
                              mospliter::Strip_G |
                              mospliter::Split_F |
                              mospliter::Split_U_or_W |
                              mospliter::Split_R_or_M |
                              mospliter::Split_EventUniv,
                              res, c_);

                  // FG(a) & FG(b) = FG(a & b)
                  formula allFG = unop_unop_multop(op::F, op::G, op::And,
                                                   std::move(*s.res_FG));
                  // Xa & Xb = X(a & b)
                  // Xa & Xb & FG(c) = X(a & b & FG(c))
                  // For Universal&Eventual formulae f1...fn we also have:
                  // Xa & Xb & f1...fn = X(a & b & f1...fn)
                  if (!s.res_X->empty() && !opt_.favor_event_univ)
                    {
                      s.res_X->emplace_back(allFG);
                      allFG = nullptr;
                      s.res_X->insert(s.res_X->begin(),
                                      s.res_EventUniv->begin(),
                                      s.res_EventUniv->end());
                    }
                  else
                    // If f1...fn are event&univ formulae, with at least
                    // one formula of the form G(...),
                    // Rewrite  g & f1...fn  as  g & G(f1..fn) while
                    // stripping any leading G from f1...fn.
                    // This gathers eventual&universal formulae
                    // under the same term.
                    {
                      vec eu;
                      bool seen_g = false;
                      for (auto f: *s.res_EventUniv)
                        if (f.is(op::G))
                          {
                            seen_g = true;
                            eu.emplace_back(f[0]);
                          }
                        else
                          {
                            eu.emplace_back(f);
                          }
                      if (seen_g)
                        {
                          eu.emplace_back(allFG);
                          allFG = nullptr;
                          formula andeu = formula::multop(op::And, eu);
                          if (!opt_.favor_event_univ)
                            s.res_G->emplace_back(andeu);
                          else
                            s.res_other->emplace_back(formula::G(andeu));
                        }
                      else
                        {
                          s.res_other->insert(s.res_other->end(),
                                              eu.begin(), eu.end());
                        }
                    }

                  // Xa & Xb & f1...fn = X(a & b & f1...fn)
                  // is built at the end of this op::And case.
                  // G(a) & G(b) = G(a & b)
                  // is built at the end of this op::And case.

                  // The following three loops perform these rewritings:
                  // (a U b) & (c U b) = (a & c) U b
                  // (a U b) & (c W b) = (a & c) U b
                  // (a W b) & (c W b) = (a & c) W b
                  // (a R b) & (a R c) = a R (b & c)
                  // (a R b) & (a M c) = a M (b & c)
                  // (a M b) & (a M c) = a M (b & c)
                  // F(a) & (a R b) = a M b
                  // F(a) & (a M b) = a M b
                  // F(b) & (a W b) = a U b
                  // F(b) & (a U b) = a U b
                  // F(c) & G(phi | e) = c M (phi | e)  if c => !phi.
                  typedef robin_hood::unordered_map<formula,
                                                    vec::iterator> fmap_t;
                  fmap_t uwmap; // associates "b" to "a U b" or "a W b"
                  fmap_t rmmap; // associates "a" to "a R b" or "a M b"
                  // (a U b) & (c U b) = (a & c) U b
                  // (a U b) & (c W b) = (a & c) U b
                  // (a W b) & (c W b) = (a & c) W b
                  for (auto i = s.res_U_or_W->begin();
                       i != s.res_U_or_W->end(); ++i)
                    {
                      formula b = (*i)[1];
                      auto j = uwmap.find(b);
                      if (j == uwmap.end())
                        {
                          // First occurrence.
                          uwmap[b] = i;
                          continue;
                        }
                      // We already have one occurrence.  Merge them.
                      formula old = *j->second;
                      op o = op::W;
                      if (i->is(op::U) || old.is(op::U))
                        o = op::U;
                      formula fst_arg = formula::And({old[0], (*i)[0]});
                      *j->second = formula::binop(o, fst_arg, b);
                      assert(j->second->is(o));
                      *i = nullptr;
                    }
                  // (a R b) & (a R c) = a R (b & c)
                  // (a R b) & (a M c) = a M (b & c)
                  // (a M b) & (a M c) = a M (b & c)
                  for (auto i = s.res_R_or_M->begin();
                       i != s.res_R_or_M->end(); ++i)
                    {
                      formula a = (*i)[0];
                      auto j = rmmap.find(a);
                      if (j == rmmap.end())
                        {
                          // First occurrence.
                          rmmap[a] = i;
                          continue;
                        }
                      // We already have one occurrence.  Merge them.
                      formula old = *j->second;
                      op o = op::R;
                      if (i->is(op::M) || old.is(op::M))
                        o = op::M;
                      formula snd_arg = formula::And({old[1], (*i)[1]});
                      *j->second = formula::binop(o, a, snd_arg);
                      assert(j->second->is(o));
                      *i = nullptr;
                    }
                  // F(b) & (a W b) = a U b
                  // F(b) & (a U b) = a U b
                  // F(a) & (a R b) = a M b
                  // F(a) & (a M b) = a M b
                  for (auto& f: *s.res_F)
                    {
                      bool superfluous = false;
                      formula c = f[0];

                      fmap_t::iterator j = uwmap.find(c);
                      if (j != uwmap.end())
                        {
                          superfluous = true;
                          formula bo = *j->second;
                          if (bo.is(op::W))
                            {
                              *j->second = formula::U(bo[0], bo[1]);
                              assert(j->second->is(op::U));
                            }
                        }
                      j = rmmap.find(c);
                      if (j != rmmap.end())
                        {
                          superfluous = true;
                          formula bo = *j->second;
                          if (bo.is(op::R))
                            {
                              *j->second = formula::M(bo[0], bo[1]);
                              assert(j->second->is(op::M));
                            }
                        }
                      if (opt_.synt_impl | opt_.containment_checks)
                        {
                          // if the input looks like o1|u1|u2|o2,
                          // return o1 | o2.  The input must have at
                          // least on universal formula.
                          auto extract_not_un =
                            [&](formula f) {
                              if (f.is(op::Or))
                                for (auto u: f)
                                  if (u.is_universal())
                                    {
                                      vec phi;
                                      phi.reserve(f.size());
                                      for (auto uu: f)
                                        if (!uu.is_universal())
                                          phi.push_back(uu);
                                      return formula::Or(phi);
                                    }
                              return formula(nullptr);
                            };

                          // F(c) & G(phi | e) = c M (phi | e)  if  c => !phi.
                          for (auto in_g = s.res_G->begin();
                               in_g != s.res_G->end();)
                            {
                              if (formula phi = extract_not_un(*in_g))
                                if (c_->implication_neg(phi, c, true))
                                  {
                                    s.res_other->push_back(formula::M(c,
                                                                      *in_g));
                                    in_g = s.res_G->erase(in_g);
                                    superfluous = true;
                                    continue;
                                  }
                              ++in_g;
                            }
                        }
                      if (superfluous)
                        f = nullptr;
                    }

                  s.res_other->reserve(s.res_other->size()
                                       + s.res_F->size()
                                       + s.res_U_or_W->size()
                                       + s.res_R_or_M->size()
                                       + 3);
                  s.res_other->insert(s.res_other->end(),
                                      s.res_F->begin(),
                                      s.res_F->end());
                  s.res_other->insert(s.res_other->end(),
                                      s.res_U_or_W->begin(),
                                      s.res_U_or_W->end());
                  s.res_other->insert(s.res_other->end(),
                                      s.res_R_or_M->begin(),
                                      s.res_R_or_M->end());

                  // Those "G" formulae that are eventual can be
                  // postponed inside the X term if there is one.
                  //
                  // In effect we rewrite
                  //   Xa&Xb&GFc&GFd&Ge as X(a&b&G(Fc&Fd))&Ge
                  if (!s.res_X->empty() && !opt_.favor_event_univ)
                    {
                      vec event;
                      for (auto& f: *s.res_G)
                        if (f.is_eventual())
                          {
                            event.emplace_back(f);
                            f = nullptr; // Remove it from res_G.
                          }
                      s.res_X->emplace_back(unop_multop(op::G, op::And,
                                                     std::move(event)));
                    }

                  // G(a) & G(b) & ... = G(a & b & ...)
                  formula allG = unop_multop(op::G, op::And,
                                             std::move(*s.res_G));
                  // Xa & Xb & ... = X(a & b & ...)
                  formula allX = unop_multop(op::X, op::And,
                                             std::move(*s.res_X));
                  s.res_other->emplace_back(allX);
                  s.res_other->emplace_back(allG);
                  s.res_other->emplace_back(allFG);
                  formula r = formula::And(std::move(*s.res_other));
                  // If we altered the formula in some way, process
                  // it another time.
                  if (r != mo)
                    return recurse(r);
                  return r;
                }
              case op::AndRat:
                {
                  mospliter s(mospliter::Split_Bool, res, c_);
                  if (!s.res_Bool->empty())
                    {
                      // b1 & b2 & b3 = b1 ∧ b2 ∧ b3
                      formula b = formula::And(std::move(*s.res_Bool));

                      vec ares;
                      for (auto& f: *s.res_other)
                        switch (f.kind())
                          {
                          case op::Star:
                            // b && r[*i..j] = b & r  if i<=1<=j
                            //               = 0      otherwise
                            if (f.min() > 1 || f.max() < 1)
                              return formula::ff();
                            ares.emplace_back(f[0]);
                            f = nullptr;
                            break;
                          case op::Fusion:
                            // b && {r1:..:rn} = b && r1 && .. && rn
                            for (auto ri: f)
                              ares.emplace_back(ri);
                            f = nullptr;
                            break;
                          case op::Concat:
                            // b && {r1;...;rn} =
                            // - b && ri if there is only one ri
                            //           that does not accept [*0]
                            // - b && (r1|...|rn) if all ri
                            //           do not accept [*0]
                            // - 0 if more than one ri accept [*0]
                            {
                              formula ri = nullptr;
                              unsigned nonempty = 0;
                              unsigned rs = f.size();
                              for (unsigned j = 0; j < rs; ++j)
                                {
                                  formula jf = f[j];
                                  if (!jf.accepts_eword())
                                    {
                                      ri = jf;
                                      ++nonempty;
                                    }
                                }
                              if (nonempty == 1)
                                {
                                  ares.emplace_back(ri);
                                }
                              else if (nonempty == 0)
                                {
                                  vec sum;
                                  for (auto j: f)
                                    sum.emplace_back(j);
                                  ares.emplace_back(formula::OrRat(sum));
                                }
                              else
                                {
                                  return formula::ff();
                                }
                              f = nullptr;
                              break;
                            }
                          default:
                            ares.emplace_back(f);
                            f = nullptr;
                            break;
                          }
                      ares.emplace_back(b);
                      auto r = formula::AndRat(std::move(ares));
                      // If we altered the formula in some way, process
                      // it another time.
                      if (r != mo)
                        return recurse(r);
                      return r;
                    }
                  // No Boolean as argument of &&.

                  // Look for occurrences of {b;r} or {b:r}.  We have
                  // {b1;r1}&&{b2;r2} = {b1&&b2};{r1&&r2}
                  //                     head1    tail1
                  // {b1:r1}&&{b2:r2} = {b1&&b2}:{r1&&r2}
                  //                     head2    tail2
                  vec head1;
                  vec tail1;
                  vec head2;
                  vec tail2;
                  for (auto& i: *s.res_other)
                    {
                      if (!i)
                        continue;
                      if (!i.is(op::Concat, op::Fusion))
                        continue;
                      formula h = i[0];
                      if (!h.is_boolean())
                        continue;
                      if (i.is(op::Concat))
                        {
                          head1.emplace_back(h);
                          tail1.emplace_back(i.all_but(0));
                        }
                      else // op::Fusion
                        {
                          head2.emplace_back(h);
                          tail2.emplace_back(i.all_but(0));
                        }
                      i = nullptr;
                    }
                  if (!head1.empty())
                    {
                      formula h = formula::And(std::move(head1));
                      formula t = formula::AndRat(std::move(tail1));
                      s.res_other->emplace_back(formula::Concat({h, t}));
                    }
                  if (!head2.empty())
                    {
                      formula h = formula::And(std::move(head2));
                      formula t = formula::AndRat(std::move(tail2));
                      s.res_other->emplace_back(formula::Fusion({h, t}));
                    }

                  // {r1;b1}&&{r2;b2} = {r1&&r2};{b1∧b2}
                  //                     head3    tail3
                  // {r1:b1}&&{r2:b2} = {r1&&r2}:{b1∧b2}
                  //                     head4    tail4
                  vec head3;
                  vec tail3;
                  vec head4;
                  vec tail4;
                  for (auto& i: *s.res_other)
                    {
                      if (!i)
                        continue;
                      if (!i.is(op::Concat, op::Fusion))
                        continue;
                      unsigned s = i.size() - 1;
                      formula t = i[s];
                      if (!t.is_boolean())
                        continue;
                      if (i.is(op::Concat))
                        {
                          tail3.emplace_back(t);
                          head3.emplace_back(i.all_but(s));
                        }
                      else // op::Fusion
                        {
                          tail4.emplace_back(t);
                          head4.emplace_back(i.all_but(s));
                        }
                      i = nullptr;
                    }
                  if (!head3.empty())
                    {
                      formula h = formula::AndRat(std::move(head3));
                      formula t = formula::And(std::move(tail3));
                      s.res_other->emplace_back(formula::Concat({h, t}));
                    }
                  if (!head4.empty())
                    {
                      formula h = formula::AndRat(std::move(head4));
                      formula t = formula::And(std::move(tail4));
                      s.res_other->emplace_back(formula::Fusion({h, t}));
                    }

                  auto r = formula::AndRat(std::move(*s.res_other));
                  // If we altered the formula in some way, process
                  // it another time.
                  if (r != mo)
                    return recurse(r);
                  return r;
                }
              case op::Or:
                {
                  // a | X(F(a) | c...) = Fa | X(c...)
                  // a | (Xa R b) = b W a
                  // a | (Xa M b) = b U a
                  // a | (b & X(b W a)) = b W a
                  // a | (b & X(b U a)) = b U a
                  if (!mo.is_syntactic_stutter_invariant()) // Skip if no X
                    {
                      typedef std::unordered_set<formula> fset_t;
                      typedef robin_hood::unordered_node_map
                        <formula, std::set<unsigned>> fmap_t;
                      fset_t xfset; // XF(...)
                      fset_t xset;  // X(...)
                      fmap_t rmset; // (X...)R(...) or (X...)M(...) or
                      // b & X(b W ...) or b & X(b U ...)

                      std::vector<bool> tokill(mos);

                      // Make a pass to search for subterms
                      // of the form XFa or  X(... | F(...|a|...) | ...)
                      for (unsigned n = 0; n < mos; ++n)
                        {
                          if (!res[n])
                            continue;
                          if (res[n].is_syntactic_stutter_invariant())
                            continue;

                          if (formula xarg = is_XRM(res[n]))
                            {
                              rmset[xarg].insert(n);
                              continue;
                            }

                          // Now we are looking for
                          // - X(...)
                          // - b & X(b W ...)
                          // - b & X(b U ...)
                          if (formula barg = is_bXbWU(res[n]))
                            {
                              rmset[barg[1]].insert(n);
                              continue;
                            }

                          if (!res[n].is(op::X))
                            continue;

                          formula c = res[n][0];

                          auto handle_F = [&xfset](formula c)
                            {
                              formula a2 = c[0];
                              if (a2.is(op::Or))
                                for (auto c: a2)
                                  xfset.insert(c);
                              else
                                xfset.insert(a2);
                            };

                          if (c.is(op::F))
                            {
                              handle_F(c);
                            }
                          else if (c.is(op::Or))
                            {
                              for (auto cc: c)
                                if (cc.is(op::F))
                                  handle_F(cc);
                                else
                                  xset.insert(cc);
                            }
                          else
                            {
                              xset.insert(c);
                            }
                          res[n] = nullptr;
                        }
                      // Make a second pass to check if we can
                      // remove all instance of XF(a).
                      unsigned allofthem = xfset.size();
                      vec resorig(res);
                      for (unsigned n = 0; n < mos; ++n)
                        {
                          formula x = resorig[n];
                          if (!x)
                            continue;
                          fset_t::const_iterator f = xfset.find(x);
                          if (f != xfset.end())
                            --allofthem;
                          assert(allofthem != -1U);
                          // At the same time, check if "a" can also
                          // be used to simplify "Xa R b", "Xa M b".
                          // "b & X(b W a)", or "b & X(b U a)".
                          fmap_t::const_iterator gs = rmset.find(x);
                          if (gs == rmset.end())
                            continue;
                          for (unsigned pos: gs->second)
                            {
                              formula rm = resorig[pos];
                              if (rm.is(op::M, op::R))
                                {
                                  // a | (Xa R b) = b W a
                                  // a | (Xa M b) = b U a
                                  op t = rm.is(op::M) ? op::U : op::W;
                                  assert(rm[0].is(op::X));
                                  formula a = rm[0][0];
                                  formula b = rm[1];
                                  res[pos] = formula::binop(t, b, a);
                                }
                              else
                                {
                                  // a | (b & X(b W a)) = b W a
                                  // a | (b & X(b U a)) = b U a
                                  rm = is_bXbWU(resorig[pos]);
                                  assert(rm);
                                  res[pos] = rm;
                                }
                              // Remember to kill "a".
                              tokill[n] = true;
                            }
                        }

                      // If we can remove all of them...
                      if (allofthem == 0)
                        // Make third pass to search for terms 'a'
                        // that also appears as 'XFa'.  Replace them
                        // by 'Fa' and delete XFa.
                        for (unsigned n = 0; n < mos; ++n)
                          {
                            formula x = res[n];
                            if (!x)
                              continue;
                            fset_t::const_iterator f = xfset.find(x);
                            if (f != xfset.end())
                              {
                                // x can appear only once.
                                formula ff = *f;
                                xfset.erase(f);
                                res[n] = formula::F(x);
                                // We don't need to kill "a" anymore.
                                tokill[n] = false;
                              }
                          }
                      // Kill any remaining "a", used to simplify Xa R b
                      // or Xa M b.
                      for (unsigned n = 0; n < mos; ++n)
                        if (tokill[n] && res[n])
                          res[n] = nullptr;

                      // Now rebuild the formula that remains.
                      vec xv;
                      size_t xfs = xfset.size();
                      xv.reserve(xset.size() + 1);
                      if (xfs > 0)
                        {
                          // Group all XF(a)|XF(b|c|...)|... as XF(a|b|c|...)
                          vec xfv;
                          xfv.reserve(xfs);
                          for (auto f: xfset)
                            xfv.emplace_back(f);
                          xv.emplace_back(unop_multop(op::F, op::Or, xfv));
                        }
                      // Also gather the remaining Xa | X(b|c) as X(b|c).
                      for (auto f: xset)
                        xv.emplace_back(f);
                      res.emplace_back(unop_multop(op::X, op::Or, xv));
                    }

                  // Gather all operand by type.
                  mospliter s(mospliter::Strip_X |
                              mospliter::Strip_GF |
                              mospliter::Strip_F |
                              mospliter::Split_G |
                              mospliter::Split_U_or_W |
                              mospliter::Split_R_or_M |
                              mospliter::Split_EventUniv,
                              res, c_);
                  // GF(a) | GF(b) = GF(a | b)
                  formula allGF = unop_unop_multop(op::G, op::F, op::Or,
                                                   std::move(*s.res_GF));

                  bool eu_has_F = false;
                  for (auto f: *s.res_EventUniv)
                    if (f.is(op::F))
                      eu_has_F = true;

                  // Xa | Xb = X(a | b)
                  // Xa | Xb | GF(c) = X(a | b | GF(c))
                  // For Universal&Eventual formula f1...fn we also have:
                  // Xa | Xb | f1...fn = X(a | b | f1...fn)
                  if (!s.res_X->empty() && !opt_.favor_event_univ)
                    {
                      s.res_X->emplace_back(allGF);
                      allGF = nullptr;
                      s.res_X->insert(s.res_X->end(),
                                      s.res_EventUniv->begin(),
                                      s.res_EventUniv->end());
                    }
                  else if (!opt_.favor_event_univ
                           && (!s.res_F->empty() || eu_has_F)
                           && s.res_G->empty()
                           && s.res_U_or_W->empty()
                           && s.res_R_or_M->empty()
                           && s.res_other->empty())
                    {
                      // If there is no X but some F and only
                      // eventual&universal formulae f1...fn|GF(c), do:
                      // Fa|Fb|f1...fn|GF(c) = F(a|b|f1...fn|GF(c))
                      //
                      // The reasoning here is that if we should
                      // move f1...fn|GF(c) inside the "F" only
                      // if it allows us to move all terms under F,
                      // allowing a nice initial self-loop.
                      //
                      // For instance:
                      //   F(a|GFb)  3st.6tr. with initial self-loop
                      //   Fa|GFb    4st.8tr. without initial self-loop
                      //
                      // However, if other terms are presents they will
                      // prevent the formation of a self-loop, and the
                      // rewriting is unwelcome:
                      //   F(a|GFb)|Gc  5st.11tr.  without initial self-loop
                      //   Fa|GFb|Gc    5st.10tr.  without initial self-loop
                      // (counting the number of "subtransitions"
                      // or, degeneralizing the automaton amplifies
                      // these differences)
                      s.res_F->emplace_back(allGF);
                      allGF = nullptr;
                      for (auto f: *s.res_EventUniv)
                        s.res_F->emplace_back(f.is(op::F) ? f[0] : f);
                    }
                  else if (opt_.favor_event_univ)
                    {
                      s.res_EventUniv->emplace_back(allGF);
                      allGF = nullptr;
                      bool seen_f = false;
                      if (s.res_EventUniv->size() > 1)
                        {
                          // If some of the EventUniv formulas start
                          // with an F, Gather them all under the
                          // same F.  Striping any leading F.
                          for (auto& f: *s.res_EventUniv)
                            if (f.is(op::F))
                              {
                                seen_f = true;
                                f = f[0];
                              }
                          if (seen_f)
                            {
                              formula eu =
                                unop_multop(op::F, op::Or,
                                            std::move(*s.res_EventUniv));
                              s.res_other->emplace_back(eu);
                            }
                        }
                      if (!seen_f)
                        s.res_other->insert(s.res_other->end(),
                                            s.res_EventUniv->begin(),
                                            s.res_EventUniv->end());
                    }
                  else
                    {
                      for (auto f: *s.res_EventUniv)
                        {
                          if (f.is(op::F))
                            s.res_F->emplace_back(f[0]);
                          else
                            s.res_other->emplace_back(f);
                        }
                    }
                  // Xa | Xb | f1...fn = X(a | b | f1...fn)
                  // is built at the end of this multop::Or case.
                  // F(a) | F(b) = F(a | b)
                  // is built at the end of this multop::Or case.

                  // The following three loops perform these rewritings:
                  // (a U b) | (a U c) = a U (b | c)
                  // (a W b) | (a U c) = a W (b | c)
                  // (a W b) | (a W c) = a W (b | c)
                  // (a R b) | (c R b) = (a | c) R b
                  // (a R b) | (c M b) = (a | c) R b
                  // (a M b) | (c M b) = (a | c) M b
                  // G(a) | (a U b) = a W b
                  // G(a) | (a W b) = a W b
                  // G(b) | (a R b) = a R b.
                  // G(b) | (a M b) = a R b.
                  // G(c) | F(phi & e) = c W (phi & e)  if  !c => phi.
                  typedef robin_hood::unordered_map<formula,
                                                    vec::iterator> fmap_t;
                  fmap_t uwmap; // associates "a" to "a U b" or "a W b"
                  fmap_t rmmap; // associates "b" to "a R b" or "a M b"
                  // (a U b) | (a U c) = a U (b | c)
                  // (a W b) | (a U c) = a W (b | c)
                  // (a W b) | (a W c) = a W (b | c)
                  for (auto i = s.res_U_or_W->begin();
                       i != s.res_U_or_W->end(); ++i)
                    {
                      formula a = (*i)[0];
                      auto j = uwmap.find(a);
                      if (j == uwmap.end())
                        {
                          // First occurrence.
                          uwmap[a] = i;
                          continue;
                        }
                      // We already have one occurrence.  Merge them.
                      formula old = *j->second;
                      op o = op::U;
                      if (i->is(op::W) || old.is(op::W))
                        o = op::W;
                      formula snd_arg = formula::Or({old[1], (*i)[1]});
                      *j->second = formula::binop(o, a, snd_arg);
                      assert(j->second->is(o));
                      *i = nullptr;
                    }
                  // (a R b) | (c R b) = (a | c) R b
                  // (a R b) | (c M b) = (a | c) R b
                  // (a M b) | (c M b) = (a | c) M b
                  for (auto i = s.res_R_or_M->begin();
                       i != s.res_R_or_M->end(); ++i)
                    {
                      formula b = (*i)[1];
                      auto j = rmmap.find(b);
                      if (j == rmmap.end())
                        {
                          // First occurrence.
                          rmmap[b] = i;
                          continue;
                        }
                      // We already have one occurrence.  Merge them.
                      formula old = *j->second;
                      op o = op::M;
                      if (i->is(op::R) || old.is(op::R))
                        o = op::R;
                      formula fst_arg = formula::Or({old[0], (*i)[0]});
                      *j->second = formula::binop(o, fst_arg, b);
                      assert(j->second->is(o));
                      *i = nullptr;
                    }
                  // G(a) | (a U b) = a W b
                  // G(a) | (a W b) = a W b
                  // G(b) | (a R b) = a R b.
                  // G(b) | (a M b) = a R b.
                  for (auto& f: *s.res_G)
                    {
                      bool superfluous = false;
                      formula c = f[0];

                      fmap_t::iterator j = uwmap.find(c);
                      if (j != uwmap.end())
                        {
                          superfluous = true;
                          formula bo = *j->second;
                          if (bo.is(op::U))
                            {
                              *j->second = formula::W(bo[0], bo[1]);
                              assert(j->second->is(op::W));
                            }
                        }
                      j = rmmap.find(c);
                      if (j != rmmap.end())
                        {
                          superfluous = true;
                          formula bo = *j->second;
                          if (bo.is(op::M))
                            {
                              *j->second = formula::R(bo[0], bo[1]);
                              assert(j->second->is(op::R));
                            }
                        }
                      if (opt_.synt_impl | opt_.containment_checks)
                        {
                          // if the input looks like o1&e1&e2&o2,
                          // return o1 & o2.  The input must have at
                          // least on eventual formula.
                          auto extract_not_ev =
                            [&](formula f) {
                              if (f.is(op::And))
                                for (auto e: f)
                                  if (e.is_eventual())
                                    {
                                      vec phi;
                                      phi.reserve(f.size());
                                      for (auto ee: f)
                                        if (!ee.is_eventual())
                                          phi.push_back(ee);
                                      return formula::And(phi);
                                    }
                              return formula(nullptr);
                            };

                          // G(c) | F(phi & e) = c W (phi & e)  if  !c => phi.
                          for (auto in_f = s.res_F->begin();
                               in_f != s.res_F->end();)
                            {
                              if (formula phi = extract_not_ev(*in_f))
                                if (c_->implication_neg(c, phi, false))
                                  {
                                    s.res_other->push_back(formula::W(c,
                                                                      *in_f));
                                    in_f = s.res_F->erase(in_f);
                                    superfluous = true;
                                    continue;
                                  }
                              ++in_f;
                            }
                        }
                      if (superfluous)
                        f = nullptr;
                    }

                  s.res_other->reserve(s.res_other->size()
                                       + s.res_G->size()
                                       + s.res_U_or_W->size()
                                       + s.res_R_or_M->size()
                                       + 3);
                  s.res_other->insert(s.res_other->end(),
                                      s.res_G->begin(),
                                      s.res_G->end());
                  s.res_other->insert(s.res_other->end(),
                                      s.res_U_or_W->begin(),
                                      s.res_U_or_W->end());
                  s.res_other->insert(s.res_other->end(),
                                      s.res_R_or_M->begin(),
                                      s.res_R_or_M->end());

                  // Those "F" formulae that are universal can be
                  // postponed inside the X term if there is one.
                  //
                  // In effect we rewrite
                  //   Xa|Xb|FGc|FGd|Fe as X(a|b|F(Gc|Gd))|Fe
                  if (!s.res_X->empty())
                    {
                      vec univ;
                      for (auto& f: *s.res_F)
                        if (f.is_universal())
                          {
                            univ.emplace_back(f);
                            f = nullptr; // Remove it from res_F.
                          }
                      s.res_X->emplace_back(unop_multop(op::F, op::Or,
                                                     std::move(univ)));
                    }

                  // F(a) | F(b) | ... = F(a | b | ...)
                  formula allF = unop_multop(op::F, op::Or,
                                             std::move(*s.res_F));
                  // Xa | Xb | ... = X(a | b | ...)
                  formula allX = unop_multop(op::X, op::Or,
                                             std::move(*s.res_X));
                  s.res_other->emplace_back(allX);
                  s.res_other->emplace_back(allF);
                  s.res_other->emplace_back(allGF);
                  formula r = formula::Or(std::move(*s.res_other));
                  // If we altered the formula in some way, process
                  // it another time.
                  if (r != mo)
                    return recurse(r);
                  return r;
                }
              case op::AndNLM:
                {
                  mospliter s(mospliter::Split_Bool, res, c_);
                  if (!s.res_Bool->empty())
                    {
                      // b1 & b2 & b3 = b1 ∧ b2 ∧ b3
                      formula b = formula::And(std::move(*s.res_Bool));

                      // now we just consider  b & rest
                      formula rest = formula::AndNLM(std::move(*s.res_other));

                      // We have  b & rest = b : rest  if rest does not
                      // accept [*0]. Otherwise  b & rest = b | (b : rest)
                      // FIXME: It would be nice to remove [*0] from rest.
                      formula r = nullptr;
                      if (rest.accepts_eword())
                        {
                          // The b & rest = b | (b : rest) rewriting
                          // augment the size, so do that only when
                          // explicitly requested.
                          if (!opt_.reduce_size_strictly)
                            return recurse(formula::OrRat
                                           ({b, formula::Fusion({b, rest})}));
                          else
                            return mo;
                        }
                      else
                        {
                          return recurse(formula::Fusion({b, rest}));
                        }
                    }
                  // No Boolean as argument of &&.

                  // Look for occurrences of {b;r} or {b:r}.  We have
                  // {b1;r1}&{b2;r2} = {b1∧b2};{r1&r2}
                  //                    head1   tail1
                  // {b1:r1}&{b2:r2} = {b1∧b2}:{r1&r2}
                  //                    head2   tail2
                  // BEWARE: The second rule is correct only when
                  // both r1 and r2 do not accept [*0].

                  vec head1;
                  vec tail1;
                  vec head2;
                  vec tail2;
                  for (auto& i: *s.res_other)
                    {
                      if (!i)
                        continue;
                      if (!i.is(op::Concat, op::Fusion))
                        continue;
                      formula h = i[0];
                      if (!h.is_boolean())
                        continue;
                      if (i.is(op::Concat))
                        {
                          head1.emplace_back(h);
                          tail1.emplace_back(i.all_but(0));
                        }
                      else // op::Fusion
                        {
                          formula t = i.all_but(0);
                          if (t.accepts_eword())
                            continue;
                          head2.emplace_back(h);
                          tail2.emplace_back(t);
                        }
                      i = nullptr;
                    }
                  if (!head1.empty())
                    {
                      formula h = formula::And(std::move(head1));
                      formula t = formula::AndNLM(std::move(tail1));
                      s.res_other->emplace_back(formula::Concat({h, t}));
                    }
                  if (!head2.empty())
                    {
                      formula h = formula::And(std::move(head2));
                      formula t = formula::AndNLM(std::move(tail2));
                      s.res_other->emplace_back(formula::Fusion({h, t}));
                    }

                  formula r = formula::AndNLM(std::move(*s.res_other));
                  // If we altered the formula in some way, process
                  // it another time.
                  if (r != mo)
                    return recurse(r);
                  return r;
                }
              case op::OrRat:
              case op::Concat:
              case op::Fusion:
                // FIXME: No simplifications yet.
                return mo;
              default:
                SPOT_UNIMPLEMENTED();
                return nullptr;
              }
            SPOT_UNREACHABLE();
          }
        return mo;
      }

    protected:
      tl_simplifier_cache* c_;
      const tl_simplifier_options& opt_;
    };


    formula
    simplify_recursively(formula f,
                         tl_simplifier_cache* c)
    {
#ifdef TRACE
      static int srec = 0;
      for (int i = srec; i; --i)
        trace << ' ';
      trace << "** simplify_recursively(" << str_psl(f) << ')';
#endif

      formula result = c->lookup_simplified(f);
      if (result)
        {
          trace << " cached: " << str_psl(result) << std::endl;
          return result;
        }
      else
        {
          trace << " miss" << std::endl;
        }

#ifdef TRACE
      ++srec;
#endif

      if (f.is_boolean() && c->options.boolean_to_isop)
        {
          result = c->boolean_to_isop(f);
        }
      else
        {
          simplify_visitor v(c);
          result = v.visit(f);
        }

#ifdef TRACE
      --srec;
      for (int i = srec; i; --i)
        trace << ' ';
      trace << "** simplify_recursively(" << str_psl(f) << ") result: "
            << str_psl(result) << std::endl;
#endif

      c->cache_simplified(f, result);
      return result;
    }

  } // anonymous namespace

    //////////////////////////////////////////////////////////////////////
    // tl_simplifier_cache


    // This implements the recursive rules for syntactic implication.
    // (To follow this code please look at the table given as an
    // appendix in the documentation for temporal logic operators.)
  inline
  bool
  tl_simplifier_cache::syntactic_implication_aux(formula f, formula g)
  {
    // We first process all lines from the table except the
    // first two, and then we process the first two as a fallback.
    //
    // However for Boolean formulas we skip the bottom lines
    // (keeping only the first one) to prevent them from being
    // further split.
    if (!f.is_boolean())
      // Deal with all lines of the table except the first two.
      switch (f.kind())
        {
        case op::X:
        case op::strong_X:
          if (g.is_eventual() && syntactic_implication(f[0], g))
            return true;
          if (g.is(op::X, op::strong_X) && syntactic_implication(f[0], g[0]))
            return true;
          break;

        case op::F:
          if (g.is_eventual() && syntactic_implication(f[0], g))
            return true;
          break;

        case op::G:
          if (g.is(op::U, op::R) && syntactic_implication(f[0], g[1]))
            return true;
          if (g.is(op::W) && (syntactic_implication(f[0], g[0])
                              || syntactic_implication(f[0], g[1])))
            return true;
          if (g.is(op::M) && (syntactic_implication(f[0], g[0])
                              && syntactic_implication(f[0], g[1])))
            return true;
          // First column.
          if (syntactic_implication(f[0], g))
            return true;
          break;

        case op::U:
          {
            formula f1 = f[0];
            formula f2 = f[1];
            if (g.is(op::U, op::W)
                && syntactic_implication(f1, g[0])
                && syntactic_implication(f2, g[1]))
              return true;
            if (g.is(op::M, op::R)
                && syntactic_implication(f1, g[1])
                && syntactic_implication(f2, g[0])
                && syntactic_implication(f2, g[1]))
              return true;
            if (g.is(op::F) && syntactic_implication(f2, g[0]))
              return true;
            // First column.
            if (syntactic_implication(f1, g) && syntactic_implication(f2, g))
              return true;
            break;
          }
        case op::W:
          {
            formula f1 = f[0];
            formula f2 = f[1];
            if (g.is(op::U) && (syntactic_implication(f1, g[1])
                                && syntactic_implication(f2, g[1])))
              return true;
            if (g.is(op::W) && (syntactic_implication(f1, g[0])
                                && syntactic_implication(f2, g[1])))
              return true;
            if (g.is(op::R) && (syntactic_implication(f1, g[1])
                                && syntactic_implication(f2, g[0])
                                && syntactic_implication(f2, g[1])))
              return true;
            if (g.is(op::F) && (syntactic_implication(f1, g[0])
                                && syntactic_implication(f2, g[0])))
              return true;
            // First column.
            if (syntactic_implication(f1, g) && syntactic_implication(f2, g))
              return true;
            break;
          }
        case op::R:
          {
            formula f1 = f[0];
            formula f2 = f[1];
            if (g.is(op::W) && (syntactic_implication(f1, g[1])
                                && syntactic_implication(f2, g[0])))
              return true;
            if (g.is(op::R) && (syntactic_implication(f1, g[0])
                                && syntactic_implication(f2, g[1])))
              return true;
            if (g.is(op::M) && (syntactic_implication(f2, g[0])
                                && syntactic_implication(f2, g[1])))
              return true;
            if (g.is(op::F) && syntactic_implication(f2, g[0]))
              return true;
            // First column.
            if (syntactic_implication(f2, g))
              return true;
            break;
          }
        case op::M:
          {
            formula f1 = f[0];
            formula f2 = f[1];
            if (g.is(op::U, op::W) && (syntactic_implication(f1, g[1])
                                       && syntactic_implication(f2,
                                                                g[0])))
              return true;
            if (g.is(op::R, op::M) && (syntactic_implication(f1, g[0])
                                       && syntactic_implication(f2,
                                                                g[1])))
              return true;
            if (g.is(op::F) && (syntactic_implication(f1, g[0])
                                || syntactic_implication(f2, g[0])))
              return true;
            // First column.
            if (syntactic_implication(f2, g))
              return true;
            break;
          }
        case op::Or:
          {
            // If we are checking something like
            //   (a | b | Xc) => g,
            // split it into
            //   (a | b) => g
            //   Xc      => g
            unsigned i = 0;
            if (formula bops = f.boolean_operands(&i))
              if (!syntactic_implication(bops, g))
                break;
            bool b = true;
            unsigned fs = f.size();
            for (; i < fs; ++i)
              if (!syntactic_implication(f[i], g))
                {
                  b = false;
                  break;
                }
            if (b)
              return true;
            break;
          }
        case op::And:
          {
            // If we are checking something like
            //   (a & b & Xc) => g,
            // split it into
            //   (a & b) => g
            //   Xc      => g
            unsigned i = 0;
            if (formula bops = f.boolean_operands(&i))
              if (syntactic_implication(bops, g))
                return true;
            unsigned fs = f.size();
            for (; i < fs; ++i)
              if (syntactic_implication(f[i], g))
                return true;
            break;
          }
        default:
          break;
        }
    // First two lines of the table.
    // (Don't check equality, it has already be done.)
    if (!g.is_boolean())
      switch (g.kind())
        {
        case op::F:
          if (syntactic_implication(f, g[0]))
            return true;
          break;

        case op::G:
        case op::X:
        case op::strong_X:
          if (f.is_universal() && syntactic_implication(f, g[0]))
            return true;
          break;

        case op::U:
        case op::W:
          if (syntactic_implication(f, g[1]))
            return true;
          break;

        case op::M:
        case op::R:
          if (syntactic_implication(f, g[0])
              && syntactic_implication(f, g[1]))
            return true;
          break;

        case op::And:
          {
            // If we are checking something like
            //   f => (a & b & Xc),
            // split it into
            //   f => (a & b)
            //   f => Xc
            unsigned i = 0;
            if (formula bops = g.boolean_operands(&i))
              if (!syntactic_implication(f, bops))
                break;
            bool b = true;
            unsigned gs = g.size();
            for (; i < gs; ++i)
              if (!syntactic_implication(f, g[i]))
                {
                  b = false;
                  break;
                }
            if (b)
              return true;
            break;
          }

        case op::Or:
          {
            // If we are checking something like
            //   f => (a | b | Xc),
            // split it into
            //   f => (a | b)
            //   f => Xc
            unsigned i = 0;
            if (formula bops = g.boolean_operands(&i))
              if (syntactic_implication(f, bops))
                return true;
            unsigned gs = g.size();
            for (; i < gs; ++i)
              if (syntactic_implication(f, g[i]))
                return true;
            break;
          }
        default:
          break;
        }
    return false;
  }

  // Return true if f => g syntactically
  bool
  tl_simplifier_cache::syntactic_implication(formula f,
                                              formula g)
  {
    // We cannot run syntactic_implication on SERE formulae,
    // except on Boolean formulae.
    if (f.is_sere_formula() && !f.is_boolean())
      return false;
    if (g.is_sere_formula() && !g.is_boolean())
      return false;

    if (f == g)
      return true;
    if (g.is_tt() || f.is_ff())
      return true;
    if (g.is_ff() || f.is_tt())
      return false;

    // Often we compare a literal (an atomic_prop or its negation)
    // to another literal.  The result is necessarily false. To be
    // true, the two literals would have to be equal, but we have
    // already checked that.
    if (f.is_literal() && g.is_literal())
      return false;

    // Cache lookup
    {
      pairf p(f, g);
      syntimpl_cache_t::const_iterator i = syntimpl_.find(p);
      if (i != syntimpl_.end())
        return i->second;
    }

    bool result;

    if (f.is_boolean() && g.is_boolean())
      result = bdd_implies(as_bdd(f), as_bdd(g));
    else
      result = syntactic_implication_aux(f, g);

    // Cache result
    {
      pairf p(f, g);
      syntimpl_[p] = result;
      // std::cerr << str_psl(f) << (result ? " ==> " : " =/=> ")
      //           << str_psl(g) << std::endl;
    }

    return result;
  }

  // If right==false, true if !f1 => f2, false otherwise.
  // If right==true, true if f1 => !f2, false otherwise.
  bool
  tl_simplifier_cache::syntactic_implication_neg(formula f1,
                                                  formula f2,
                                                  bool right)
  {
    // We cannot run syntactic_implication_neg on SERE formulae,
    // except on Boolean formulae.
    if (f1.is_sere_formula() && !f1.is_boolean())
      return false;
    if (f2.is_sere_formula() && !f2.is_boolean())
      return false;
    if (right)
      f2 = nenoform_rec(f2, true, this, false);
    else
      f1 = nenoform_rec(f1, true, this, false);
    return syntactic_implication(f1, f2);
  }


  /////////////////////////////////////////////////////////////////////
  // tl_simplifier

  tl_simplifier::tl_simplifier(const bdd_dict_ptr& d)
  {
    cache_ = new tl_simplifier_cache(d);
  }

  tl_simplifier::tl_simplifier(const tl_simplifier_options& opt,
                                 bdd_dict_ptr d)
  {
    cache_ = new tl_simplifier_cache(d, opt);
  }

  tl_simplifier::~tl_simplifier()
  {
    delete cache_;
  }

  formula
  tl_simplifier::simplify(formula f)
  {
    if (!f.is_in_nenoform())
      f = negative_normal_form(f, false);
    return simplify_recursively(f, cache_);
  }

  tl_simplifier_options&
  tl_simplifier::options()
  {
    return cache_->options;
  }

  formula
  tl_simplifier::negative_normal_form(formula f, bool negated)
  {
    return nenoform_rec(f, negated, cache_, false);
  }

  bool
  tl_simplifier::syntactic_implication(formula f1, formula f2)
  {
    return cache_->syntactic_implication(f1, f2);
  }

  bool
  tl_simplifier::syntactic_implication_neg(formula f1,
                                            formula f2, bool right)
  {
    return cache_->syntactic_implication_neg(f1, f2, right);
  }

  bool
  tl_simplifier::are_equivalent(formula f, formula g)
  {
    return cache_->lcc.equal(f, g);
  }

  bool
  tl_simplifier::implication(formula f, formula g)
  {
    return cache_->lcc.contained(f, g);
  }

  bdd
  tl_simplifier::as_bdd(formula f)
  {
    return cache_->as_bdd(f);
  }

  formula
  tl_simplifier::star_normal_form(formula f)
  {
    return cache_->star_normal_form(f);
  }

  formula
  tl_simplifier::boolean_to_isop(formula f)
  {
    return cache_->boolean_to_isop(f);
  }

  bdd_dict_ptr
  tl_simplifier::get_dict() const
  {
    return cache_->dict;
  }

  void
  tl_simplifier::print_stats(std::ostream& os) const
  {
    cache_->print_stats(os);
  }

  void
  tl_simplifier::clear_as_bdd_cache()
  {
    cache_->clear_as_bdd_cache();
    cache_->lcc.clear();
  }

  void
  tl_simplifier::clear_caches()
  {
    tl_simplifier_cache* c =
      new tl_simplifier_cache(get_dict(), cache_->options);
    std::swap(c, cache_);
    delete c;
  }
}
