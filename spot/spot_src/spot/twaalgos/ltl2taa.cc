// -*- coding: utf-8 -*-
// Copyright (C) 2009-2010, 2012-2016, 2018-2019 Laboratoire de
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
#include <utility>
#include <algorithm>
#include <cassert>
#include <spot/tl/unabbrev.hh>
#include <spot/tl/nenoform.hh>
#include <spot/tl/contain.hh>
#include <spot/twaalgos/ltl2taa.hh>

namespace spot
{
  namespace
  {
    /// \brief Recursively translate a formula into a TAA.
    class ltl2taa_visitor final
    {
    public:
      ltl2taa_visitor(const taa_tgba_formula_ptr& res,
                      language_containment_checker* lcc,
                      bool refined = false, bool negated = false)
        : res_(res), refined_(refined), negated_(negated),
          lcc_(lcc), init_(), succ_()
      {
      }

      taa_tgba_formula_ptr&
      result()
      {
        res_->set_init_state(init_);
        return res_;
      }

      void
      visit(formula f)
      {
        init_ = f;
        switch (f.kind())
          {
          case op::ff:
            return;
          case op::tt:
            {
              std::vector<formula> empty;
              res_->create_transition(init_, empty);
              succ_state ss = { empty, f, empty };
              succ_.emplace_back(ss);
              return;
            }
          case op::eword:
            SPOT_UNIMPLEMENTED();
          case op::ap:
            {
              res_->register_ap(f);
              if (negated_)
                f = formula::Not(f);
              init_ = f;
              std::vector<formula> empty;
              taa_tgba::transition* t = res_->create_transition(init_, empty);
              res_->add_condition(t, f);
              succ_state ss = { empty, f, empty };
              succ_.emplace_back(ss);
              return;
            }
          case op::X:
          case op::strong_X:
            {
              ltl2taa_visitor v = recurse(f[0]);
              std::vector<formula> dst;
              std::vector<formula> a;
              if (v.succ_.empty()) // Handle X(0)
                return;
              dst.emplace_back(v.init_);
              res_->create_transition(init_, dst);
              succ_state ss = { dst, formula::tt(), a };
              succ_.emplace_back(ss);
              return;
            }
          case op::F:
          case op::G:
            SPOT_UNIMPLEMENTED(); // TBD
            return;
          case op::Not:
            {
              negated_ = true;
              ltl2taa_visitor v = recurse(f[0]);
              // Done in recurse
              succ_ = v.succ_;
              return;
            }
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
          case op::Star:
          case op::FStar:
          case op::Xor:
          case op::Implies:
          case op::Equiv:
          case op::UConcat:
          case op::EConcat:
          case op::EConcatMarked:
          case op::Concat:
          case op::Fusion:
          case op::AndNLM:
          case op::AndRat:
          case op::OrRat:
          case op::first_match:
            SPOT_UNIMPLEMENTED();

          case op::U:
          case op::W:
          case op::R:
          case op::M:
            visit_binop(f);
            return;
          case op::And:
          case op::Or:
            visit_multop(f);
            return;
          }
      }

      void
      visit_binop(formula f)
      {
        ltl2taa_visitor v1 = recurse(f[0]);
        ltl2taa_visitor v2 = recurse(f[1]);

        std::vector<succ_state>::iterator i1;
        std::vector<succ_state>::iterator i2;
        taa_tgba::transition* t = nullptr;
        bool contained = false;
        bool strong = false;

        switch (f.kind())
        {
        case op::U:
          strong = true;
          SPOT_FALLTHROUGH;
        case op::W:
          if (refined_)
            contained = lcc_->contained(f[0], f[1]);
          for (i1 = v1.succ_.begin(); i1 != v1.succ_.end(); ++i1)
            {
              // Refined rule
              if (refined_ && contained)
                i1->Q.erase
                  (remove(i1->Q.begin(), i1->Q.end(), v1.init_), i1->Q.end());

              i1->Q.emplace_back(init_); // Add the initial state
              if (strong)
                i1->acc.emplace_back(f[1]);
              t = res_->create_transition(init_, i1->Q);
              res_->add_condition(t, i1->condition);
              if (strong)
                res_->add_acceptance_condition(t, f[1]);
              else
                for (unsigned i = 0; i < i1->acc.size(); ++i)
                  res_->add_acceptance_condition(t, i1->acc[i]);
              succ_.emplace_back(*i1);
            }
          for (i2 = v2.succ_.begin(); i2 != v2.succ_.end(); ++i2)
            {
              t = res_->create_transition(init_, i2->Q);
              res_->add_condition(t, i2->condition);
              succ_.emplace_back(*i2);
            }
          return;
        case op::M: // Strong Release
          strong = true;
          SPOT_FALLTHROUGH;
        case op::R: // Weak Release
          if (refined_)
            contained = lcc_->contained(f[0], f[1]);

          for (i2 = v2.succ_.begin(); i2 != v2.succ_.end(); ++i2)
            {
              for (i1 = v1.succ_.begin(); i1 != v1.succ_.end(); ++i1)
                {
                  std::vector<formula> u; // Union
                  std::vector<formula> a; // Acceptance conditions
                  std::copy(i1->Q.begin(), i1->Q.end(), ii(u, u.end()));
                  formula f = i1->condition; // Refined rule
                  if (!refined_ || !contained)
                    {
                      std::copy(i2->Q.begin(), i2->Q.end(), ii(u, u.end()));
                      f = formula::And({f, i2->condition});
                    }
                  t = res_->create_transition(init_, u);
                  res_->add_condition(t, f);
                  succ_state ss = { u, f, a };
                  succ_.emplace_back(ss);
                }

              if (refined_) // Refined rule
                i2->Q.erase
                  (remove(i2->Q.begin(), i2->Q.end(), v2.init_), i2->Q.end());


              i2->Q.emplace_back(init_); // Add the initial state
              t = res_->create_transition(init_, i2->Q);
              res_->add_condition(t, i2->condition);

              if (strong)
                {
                  i2->acc.emplace_back(f[0]);
                  res_->add_acceptance_condition(t, f[0]);
                }
              else if (refined_)
                for (unsigned i = 0; i < i2->acc.size(); ++i)
                  res_->add_acceptance_condition(t, i2->acc[i]);
              succ_.emplace_back(*i2);
            }
          return;
        default:
          SPOT_UNIMPLEMENTED();
        }
        SPOT_UNREACHABLE();
      }

      void
      visit_multop(formula f)
      {
        bool ok = true;
        std::vector<ltl2taa_visitor> vs;
        for (unsigned n = 0, s = f.size(); n < s; ++n)
        {
          vs.emplace_back(recurse(f[n]));
          if (vs[n].succ_.empty()) // Handle 0
            ok = false;
        }

        taa_tgba::transition* t = nullptr;
        switch (f.kind())
          {
          case op::And:
            {
              if (!ok)
                return;
              std::vector<succ_state> p = all_n_tuples(vs);
              for (unsigned n = 0; n < p.size(); ++n)
                {
                  if (refined_)
                    {
                      std::vector<formula> v; // All sub initial states.
                      sort(p[n].Q.begin(), p[n].Q.end());
                      for (unsigned m = 0; m < f.size(); ++m)
                        {
                          if (!binary_search(p[n].Q.begin(), p[n].Q.end(),
                                             vs[m].init_))
                            break;
                          v.emplace_back(vs[m].init_);
                        }

                      if (v.size() == f.size())
                        {
                          std::vector<formula> Q;
                          sort(v.begin(), v.end());
                          for (unsigned m = 0; m < p[n].Q.size(); ++m)
                            if (!binary_search(v.begin(), v.end(), p[n].Q[m]))
                              Q.emplace_back(p[n].Q[m]);
                          Q.emplace_back(init_);
                          t = res_->create_transition(init_, Q);
                          res_->add_condition(t, p[n].condition);
                          for (unsigned i = 0; i < p[n].acc.size(); ++i)
                            res_->add_acceptance_condition(t, p[n].acc[i]);
                          succ_.emplace_back(p[n]);
                          continue;
                        }
                    }
                  t = res_->create_transition(init_, p[n].Q);
                  res_->add_condition(t, p[n].condition);
                  succ_.emplace_back(p[n]);
                }
              return;
            }
          case op::Or:
            for (unsigned n = 0, s = f.size(); n < s; ++n)
              for (auto i: vs[n].succ_)
                {
                  t = res_->create_transition(init_, i.Q);
                  res_->add_condition(t, i.condition);
                  succ_.emplace_back(i);
                }
            return;
          default:
            SPOT_UNIMPLEMENTED();
          }
        SPOT_UNREACHABLE();
      }

      ltl2taa_visitor
      recurse(formula f)
      {
        ltl2taa_visitor v(res_, lcc_, refined_, negated_);
        v.visit(f);
        return v;
      }

    private:
      taa_tgba_formula_ptr res_;
      bool refined_;
      bool negated_;
      language_containment_checker* lcc_;

      typedef std::insert_iterator<std::vector<formula>> ii;

      struct succ_state
      {
        std::vector<formula> Q; // States
        formula condition;
        std::vector<formula> acc;
      };

      formula init_;
      std::vector<succ_state> succ_;

    public:
      std::vector<succ_state>
      all_n_tuples(const std::vector<ltl2taa_visitor>& vs)
      {
        std::vector<succ_state> product;

        std::vector<int> pos(vs.size());
        for (unsigned i = 0; i < vs.size(); ++i)
          pos[i] = vs[i].succ_.size();

        // g++ (Debian 8.3.0-3) 8.3.0 in --coverage mode,
        // reports a "potential null pointer dereference" on the next
        // line without this assert...
        assert(pos.size() > 0);
        while (pos[0] != 0)
        {
          std::vector<formula> u; // Union
          std::vector<formula> a; // Acceptance conditions
          formula f = formula::tt();
          for (unsigned i = 0; i < vs.size(); ++i)
          {
            if (vs[i].succ_.empty())
              continue;
            const succ_state& ss(vs[i].succ_[pos[i] - 1]);
            std::copy(ss.Q.begin(), ss.Q.end(), ii(u, u.end()));
            f = formula::And({ss.condition, f});
            for (unsigned i = 0; i < ss.acc.size(); ++i)
            {
              formula g = ss.acc[i];
              a.emplace_back(g);
            }
          }
          succ_state ss = { u, f, a };
          product.emplace_back(ss);

          for (int i = vs.size() - 1; i >= 0; --i)
          {
            if (vs[i].succ_.empty())
              continue;
            if (pos[i] > 1 || (i == 0 && pos[0] == 1))
            {
              --pos[i];
              break;
            }
            else
              pos[i] = vs[i].succ_.size();
          }
        }
        return product;
      }
    };
  } // anonymous

  taa_tgba_formula_ptr
  ltl_to_taa(formula f,
             const bdd_dict_ptr& dict, bool refined_rules)
  {
    // TODO: implement translation of F and G
    auto f2 = negative_normal_form(unabbreviate(f, "^ieFG"));
    auto res = make_taa_tgba_formula(dict);
    language_containment_checker* lcc =
      new language_containment_checker(make_bdd_dict(),
                                       false, false, false, false);
    ltl2taa_visitor v(res, lcc, refined_rules);
    v.visit(f2);
    auto taa = v.result();
    delete lcc;
    taa->acc().set_generalized_buchi();
    return taa;
  }
}
