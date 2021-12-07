// -*- coding: utf-8 -*-
// Copyright (C) 2008-2012, 2014-2016, 2018-2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2005 Laboratoire d'Informatique de Paris 6
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
#include <cassert>
#include <algorithm>
#include <spot/tl/randomltl.hh>
#include <spot/misc/random.hh>
#include <iostream>
#include <cstring>
#include <spot/misc/optionmap.hh>
#include <spot/tl/defaultenv.hh>
#include <sstream>

namespace spot
{
  namespace
  {
    static formula
    ap_builder(const random_formula* rl, int n)
    {
      assert(n == 1);
      (void) n;
      atomic_prop_set::const_iterator i = rl->ap()->begin();
      std::advance(i, mrand(rl->ap()->size()));
      return *i;
    }

    static formula
    true_builder(const random_formula*, int n)
    {
      assert(n == 1);
      (void) n;
      return formula::tt();
    }

    static formula
    false_builder(const random_formula*, int n)
    {
      assert(n == 1);
      (void) n;
      return formula::ff();
    }

    static formula
    eword_builder(const random_formula*, int n)
    {
      assert(n == 1);
      (void) n;
      return formula::eword();
    }

    static formula
    boolform_builder(const random_formula* rl, int n)
    {
      assert(n >= 1);
      const random_sere* rs = static_cast<const random_sere*>(rl);
      return rs->rb.generate(n);
    }

    template <op Op>
    static formula
    unop_builder(const random_formula* rl, int n)
    {
      assert(n >= 2);
      return formula::unop(Op, rl->generate(n - 1));
    }

    static formula
    closure_builder(const random_formula* rl, int n)
    {
      assert(n >= 2);
      const random_psl* rp = static_cast<const random_psl*>(rl);
      return formula::Closure(rp->rs.generate(n - 1));
    }

    template <op Op>
    static formula
    binop_builder(const random_formula* rl, int n)
    {
      assert(n >= 3);
      --n;
      int l = rrand(1, n - 1);
      // Force the order of generation of operands to be right, then
      // left.  This is historical, because gcc evaluates argument
      // from right to left and we used to make the two calls to
      // generate() inside of the call to instance() before
      // discovering that clang would perform the nested calls from
      // left to right.
      auto right = rl->generate(n - l);
      return formula::binop(Op, rl->generate(l), right);
    }

    template <op Op>
    static formula
    binop_SERELTL_builder(const random_formula* rl, int n)
    {
      assert(n >= 3);
      --n;
      const random_psl* rp = static_cast<const random_psl*>(rl);
      int l = rrand(1, n - 1);
      // See comment in binop_builder.
      auto right = rl->generate(n - l);
      return formula::binop(Op, rp->rs.generate(l), right);
    }

    template <op Op>
    static formula
    bunop_unbounded_builder(const random_formula* rl, int n)
    {
      assert(n >= 2);
      return formula::bunop(Op, rl->generate(n - 1));
    }

    template <op Op>
    static formula
    bunop_bounded_builder(const random_formula* rl, int n)
    {
      assert(n >= 2);
      int min = rrand(0, 2);
      int max = rrand(min, 3);
      return formula::bunop(Op, rl->generate(n - 1), min, max);
    }

    template <op Op>
    static formula
    bunop_bool_bounded_builder(const random_formula* rl, int n)
    {
      assert(n >= 2);
      int min = rrand(0, 2);
      int max = rrand(min, 3);
      const random_sere* rp = static_cast<const random_sere*>(rl);
      return formula::bunop(Op, rp->rb.generate(n - 1), min, max);
    }


    template <op Op>
    static formula
    multop_builder(const random_formula* rl, int n)
    {
      assert(n >= 3);
      --n;
      int l = rrand(1, n - 1);
      // See comment in binop_builder.
      auto right = rl->generate(n - l);
      return formula::multop(Op, {rl->generate(l), right});
    }

  } // anonymous

  void
  random_formula::op_proba::setup(const char* name, int min_n, builder build)
  {
    this->name = name;
    this->min_n = min_n;
    this->proba = 1.0;
    this->build = build;
  }

  void
  random_formula::update_sums()
  {
    total_1_ = 0.0;
    total_2_ = 0.0;
    total_2_and_more_ = 0.0;
    for (unsigned i = 0; i < proba_size_; ++i)
      {
        if (proba_[i].min_n == 1)
          {
            total_1_ += proba_[i].proba;
            if (proba_ + i >= proba_2_)
              total_2_ += proba_[i].proba;
            if (proba_ + i >= proba_2_or_more_)
              total_2_and_more_ += proba_[i].proba;
          }
        else if (proba_[i].min_n == 2)
          {
            total_2_ += proba_[i].proba;
            if (proba_ + i >= proba_2_or_more_)
              total_2_and_more_ += proba_[i].proba;
          }
        else if (proba_[i].min_n > 2)
          total_2_and_more_ += proba_[i].proba;
        else
          SPOT_UNREACHABLE(); // unexpected max_n
      }
    assert(total_2_and_more_ >= total_2_);
  }

  formula
  random_formula::generate(int n) const
  {
    assert(n > 0);

    double r = drand();
    op_proba* p;

    // Approximate impossible cases.
    if (n == 1 && total_1_ == 0.0)
      {
        if (total_2_ != 0.0)
          n = 2;
        else
          n = 3;
      }
    else if (n == 2 && total_2_ == 0.0)
      {
        if (total_1_ != 0.0)
          n = 1;
        else
          n = 3;
      }
    else if (n > 2 && total_2_and_more_ == 0.0)
      {
        if (total_1_ != 0.0)
          n = 1;
        else
          assert(total_2_ == 0.0);
      }


    if (n == 1)
      {
        r *= total_1_;
        p = proba_;
      }
    else if (n == 2)
      {
        r *= total_2_;
        p = proba_2_;
      }
    else
      {
        r *= total_2_and_more_;
        p = proba_2_or_more_;
      }

    double s = p->proba;
    while (s < r)
      {
        ++p;
        s += p->proba;
      }

    return p->build(this, n);
  }

  const char*
  random_formula::parse_options(char* options)
  {
    if (!options)
      return nullptr;
    char* key = strtok(options, "=\t, :;");
    while (key)
      {
        char* value = strtok(nullptr, "=\t, :;");
        if (!value)
          return key;

        char* endptr;
        double res = strtod(value, &endptr);
        if (*endptr)
          return value;

        unsigned i;
        for (i = 0; i < proba_size_; ++i)
          {
            if (('a' <= *proba_[i].name && *proba_[i].name <= 'z'
                 && !strcasecmp(proba_[i].name, key))
                || !strcmp(proba_[i].name, key))
              {
                proba_[i].proba = res;
                break;
              }
          }
        if (i == proba_size_)
          return key;

        key = strtok(nullptr, "=\t, :;");
      }
    update_sums();
    return nullptr;
  }

  std::ostream&
  random_formula::dump_priorities(std::ostream& os) const
  {
    for (unsigned i = 0; i < proba_size_; ++i)
      os << proba_[i].name << '\t' << proba_[i].proba << '\n';
    return os;
  }

  // SEREs
  random_sere::random_sere(const atomic_prop_set* ap)
    : random_formula(12, ap), rb(ap)
  {
    proba_[0].setup("eword",   1, eword_builder);
    proba_2_ = proba_ + 1;
    proba_2_or_more_ = proba_ + 1;
    proba_[1].setup("boolform", 1, boolform_builder);
    proba_[2].setup("star",    2, bunop_unbounded_builder<op::Star>);
    proba_[3].setup("star_b",  2, bunop_bounded_builder<op::Star>);
    proba_[4].setup("fstar",   2, bunop_unbounded_builder<op::FStar>);
    proba_[5].setup("fstar_b", 2, bunop_bounded_builder<op::FStar>);
    proba_[6].setup("first_match", 2, unop_builder<op::first_match>);
    proba_[7].setup("and",     3, multop_builder<op::AndRat>);
    proba_[8].setup("andNLM",  3, multop_builder<op::AndNLM>);
    proba_[9].setup("or",      3, multop_builder<op::OrRat>);
    proba_[10].setup("concat",  3, multop_builder<op::Concat>);
    proba_[11].setup("fusion",  3, multop_builder<op::Fusion>);

    update_sums();
  }

  // Boolean formulae
  random_boolean::random_boolean(const atomic_prop_set* ap)
    : random_formula(9, ap)
  {
    proba_[0].setup("ap",      1, ap_builder);
    proba_[0].proba = ap_->size();
    proba_[1].setup("false",   1, false_builder);
    proba_[2].setup("true",    1, true_builder);
    proba_2_or_more_ = proba_2_ = proba_ + 3;
    proba_[3].setup("not",     2, unop_builder<op::Not>);
    proba_[4].setup("equiv",   3, binop_builder<op::Equiv>);
    proba_[5].setup("implies", 3, binop_builder<op::Implies>);
    proba_[6].setup("xor",     3, binop_builder<op::Xor>);
    proba_[7].setup("and",     3, multop_builder<op::And>);
    proba_[8].setup("or",      3, multop_builder<op::Or>);

    update_sums();
  }

  // LTL formulae
  void
  random_ltl::setup_proba_()
  {
    proba_[0].setup("ap",      1, ap_builder);
    proba_[0].proba = ap_->size();
    proba_[1].setup("false",   1, false_builder);
    proba_[2].setup("true",    1, true_builder);
    proba_2_or_more_ = proba_2_ = proba_ + 3;
    proba_[3].setup("not",     2, unop_builder<op::Not>);
    proba_[4].setup("F",       2, unop_builder<op::F>);
    proba_[5].setup("G",       2, unop_builder<op::G>);
    proba_[6].setup("X",       2, unop_builder<op::X>);
    proba_[7].setup("equiv",   3, binop_builder<op::Equiv>);
    proba_[8].setup("implies", 3, binop_builder<op::Implies>);
    proba_[9].setup("xor",     3, binop_builder<op::Xor>);
    proba_[10].setup("R",      3, binop_builder<op::R>);
    proba_[11].setup("U",      3, binop_builder<op::U>);
    proba_[12].setup("W",      3, binop_builder<op::W>);
    proba_[13].setup("M",      3, binop_builder<op::M>);
    proba_[14].setup("and",    3, multop_builder<op::And>);
    proba_[15].setup("or",     3, multop_builder<op::Or>);
  }

  random_ltl::random_ltl(const atomic_prop_set* ap)
    : random_formula(16, ap)
  {
    setup_proba_();
    update_sums();
  }

  random_ltl::random_ltl(int size, const atomic_prop_set* ap)
    : random_formula(size, ap)
  {
    setup_proba_();
    // No call to update_sums(), this functions is always
    // called by the random_psl constructor.
  }

  // PSL
  random_psl::random_psl(const atomic_prop_set* ap)
    : random_ltl(19, ap), rs(ap)
  {
    // FIXME: This looks very fragile.
    memmove(proba_ + 8, proba_ + 7,
            ((proba_ + 16) - (proba_ + 7)) * sizeof(*proba_));

    proba_[7].setup("Closure", 2, closure_builder);
    proba_[17].setup("EConcat", 3, binop_SERELTL_builder<op::EConcat>);
    proba_[18].setup("UConcat", 3, binop_SERELTL_builder<op::UConcat>);
    update_sums();
  }

  randltlgenerator::randltlgenerator(atomic_prop_set aprops,
                                     const option_map& opts,
                                     char* opt_pL,
                                     char* opt_pS,
                                     char* opt_pB)
    : opt_simpl_level_(opts.get("simplification_level", 3)),
      simpl_(tl_simplifier_options{opt_simpl_level_})
  {
    aprops_ = aprops;
    output_ = opts.get("output", randltlgenerator::LTL);
    opt_seed_ = opts.get("seed", 0);
    opt_tree_size_min_ = opts.get("tree_size_min", 15);
    opt_tree_size_max_ = opts.get("tree_size_max", 15);
    opt_unique_ = opts.get("unique", 1);
    opt_wf_ = opts.get("wf", 0);

    const char* tok_pL = nullptr;
    const char* tok_pS = nullptr;
    const char* tok_pB = nullptr;

    switch (output_)
      {
      case randltlgenerator::LTL:
        rf_ = new random_ltl(&aprops_);
        if (opt_pS)
          throw std::invalid_argument("Cannot set sere priorities with "
                                      "LTL output");
        if (opt_pB)
          throw std::invalid_argument("Cannot set boolean priorities with "
                                      "LTL output");
        tok_pL = rf_->parse_options(opt_pL);
        break;
      case randltlgenerator::Bool:
        rf_ = new random_boolean(&aprops_);
        tok_pB = rf_->parse_options(opt_pB);
        if (opt_pL)
          throw std::invalid_argument("Cannot set ltl priorities with "
                                      "Boolean output");
        if (opt_pS)
          throw std::invalid_argument("Cannot set sere priorities "
                                      "with Boolean output");
        break;
      case randltlgenerator::SERE:
        rf_ = rs_ = new random_sere(&aprops_);
        tok_pS = rs_->parse_options(opt_pS);
        tok_pB = rs_->rb.parse_options(opt_pB);
        if (opt_pL)
          throw std::invalid_argument("Cannot set ltl priorities "
                                      "with SERE output");
        break;
      case randltlgenerator::PSL:
        rf_ = rp_ = new random_psl(&aprops_);
        rs_ = &rp_->rs;
        tok_pL = rp_->parse_options(opt_pL);
        tok_pS = rs_->parse_options(opt_pS);
        tok_pB = rs_->rb.parse_options(opt_pB);
        break;
      }

    if (tok_pL)
      throw std::invalid_argument("failed to parse LTL priorities near "
                                  + std::string(tok_pL));
    if (tok_pS)
      throw std::invalid_argument("failed to parse SERE priorities near "
                                  + std::string(tok_pS));
    if (tok_pB)
      throw std::invalid_argument("failed to parse Boolean priorities near "
                                  + std::string(tok_pB));

    spot::srand(opt_seed_);
  }

  randltlgenerator::randltlgenerator(int aprops_n,
                                     const option_map& opts,
                                     char* opt_pL,
                                     char* opt_pS,
                                     char* opt_pB)
    : randltlgenerator(create_atomic_prop_set(aprops_n), opts,
                       opt_pL, opt_pS, opt_pB)
  {
  }

  randltlgenerator::~randltlgenerator()
  {
    delete rf_;
  }

  formula randltlgenerator::next()
  {
    unsigned trials = MAX_TRIALS;
    bool ignore;
    formula f = nullptr;
    do
      {
        ignore = false;
        int size = opt_tree_size_min_;
        if (size != opt_tree_size_max_)
          size = spot::rrand(size, opt_tree_size_max_);
        f = rf_->generate(size);

        if (opt_wf_)
          {
            atomic_prop_set s = aprops_;
            remove_some_props(s);
            f = formula::And({f, GF_n()});
          }

        if (opt_simpl_level_)
          f = simpl_.simplify(f);

        if (opt_unique_ && !unique_set_.insert(f).second)
          ignore = true;
      } while (ignore && --trials);
    if (trials <= 0)
      return nullptr;
    return f;
  }

  void
  randltlgenerator::remove_some_props(atomic_prop_set& s)
  {
    // How many propositions to remove from s?
    // (We keep at least one.)
    size_t n = spot::mrand(aprops_.size());

    while (n--)
      {
        auto i = s.begin();
        std::advance(i, spot::mrand(s.size()));
        s.erase(i);
      }
  }

  // GF(p_1) & GF(p_2) & ... & GF(p_n)
  formula
  randltlgenerator::GF_n()
  {
    formula res = nullptr;
    for (auto v: aprops_)
      {
        formula f = formula::G(formula::F(v));
        if (res)
          res = formula::And({f, res});
        else
          res = f;
      }
    return res;
  }

  void
  randltlgenerator::dump_ltl_priorities(std::ostream& os)
  {
    rf_->dump_priorities(os);
  }

  void
  randltlgenerator::dump_bool_priorities(std::ostream& os)
  {
    rf_->dump_priorities(os);
  }

  void
  randltlgenerator::dump_psl_priorities(std::ostream& os)
  {
    rp_->dump_priorities(os);
  }

  void
  randltlgenerator::dump_sere_priorities(std::ostream& os)
  {
    rs_->dump_priorities(os);
  }

  void
  randltlgenerator::dump_sere_bool_priorities(std::ostream& os)
  {
    rs_->rb.dump_priorities(os);
  }
}
