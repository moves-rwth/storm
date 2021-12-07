// -*- coding: utf-8 -*-
// Copyright (C) 2013-2018, 2020 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/compsusp.hh>
#include <spot/misc/optionmap.hh>
#include <spot/tl/relabel.hh>
#include <spot/twaalgos/relabel.hh>
#include <spot/twaalgos/gfguarantee.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/hoa.hh>

namespace spot
{

  void translator::setup_opt(const option_map* opt)
  {
    comp_susp_ = early_susp_ = skel_wdba_ = skel_simul_ = 0;
    relabel_bool_ = 4;
    tls_impl_ = -1;
    ltl_split_ = true;

    opt_ = opt;
    if (!opt)
      return;

    relabel_bool_ = opt->get("relabel-bool", 4);
    comp_susp_ = opt->get("comp-susp", 0);
    if (comp_susp_ == 1)
      {
        early_susp_ = opt->get("early-susp", 0);
        skel_wdba_ = opt->get("skel-wdba", -1);
        skel_simul_ = opt->get("skel-simul", 1);
      }
    tls_impl_ = opt->get("tls-impl", -1);
    int gfg = opt->get("gf-guarantee", -1);
    if (gfg >= 0)
      {
        gf_guarantee_ = !!gfg;
        gf_guarantee_set_ = true;
      }
    ltl_split_ = opt->get("ltl-split", 1);
  }

  void translator::build_simplifier(const bdd_dict_ptr& dict)
  {
    tl_simplifier_options options(false, false, false);
    switch (level_)
      {
      case High:
        options.containment_checks = true;
        options.containment_checks_stronger = true;
        SPOT_FALLTHROUGH;
      case Medium:
        options.synt_impl = true;
        SPOT_FALLTHROUGH;
      case Low:
        options.reduce_basics = true;
        options.event_univ = true;
      }
    // User-supplied fine-tuning?
    if (tls_impl_ >= 0)
      switch (tls_impl_)
        {
        case 0:
          options.synt_impl = false;
          options.containment_checks = false;
          options.containment_checks_stronger = false;
          break;
        case 1:
          options.synt_impl = true;
          options.containment_checks = false;
          options.containment_checks_stronger = false;
          break;
        case 2:
          options.synt_impl = true;
          options.containment_checks = true;
          options.containment_checks_stronger = false;
          break;
        case 3:
          options.synt_impl = true;
          options.containment_checks = true;
          options.containment_checks_stronger = true;
          break;
        default:
          throw std::runtime_error
            ("tls-impl should take a value between 0 and 3");
        }
    simpl_owned_ = simpl_ = new tl_simplifier(options, dict);
  }


  twa_graph_ptr translator::run_aux(formula r)
  {
#define PREF_ (pref_ & (Small | Deterministic))

    bool unambiguous = (pref_ & postprocessor::Unambiguous);
    if (unambiguous && type_ == postprocessor::Monitor)
      {
        // Deterministic monitor are unambiguous, so the unambiguous
        // option is not really relevant for monitors.
        unambiguous = false;
        set_pref(pref_ | postprocessor::Deterministic);
      }

    // This helps ltl_to_tgba_fm() to order BDD variables in a more
    // natural way (improving the degeneralization).
    simpl_->clear_as_bdd_cache();

    twa_graph_ptr aut;
    twa_graph_ptr aut2 = nullptr;

    if (ltl_split_ && !r.is_syntactic_obligation())
      {
        formula r2 = r;
        unsigned leading_x = 0;
        while (r2.is(op::X))
          {
            r2 = r2[0];
            ++leading_x;
          }
        if (type_ == Generic || type_ == TGBA)
          {
            // F(q|u|f) = q|F(u)|F(f)  only for generic acceptance
            // G(q&e&f) = q&G(e)&G(f)
            bool want_u = r2.is({op::F, op::Or}) && (type_ == Generic);
            if (want_u || r2.is({op::G, op::And}))
              {
                std::vector<formula> susp;
                std::vector<formula> rest;
                auto op1 = r2.kind();
                auto op2 = r2[0].kind();
                for (formula child: r2[0])
                  {
                    bool u = child.is_universal();
                    bool e = child.is_eventual();
                    if (u && e)
                      susp.push_back(child);
                    else if ((want_u && u) || (!want_u && e))
                      susp.push_back(formula::unop(op1, child));
                    else
                      rest.push_back(child);
                  }
                susp.push_back(formula::unop(op1, formula::multop(op2, rest)));
                r2 = formula::multop(op2, susp);
              }
          }
        if (r2.is_syntactic_obligation() || !r2.is(op::And, op::Or,
                                                   op::Xor, op::Equiv) ||
            // For TGBA/BA we only do conjunction.  There is nothing wrong
            // with disjunction, but it seems to generate larger automata
            // in many cases and it needs to be further investigated.  Maybe
            // this could be relaxed in the case of deterministic output.
            (!r2.is(op::And) && (type_ == TGBA || type_ == BA)))
          goto nosplit;

        op topop = r2.kind();
        // Let's classify subformulas.
        std::vector<formula> oblg;
        std::vector<formula> susp;
        std::vector<formula> rest;
        bool want_g = type_ == TGBA || type_ == BA;
        for (formula child: r2)
          {
            if (child.is_syntactic_obligation())
              oblg.push_back(child);
            else if (child.is_eventual() && child.is_universal()
                     && (!want_g || child.is(op::G)))
              susp.push_back(child);
            else
              rest.push_back(child);
          }

        // Safety formulas are quite easy to translate since they do
        // not introduce marks.  If rest is non-empty, it seems
        // preferable to translate the safety inside rest, as this may
        // constrain the translation.
        if (!rest.empty() && !oblg.empty())
          {
            auto safety = [](formula f)
              {
                return f.is_syntactic_safety();
              };
            auto i = std::remove_if(oblg.begin(), oblg.end(), safety);
            rest.insert(rest.end(), i, oblg.end());
            oblg.erase(i, oblg.end());
          }

        if (!susp.empty())
          {
            // The only cases where we accept susp and rest to be both
            // non-empty is when doing Generic acceptance or TGBA.
            if (!rest.empty() && !(type_ == Generic || type_ == TGBA))
              {
                rest.insert(rest.end(), susp.begin(), susp.end());
                susp.clear();
              }
            // For Parity, we want to translate all suspendable
            // formulas at once.
            if (rest.empty() && type_ & Parity)
              susp = { formula::multop(r2.kind(), susp) };
          }
        // For TGBA and BA, we only split if there is something to
        // suspend.
        if (susp.empty() && (type_ == TGBA || type_ == BA))
          goto nosplit;

        option_map om_wos;
        option_map om_ws;
        if (opt_)
          om_ws = *opt_;
        // Don't blindingly apply reduce_parity() in the
        // generic case, for issue #402.
        om_ws.set("gen-reduce-parity", 0);
        om_wos = om_ws;
        om_wos.set("ltl-split", 0);
        translator translate_without_split(simpl_, &om_wos);
        // Never force colored automata at intermediate steps.
        // This is best added at the very end.
        translate_without_split.set_pref(pref_ & ~Colored);
        translate_without_split.set_level(level_);
        translate_without_split.set_type(type_);
        translator translate_with_split(simpl_, &om_ws);
        translate_with_split.set_pref(pref_ & ~Colored);
        translate_with_split.set_level(level_);
        translate_with_split.set_type(type_);

        auto transrun = [&](formula f)
          {
            if (f == r2)
              return translate_without_split.run(f);
            else
              return translate_with_split.run(f);
          };

        // std::cerr << "splitting\n";
        aut = nullptr;
        // All obligations can be converted into a minimal WDBA.
        if (!oblg.empty())
          {
            formula oblg_f = formula::multop(r2.kind(), oblg);
            //std::cerr << "oblg: " << oblg_f << '\n';
            aut = transrun(oblg_f);
          }
        if (!rest.empty())
          {
            formula rest_f = formula::multop(r2.kind(), rest);
            //std::cerr << "rest: " << rest_f << '\n';
            twa_graph_ptr rest_aut = transrun(rest_f);
            if (aut == nullptr)
              aut = rest_aut;
            else if (topop == op::And)
              aut = product(aut, rest_aut);
            else if (topop == op::Or)
              aut = product_or(aut, rest_aut);
            else if (topop == op::Xor)
              aut = product_xor(aut, rest_aut);
            else if (topop == op::Equiv)
              aut = product_xnor(aut, rest_aut);
            else
              SPOT_UNREACHABLE();
          }
        if (!susp.empty())
          {
            twa_graph_ptr susp_aut = nullptr;
            // Each suspendable formula separately
            for (formula f: susp)
              {
                //std::cerr << "susp: " << f << '\n';
                twa_graph_ptr one = transrun(f);
                if (!susp_aut)
                  susp_aut = one;
                else if (topop == op::And)
                  susp_aut = product(susp_aut, one);
                else if (topop == op::Or)
                  susp_aut = product_or(susp_aut, one);
                else if (topop == op::Xor)
                  susp_aut = product_xor(susp_aut, one);
                else if (topop == op::Equiv)
                  susp_aut = product_xnor(susp_aut, one);
                else
                  SPOT_UNREACHABLE();
              }
            if (susp_aut->prop_universal().is_true())
              {
                // In a deterministic and suspendable automaton, all
                // state recognize the same language, so we can move
                // the initial state into a bottom accepting SCC.
                scc_info si(susp_aut, scc_info_options::NONE);
                if (si.is_trivial(si.scc_of(susp_aut->get_init_state_number())))
                  {
                    unsigned st = si.one_state_of(0);
                    // The bottom SCC can actually be trivial if it
                    // has no successor because the formula is
                    // equivalent to false.
                    assert(!si.is_trivial(0) ||
                           susp_aut->out(st).begin()
                           == susp_aut->out(st).end());
                    susp_aut->set_init_state(st);
                    susp_aut->purge_unreachable_states();
                  }
              }
            if (aut == nullptr)
              aut = susp_aut;
            else if (topop == op::And)
              aut = product_susp(aut, susp_aut);
            else if (topop == op::Or)
              aut = product_or_susp(aut, susp_aut);
            else if (topop == op::Xor) // No suspension here
              aut = product_xor(aut, susp_aut);
            else if (topop == op::Equiv) // No suspension here
              aut = product_xnor(aut, susp_aut);
            //if (aut && susp_aut)
            //  {
            //    print_hoa(std::cerr << "AUT\n", aut) << '\n';
            //    print_hoa(std::cerr << "SUSPAUT\n", susp_aut) << '\n';
            //  }
          }
        if (leading_x > 0)
          {
            unsigned init = aut->get_init_state_number();
            do
              {
                unsigned tmp = aut->new_state();
                aut->new_edge(tmp, init, bddtrue);
                init = tmp;
              }
            while (--leading_x);
            aut->set_init_state(init);
            // Adding initial edges is very likely to kill stutter
            // invariance (and it certainly cannot fix it).
            if (aut->prop_stutter_invariant().is_true())
              aut->prop_stutter_invariant(trival::maybe());
          }
      }
    else
      {
      nosplit:
        if (comp_susp_ > 0)
          {
            // FIXME: Handle unambiguous_ automata?
            int skel_wdba = skel_wdba_;
            if (skel_wdba < 0)
              skel_wdba = (pref_ & postprocessor::Deterministic) ? 1 : 2;

            aut = compsusp(r, simpl_->get_dict(), skel_wdba == 0,
                           skel_simul_ == 0, early_susp_ != 0,
                           comp_susp_ == 2, skel_wdba == 2, false);
          }
        else
          {
            if (gf_guarantee_ && PREF_ != Any)
              {
                bool det = unambiguous || (PREF_ == Deterministic);
                bool sba = type_ == BA || (pref_ & SBAcc);
                if ((type_ & (BA | Parity | Generic)) || type_ == TGBA)
                  aut2 = gf_guarantee_to_ba_maybe(r, simpl_->get_dict(),
                                                  det, sba);
                if (aut2 && ((type_ == BA) || (type_ & Parity))
                    && (pref_ & Deterministic))
                  return finalize(aut2);
                if (!aut2 && (type_ == Generic
                              || type_ & (Parity | CoBuchi)))
                  {
                    aut2 = fg_safety_to_dca_maybe(r, simpl_->get_dict(), sba);
                    if (aut2
                        && (type_ & (CoBuchi | Parity))
                        && (pref_ & Deterministic))
                      return finalize(aut2);
                  }
              }
          }
        bool exprop = unambiguous || level_ == postprocessor::High;
        aut = ltl_to_tgba_fm(r, simpl_->get_dict(), exprop,
                             true, false, false, nullptr, nullptr,
                             unambiguous);
      }

    aut = this->postprocessor::run(aut, r);
    if (aut2)
      {
        aut2 = this->postprocessor::run(aut2, r);
        unsigned s2 = aut2->num_states();
        unsigned s1 = aut->num_states();
        bool d2_more_det = !is_deterministic(aut) && is_deterministic(aut2);
        if (((PREF_ == Deterministic) && d2_more_det)
            || (s2 < s1)
            || (s2 == s1
                && ((aut2->num_sets() < aut->num_sets()) || d2_more_det)))
          aut = std::move(aut2);
      }

    return aut;
  }

  twa_graph_ptr translator::run(formula* f)
  {
    if (simpl_owned_)
      {
        // Modify the options according to set_pref() and set_type().
        // We do it for all translation, but really only the first one
        // matters.
        auto& opt = simpl_owned_->options();
        if (comp_susp_ > 0 || (ltl_split_ && type_ == Generic))
          opt.favor_event_univ = true;
        if (type_ == Generic && ltl_split_ && (pref_ & Deterministic))
          opt.keep_top_xor = true;
      }

    // Do we want to relabel Boolean subformulas?
    // If we have a huge formula such as
    //  (a1 & a2 & ... & an) U (b1 | b2 | ... | bm)
    // then it is more efficient to translate
    //  a U b
    // and then fix the automaton.  We use relabel_bse() to find
    // sub-formulas that are Boolean but do not have common terms.
    //
    // This rewriting is enabled only if the formula
    //  1) has some Boolean subformula
    //  2) has more than relabel_bool_ atomic propositions (the default
    //     is 4, but this can be changed)
    //  3) relabel_bse() actually reduces the number of atomic
    //     propositions.
    relabeling_map m;
    formula to_work_on = *f;
    if (relabel_bool_ > 0)
      {
        bool has_boolean_sub = false; // that is not atomic
        std::set<formula> aps;
        to_work_on.traverse([&](const formula& f)
                            {
                              if (f.is(op::ap))
                                aps.insert(f);
                              else if (f.is_boolean())
                                has_boolean_sub = true;
                              return false;
                            });
        unsigned atomic_props = aps.size();
        if (has_boolean_sub && (atomic_props >= (unsigned) relabel_bool_))
          {
            formula relabeled = relabel_bse(to_work_on, Pnn, &m);
            if (m.size() < atomic_props)
              to_work_on = relabeled;
            else
              m.clear();
          }
      }

    formula r = simpl_->simplify(to_work_on);
    if (to_work_on == *f)
      *f = r;
    else
      *f = relabel_apply(r, &m);

    auto aut = run_aux(r);

    if (!m.empty())
      relabel_here(aut, &m);
    return aut;
  }

  twa_graph_ptr translator::run(formula f)
  {
    return run(&f);
  }

  void translator::clear_caches()
  {
    simpl_->clear_caches();
  }
}
