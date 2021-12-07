// -*- coding: utf-8 -*-
// Copyright (C) 2012-2020 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/stripacc.hh>
#include <cstdlib>
#include <spot/misc/optionmap.hh>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/dtbasat.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sepsets.hh>
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/parity.hh>
#include <spot/twaalgos/cobuchi.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/toparity.hh>

namespace spot
{
  namespace
  {
    static twa_graph_ptr
    ensure_ba(twa_graph_ptr& a)
    {
      if (a->num_sets() == 0)
        {
          auto m = a->set_buchi();
          for (auto& t: a->edges())
            t.acc = m;
        }
      return a;
    }
  }

  postprocessor::postprocessor(const option_map* opt)
  {
    if (opt)
      {
        degen_order_ = opt->get("degen-order", 0);
        degen_reset_ = opt->get("degen-reset", 1);
        degen_cache_ = opt->get("degen-lcache", 1);
        degen_lskip_ = opt->get("degen-lskip", 1);
        degen_lowinit_ = opt->get("degen-lowinit", 0);
        degen_remscc_ = opt->get("degen-remscc", 1);
        det_scc_ = opt->get("det-scc", 1);
        det_simul_ = opt->get("det-simul", 1);
        det_stutter_ = opt->get("det-stutter", 1);
        det_max_states_ = opt->get("det-max-states", -1);
        det_max_edges_ = opt->get("det-max-edges", -1);
        simul_ = opt->get("simul", -1);
        scc_filter_ = opt->get("scc-filter", -1);
        ba_simul_ = opt->get("ba-simul", -1);
        tba_determinisation_ = opt->get("tba-det", 0);
        sat_minimize_ = opt->get("sat-minimize", 0);
        sat_incr_steps_ = opt->get("sat-incr-steps", -2); // -2 or any num < -1
        sat_langmap_ = opt->get("sat-langmap", 0);
        sat_acc_ = opt->get("sat-acc", 0);
        sat_states_ = opt->get("sat-states", 0);
        state_based_ = opt->get("state-based", 0);
        wdba_minimize_ = opt->get("wdba-minimize", -1);
        gen_reduce_parity_ = opt->get("gen-reduce-parity", 1);

        if (sat_acc_ && sat_minimize_ == 0)
          sat_minimize_ = 1;        // Dicho.
        if (sat_states_ && sat_minimize_ == 0)
          sat_minimize_ = 1;
        if (sat_minimize_)
          {
            tba_determinisation_ = 1;
            if (sat_acc_ <= 0)
              sat_acc_ = -1;
            if (sat_states_ <= 0)
              sat_states_ = -1;
          }

        // Set default param value if not provided and used.
        if (sat_minimize_ == 2 && sat_incr_steps_ < 0) // Assume algorithm.
            sat_incr_steps_ = 6;
        else if (sat_minimize_ == 3 && sat_incr_steps_ < -1) // Incr algorithm.
            sat_incr_steps_ = 2;
      }
  }

  twa_graph_ptr
  postprocessor::do_simul(const twa_graph_ptr& a, int opt) const
  {
    if (!has_separate_sets(a))
      return a;
    switch (opt)
      {
      case 0:
        return a;
      case 1:
        return simulation(a);
      case 2:
        return cosimulation(a);
      case 3:
      default:
        return iterated_simulations(a);
      }
  }

  twa_graph_ptr
  postprocessor::do_sba_simul(const twa_graph_ptr& a, int opt) const
  {
    if (ba_simul_ <= 0)
      return a;
    switch (opt)
      {
      case 0:
        return a;
      case 1:
        return simulation_sba(a);
      case 2:
        return cosimulation_sba(a);
      case 3:
      default:
        return iterated_simulations_sba(a);
      }
  }

  twa_graph_ptr
  postprocessor::do_degen(const twa_graph_ptr& a) const
  {
    auto d = degeneralize(a,
                          degen_reset_, degen_order_,
                          degen_cache_, degen_lskip_,
                          degen_lowinit_, degen_remscc_);
    return do_sba_simul(d, ba_simul_);
  }

  twa_graph_ptr
  postprocessor::do_degen_tba(const twa_graph_ptr& a) const
  {
    return degeneralize_tba(a,
                            degen_reset_, degen_order_,
                            degen_cache_, degen_lskip_,
                            degen_lowinit_, degen_remscc_);
  }

  static void
  force_buchi(twa_graph_ptr& a)
  {
    assert(a->acc().is_t());
    acc_cond::mark_t m = a->set_buchi();
    for (auto& e: a->edges())
      e.acc = m;
  }

  twa_graph_ptr
  postprocessor::do_scc_filter(const twa_graph_ptr& a, bool arg) const
  {
    if (scc_filter_ == 0)
      return a;
    if (state_based_ && a->prop_state_acc().is_true())
      return scc_filter_states(a, arg);
    else
      return scc_filter(a, arg);
  }

  twa_graph_ptr
  postprocessor::do_scc_filter(const twa_graph_ptr& a) const
  {
    return do_scc_filter(a, scc_filter_ > 1);
  }

#define PREF_ (pref_ & (Small | Deterministic))
#define COMP_ (pref_ & Complete)
#define SBACC_ (pref_ & SBAcc)
#define COLORED_ (pref_ & Colored)


  twa_graph_ptr
  postprocessor::finalize(twa_graph_ptr tmp) const
  {
    if (COMP_)
      tmp = complete(tmp);
    bool want_parity = type_ & Parity;
    if (want_parity && tmp->acc().is_generalized_buchi())
      tmp = SBACC_ ? do_degen(tmp) : do_degen_tba(tmp);
    if (SBACC_)
      tmp = sbacc(tmp);
    if (type_ == BA && tmp->acc().is_t())
      force_buchi(tmp);
    if (want_parity)
      {
        reduce_parity_here(tmp, COLORED_);
        parity_kind kind = parity_kind_any;
        parity_style style = parity_style_any;
        if ((type_ & ParityMin) == ParityMin)
          kind = parity_kind_min;
        else if ((type_ & ParityMax) == ParityMax)
          kind = parity_kind_max;
        if ((type_ & ParityOdd) == ParityOdd)
          style = parity_style_odd;
        else if ((type_ & ParityEven) == ParityEven)
          style = parity_style_even;
        change_parity_here(tmp, kind, style);
      }
    return tmp;
  }

  twa_graph_ptr
  postprocessor::run(twa_graph_ptr a, formula f)
  {
    if (simul_ < 0)
      simul_ = (level_ == Low) ? 1 : 3;
    if (ba_simul_ < 0)
      ba_simul_ = (level_ == High) ? 3 : 0;
    if (scc_filter_ < 0)
      scc_filter_ = 1;
    if (type_ == BA || SBACC_)
      state_based_ = true;

    bool via_gba = (type_ == BA) || (type_ == TGBA) || (type_ == Monitor);
    bool want_parity = type_ & Parity;
    if (COLORED_ && !want_parity)
      throw std::runtime_error("postprocessor: the Colored setting only works "
                               "for parity acceptance");



    // Attempt to simplify the acceptance condition, unless this is a
    // parity automaton and we want parity acceptance in the output.
    auto simplify_acc = [&](twa_graph_ptr in)
      {
        if (PREF_ == Any && level_ == Low)
          return in;
        bool isparity = in->acc().is_parity();
        if (isparity && in->is_existential()
            && (type_ == Generic || want_parity))
          {
            auto res = reduce_parity(in);
            if (want_parity || gen_reduce_parity_)
              return res;
            // In case type_ == Generic and gen_reduce_parity_ == 0,
            // we only return the result of reduce_parity() if it can
            // lower the number of colors.  Otherwise,
            // simplify_acceptance() will not do better and we return
            // the input unchanged.  The reason for not always using
            // the output of reduce_parity() is that is may hide
            // symmetries between automata, as in issue #402.
            return (res->num_sets() < in->num_sets()) ? res : in;
          }
        if (want_parity && isparity)
          return cleanup_parity(in);
        if (level_ == High)
          return simplify_acceptance(in);
        else
          return cleanup_acceptance(in);
      };
    a = simplify_acc(a);

    if (!a->is_existential() &&
        // We will probably have to revisit this condition later.
        // Currently, the intent is that postprocessor should never
        // return an alternating automaton, unless it is called with
        // its laxest settings.
        !(type_ == Generic && PREF_ == Any && level_ == Low))
      a = remove_alternation(a);

    if ((via_gba || (want_parity && !a->acc().is_parity()))
        && !a->acc().is_generalized_buchi())
      {
        // If we do want a parity automaton, we can use to_parity().
        // However (1) degeneralization is better if the input is
        // GBA, and (2) if we want a deterministic parity automaton and the
        // input is not deterministic, that is useless here.  We need
        // to determinize it first, and our deterministization
        // function only deal with TGBA as input.
        if (want_parity && (PREF_ != Deterministic || is_deterministic(a)))
          {
            a = to_parity(a);
          }
        else
          {
            a = to_generalized_buchi(a);
            if (PREF_ == Any && level_ == Low)
              a = do_scc_filter(a, true);
          }
      }

    if (PREF_ == Any && level_ == Low
        && (type_ == Generic
            || type_ == TGBA
            || (type_ == BA && a->is_sba())
            || (type_ == Monitor && a->num_sets() == 0)
            || (want_parity && a->acc().is_parity())
            || (type_ == CoBuchi && a->acc().is_co_buchi())))
      return finalize(a);

    int original_acc = a->num_sets();

    // Remove useless SCCs.
    if (type_ == Monitor)
      // Do not bother about acceptance conditions, they will be
      // ignored.
      a = scc_filter_states(a);
    else
      a = do_scc_filter(a, (PREF_ == Any));

    if (type_ == Monitor)
      {
        if (PREF_ == Deterministic)
          a = minimize_monitor(a);
        else
          strip_acceptance_here(a);

        if (PREF_ != Any)
          {
            if (PREF_ != Deterministic)
              a = do_simul(a, simul_);

            // For Small,High we return the smallest between the output of
            // the simulation, and that of the deterministic minimization.
            // Prefer the deterministic automaton in case of equality.
            if (PREF_ == Small && level_ == High && simul_)
              {
                auto m = minimize_monitor(a);
                if (m->num_states() <= a->num_states())
                  a = m;
              }
            a->remove_unused_ap();
          }
        return finalize(a);
      }

    if (PREF_ == Any)
      {
        if (type_ == BA)
          a = do_degen(a);
        else if (type_ == CoBuchi)
          a = to_nca(a);
        return finalize(a);
      }

    bool dba_is_wdba = false;
    bool dba_is_minimal = false;
    twa_graph_ptr dba = nullptr;
    twa_graph_ptr sim = nullptr;

    output_aborter aborter_
      (det_max_states_ >= 0 ? static_cast<unsigned>(det_max_states_) : -1U,
       det_max_edges_ >= 0 ? static_cast<unsigned>(det_max_edges_) : -1U);
    output_aborter* aborter =
      (det_max_states_ >= 0 || det_max_edges_ >= 0) ? &aborter_ : nullptr;

    int wdba_minimize = wdba_minimize_;
    if (wdba_minimize == -1)
      {
        if (level_ == High)
          wdba_minimize = 1;
        else if (level_ == Medium || PREF_ == Deterministic)
          wdba_minimize = 2;
        else
          wdba_minimize = 0;
      }
    if (wdba_minimize == 2)
      wdba_minimize = minimize_obligation_garanteed_to_work(a, f);
    if (wdba_minimize)
      {
        bool reject_bigger = (PREF_ == Small) && (level_ <= Medium);
        dba = minimize_obligation(a, f, nullptr, reject_bigger, aborter);
        if (dba
            && dba->prop_inherently_weak().is_true()
            && dba->prop_universal().is_true())
          {
            // The WDBA is a BA, so no degeneralization is required.
            // We just need to add an acceptance set if there is none.
            dba_is_minimal = dba_is_wdba = true;
            if (type_ == BA)
              ensure_ba(dba);
          }
        else
          {
            // Minimization failed.
            dba = nullptr;
          }
      }

    // Run a simulation when wdba failed (or was not run), or
    // at hard levels if we want a small output.
    if (!dba || (level_ == High && PREF_ == Small))
      {
        if (((SBACC_ && a->prop_state_acc().is_true())
             || (type_ == BA && a->is_sba()))
            && !tba_determinisation_)
          {
            sim = do_sba_simul(a, ba_simul_);
          }
        else
          {
            sim = do_simul(a, simul_);
            // Degeneralize the result of the simulation if needed.
            // No need to do that if tba_determinisation_ will be used.
            if (type_ == BA && !tba_determinisation_)
              sim = do_degen(sim);
            else if (want_parity && !sim->acc().is_parity())
              sim = do_degen_tba(sim);
            else if (SBACC_ && !tba_determinisation_)
              sim = sbacc(sim);
          }
      }

    // If WDBA failed, but the simulation returned a deterministic
    // automaton, use it as dba.
    assert(dba || sim);
    if (!dba && is_deterministic(sim))
      {
        std::swap(sim, dba);
        // We postponed degeneralization above i case we would need
        // to perform TBA-determinisation, but now it is clear
        // that we won't perform it.  So do degeneralize.
        if (tba_determinisation_)
          {
            if (type_ == BA)
              {
                dba = do_degen(dba);
                assert(is_deterministic(dba));
              }
            else if (SBACC_)
              {
                dba = sbacc(dba);
                assert(is_deterministic(dba));
              }
          }
      }

    // If we don't have a DBA, attempt tba-determinization if requested.
    if (tba_determinisation_ && !dba)
      {
        twa_graph_ptr tmpd = nullptr;
        if (PREF_ == Deterministic
            && f
            && f.is_syntactic_recurrence()
            && sim->num_sets() > 1)
          tmpd = degeneralize_tba(sim);

        auto in = tmpd ? tmpd : sim;

        // These thresholds are arbitrary.
        //
        // For producing Small automata, we assume that a
        // deterministic automaton that is twice the size of the
        // original will never get reduced to a smaller one.  We also
        // do not want more than 2^13 cycles in an SCC.
        //
        // For Deterministic automata, we accept automata that
        // are 8 times bigger, with no more that 2^15 cycle per SCC.
        // The cycle threshold is the most important limit here.  You
        // may up it if you want to try producing larger automata.
        auto tmp =
          tba_determinize_check(in,
                                (PREF_ == Small) ? 2 : 8,
                                1 << ((PREF_ == Small) ? 13 : 15),
                                f);
        if (tmp && tmp != in)
          {
            // There is no point in running the reverse simulation on
            // a deterministic automaton, since all prefixes are
            // unique.
            dba = simulation(tmp);
          }
        if (dba && PREF_ == Deterministic)
          {
            // disregard the result of the simulation.
            sim = nullptr;
          }
        else
          {
            // degeneralize sim, because we did not do it earlier
            if (type_ == BA)
              sim = do_degen(sim);
          }
      }

    if ((PREF_ == Deterministic && (type_ == Generic || want_parity)) && !dba)
      {
        dba = tgba_determinize(to_generalized_buchi(sim),
                               false, det_scc_, det_simul_, det_stutter_,
                               aborter);
        // Setting det-max-states or det-max-edges may cause tgba_determinize
        // to fail.
        if (dba)
          {
            dba = simplify_acc(dba);
            if (level_ != Low)
              dba = simulation(dba);
            sim = nullptr;
          }
      }

    // Now dba contains either the result of WDBA-minimization (in
    // that case dba_is_wdba=true), or some deterministic automaton
    // that is either the result of the simulation or of the
    // TBA-determinization (dba_is_wdba=false in both cases), or a
    // parity automaton coming from tgba_determinize().  If the dba is
    // a WDBA, we do not have to run SAT-minimization.  A negative
    // value in sat_minimize_ can force its use for debugging.
    if (sat_minimize_ && dba && (!dba_is_wdba || sat_minimize_ < 0))
      {
        if (type_ == Generic)
          throw std::runtime_error
            ("postproc() not yet updated to mix sat-minimize and Generic");
        unsigned target_acc;
        if (type_ == BA)
          target_acc = 1;
        else if (sat_acc_ != -1)
          target_acc = sat_acc_;
        else
          // Take the number of acceptance conditions from the input
          // automaton, not from dba, because dba often has been
          // degeneralized by tba_determinize_check().  Make sure it
          // is at least 1.
          target_acc = original_acc > 0 ? original_acc : 1;

        const_twa_graph_ptr in = nullptr;
        if (target_acc == 1)
          {
            // If we are seeking a minimal DBA with unknown number of
            // states, then we should start from the degeneralized,
            // because the input TBA might be smaller.
            if (state_based_)
              in = degeneralize(dba);
            else
              in = degeneralize_tba(dba);
          }
        else
          {
            in = dba;
          }

        twa_graph_ptr res = complete(in);
        if (target_acc == 1)
          {
            if (sat_states_ != -1)
              res = dtba_sat_synthetize(res, sat_states_, state_based_);
            else if (sat_minimize_ == 1)
              res = dtba_sat_minimize_dichotomy
                (res, state_based_, sat_langmap_);
            else if (sat_minimize_ == 2)
              res = dtba_sat_minimize_assume(res, state_based_, -1,
                                             sat_incr_steps_);
            else if (sat_minimize_ == 3)
              res = dtba_sat_minimize_incr(res, state_based_, -1,
                                           sat_incr_steps_);
            else // if (sat_minimize == 4 || sat_minimize == -1)
              res = dtba_sat_minimize(res, state_based_);
          }
        else
          {
            if (sat_states_ != -1)
              res = dtwa_sat_synthetize
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 sat_states_, state_based_);
            else if (sat_minimize_ == 1)
              res = dtwa_sat_minimize_dichotomy
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, sat_langmap_);
            else if (sat_minimize_ == 2)
              res = dtwa_sat_minimize_assume
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, -1, false, sat_incr_steps_);
            else if (sat_minimize_ == 3)
              res = dtwa_sat_minimize_incr
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_, -1, false, sat_incr_steps_);
            else // if (sat_minimize_ == 4 || sat_minimize_ == -1)
              res = dtwa_sat_minimize
                (res, target_acc,
                 acc_cond::acc_code::generalized_buchi(target_acc),
                 state_based_);
          }

        if (res)
          {
            dba = do_scc_filter(res, true);
            dba_is_minimal = true;
          }
      }

    // Degeneralize the dba resulting from tba-determinization or
    // sat-minimization (which is a TBA) if requested and needed.
    if (dba && !dba_is_wdba && type_ == BA
        && !(dba_is_minimal && state_based_ && dba->num_sets() == 1))
      dba = degeneralize(dba);

    if (dba && sim)
      {
        if (dba->num_states() > sim->num_states())
          dba = nullptr;
        else
          sim = nullptr;
      }

    if (level_ == High && scc_filter_ != 0)
      {
        if (dba)
          {
            // Do that even for WDBA, to remove marks from transitions
            // leaving trivial SCCs.
            dba = do_scc_filter(dba, true);
            assert(!sim);
          }
        else if (sim)
          {
            sim = do_scc_filter(sim, true);
            assert(!dba);
          }
      }

    sim = dba ? dba : sim;

    sim->remove_unused_ap();

    if (type_ == CoBuchi)
      {
        unsigned ns = sim->num_states();
        if (PREF_ == Deterministic)
          sim = to_dca(sim);
        else
          sim = to_nca(sim);

        // if the input of to_dca/to_nca was weak, the number of
        // states has not changed, and running simulation is useless.
        if (level_ != Low && ns < sim->num_states())
          sim = do_simul(sim, simul_);
      }

    return finalize(sim);
  }
}
