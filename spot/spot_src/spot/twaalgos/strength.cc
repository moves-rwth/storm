// -*- coding: utf-8 -*-
// Copyright (C) 2010-2011, 2013-2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE)
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
#include <spot/twaalgos/strength.hh>
#include <spot/misc/hash.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/mask.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/sccfilter.hh>

using namespace std::string_literals;

namespace spot
{
  namespace
  {
    template <bool terminal, bool inweak = false, bool set = false>
    bool is_type_automaton(const twa_graph_ptr& aut, scc_info* si,
                           bool ignore_trivial_term = false)
    {
      // Create an scc_info if the user did not give one to us.
      bool need_si = !si;
      if (need_si)
        si = new scc_info(aut);
      if (inweak)
        si->determine_unknown_acceptance();

      bool is_inweak = true;
      bool is_weak = true;
      bool is_single_state_scc = true;
      bool is_term = true;
      unsigned n = si->scc_count();
      for (unsigned i = 0; i < n; ++i)
        {
          if (si->is_trivial(i))
            continue;
          if (si->states_of(i).size() > 1)
            is_single_state_scc = false;
          bool first = true;
          acc_cond::mark_t m = {};
          if (is_weak)
            for (auto& t: si->edges_of(i))
              // In case of a universal edge we only need to check if
              // the first destination of an edge is inside the SCC,
              // because the others have the same t.acc.
              if (si->scc_of(*aut->univ_dests(t.dst).begin()) == i)
                {
                  if (first)
                    {
                      first = false;
                      m = t.acc;
                    }
                  else if (m != t.acc)
                    {
                      is_weak = false;
                      if (!inweak)
                        goto exit;
                    }
                }
          if (!is_weak && si->is_accepting_scc(i))
            {
              assert(inweak);
              if (scc_has_rejecting_cycle(*si, i))
                {
                  is_inweak = false;
                  break;
                }
            }
          if (terminal && si->is_accepting_scc(i) && !is_complete_scc(*si, i))
            {
              is_term = false;
              if (!set)
                break;
            }
        }
      // A terminal automaton should accept any word that has a prefix
      // leading to an accepting edge.  In other words, we cannot have
      // an accepting edge that goes into a rejecting SCC.
      if (terminal && is_term && !ignore_trivial_term)
        for (auto& e: aut->edges())
          if (si->is_rejecting_scc(si->scc_of(e.dst))
              && aut->acc().accepting(e.acc))
            {
              is_term = false;
              break;
            }
    exit:
      if (need_si)
        delete si;
      if (set)
        {
          if (terminal)
            {
              if (!ignore_trivial_term)
                aut->prop_terminal(is_term && is_weak);
              else if (is_term && is_weak)
                aut->prop_terminal(true);
            }
          aut->prop_weak(is_weak);
          aut->prop_very_weak(is_single_state_scc && is_weak);
          if (inweak)
            aut->prop_inherently_weak(is_inweak);
        }
      if (inweak)
        return is_inweak;
      return is_weak && is_term;
    }
  }

  bool
  is_terminal_automaton(const const_twa_graph_ptr& aut, scc_info* si,
                        bool ignore_trivial_term)
  {
    trival v = aut->prop_terminal();
    if (v.is_known())
      return v.is_true();
    bool res =
      is_type_automaton<true>(std::const_pointer_cast<twa_graph>(aut), si,
                              ignore_trivial_term);
    std::const_pointer_cast<twa_graph>(aut)->prop_terminal(res);
    return res;
  }

  bool
  is_weak_automaton(const const_twa_graph_ptr& aut, scc_info* si)
  {
    trival v = aut->prop_weak();
    if (v.is_known())
      return v.is_true();
    bool res =
      is_type_automaton<false>(std::const_pointer_cast<twa_graph>(aut), si);
    std::const_pointer_cast<twa_graph>(aut)->prop_weak(res);
    return res;
  }

  bool
  is_very_weak_automaton(const const_twa_graph_ptr& aut, scc_info* si)
  {
    trival v = aut->prop_very_weak();
    if (v.is_known())
      return v.is_true();
    is_type_automaton<false, false, true>
      (std::const_pointer_cast<twa_graph>(aut), si);
    return aut->prop_very_weak().is_true();
  }

  bool
  is_inherently_weak_automaton(const const_twa_graph_ptr& aut, scc_info* si)
  {
    trival v = aut->prop_inherently_weak();
    if (v.is_known())
      return v.is_true();
    bool res = is_type_automaton<false, true>
      (std::const_pointer_cast<twa_graph>(aut), si);
    std::const_pointer_cast<twa_graph>(aut)->prop_inherently_weak(res);
    return res;
  }

  void check_strength(const twa_graph_ptr& aut, scc_info* si)
  {
    if (!aut->is_existential())
      is_type_automaton<false, false, true>(aut, si);
    else
      is_type_automaton<true, true, true>(aut, si);
  }

  bool is_safety_automaton(const const_twa_graph_ptr& aut, scc_info* si)
  {
    if (aut->acc().is_t())
      return true;

    bool need_si = !si;
    if (need_si)
      si = new scc_info(aut);

    bool res = true;
    unsigned scount = si->scc_count();
    for (unsigned scc = 0; scc < scount; ++scc)
      if (!si->is_trivial(scc) && si->is_rejecting_scc(scc))
        {
          res = false;
          break;
        }

    if (need_si)
      delete si;
    return res;
  }


  twa_graph_ptr
  decompose_scc(scc_info& si, const char* keep_opt)
  {
    if (keep_opt == nullptr || *keep_opt == 0)
      throw std::runtime_error
        ("option for decompose_scc() should not be empty");

    enum strength {
      Ignore = 0,
      Terminal = 1,
      WeakStrict = 2,
      Weak = Terminal | WeakStrict,
      Strong = 4,
      Needed = 8,  // Needed SCCs are those that lead to the SCCs we
                   // want to keep, and also unaccepting SCC we were
                   // asked to keep.
    };

    auto aut = si.get_aut();
    si.determine_unknown_acceptance();
    unsigned n = si.scc_count();
    std::vector<unsigned char> want(n, Ignore);

    unsigned char keep = Ignore;
    while (auto c = *keep_opt++)
      switch (c)
        {
        case ',':
        case ' ':
          break;
        case '0':                 // SCC number N.
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          {
            char* endptr;
            long int scc = strtol(keep_opt - 1, &endptr, 10);
            if ((long unsigned) scc >= n)
              {
                throw std::runtime_error
                  ("decompose_scc(): there is no SCC "s
                   + std::to_string(scc) + " in this automaton");
              }
            keep_opt = endptr;
            want[scc] = Needed;
            break;
          }
        case 'a':               // Accepting SCC number N.
          {
            char* endptr;
            long int scc = strtol(keep_opt, &endptr, 10);
            if (endptr == keep_opt)
              throw std::runtime_error
                ("decompose_scc(): 'a' should be followed by an SCC number");
            int j = 0;
            for (unsigned s = 0; s < n; ++s)
              if (si.is_accepting_scc(s))
                if (j++ == scc)
                  {
                    want[s] = Needed;
                    break;
                  }
            if (j != scc + 1)
              {
                throw std::runtime_error
                  ("decompose_scc(): there is no SCC 'a"s
                   + std::to_string(scc) + "' in this automaton");
              }
            keep_opt = endptr;
            break;
          }
        case 's':
          keep |= Strong;
          break;
        case 't':
          keep |= Terminal;
          break;
        case 'w':
          keep |= WeakStrict;
          break;
        default:
          throw std::runtime_error
            ("unknown option for decompose_scc(): "s + c);
        }

    auto p = aut->acc().unsat_mark();
    bool all_accepting = !p.first;
    acc_cond::mark_t wacc = {};        // acceptance for weak SCCs
    acc_cond::mark_t uacc = p.second; // Acceptance for "needed" SCCs, that
                                      // we only want to traverse.

    // If the acceptance condition is always satisfiable, we will
    // consider the automaton as weak (even if that is not the
    // case syntactically) and not output any strong part.
    if (all_accepting)
      keep &= ~Strong;

    bool nonempty = false;
    bool kept_scc_are_weak = true;
    bool kept_scc_are_terminal = true;

    for (unsigned i = 0; i < n; ++i) // SCC are topologically ordered
      {
        if (si.is_accepting_scc(i))
          {
            strength scc_strength = Ignore;
            if (all_accepting | is_inherently_weak_scc(si, i))
              scc_strength = is_complete_scc(si, i) ? Terminal : WeakStrict;
            else
              scc_strength = Strong;

            if (want[i] == Needed)
              want[i] = scc_strength;
            else
              want[i] = scc_strength & keep;

            if (want[i])
              {
                if (!(scc_strength & Weak))
                  kept_scc_are_weak = false;
                if (!(scc_strength & Terminal))
                  kept_scc_are_terminal = false;
              }
          }

        nonempty |= want[i];    // also works "Needed" rejecting SCCs

        // An SCC is needed if one of its successor is.
        for (unsigned j: si.succ(i))
          if (want[j])
            {
              want[i] |= Needed;
              break;
            }
      }

    if (!nonempty)
      return nullptr;

    twa_graph_ptr res = make_twa_graph(aut->get_dict());
    res->copy_ap_of(aut);
    res->prop_copy(aut, { true, false, false, true, false, false });

    if (kept_scc_are_weak)
      wacc = res->set_buchi();
    else
      res->copy_acceptance_of(aut);

    auto fun = [&si, &want, uacc, wacc, kept_scc_are_weak]
      (unsigned src, bdd& cond, acc_cond::mark_t& acc, unsigned dst)
      {
        if (want[si.scc_of(dst)] == Ignore)
          {
            cond = bddfalse;
            return;
          }
        if (want[si.scc_of(src)] == Needed)
          {
            acc = uacc;
            return;
          }
        if (kept_scc_are_weak)
          acc = wacc;
      };

    transform_accessible(aut, res, fun);

    res->prop_weak(kept_scc_are_weak);
    res->prop_terminal(kept_scc_are_terminal);
    return res;
  }


  twa_graph_ptr
  decompose_scc(const const_twa_graph_ptr& aut, const char* keep_opt)
  {
    scc_info si(aut);
    return decompose_scc(si, keep_opt);
  }

    twa_graph_ptr
  decompose_strength(const const_twa_graph_ptr& aut, const char* keep_opt)
  {
    return decompose_scc(aut, keep_opt);
  }

  twa_graph_ptr
  decompose_scc(scc_info& sm, unsigned scc_num, bool accepting)
  {
    std::string num = std::to_string(scc_num);
    return decompose_scc(sm, (accepting ? ('a' + num) : num).c_str());
  }

  bool
  is_liveness_automaton(const const_twa_graph_ptr& aut)
  {
    auto mon = minimize_monitor(scc_filter_states(aut));
    return mon->num_states() == 1 && is_complete(mon);
  }


}
