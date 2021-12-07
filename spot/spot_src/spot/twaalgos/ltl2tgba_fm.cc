// -*- coding: utf-8 -*-
// Copyright (C) 2008-2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003-2006 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <spot/misc/hash.hh>
#include <spot/misc/bddlt.hh>
#include <spot/misc/minato.hh>
#include <spot/tl/nenoform.hh>
#include <spot/tl/print.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/mark.hh>
#include <cassert>
#include <memory>
#include <utility>
#include <algorithm>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/priv/robin_hood.hh>

namespace spot
{
  namespace
  {
    typedef std::vector<formula> vec;

    // This should only be called on And formulae and return
    // the set of subformula that are implied by the formulas
    // already in the And.
    // If f = Ga & (b R c) & G(d & (e R (g R h)) & Xj) & Xk this
    // returns the set {a,  # implied by Ga
    //                  c,  # implied by b R c
    //                  d, e R (g R h), g R h, h, Xj # implied by G(d & ...)
    //                 }
    // Leave recurring to false on first call.
    typedef std::set<formula> formula_set;
    void
    implied_subformulae(formula f, formula_set& rec, bool recurring = false)
    {
      if (!f.is(op::And))
        {
          // Only recursive calls should be made with an operator that
          // is not And.
          assert(recurring);
          rec.insert(f);
          return;
        }
      unsigned s = f.size();
      for (unsigned n = 0; n < s; ++n)
        {
          formula sub = f[n];
          // Recurring is set if we are under "G(...)" or "0 R (...)"
          // or (...) W 0".
          if (recurring)
            rec.insert(sub);
          if (sub.is(op::G))
            {
              implied_subformulae(sub[0], rec, true);
            }
          else if (sub.is(op::W))
            {
              // f W 0 = Gf
              if (sub[1].is_ff())
                implied_subformulae(sub[0], rec, true);
            }
          else
            while (sub.is(op::R, op::M))
              {
                // in 'f R g' and 'f M g' always evaluate 'g'.
                formula b = sub;
                sub = b[1];
                if (b[0].is_ff())
                  {
                    assert(b.is(op::R)); // because 0 M g = 0
                    // 0 R f = Gf
                    implied_subformulae(sub, rec, true);
                    break;
                  }
                rec.insert(sub);
              }
        }
    }

    class translate_dict;

    class ratexp_to_dfa final
    {
      typedef twa_graph::namer<formula> namer;
    public:
      ratexp_to_dfa(translate_dict& dict);
      std::tuple<const_twa_graph_ptr, const namer*, const state*>
      succ(formula f);
      ~ratexp_to_dfa();

    protected:
      // Use robin_hood::pair because std::pair is not no-throw constructible
      typedef robin_hood::pair<twa_graph_ptr, const namer*> labelled_aut;
      labelled_aut translate(formula f);

    private:
      translate_dict& dict_;
      typedef robin_hood::unordered_node_map<formula, labelled_aut> f2a_t;
      std::vector<labelled_aut> automata_;
      f2a_t f2a_;
    };

    // Helper dictionary.  We represent formulae using BDDs to
    // simplify them, and then translate BDDs back into formulae.
    //
    // The name of the variables are inspired from Couvreur's FM paper.
    //   "a" variables are promises (written "a" in the paper)
    //   "next" variables are X's operands (the "r_X" variables from the paper)
    //   "var" variables are atomic propositions.
    class translate_dict final
    {
    public:

      translate_dict(twa_graph_ptr& a, tl_simplifier* ls, bool exprop,
                     bool single_acc, bool unambiguous)
        : a_(a),
          dict(a->get_dict()),
          ls(ls),
          a_set(bddtrue),
          var_set(bddtrue),
          next_set(bddtrue),
          transdfa(*this),
          exprop(exprop),
          single_acc(single_acc),
          acc(a->acc()),
          unambiguous(unambiguous)
      {
      }

      ~translate_dict()
      {
        dict->unregister_all_my_variables(this);
      }

      twa_graph_ptr& a_;
      bdd_dict_ptr dict;
      tl_simplifier* ls;
      mark_tools mt;

      typedef robin_hood::unordered_flat_map<formula, int> fv_map;
      typedef std::vector<formula> vf_map;

      fv_map next_map;               ///< Maps "Next" variables to BDD variables
      vf_map next_formula_map; ///< Maps BDD variables to "Next" variables

      bdd a_set;
      bdd var_set;
      bdd next_set;

      ratexp_to_dfa transdfa;
      bool exprop;
      bool single_acc;
      acc_cond& acc;
      // Map BDD variables to acceptance marks.
      robin_hood::unordered_flat_map<int, unsigned> bm;
      bool unambiguous;

      enum translate_flags
        {
          flags_none = 0,
          // Keep these bits slightly apart as we will use them as-is
          // in the hash function for flagged_formula.
          flags_mark_all = (1<<10),
          flags_recurring = (1<<14),
        };

      struct flagged_formula
      {
        formula f;
        unsigned flags;                // a combination of translate_flags

        bool
        operator==(const flagged_formula& other) const
        {
          return this->f == other.f && this->flags == other.flags;
        }
      };

      struct flagged_formula_hash:
        public std::unary_function<flagged_formula, size_t>
      {
        size_t
        operator()(const flagged_formula& that) const
        {
          return that.f.id() ^ size_t(that.flags);
        }
      };

      struct translated
      {
        bdd symbolic;
        bool has_rational:1;
        bool has_marked:1;
      };

      typedef robin_hood::unordered_node_map
        <flagged_formula, translated, flagged_formula_hash>
        flagged_formula_to_bdd_map;
    private:
      flagged_formula_to_bdd_map ltl_bdd_;

    public:


      int
      register_proposition(formula f)
      {
        int num = dict->register_proposition(f, this);
        var_set &= bdd_ithvar(num);
        return num;
      }

      acc_cond::mark_t
      bdd_to_mark(bdd a)
      {
        bdd o = a;
        if (a == bddtrue)
          return {};
        assert(a != bddfalse);
        acc_cond::mark_t res = {};
        do
          {
            int v = bdd_var(a);
            bdd h = bdd_high(a);
            a = bdd_low(a);
            if (h != bddfalse)
              {
                res.set(bm[v]);
                if (a == bddfalse)
                  a = h;
              }
          }
        while (a != bddtrue);
        return res;
      }

      int
      register_a_variable(formula f)
      {
        if (single_acc)
          {
            int num = dict->register_acceptance_variable(formula::tt(), this);
            a_set &= bdd_ithvar(num);

            auto p = bm.emplace(num, 0U);
            if (p.second)
              p.first->second = acc.add_set();
            return num;
          }
        // A promise of 'x', noted P(x) is pretty much like the F(x)
        // LTL formula, it ensure that 'x' will be fulfilled (= not
        // promised anymore) eventually.
        // So   a U b = ((a&Pb) W b)
        //      a U (b U c) = (a&P(b U c)) W (b&P(c) W c)
        // the latter encoding may be simplified to
        //      a U (b U c) = (a&P(c)) W (b&P(c) W c)
        //
        // Similarly
        //      a M b = (a R (b&P(a)))
        //      (a M b) M c = (a R (b & Pa)) R (c & P(a M b))
        //                  = (a R (b & Pa)) R (c & P(a & b))
        //
        // The code below therefore implement the following
        // rules:
        // P(a U b) = P(b)
        // P(F(a))  = P(a)
        // P(a M b) = P(a & b)
        //
        // The latter rule INCORRECTLY appears as P(a M b)=P(a)
        // in section 3.5 of
        //    "LTL translation improvements in Spot 1.0",
        //     A. Duret-Lutz. IJCCBS 5(1/2):31-54, March 2014.
        // and was unfortunately implemented this way until Spot
        // 1.2.4.  A counterexample is given by the formula
        //    G(Fa & ((a M b) U ((c U !d) M d)))
        // that was found by Joachim Klein.  Here P((c U !d) M d)
        // and P(c U !d) should not both be simplified to P(!d).
        for (;;)
          {
            if (f.is(op::U))
              {
                // P(a U b) = P(b)
                f = f[1];
              }
            else if (f.is(op::M))
              {
                // P(a M b) = P(a & b)
                formula g = formula::And({f[0], f[1]});
                int num = dict->register_acceptance_variable(g, this);
                a_set &= bdd_ithvar(num);

                auto p = bm.emplace(num, 0U);
                if (p.second)
                  p.first->second = acc.add_set();
                return num;
              }
            else if (f.is(op::F))
              {
                // P(F(a)) = P(a)
                f = f[0];
              }
            else
              {
                break;
              }
          }
        int num = dict->register_acceptance_variable(f, this);
        a_set &= bdd_ithvar(num);

        auto p = bm.emplace(num, 0U);
        if (p.second)
          p.first->second = acc.add_set();

        return num;
      }

      int
      register_next_variable(formula f)
      {
        int num;
        // Do not build a Next variable that already exists.
        fv_map::iterator sii = next_map.find(f);
        if (sii != next_map.end())
          {
            num = sii->second;
          }
        else
          {
            num = dict->register_anonymous_variables(1, this);
            next_map[f] = num;
            next_formula_map.resize(bdd_varnum());
            next_formula_map[num] = f;
          }
        next_set &= bdd_ithvar(num);
        return num;
      }

      std::ostream&
      dump(std::ostream& os) const
      {
        os << "Next Variables:\n";
        for (auto& fi: next_map)
        {
          os << "  " << fi.second << ": Next[";
          print_psl(os, fi.first) << "]\n";
        }
        os << "Shared Dict:\n";
        dict->dump(os);
        return os;
      }

      formula
      var_to_formula(int var) const
      {
        const bdd_dict::bdd_info& i = dict->bdd_map[var];
        if (i.type != bdd_dict::anon)
          {
            assert(i.type == bdd_dict::acc || i.type == bdd_dict::var);
            return i.f;
          }
        formula f = next_formula_map[var];
        assert(f);
        return f;
      }

      bdd
      boolean_to_bdd(formula f)
      {
        bdd res = ls->as_bdd(f);
        var_set &= bdd_support(res);

        bdd all = var_set;
        while (all != bddfalse)
          {
            bdd one = bdd_satone(all);
            all -= one;
            while (one != bddtrue)
              {
                int v = bdd_var(one);
                a_->register_ap(var_to_formula(v));
                if (bdd_high(one) == bddfalse)
                  one = bdd_low(one);
                else
                  one = bdd_high(one);
              }
          }


        return res;
      }

      formula
      conj_bdd_to_formula(bdd b, op o = op::And) const
      {
        if (b == bddtrue)
          return formula::tt();
        if (b == bddfalse)
          return formula::ff();
        // Unroll the first loop of the next do/while loop so that we
        // do not have to create v when b is not a conjunction.
        formula res = var_to_formula(bdd_var(b));
        bdd high = bdd_high(b);
        if (high == bddfalse)
          {
            res = formula::Not(res);
            b = bdd_low(b);
          }
        else
          {
            assert(bdd_low(b) == bddfalse);
            b = high;
          }
        if (b == bddtrue)
          return res;
        vec v{std::move(res)};
        do
          {
            res = var_to_formula(bdd_var(b));
            high = bdd_high(b);
            if (high == bddfalse)
              {
                res = formula::Not(res);
                b = bdd_low(b);
              }
            else
              {
                assert(bdd_low(b) == bddfalse);
                b = high;
              }
            assert(b != bddfalse);
            v.emplace_back(std::move(res));
          }
        while (b != bddtrue);
        return formula::multop(o, std::move(v));
      }

      formula
      conj_bdd_to_sere(bdd b) const
      {
        return conj_bdd_to_formula(b, op::AndRat);
      }

      formula
      bdd_to_formula(bdd f)
      {
        if (f == bddfalse)
          return formula::ff();

        vec v;
        minato_isop isop(f);
        bdd cube;
        while ((cube = isop.next()) != bddfalse)
          v.emplace_back(conj_bdd_to_formula(cube));
        return formula::Or(std::move(v));
      }

      formula
      bdd_to_sere(bdd f)
      {
        if (f == bddfalse)
          return formula::ff();

        vec v;
        minato_isop isop(f);
        bdd cube;
        while ((cube = isop.next()) != bddfalse)
          v.emplace_back(conj_bdd_to_sere(cube));
        return formula::OrRat(std::move(v));
      }

      const translated&
      ltl_to_bdd(formula f, bool mark_all, bool recurring = false);

    };

#ifdef __GNUC__
#  define unused __attribute__((unused))
#else
#  define unused
#endif

    // Debugging function.
    static unused
    std::ostream&
    trace_ltl_bdd(const translate_dict& d, bdd f)
    {
      std::cerr << "Displaying BDD ";
      bdd_print_set(std::cerr, d.dict, f) << ":\n";

      minato_isop isop(f);
      bdd cube;
      while ((cube = isop.next()) != bddfalse)
        {
          bdd label = bdd_exist(cube, d.next_set);
          bdd dest_bdd = bdd_existcomp(cube, d.next_set);
          formula dest = d.conj_bdd_to_formula(dest_bdd);
          bdd_print_set(std::cerr, d.dict, label) << " => ";
          bdd_print_set(std::cerr, d.dict, dest_bdd) << " = ";
          print_psl(std::cerr, dest) << '\n';
        }
      return std::cerr;
    }

    bdd translate_ratexp(formula f, translate_dict& dict,
                         formula to_concat = nullptr);

    // Rewrite rule for rational operators.
    class ratexp_trad_visitor final
    {
    public:
      // negated should only be set for constants or atomic properties
      ratexp_trad_visitor(translate_dict& dict, formula to_concat = nullptr)
        : dict_(dict), to_concat_(to_concat)
      {
      }

      bdd next_to_concat()
      {
        // Encoding X[*0] when there is nothing to concatenate is a
        // way to ensure that we distinguish the rational formula "a"
        // (encoded as "a&X[*0]") from the rational formula "a;[*]"
        // (encoded as "a&X[*]").
        //
        // It's important that when we do "a && (a;[*])" we do not get
        // "a;[*]" as it would occur if we had simply encoded "a" as
        // "a".
        if (!to_concat_)
          to_concat_ = formula::eword();
        int x = dict_.register_next_variable(to_concat_);
        return bdd_ithvar(x);
      }

      bdd now_to_concat()
      {
        if (to_concat_ && !to_concat_.is(op::eword))
          return recurse(to_concat_);
        return bddfalse;
      }

      // Append to_concat_ to all Next variables in IN.
      bdd concat_dests(bdd in)
      {
        if (!to_concat_)
          return in;
        minato_isop isop(in);
        bdd cube;
        bdd out = bddfalse;
        while ((cube = isop.next()) != bddfalse)
          {
            bdd label = bdd_exist(cube, dict_.next_set);
            bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
            formula dest = dict_.conj_bdd_to_sere(dest_bdd);
            if (dest.is(op::eword))
              {
                out |= label & next_to_concat();
              }
            else
              {
                formula dest2 = formula::Concat({dest, to_concat_});
                if (!dest2.is_ff())
                  out |=
                    label & bdd_ithvar(dict_.register_next_variable(dest2));
              }
          }
        return out;
      }

      bdd visit(formula f)
      {
        switch (op o = f.kind())
          {
          case op::ff:
            return bddfalse;
          case op::tt:
            return next_to_concat();
          case op::eword:
            return now_to_concat();
          case op::ap:
            return (bdd_ithvar(dict_.register_proposition(f))
                    & next_to_concat());
          case op::F:
          case op::G:
          case op::X:
          case op::strong_X:
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
            SPOT_UNREACHABLE();        // Because not rational operator
          case op::Not:
            {
              // Not can only appear in front of Boolean
              // expressions.
              formula g = f[0];
              assert(g.is_boolean());
              return (!recurse(g)) & next_to_concat();
            }
          case op::Star:
          case op::FStar:
            {
              formula bo = f;
              unsigned min = f.min();
              unsigned max = f.max();
              assert(max > 0);

              // we will interpret
              //         c[*i..j]
              //     or  c[:*i..j]
              // as
              //         c;c[*i-1..j-1]
              //     or  c:c[*i-1..j-1]
              //           \........../
              //            this is f
              unsigned min2 = (min == 0) ? 0 : (min - 1);
              unsigned max2 =
                max == formula::unbounded() ? formula::unbounded() : (max - 1);
              f = formula::bunop(o, f[0], min2, max2);

              // If we have something to append, we can actually append it
              // to f.  This is correct even in the case of FStar, as f
              // cannot accept [*0].
              if (to_concat_)
                f = formula::Concat({f, to_concat_});

              if (o == op::Star)
                {
                  if (!bo[0].accepts_eword())
                    {
                      //   f*;g  ->  f;f*;g | g
                      //
                      // If f does not accept the empty word, we can easily
                      // add "f*;g" as to_concat_ when translating f.
                      bdd res = recurse(bo[0], f);
                      if (min == 0)
                        res |= now_to_concat();
                      return res;
                    }
                  else
                    {
                      // if "f" accepts the empty word, doing the above
                      // would lead to an infinite loop:
                      //   f*;g -> f;f*;g | g
                      //   f;f*;g -> f*;g | ...
                      //
                      // So we do it in three steps:
                      //  1. translate f,
                      //  2. append f*;g to all destinations
                      //  3. add |g
                      bdd res = recurse(bo[0]);
                      //   f*;g  ->  f;f*;g
                      minato_isop isop(res);
                      bdd cube;
                      res = bddfalse;
                      while ((cube = isop.next()) != bddfalse)
                        {
                          bdd label = bdd_exist(cube, dict_.next_set);
                          bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
                          formula dest = dict_.conj_bdd_to_sere(dest_bdd);
                          if (dest.is(op::eword))
                            {
                              res |= label &
                                bdd_ithvar(dict_.register_next_variable(f));
                            }
                          else
                            {
                              formula dest2 = formula::Concat({dest, f});
                              if (!dest2.is_ff())
                                res |= label & bdd_ithvar
                                  (dict_.register_next_variable(dest2));
                            }
                        }
                      return res | now_to_concat();
                    }
                }
              else // FStar
                {
                  bdd tail_bdd;
                  bool tail_computed = false;

                  minato_isop isop(recurse(bo[0]));
                  bdd cube;
                  bdd res = bddfalse;
                  if (min == 0)
                    {
                      // f[:*0..j];g  can be satisfied by X(g).
                      res = next_to_concat();
                    }
                  while ((cube = isop.next()) != bddfalse)
                    {
                      bdd label = bdd_exist(cube, dict_.next_set);
                      bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
                      formula dest = dict_.conj_bdd_to_sere(dest_bdd);

                      // The destination is a final state.  Make sure we
                      // can also exit if tail is satisfied.  We do not
                      // even have to check the tail if min == 0.
                      if (dest.accepts_eword() && min != 0)
                        {
                          if (!tail_computed)
                            {
                              tail_bdd = recurse(f); // FIXME: inf call!!!
                              tail_computed = true;
                            }
                          res |= label & tail_bdd;
                        }

                      // If the destination is not 0 or [*0], it means it
                      // can have successors.  Fusion the tail.
                      if (!dest.is(op::ff, op::eword))
                        {
                          formula dest2 = formula::Fusion({dest, f});
                          if (!dest2.is_ff())
                            res |= label &
                              bdd_ithvar(dict_.register_next_variable(dest2));
                        }
                    }
                  return res;
                }
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
          case op::And:
          case op::Or:
            // Not a rational operator
            SPOT_UNREACHABLE();
          case op::AndNLM:
            {
              unsigned s = f.size();
              vec final;
              vec non_final;

              for (auto g: f)
                if (g.accepts_eword())
                  final.emplace_back(g);
                else
                  non_final.emplace_back(g);

              if (non_final.empty())
                // (a* & b*);c = (a*|b*);c
                return recurse_and_concat(formula::OrRat(std::move(final)));
              if (!final.empty())
                {
                  // let F_i be final formulae
                  //     N_i be non final formula
                  // (F_1 & ... & F_n & N_1 & ... & N_m)
                  // =   (F_1 | ... | F_n);[*] && (N_1 & ... & N_m)
                  //   | (F_1 | ... | F_n) && (N_1 & ... & N_m);[*]
                  formula f = formula::OrRat(std::move(final));
                  formula n = formula::AndNLM(std::move(non_final));
                  formula t = formula::one_star();
                  formula ft = formula::Concat({f, t});
                  formula nt = formula::Concat({n, t});
                  formula ftn = formula::AndRat({ft, n});
                  formula fnt = formula::AndRat({f, nt});
                  return recurse_and_concat(formula::OrRat({ftn, fnt}));
                }
              // No final formula.
              // Translate N_1 & N_2 & ... & N_n into
              //   N_1 && (N_2;[*]) && ... && (N_n;[*])
              // | (N_1;[*]) && N_2 && ... && (N_n;[*])
              // | (N_1;[*]) && (N_2;[*]) && ... && N_n
              formula star = formula::one_star();
              vec disj;
              for (unsigned n = 0; n < s; ++n)
                {
                  vec conj;
                  for (unsigned m = 0; m < s; ++m)
                    {
                      formula g = f[m];
                      if (n != m)
                        g = formula::Concat({g, star});
                      conj.emplace_back(g);
                    }
                  disj.emplace_back(formula::AndRat(std::move(conj)));
                }
              return recurse_and_concat(formula::OrRat(std::move(disj)));
            }
          case op::AndRat:
            {
              bdd res = bddtrue;
              for (auto g: f)
                res &= recurse(g);

              // If we have translated (a* && b*) in (a* && b*);c, we
              // have to append ";c" to all destinations.
              res = concat_dests(res);

              if (f.accepts_eword())
                res |= now_to_concat();
              return res;
            }
          case op::OrRat:
            {
              bdd res = bddfalse;
              for (auto g: f)
                res |= recurse_and_concat(g);
              return res;
            }
          case op::Concat:
            {
              vec v;
              unsigned s = f.size();
              v.reserve(s);
              for (unsigned n = 1; n < s; ++n)
                v.emplace_back(f[n]);
              if (to_concat_)
                v.emplace_back(to_concat_);
              return recurse(f[0], formula::Concat(std::move(v)));
            }
          case op::Fusion:
            {
              assert(f.size() >= 2);

              // the head
              bdd res = recurse(f[0]);

              // the tail
              formula tail = f.all_but(0);
              bdd tail_bdd;
              bool tail_computed = false;

              //trace_ltl_bdd(dict_, res);

              minato_isop isop(res);
              bdd cube;
              res = bddfalse;
              while ((cube = isop.next()) != bddfalse)
                {
                  bdd label = bdd_exist(cube, dict_.next_set);
                  bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
                  formula dest = dict_.conj_bdd_to_sere(dest_bdd);

                  if (dest.accepts_eword())
                    {
                      // The destination is a final state.  Make sure we
                      // can also exit if tail is satisfied.
                      if (!tail_computed)
                        {
                          tail_bdd = recurse(tail);
                          tail_computed = true;
                        }
                      res |= concat_dests(label & tail_bdd);
                    }

                  // If the destination is not 0 or [*0], it means it
                  // can have successors.  Fusion the tail and append
                  // anything to concatenate.
                  if (!dest.is(op::ff, op::eword))
                    {
                      formula dest2 = formula::Fusion({dest, tail});
                      if (to_concat_)
                        dest2 = formula::Concat({dest2, to_concat_});
                      if (!dest2.is_ff())
                        res |= label
                          & bdd_ithvar(dict_.register_next_variable(dest2));
                    }
                }
              return res;
            }
          case op::first_match:
            {
              assert(f.size() == 1);
              assert(!f.accepts_eword());
              // Build deterministic successors, and
              // rewrite each destination D as first_match(D).
              // This will handle the semantic of the operator automatically,
              // since first_match(D) = [*0] if D accepts [*0].
              bdd res_ndet = recurse(f[0]);
              bdd res_det = bddfalse;
              bdd var_set = bdd_existcomp(bdd_support(res_ndet), dict_.var_set);
              bdd all_props = bdd_existcomp(res_ndet, dict_.var_set);
              while (all_props != bddfalse)
                {
                  bdd label = bdd_satoneset(all_props, var_set, bddtrue);
                  all_props -= label;

                  formula dest =
                    dict_.bdd_to_sere(bdd_appex(res_ndet, label, bddop_and,
                                                dict_.var_set));
                  dest = formula::first_match(dest);
                  if (to_concat_)
                    dest = formula::Concat({dest, to_concat_});
                  if (!dest.is_ff())
                    res_det |= label
                      & bdd_ithvar(dict_.register_next_variable(dest));
                }
              return res_det;
            }
          }
        SPOT_UNREACHABLE();
      }

      bdd
      recurse(formula f, formula to_concat = nullptr)
      {
        return translate_ratexp(f, dict_, to_concat);
      }

      bdd
      recurse_and_concat(formula f)
      {
        return translate_ratexp(f, dict_, to_concat_);
      }

    private:
      translate_dict& dict_;
      formula to_concat_;
    };

    bdd
    translate_ratexp(formula f, translate_dict& dict,
                     formula to_concat)
    {
      bdd res;
      if (!f.is_boolean())
        {
          ratexp_trad_visitor v(dict, to_concat);
          res = v.visit(f);
        }
      else
        {
          res = dict.boolean_to_bdd(f);
          // See comment for similar code in next_to_concat.
          if (!to_concat)
            to_concat = formula::eword();
          int x = dict.register_next_variable(to_concat);
          res &= bdd_ithvar(x);
        }
      return res;
    }


    ratexp_to_dfa::ratexp_to_dfa(translate_dict& dict)
      : dict_(dict)
    {
    }

    ratexp_to_dfa::~ratexp_to_dfa()
    {
      for (auto i: automata_)
        delete i.second;
    }

    ratexp_to_dfa::labelled_aut
    ratexp_to_dfa::translate(formula f)
    {
      assert(f.is_in_nenoform());

      auto a = make_twa_graph(dict_.dict);
      auto namer = a->create_namer<formula>();

      typedef std::set<formula> set_type;
      set_type formulae_to_translate;

      formulae_to_translate.insert(f);
      namer->new_state(f);
      //a->set_init_state(f);

      while (!formulae_to_translate.empty())
        {
          // Pick one formula.
          formula now = *formulae_to_translate.begin();
          formulae_to_translate.erase(formulae_to_translate.begin());

          // Translate it
          bdd res = translate_ratexp(now, dict_);

          // Generate (deterministic) successors
          bdd var_set = bdd_existcomp(bdd_support(res), dict_.var_set);
          bdd all_props = bdd_existcomp(res, dict_.var_set);
          while (all_props != bddfalse)
            {
              bdd label = bdd_satoneset(all_props, var_set, bddtrue);
              all_props -= label;

              formula dest =
                dict_.bdd_to_sere(bdd_appex(res, label, bddop_and,
                                            dict_.var_set));
              f2a_t::const_iterator i = f2a_.find(dest);
              if (i != f2a_.end() && i->second.first == nullptr)
                continue;

              if (!namer->has_state(dest))
                {
                  formulae_to_translate.insert(dest);
                  namer->new_state(dest);
                }
              namer->new_edge(now, dest, label);
            }
        }

      // The following code trims the automaton in a crude way by
      // eliminating SCCs that are not coaccessible.  It does not
      // actually remove the states, it simply marks the corresponding
      // formulae as associated to the null pointer in the f2a_ map.
      // The method succ() interprets this as False.

      scc_info* sm = new scc_info(a);
      unsigned scc_count = sm->scc_count();
      // Remember whether each SCC is coaccessible.
      std::vector<bool> coaccessible(scc_count);
      // SCC are numbered in topological order
      for (unsigned n = 0; n < scc_count; ++n)
        {
          // The SCC is coaccessible if any of its states
          // is final (i.e., it accepts [*0])...
          bool coacc = false;
          auto& st = sm->states_of(n);
          for (auto l: st)
            if (namer->get_name(l).accepts_eword())
              {
                coacc = true;
                break;
              }
          if (!coacc)
            {
              // ... or if any of its successors is coaccessible.
              for (unsigned i: sm->succ(n))
                if (coaccessible[i])
                  {
                    coacc = true;
                    break;
                  }
            }
          if (!coacc)
            {
              // Mark all formulas of this SCC as useless.
              for (auto f: st)
                f2a_.emplace(std::piecewise_construct,
                             std::forward_as_tuple(namer->get_name(f)),
                             std::forward_as_tuple(nullptr, nullptr));
            }
          else
            {
              for (auto f: st)
                f2a_.emplace(std::piecewise_construct,
                             std::forward_as_tuple(namer->get_name(f)),
                             std::forward_as_tuple(a, namer));
            }
          coaccessible[n] = coacc;
        }
      delete sm;
      if (coaccessible[scc_count - 1])
        {
          automata_.emplace_back(a, namer);
          return labelled_aut(a, namer);
        }
      else
        {
          delete namer;
          return labelled_aut(nullptr, nullptr);
        }
    }

    // FIXME: use the new tgba::succ() interface
    std::tuple<const_twa_graph_ptr,
               const ratexp_to_dfa::namer*,
               const state*>
    ratexp_to_dfa::succ(formula f)
    {
      f2a_t::const_iterator it = f2a_.find(f);
      labelled_aut a;
      if (it != f2a_.end())
        a = it->second;
      else
        a = translate(f);

      // Using return std::make_tuple(nullptr, nullptr, nullptr) works
      // with GCC 6.1.1, but breaks with clang++ 3.7.1 when using the
      // same header file for <tuple>.  So let's use the output type
      // explicitly.
      typedef std::tuple<const_twa_graph_ptr,
                         const ratexp_to_dfa::namer*,
                         const state*> res_t;

      // If a is null, f has an empty language.
      if (!a.first)
        return res_t{nullptr, nullptr, nullptr};

      auto namer = a.second;
      assert(namer->has_state(f));
      auto st = a.first->state_from_number(namer->get_state(f));
      return res_t{a.first, namer, st};
    }

    // The rewrite rules used here are adapted from Jean-Michel
    // Couvreur's FM'99 paper, augmented to support rational operators
    // (from PSL), and a view other optimization.  See the
    // Duret-Lutz's paper "LTL Translation Improvements in Spot 1.0"
    // (IJCCBS 2014), for the optimization.  The PSL stuff is
    // unpublished yet.
    class ltl_trad_visitor final
    {
    public:
      ltl_trad_visitor(translate_dict& dict, bool mark_all = false,
                       bool exprop = false, bool recurring = false)
        : dict_(dict), rat_seen_(false), has_marked_(false),
          mark_all_(mark_all), exprop_(exprop), recurring_(recurring)
      {
      }

      virtual
      ~ltl_trad_visitor()
      {
      }

      bdd
      neg_of(formula node)
      {
        return recurse(dict_.ls->negative_normal_form(node, true));
      }

      void
      reset(bool mark_all)
      {
        rat_seen_ = false;
        has_marked_ = false;
        mark_all_ = mark_all;
      }

      const translate_dict&
      get_dict() const
      {
        return dict_;
      }

      bool
      has_rational() const
      {
        return rat_seen_;
      }

      bool
      has_marked() const
      {
        return has_marked_;
      }

      bdd
      visit(formula node)
      {
        switch (op o = node.kind())
          {
          case op::ff:
            return bddfalse;
          case op::tt:
            return bddtrue;
          case op::eword:
            SPOT_UNIMPLEMENTED();
          case op::ap:
            return bdd_ithvar(dict_.register_proposition(node));
          case op::F:
            {
              // r(Fy) = r(y) + a(y)X(Fy)   if not recurring
              // r(Fy) = r(y) + a(y)        if recurring (see comment in G)
              formula child = node[0];
              bdd y = recurse(child);
              bdd a = bdd_ithvar(dict_.register_a_variable(child));
              if (!recurring_)
                a &= bdd_ithvar(dict_.register_next_variable(node));
              if (dict_.unambiguous)
                a &= neg_of(child);
              return y | a;
            }
          case op::G:
            {
              // Couvreur's paper suggests that we optimize GFy
              // as
              //   r(GFy) = (r(y) + a(y))X(GFy)
              // instead of
              //   r(GFy) = (r(y) + a(y)X(Fy)).X(GFy)
              // but this is just a particular case
              // of the "merge all states with the same
              // symbolic rewriting" optimization we do later.
              // (r(Fy).r(GFy) and r(GFy) have the same symbolic
              // rewriting, see Fig.6 in Duret-Lutz's VECOS'11
              // paper for an illustration.)
              //
              // We used to keep things simple and not implement this
              // step, that does not change the result.  However it
              // turns out that this extra optimization significantly
              // speeds up (≈×2) the translation of formulas of the
              // form GFa & GFb & ... GFz
              //
              // Unfortunately, our rewrite rules will put such a
              // formula as G(Fa & Fb & ... Fz) which has a different
              // form.  We could encode specifically
              // r(G(Fa & Fb & c)) =
              //   (r(a)+a(a))(r(b)+a(b))r(c)X(G(Fa & Fb & c))
              // but that would be lots of special cases for G.
              // And if we do it for G, why not for R?
              //
              // Here we generalize this trick by propagating
              // to "recurring" information to subformulas
              // and letting them decide.

              // r(Gy) = r(y)X(Gy)
              int x = dict_.register_next_variable(node);
              bdd y = recurse(node[0], /* recurring = */ true);
              return y & bdd_ithvar(x);
            }
          case op::Not:
            {
              // r(!y) = !r(y)
              return bdd_not(recurse(node[0]));
            }
          case op::X:
          case op::strong_X:
            {
              // r(Xy) = Next[y]
              // r(X(a&b&c)) = Next[a]&Next[b]&Next[c]
              // r(X(a|b|c)) = Next[a]|Next[b]|Next[c]
              //
              // The special case for And is to that
              // (p&XF!p)|(!p&XFp)|X(Fp&F!p)      (1)
              // get translated as
              // (p&XF!p)|(!p&XFp)|XFp&XF!p       (2)
              // and then automatically reduced to
              // (p&XF!p)|(!p&XFp)
              //
              // Formula (2) appears as an example of Boolean
              // simplification in Wring, but our LTL rewriting
              // rules tend to rewrite it as (1).
              //
              // The special case for Or follows naturally, but it's
              // effect is less clear.  Benchmarks show that it
              // reduces the number of states and transitions, but it
              // increases the number of non-deterministic states...
              formula y = node[0];
              bdd res;
              if (y.is(op::And))
                {
                  res = bddtrue;
                  for (auto f: y)
                    res &= bdd_ithvar(dict_.register_next_variable(f));
                }
#if 0
              else if (y.is(op::Or))
                {
                  res = bddfalse;
                  for (auto f: y)
                    res |= bdd_ithvar(dict_.register_next_variable(f));
                }
#endif
              else
                {
                  res = bdd_ithvar(dict_.register_next_variable(y));
                }
              return res;
            }
          case op::Closure:
            {
              // rat_seen_ = true;
              formula f = node[0];
              auto p = dict_.transdfa.succ(f);
              bdd res = bddfalse;
              auto aut = std::get<0>(p);
              auto namer = std::get<1>(p);
              auto st = std::get<2>(p);
              if (!aut)
                return res;
              for (auto i: aut->succ(st))
                {
                  bdd label = i->cond();
                  const state* s = i->dst();
                  formula dest =
                    namer->get_name(aut->state_number(s));

                  if (dest.accepts_eword())
                    {
                      res |= label;
                    }
                  else
                    {
                      formula dest2 = formula::unop(o, dest);
                      if (dest2.is_ff())
                        continue;
                      res |=
                        label & bdd_ithvar(dict_.register_next_variable(dest2));
                    }
                }
              return res;
            }
          case op::NegClosureMarked:
            has_marked_ = true;
            SPOT_FALLTHROUGH;
          case op::NegClosure:
            rat_seen_ = true;
            {
              if (mark_all_)
                {
                  o = op::NegClosureMarked;
                  has_marked_ = true;
                }

              formula f = node[0];
              auto p = dict_.transdfa.succ(f);
              auto aut = std::get<0>(p);

              if (!aut)
                return bddtrue;

              auto namer = std::get<1>(p);
              auto st = std::get<2>(p);
              bdd res = bddfalse;
              bdd missing = bddtrue;

              for (auto i: aut->succ(st))
                {
                  bdd label = i->cond();
                  const state* s = i->dst();
                  formula dest = namer->get_name(aut->state_number(s));

                  missing -= label;

                  if (!dest.accepts_eword())
                    {
                      formula dest2 = formula::unop(o, dest);
                      if (dest2.is_ff())
                        continue;
                      res |= label
                        & bdd_ithvar(dict_.register_next_variable(dest2));
                    }
                }

              res |= missing &
                // stick X(1) to preserve determinism.
                bdd_ithvar(dict_.register_next_variable(formula::tt()));
              //trace_ltl_bdd(dict_, res_);
              return res;
            }
          case op::Star:
          case op::FStar:
          case op::first_match:
            SPOT_UNREACHABLE();         // Not an LTL operator
            // r(f1 logical-op f2) = r(f1) logical-op r(f2)
          case op::Xor:
          case op::Implies:
          case op::Equiv:
            // These operators should only appear in Boolean formulas,
            // which must have been dealt with earlier (in
            // translate_dict::ltl_to_bdd()).
            SPOT_UNREACHABLE();
          case op::U:
            {
              bdd f1 = recurse(node[0]);
              bdd f2 = recurse(node[1]);
              // r(f1 U f2) = r(f2) + a(f2)r(f1)X(f1 U f2) if not recurring
              //                                           and f1 not universal
              // r(f1 U f2) = r(f2) + a(f2)r(f1)X(Ff2)     if not recurring
              //                                           and f1 universal
              // r(f1 U f2) = r(f2) + a(f2)r(f1)           if recurring
              f1 &= bdd_ithvar(dict_.register_a_variable(node[1]));
              if (!recurring_)
                {
                  formula nxt =
                    node[0].is_universal() ? formula::F(node[1]) : node;
                  // rewriting r(f1)X(f1 U f2) as r(f1)X(Ff2) when f1 is
                  // universal is an optimization that helps with formulas
                  // such as (G(Fa & Fb)) U a.
                  f1 &= bdd_ithvar(dict_.register_next_variable(nxt));
                }
              if (dict_.unambiguous)
                f1 &= neg_of(node[1]);
              return f2 | f1;
            }
          case op::W:
            {
              // r(f1 W f2) = r(f2) + r(f1)X(f1 W f2) if not recurring
              // r(f1 W f2) = r(f2) + r(f1)           if recurring
              //
              // also f1 W 0 = G(f1), so we can enable recurring on f1
              bdd f1 = recurse(node[0], node[1].is_ff());
              bdd f2 = recurse(node[1]);
              if (!recurring_)
                f1 &= bdd_ithvar(dict_.register_next_variable(node));
              if (dict_.unambiguous)
                f1 &= neg_of(node[1]);
              return f2 | f1;
            }
          case op::R:
            {
              // r(f2) is in factor, so we can propagate the recurring_ flag.
              // if f1=false, we can also turn it on (0 R f = Gf).
              bdd res = recurse(node[1],
                                recurring_ || node[0].is_ff());
              // r(f1 R f2) = r(f2)(r(f1) + X(f1 R f2))  if not recurring
              // r(f1 R f2) = r(f2)                      if recurring
              if (recurring_ && !dict_.unambiguous)
                return res;
              bdd f1 = recurse(node[0]);
              bdd f2 = bddtrue;
              if (!recurring_)
                f2 = bdd_ithvar(dict_.register_next_variable(node));
              if (dict_.unambiguous)
                f2 &= neg_of(node[0]);
              return res & (f1 | f2);
            }
          case op::M:
            {
              bdd res = recurse(node[1], recurring_);
              bdd f1 = recurse(node[0]);
              // r(f1 M f2) = r(f2)(r(f1) + a(f1&f2)X(f1 M f2)) if not recurring
              // r(f1 M f2) = r(f2)(r(f1) + a(f1&f2))           if recurring
              //
              // Note that the rule above differs from the one given
              // in Figure 2 of
              //    "LTL translation improvements in Spot 1.0",
              //     A. Duret-Lutz. IJCCBS 5(1/2):31-54, March 2014.
              // Both rules should be OK, but this one is a better fit
              // to the promises simplifications performed in
              // register_a_variable() (see comments in this function).
              // We do not want a U (c M d) to generate two different
              // promises.  Generating c&d also makes the output similar
              // to what we would get with the equivalent a U (d U (c & d)).
              //
              // Here we just appear to emit a(f1 M f2) and the conversion
              // to a(f1&f2) is done  by register_a_variable().
              bdd a = bdd_ithvar(dict_.register_a_variable(node));
              if (!recurring_)
                a &= bdd_ithvar(dict_.register_next_variable(node));
              if (dict_.unambiguous)
                a &= neg_of(node[0]);
              return res & (f1 | a);
            }
          case op::EConcatMarked:
            has_marked_ = true;
            SPOT_FALLTHROUGH;
          case op::EConcat:
            rat_seen_ = true;
            {
              // Recognize f2 on transitions going to destinations
              // that accept the empty word.
              bdd f2 = recurse(node[1]);
              bdd f1 = translate_ratexp(node[0], dict_);
              bdd res = bddfalse;

              if (mark_all_)
                {
                  o = op::EConcatMarked;
                  has_marked_ = true;
                }

              if (exprop_)
                {
                  bdd var_set = bdd_existcomp(bdd_support(f1), dict_.var_set);
                  bdd all_props = bdd_existcomp(f1, dict_.var_set);
                  while (all_props != bddfalse)
                    {
                      bdd label = bdd_satoneset(all_props, var_set, bddtrue);
                      all_props -= label;

                      formula dest =
                        dict_.bdd_to_sere(bdd_appex(f1, label, bddop_and,
                                                    dict_.var_set));

                      formula dest2 = formula::binop(o, dest, node[1]);
                      bool unamb = dict_.unambiguous;
                      if (!dest2.is_ff())
                        {
                          // If the rhs is Boolean, the
                          // unambiguous code will produce a more
                          // deterministic automaton at no additional
                          // cost.  You can test this on
                          //   G({{1;1}*}<>->a)
                          if (node[1].is_boolean())
                            unamb = true;

                          bdd toadd = label &
                            bdd_ithvar(dict_.register_next_variable(dest2));
                          if (dest.accepts_eword() && unamb)
                            toadd &= neg_of(node[1]);
                          res |= toadd;
                        }
                      if (dest.accepts_eword())
                        {
                          bdd toadd = label & f2;
                          if (unamb)
                            // Preserve determinism
                            toadd &= bdd_ithvar(dict_.register_next_variable
                                                (formula::tt()));
                          res |= toadd;
                        }
                    }
                }
              else
                {
                  minato_isop isop(f1);
                  bdd cube;
                  while ((cube = isop.next()) != bddfalse)
                    {
                      bdd label = bdd_exist(cube, dict_.next_set);
                      bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
                      formula dest = dict_.conj_bdd_to_sere(dest_bdd);

                      if (dest.is(op::eword))
                        {
                          res |= label & f2;
                        }
                      else
                        {
                          formula dest2 = formula::binop(o, dest, node[1]);
                          if (!dest2.is_ff())
                            res |= label &
                              bdd_ithvar(dict_.register_next_variable(dest2));
                          if (dest.accepts_eword())
                            res |= label & f2;
                        }
                    }
                }
              return res;
            }
          case op::UConcat:
            {
              // Transitions going to destinations accepting the empty
              // word should recognize f2, and the automaton for f1
              // should be understood as universal.
              //
              // The crux of this translation (i.e., the
              // interpretation of first() as a universal automaton,
              // and using implication to encode it)  was explained
              // to me (adl) by Felix Klaedtke.
              bdd f2 = recurse(node[1]);
              bdd f1 = translate_ratexp(node[0], dict_);

              if (exprop_)
                {
                  bdd res = bddfalse;
                  bdd var_set = bdd_existcomp(bdd_support(f1), dict_.var_set);
                  bdd all_props = bdd_existcomp(f1, dict_.var_set);
                  bdd missing = !all_props;
                  while (all_props != bddfalse)
                    {
                      bdd label = bdd_satoneset(all_props, var_set, bddtrue);
                      all_props -= label;

                      formula dest =
                        dict_.bdd_to_sere(bdd_appex(f1, label, bddop_and,
                                                    dict_.var_set));

                      formula dest2 = formula::binop(o, dest, node[1]);

                      bdd udest =
                        bdd_ithvar(dict_.register_next_variable(dest2));

                      if (dest.accepts_eword())
                        udest &= f2;

                      res |= label & udest;
                    }
                  // Make the automaton complete.
                  res |= missing &
                    // stick X(1) to preserve determinism.
                    bdd_ithvar(dict_.register_next_variable(formula::tt()));
                  return res;
                }
              else
                {
                  bdd res = bddtrue;
                  minato_isop isop(f1);
                  bdd cube;
                  while ((cube = isop.next()) != bddfalse)
                    {
                      bdd label = bdd_exist(cube, dict_.next_set);
                      bdd dest_bdd = bdd_existcomp(cube, dict_.next_set);
                      formula dest = dict_.conj_bdd_to_sere(dest_bdd);
                      formula dest2 = formula::binop(o, dest, node[1]);

                      bdd udest =
                        bdd_ithvar(dict_.register_next_variable(dest2));

                      if (dest.accepts_eword())
                        udest &= f2;

                      res &= bdd_apply(label, udest, bddop_imp);
                    }
                  return res;
                }
            }
          case op::And:
            {
              formula_set implied;
              implied_subformulae(node, implied);

              bdd res = bddtrue;
              for (auto sub: node)
                {
                  // Skip implied subformula.  For instance
                  // when translating Fa & GFa, we should not
                  // attempt to translate Fa.
                  //
                  // This optimization combines nicely with the
                  // "recurring" optimization whereby GFp will be
                  // translated as r(GFp) = (r(p) | a(p))X(GFp)
                  // without showing Fp instead of r(GFp) =
                  // r(Fp)X(GFp).  See the comment for the translation
                  // of G.
                  if (implied.find(sub) != implied.end())
                    continue;
                  // Propagate the recurring_ flag so that
                  // G(Fa & Fb) get optimized.  See the comment in
                  // the case handling G.
                  res &= recurse(sub, recurring_);
                }
              return res;
            }
          case op::Or:
            {
              bdd res = bddfalse;
              if (!dict_.unambiguous)
                {
                  for (auto sub: node)
                    res |= recurse(sub);
                }
              else
                {
                  bdd prev = bddtrue;
                  for (auto sub: node)
                    {
                      res |= prev & recurse(sub);
                      prev &= neg_of(sub);
                    }
                }
              return res;
            }
          case op::Concat:
          case op::Fusion:
          case op::AndNLM:
          case op::AndRat:
          case op::OrRat:
            SPOT_UNREACHABLE(); // Not an LTL operator
          }
        SPOT_UNREACHABLE();
        return bddfalse;
      }

      bdd
      recurse(formula f, bool recurring = false)
      {
        const translate_dict::translated& t =
          dict_.ltl_to_bdd(f, mark_all_, recurring);
        rat_seen_ |= t.has_rational;
        has_marked_ |= t.has_marked;
        return t.symbolic;
      }


    private:
      translate_dict& dict_;
      bool rat_seen_;
      bool has_marked_;
      bool mark_all_;
      bool exprop_;
      bool recurring_;
    };

    const translate_dict::translated&
    translate_dict::ltl_to_bdd(formula f, bool mark_all, bool recurring)
    {
      flagged_formula ff;
      ff.f = f;
      ff.flags =
        ((mark_all || f.is_ltl_formula()) ? flags_mark_all : flags_none)
        | (recurring ? flags_recurring : flags_none);

      flagged_formula_to_bdd_map::const_iterator i = ltl_bdd_.find(ff);

      if (i != ltl_bdd_.end())
        return i->second;

      translated t;
      if (f.is_boolean())
        {
          t.symbolic = boolean_to_bdd(f);
          t.has_rational = false;
          t.has_marked = false;
        }
      else
        {
          ltl_trad_visitor v(*this, mark_all, exprop, recurring);
          t.symbolic = v.visit(f);
          t.has_rational = v.has_rational();
          t.has_marked = v.has_marked();
        }

      return ltl_bdd_.emplace(ff, t).first->second;
    }


    // Check whether a formula has a R, W, or G operator at its
    // top-level (preceding logical operators do not count).
    bool ltl_possible_fair_loop_check(formula f)
    {
      if (f.is(op::G) || f.is(op::R, op::W))
        return true;
      if (f.is(op::Xor, op::Equiv) || f.is(op::Implies)
          || f.is(op::And, op::Or))
        for (auto g: f)
          if (ltl_possible_fair_loop_check(g))
            return true;
      return false;
    }

    // Check whether a formula can be part of a fair loop.
    // Cache the result for efficiency.
    class possible_fair_loop_checker final
    {
    public:
      bool
      check(formula f)
      {
        pfl_map::const_iterator i = pfl_.find(f);
        if (i != pfl_.end())
          return i->second;
        return pfl_[f] = ltl_possible_fair_loop_check(f);
      }

    private:
      typedef robin_hood::unordered_flat_map<formula, bool> pfl_map;
      pfl_map pfl_;
    };

    class formula_canonicalizer final
    {
    public:
      formula_canonicalizer(translate_dict& d,
                            bool fair_loop_approx, bdd all_promises)
        : fair_loop_approx_(fair_loop_approx),
          all_promises_(all_promises),
          d_(d)
      {
        // For cosmetics, register 1 initially, so the algorithm will
        // not register an equivalent formula first.
        b2f_[bddtrue] = formula::tt();
      }

      // This wrap translate_dict::ltl_to_bdd() for top-level formulas.
      // In case the formula contains SERE operators, we need to decide
      // if we have to mark unmarked operators, and more
      const translate_dict::translated&
      translate(formula f, bool* new_flag = nullptr)
      {
        // Use the cached result if available.
        formula_to_bdd_map::const_iterator i = f2b_.find(f);
        if (i != f2b_.end())
          return i->second;

        if (new_flag)
          *new_flag = true;

        // Perform the actual translation.
        translate_dict::translated t = d_.ltl_to_bdd(f, !f.is_marked());

        // std::cerr << "-----" << std::endl;
        // std::cerr << "Formula: " << str_psl(f) << std::endl;
        // std::cerr << "Rational: " << t.has_rational << std::endl;
        // std::cerr << "Marked: " << t.has_marked << std::endl;
        // std::cerr << "Mark all: " << !f.is_marked() << std::endl;
        // std::cerr << "Transitions:" << std::endl;
        // trace_ltl_bdd(d_, t.symbolic);
        // std::cerr << "-----" << std::endl;

        if (t.has_rational)
          {
            bdd res = bddfalse;

            bdd var_set = bdd_existcomp(bdd_support(t.symbolic), d_.var_set);
            bdd all_props = bdd_existcomp(t.symbolic, d_.var_set);
            while (all_props != bddfalse)
              {
                bdd one_prop_set = bddtrue;
                if (d_.exprop)
                  one_prop_set = bdd_satoneset(all_props, var_set, bddtrue);
                all_props -= one_prop_set;

                minato_isop isop(t.symbolic & one_prop_set);
                bdd cube;
                while ((cube = isop.next()) != bddfalse)
                  {
                    bdd label = bdd_exist(cube, d_.next_set);
                    bdd dest_bdd = bdd_existcomp(cube, d_.next_set);
                    formula dest =
                      d_.conj_bdd_to_formula(dest_bdd);

                    // Handle a Miyano-Hayashi style unrolling for
                    // rational operators.  Marked nodes correspond to
                    // subformulae in the Miyano-Hayashi set.
                    dest =  d_.mt.simplify_mark(dest);

                    if (dest.is_marked())
                      {
                        // Make the promise that we will exit marked sets.
                        int a =
                          d_.register_a_variable(formula::tt());
                        label &= bdd_ithvar(a);
                      }
                    else
                      {
                        // We have no marked operators, but still
                        // have other rational operator to check.
                        // Start a new marked cycle.
                        dest = d_.mt.mark_concat_ops(dest);
                      }
                    // Note that simplify_mark may have changed dest.
                    dest_bdd = bdd_ithvar(d_.register_next_variable(dest));
                    res |= label & dest_bdd;
                  }
              }
            t.symbolic = res;
//            std::cerr << "Marking rewriting:" << std::endl;
//            trace_ltl_bdd(v_.get_dict(), t.symbolic);
          }

        // Apply the fair-loop approximation if requested.
        if (fair_loop_approx_
            // If the source cannot possibly be part of a fair
            // loop, make all possible promises.
            && !f.is_tt() && !pflc_.check(f))
          t.symbolic &= all_promises_;

        // Register the reverse mapping if it is not already done.
        if (b2f_.find(t.symbolic) == b2f_.end())
          b2f_[t.symbolic] = f;

        return f2b_.emplace(f, t).first->second;
      }

      formula
      canonicalize(formula f)
      {
        bool new_variable = false;
        bdd b = translate(f, &new_variable).symbolic;

        bdd_to_formula_map::iterator i = b2f_.find(b);
        // Since we have just translated the formula, it is
        // necessarily in b2f_.
        assert(i != b2f_.end());

        if (i->second != f)
          // The translated bdd maps to an already seen formula.
          f = i->second;
        return f;
      }

      bdd used_vars()
      {
        return d_.var_set;
      }

    private:
      // Map a representation of successors to a canonical formula.
      // We do this because many formulae (such as `aR(bRc)' and
      // `aR(bRc).(bRc)') are equivalent, and are trivially identified
      // by looking at the set of successors.
      typedef robin_hood::unordered_node_map<bdd, formula,
                                             bdd_hash> bdd_to_formula_map;
      bdd_to_formula_map b2f_;
      // Map each formula to its associated bdd.  This speed things up when
      // the same formula is translated several times, which especially
      // occurs when canonicalize() is called repeatedly inside exprop.
      typedef robin_hood::unordered_node_map
        <formula, translate_dict::translated> formula_to_bdd_map;
      formula_to_bdd_map f2b_;

      possible_fair_loop_checker pflc_;
      bool fair_loop_approx_;
      bdd all_promises_;
      translate_dict& d_;
    };

  }

  namespace
  {
    struct transition
    {
      formula dest;
      bdd prom;
      bdd cond;

      transition(formula dest, bdd cond, bdd prom)
        : dest(dest), prom(prom), cond(cond)
      {
      }

      bool operator<(const transition& other) const
      {
        if (dest < other.dest)
          return true;
        if (other.dest < dest)
          return false;
        if (prom.id() < other.prom.id())
          return true;
        if (prom.id() > other.prom.id())
          return false;
        return cond.id() < other.cond.id();
      }
    };

    bool postponement_cmp(const transition& lhs, const transition& rhs)
    {
      if (lhs.prom.id() < rhs.prom.id())
        return true;
      if (lhs.prom.id() > rhs.prom.id())
        return false;
      if (lhs.cond.id() < rhs.cond.id())
        return true;
      if (lhs.cond.id() > rhs.cond.id())
        return false;
      return lhs.dest < rhs.dest;
    }

    typedef std::vector<transition> dest_map;
  }

  twa_graph_ptr
  ltl_to_tgba_fm(formula f2, const bdd_dict_ptr& dict,
                 bool exprop, bool symb_merge, bool branching_postponement,
                 bool fair_loop_approx, const atomic_prop_set* unobs,
                 tl_simplifier* simplifier, bool unambiguous)
  {
    tl_simplifier* s = simplifier;

    // Simplify the formula, if requested.
    if (s)
      {
        // This will normalize the formula regardless of the
        // configuration of the simplifier.
        f2 = s->simplify(f2);
      }
    else
      {
        // Otherwise, at least normalize the formula.  We want all the
        // negations on the atomic propositions.  We also suppress
        // logic abbreviations such as <=>, =>, or XOR, since they
        // would involve negations at the BDD level.
        s = new tl_simplifier(dict);
        f2 = s->negative_normal_form(f2, false);
      }
    assert(f2.is_in_nenoform());

    typedef std::set<formula> set_type;
    set_type formulae_to_translate;

    assert(dict == s->get_dict());

    twa_graph_ptr a = make_twa_graph(dict);
    auto namer = a->create_namer<formula>();

    // Even if the input is a persistence formula, the unambiguous option might
    // cause the resulting automaton not to be weak.  For instance formulas
    // such as "FGa | FGb" (note: not "F(Ga | Gb)") will introduce terms like
    // "FGb&GF!a" that are not syntactic persistence.
    bool one_set_enough = (unambiguous
                            ? f2.is_syntactic_obligation()
                            : f2.is_syntactic_persistence());
    translate_dict d(a, s, exprop, one_set_enough, unambiguous);

    // Compute the set of all promises that can possibly occur inside
    // the formula.  These are the right-hand sides of U or F
    // operators.
    bdd all_promises = bddtrue;
    if (fair_loop_approx || unobs)
      f2.traverse([&all_promises, &d](formula f)
                  {
                    if (f.is(op::F))
                      all_promises &=
                        bdd_ithvar(d.register_a_variable(f[0]));
                    else if (f.is(op::U))
                      all_promises &=
                        bdd_ithvar(d.register_a_variable(f[1]));
                    else if (f.is(op::M))
                      all_promises &=
                        bdd_ithvar(d.register_a_variable(f));
                    return f.is_boolean();
                  });

    formula_canonicalizer fc(d, fair_loop_approx, all_promises);

    // These are used when atomic propositions are interpreted as
    // events.  There are two kinds of events: observable events are
    // those used in the formula, and unobservable events or other
    // events that can occur at anytime.  All events exclude each
    // other.
    bdd observable_events = bddfalse;
    bdd unobservable_events = bddfalse;
    if (unobs)
      {
        bdd neg_events = bddtrue;
        auto aps = std::unique_ptr<atomic_prop_set>(atomic_prop_collect(f2));
        for (auto pi: *aps)
          {
            int p = d.register_proposition(pi);
            bdd pos = bdd_ithvar(p);
            bdd neg = bdd_nithvar(p);
            observable_events = (observable_events & neg) | (neg_events & pos);
            neg_events &= neg;
          }
        for (auto pi: *unobs)
          {
            int p = d.register_proposition(pi);
            bdd pos = bdd_ithvar(p);
            bdd neg = bdd_nithvar(p);
            unobservable_events = ((unobservable_events & neg)
                                   | (neg_events & pos));
            observable_events &= neg;
            neg_events &= neg;
          }
      }
    bdd all_events = observable_events | unobservable_events;

    auto orig_f = f2;

    // This is in case the initial state is equivalent to true...
    if (symb_merge)
      f2 = fc.canonicalize(f2);

    formulae_to_translate.insert(f2);
    a->set_init_state(namer->new_state(f2));

    dest_map dests;
    while (!formulae_to_translate.empty())
      {
        // Pick one formula.
        formula now = *formulae_to_translate.begin();
        formulae_to_translate.erase(formulae_to_translate.begin());

        // Translate it into a BDD to simplify it.
        const translate_dict::translated& t = fc.translate(now);
        bdd res = t.symbolic;

        if (res == bddfalse)
          continue;

        // Handle exclusive events.
        if (unobs)
          {
            res &= observable_events;
            int n = d.register_next_variable(now);
            res |= unobservable_events & bdd_ithvar(n) & all_promises;
          }

        // We used to factor only Next and A variables while computing
        // prime implicants, with
        //    minato_isop isop(res, d.next_set & d.a_set);
        // in order to obtain transitions with formulae of atomic
        // proposition directly, but unfortunately this led to strange
        // factorizations.  For instance f U g was translated as
        //     r(f U g) = g + a(g).r(X(f U g)).(f + g)
        // instead of just
        //     r(f U g) = g + a(g).r(X(f U g)).f
        // Of course both formulae are logically equivalent, but the
        // latter is "more deterministic" than the former, so it should
        // be preferred.
        //
        // Therefore we now factor all variables.  This may lead to more
        // transitions than necessary (e.g.,  r(f + g) = f + g  will be
        // coded as two transitions), but we later merge all transitions
        // with same source/destination and acceptance conditions.  This
        // is the goal of the `dests' hash.
        //
        // Note that this is still not optimal.  For instance it is
        // better to encode `f U g' as
        //     r(f U g) = g + a(g).r(X(f U g)).f.!g
        // because that leads to a deterministic automaton.  In order
        // to handle this, we take the conditions of any transition
        // going to true (it's `g' here), and remove it from the other
        // transitions.
        //
        // In `exprop' mode, considering all possible combinations of
        // outgoing propositions generalizes the above trick.
        dests.clear();

        // Compute all outgoing arcs.

        // If EXPROP is set, we will refine the symbolic
        // representation of the successors for all combinations of
        // the atomic properties involved in the formula.
        // VAR_SET is the set of these properties.
        bdd var_set = bdd_existcomp(bdd_support(res), d.var_set);
        // ALL_PROPS is the combinations we have yet to consider.
        // We used to start with `all_props = bddtrue', but it is
        // more efficient to start with the set of all satisfiable
        // variables combinations.
        bdd all_props = bdd_existcomp(res, d.var_set);
        while (all_props != bddfalse)
          {
            bdd one_prop_set = bddtrue;
            if (exprop)
              one_prop_set = bdd_satoneset(all_props, var_set, bddtrue);
            all_props -= one_prop_set;

            // Compute prime implicants.
            // The reason we use prime implicants and not bdd_satone()
            // is that we do not want to get any negation in front of Next
            // or Acc variables.  We wouldn't know what to do with these.
            // We never added negations in front of these variables when
            // we built the BDD, so prime implicants will not "invent" them.
            //
            // FIXME: minato_isop is quite expensive, and I (=adl)
            // don't think we really care that much about getting the
            // smalled sum of products that minato_isop strives to
            // compute.  Given that Next and Acc variables should
            // always be positive, maybe there is a faster way to
            // compute the successors?  E.g. using bdd_satone() and
            // ignoring negated Next and Acc variables.
            minato_isop isop(res & one_prop_set);
            bdd cube;
            while ((cube = isop.next()) != bddfalse)
              {
                bdd label = bdd_exist(cube, d.next_set);
                bdd dest_bdd = bdd_existcomp(cube, d.next_set);
                formula dest = d.conj_bdd_to_formula(dest_bdd);

                // Simplify the formula, if requested.
                if (simplifier)
                  {
                    dest = simplifier->simplify(dest);
                    // Ignore the arc if the destination reduces to false.
                    if (dest.is_ff())
                      continue;
                  }

                // If we already know a state with the same
                // successors, use it in lieu of the current one.
                if (symb_merge)
                  dest = fc.canonicalize(dest);

                bdd conds = bdd_existcomp(label, d.var_set);
                bdd promises = bdd_existcomp(label, d.a_set);
                dests.emplace_back(transition(dest, conds, promises));
              }
          }

        assert(dests.size() > 0);
        if (branching_postponement && dests.size() > 1)
          {
            std::sort(dests.begin(), dests.end(), postponement_cmp);
            // Iterate over all dests, and merge the destination of
            // transitions with identical labels.
            dest_map::iterator out = dests.begin();
            dest_map::const_iterator in = out;
            do
              {
                transition t = *in;
                while (++in != dests.end()
                       && t.cond == in->cond && t.prom == in->prom)
                  t.dest = formula::Or({t.dest, in->dest});
                *out++ = t;
              }
            while (in != dests.end());
            dests.erase(out, dests.end());
          }
        std::sort(dests.begin(), dests.end());
        // If we have some transitions to true, they are the first
        // ones.  Remove the sum of their conditions from other
        // transitions.  It might sounds that this is not needed when
        // exprop is used, but in fact it is complementary.
        //
        // Consider
        //   f = r(X(1) R p) = p.(1 + r(X(1) R p))
        // with exprop the two outgoing arcs would be
        //         p               p
        //     f ----> 1       f ----> f
        //
        // where in fact we could output
        //         p
        //     f ----> 1
        //
        // because there is no point in looping on f if we can go to 1.
        if (dests.front().dest.is_tt())
          {
            dest_map::iterator i = dests.begin();
            bdd c = bddfalse;
            while (i != dests.end() && i->dest.is_tt())
              c |= i++->cond;
            if (c != bddfalse)
              for (; i != dests.end(); ++i)
                i->cond -= c;
          }

        // Create transitions in the automaton
        {
          dest_map::const_iterator in = dests.begin();
          do
            {
              // Merge transitions with same destination and
              // acceptance.
              transition t = *in;
              while (++in != dests.end()
                     && t.prom == in->prom && t.dest == in->dest)
                t.cond |= in->cond;
              // Actually create the transition
              if (t.cond != bddfalse)
                {
                  // When translating LTL for an event-based logic
                  // with unobservable events, the 1 state should
                  // accept all events, even unobservable events.
                  if (unobs && t.dest.is_tt() && now.is_tt())
                    t.cond = all_events;

                  // Will this be a new state?
                  if (!namer->has_state(t.dest))
                    {
                      formulae_to_translate.insert(t.dest);
                      namer->new_state(t.dest);
                    }
                  namer->new_edge(now, t.dest, t.cond, d.bdd_to_mark(t.prom));
                }
            }
          while (in != dests.end());
        }
      }

    auto& acc = a->acc();
    for (auto& e: a->edges())
      e.acc = acc.comp(e.acc);

    acc.set_generalized_buchi();

    if (orig_f.is_syntactic_stutter_invariant())
      a->prop_stutter_invariant(true);
    // We cannot assume weak automata if the unambibuous construction
    // is used.
    //
    // In an obligation formula such as (b W Ga)&F!a, initial state is
    // not accepting and goes to a state (b W Ga) that is accepting.
    // Adding the unambiguous option will add a back-edge between
    // these two states, creating an non-weak SCC.
    //
    // For guarantee formulas, X(G(b))) -> (!((a) W (G(b)))) has a
    // non-weak output.
    if (unambiguous)
      {
        a->prop_unambiguous(true);
      }
    else
      {
        if (orig_f.is_syntactic_persistence())
          a->prop_weak(true);
        if (orig_f.is_syntactic_guarantee())
          a->prop_terminal(true);
      }
    // Set the following to true to preserve state names.
    a->release_formula_namer(namer, false);

    if (!simplifier)
      // This should not be deleted before we have registered all propositions.
      delete s;
    return a;
  }

}
