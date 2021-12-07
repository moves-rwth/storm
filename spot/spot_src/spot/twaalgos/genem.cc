// -*- coding: utf-8 -*-
// Copyright (C) 2017-2020 Laboratoire de Recherche et Developpement
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
#include <spot/twaalgos/genem.hh>
#include <spot/twaalgos/cleanacc.hh>

namespace spot
{
  namespace
  {
    enum genem_version_t { spot28, atva19, spot29 };
    static genem_version_t genem_version = spot29;
  }

  void generic_emptiness_check_select_version(const char* emversion)
  {
    if (emversion == nullptr || !strcasecmp(emversion, "spot29"))
      genem_version = spot29;
    else if (!strcasecmp(emversion, "spot28"))
      genem_version = spot28;
    else if (!strcasecmp(emversion, "atva19"))
      genem_version = atva19;
    else
      throw std::invalid_argument("generic_emptiness_check version should"
                                  " be one of {spot28, atva19, spot29}");
  }

  namespace
  {
    static bool
    is_scc_empty(const scc_info& si, unsigned scc,
                 const acc_cond& autacc, twa_run_ptr run,
                 acc_cond::mark_t tocut = {});

    static bool
    scc_split_check(const scc_info& si, unsigned scc, const acc_cond& acc,
                    twa_run_ptr run, acc_cond::mark_t tocut)
    {
      scc_and_mark_filter filt(si, scc, tocut);
      filt.override_acceptance(acc);
      scc_info upper_si(filt, scc_info_options::STOP_ON_ACC);

      const int accepting_scc = upper_si.one_accepting_scc();
      if (accepting_scc >= 0)
        {
          if (run)
            upper_si.get_accepting_run(accepting_scc, run);
          return false;
        }
      if (!acc.uses_fin_acceptance())
        return true;
      unsigned nscc = upper_si.scc_count();
      for (unsigned scc = 0; scc < nscc; ++scc)
        if (!is_scc_empty(upper_si, scc, acc, run, tocut))
          return false;
      return true;
    }

    static bool
    is_scc_empty(const scc_info& si, unsigned scc,
                 const acc_cond& autacc, twa_run_ptr run,
                 acc_cond::mark_t tocut)
    {
      if (si.is_rejecting_scc(scc))
        return true;
      acc_cond::mark_t sets = si.acc_sets_of(scc);
      acc_cond acc = autacc.restrict_to(sets);
      acc = acc.remove(si.common_sets_of(scc), false);

      if (SPOT_LIKELY(genem_version == spot29))
        do
          {
            acc_cond::acc_code rest = acc_cond::acc_code::f();
            for (const acc_cond& disjunct: acc.top_disjuncts())
              if (acc_cond::mark_t fu = disjunct.fin_unit())
                {
                  if (!scc_split_check
                      (si, scc, disjunct.remove(fu, true), run, fu))
                    return false;
                }
              else
                {
                  rest |= disjunct.get_acceptance();
                }
            if (rest.is_f())
              break;
            acc_cond subacc(acc.num_sets(), std::move(rest));
            int fo = subacc.fin_one();
            assert(fo >= 0);
            // Try to accept when Fin(fo) == true
            acc_cond::mark_t fo_m = {(unsigned) fo};
            if (!scc_split_check
                (si, scc, subacc.remove(fo_m, true), run, fo_m))
              return false;
            // Try to accept when Fin(fo) == false
            acc = subacc.force_inf(fo_m);
          }
        while (!acc.is_f());
      else
        {
          for (const acc_cond& disjunct: acc.top_disjuncts())
            if (acc_cond::mark_t fu = disjunct.fin_unit())
              {
                if (!scc_split_check
                    (si, scc, disjunct.remove(fu, true), run, fu))
                  return false;
              }
            else
              {
                int fo = (SPOT_UNLIKELY(genem_version == spot28)
                          ? acc.fin_one() : disjunct.fin_one());
                assert(fo >= 0);
                // Try to accept when Fin(fo) == true
                acc_cond::mark_t fo_m = {(unsigned) fo};
                if (!scc_split_check
                    (si, scc, disjunct.remove(fo_m, true), run, fo_m))
                  return false;
                // Try to accept when Fin(fo) == false
                if (!is_scc_empty(si, scc, disjunct.force_inf(fo_m),
                                  run, tocut))
                  return false;
              }
        }
      return true;
    }

    static bool
    generic_emptiness_check_main(const twa_graph_ptr& aut,
                                 twa_run_ptr run)
    {
      // We used to call cleanup_acceptance_here(aut, false),
      // but it turns out this is usually a waste of time.
      auto& aut_acc = aut->acc();
      if (aut_acc.is_f())
        return true;
      if (!aut_acc.uses_fin_acceptance())
        {
          if (!run)
            return aut->is_empty();
          if (auto p = aut->accepting_run())
            {
              *run = *p;
              return false;
            }
          return true;
        }
      // Filter with fin_unit() right away if possible.
      // scc_and_mark_filter will have no effect if fin_unit() is
      // empty.
      scc_and_mark_filter filt(aut, aut_acc.fin_unit());
      scc_info si(filt, scc_info_options::STOP_ON_ACC);

      const int accepting_scc = si.one_accepting_scc();
      if (accepting_scc >= 0)
        {
          if (run)
            si.get_accepting_run(accepting_scc, run);
          return false;
        }

      unsigned nscc = si.scc_count();
      for (unsigned scc = 0; scc < nscc; ++scc)
        if (!is_scc_empty(si, scc, aut_acc, run))
          return false;
      return true;
    }
  }

  bool generic_emptiness_check(const const_twa_graph_ptr& aut)
  {
    if (SPOT_UNLIKELY(!aut->is_existential()))
      throw std::runtime_error("generic_emptiness_check() "
                               "does not support alternating automata");
    auto aut_ = std::const_pointer_cast<twa_graph>(aut);
    acc_cond old = aut_->acc();
    bool res = generic_emptiness_check_main(aut_, nullptr);
    aut_->set_acceptance(old);
    return res;
  }

  twa_run_ptr generic_accepting_run(const const_twa_graph_ptr& aut)
  {
    if (SPOT_UNLIKELY(!aut->is_existential()))
      throw std::runtime_error("generic_accepting_run() "
                               "does not support alternating automata");
    auto aut_ = std::const_pointer_cast<twa_graph>(aut);
    acc_cond old = aut_->acc();
    twa_run_ptr run = std::make_shared<twa_run>(aut_);
    bool res = generic_emptiness_check_main(aut_, run);
    aut_->set_acceptance(old);
    if (!res)
      return run;
    return nullptr;
  }

  bool generic_emptiness_check_for_scc(const scc_info& si,
                                       unsigned scc)
  {
    if (si.is_accepting_scc(scc))
      return false;
    return is_scc_empty(si, scc, si.get_aut()->acc(), nullptr);
  }

  bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc)
  {
    if (si.is_trivial(scc))
      return true;
    return scc_split_check(si, scc, forced_acc, nullptr, {});
  }

}
