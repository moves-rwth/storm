// -*- coding: utf-8 -*-
// Copyright (C) 2012-2018 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  namespace
  {
    template<bool count>
    static
    unsigned
    count_nondet_states_aux(const const_twa_graph_ptr& aut)
    {
      unsigned nondet_states = 0;
      unsigned ns = aut->num_states();
      for (unsigned src = 0; src < ns; ++src)
        {
          bdd available = bddtrue;
          for (auto& t: aut->out(src))
            if (!bdd_implies(t.cond, available))
              {
                ++nondet_states;
                break;
              }
            else
              {
                available -= t.cond;
              }
          // If we are not counting non-deterministic states, abort as
          // soon as possible.
          if (!count && nondet_states)
            break;
        }
      std::const_pointer_cast<twa_graph>(aut)
        ->prop_universal(!nondet_states);
      return nondet_states;
    }
  }

  unsigned
  count_nondet_states(const const_twa_graph_ptr& aut)
  {
    if (aut->prop_universal())
      return 0;
    return count_nondet_states_aux<true>(aut);
  }

  bool
  is_universal(const const_twa_graph_ptr& aut)
  {
    trival d = aut->prop_universal();
    if (d.is_known())
      return d.is_true();
    return !count_nondet_states_aux<false>(aut);
  }

  bool
  is_deterministic(const const_twa_graph_ptr& aut)
  {
    return aut->is_existential() && is_universal(aut);
  }

  void
  highlight_nondet_states(twa_graph_ptr& aut, unsigned color)
  {
    if (aut->prop_universal())
      return;
    unsigned ns = aut->num_states();
    auto* highlight = aut->get_or_set_named_prop<std::map<unsigned, unsigned>>
      ("highlight-states");
    bool universal = true;
    for (unsigned src = 0; src < ns; ++src)
      {
        bdd available = bddtrue;
        for (auto& t: aut->out(src))
          if (!bdd_implies(t.cond, available))
            {
              (*highlight)[src] = color;
              universal = false;
            }
          else
            {
              available -= t.cond;
            }
      }
    aut->prop_universal(universal);
  }

  void
  highlight_nondet_edges(twa_graph_ptr& aut, unsigned color)
  {
    if (aut->prop_universal())
      return;
    unsigned ns = aut->num_states();
    auto* highlight = aut->get_or_set_named_prop<std::map<unsigned, unsigned>>
      ("highlight-edges");
    bool universal = true;
    for (unsigned src = 0; src < ns; ++src)
      {
        // Make a first pass to gather non-deterministic labels
        bdd available = bddtrue;
        bdd extra = bddfalse;
        for (auto& t: aut->out(src))
          if (!bdd_implies(t.cond, available))
            {
              extra |= (t.cond - available);
              universal = false;
            }
          else
            {
              available -= t.cond;
            }
        // Second pass to gather the relevant edges.
        if (!universal)
          for (auto& t: aut->out(src))
            if ((t.cond & extra) != bddfalse)
              (*highlight)[aut->get_graph().index_of_edge(t)] = color;
      }
    aut->prop_universal(universal);
  }

  void highlight_semidet_sccs(scc_info& si, unsigned color)
  {
    auto det_sccs = semidet_sccs(si);
    if (det_sccs.empty())
      return;
    auto aut = si.get_aut();
    auto* highlight = std::const_pointer_cast<twa_graph>(aut)
      ->get_or_set_named_prop<std::map<unsigned, unsigned>>("highlight-states");
    for (unsigned scc = 0; scc < si.scc_count(); scc++)
      {
        if (det_sccs[scc])
          {
            for (auto& t : si.states_of(scc))
              (*highlight)[t] = color;
          }
      }
  }

  bool
  is_complete(const const_twa_graph_ptr& aut)
  {
    trival cp = aut->prop_complete();
    if (cp.is_known())
      return cp.is_true();
    unsigned ns = aut->num_states();
    for (unsigned src = 0; src < ns; ++src)
      {
        bdd available = bddtrue;
        for (auto& t: aut->out(src))
          available -= t.cond;
        if (available != bddfalse)
          {
            std::const_pointer_cast<twa_graph>(aut)->prop_complete(false);
            return false;
          }
      }
    // The empty automaton is not complete since it does not have an
    // initial state.
    bool res = ns > 0;
    std::const_pointer_cast<twa_graph>(aut)->prop_complete(res);
    return res;
  }

  namespace
  {
    static bool
    check_semi_determism(const const_twa_graph_ptr& aut, bool and_determinism)
    {
      trival sd = aut->prop_semi_deterministic();
      if (sd.is_known() &&
          (!and_determinism || aut->prop_universal().is_known()))
        return sd.is_true();
      scc_info si(aut);
      si.determine_unknown_acceptance();
      unsigned nscc = si.scc_count();
      assert(nscc);
      std::vector<bool> reachable_from_acc(nscc);
      bool semi_det = true;
      do // iterator of SCCs in reverse topological order
        {
          --nscc;
          if (si.is_accepting_scc(nscc) || reachable_from_acc[nscc])
            {
              for (unsigned succ: si.succ(nscc))
                reachable_from_acc[succ] = true;
              for (unsigned src: si.states_of(nscc))
                {
                  bdd available = bddtrue;
                  for (auto& t: aut->out(src))
                    if (!bdd_implies(t.cond, available))
                      {
                        semi_det = false;
                        goto done;
                      }
                    else
                      {
                        available -= t.cond;
                      }
                }
            }
        }
      while (nscc);
    done:
      std::const_pointer_cast<twa_graph>(aut)
        ->prop_semi_deterministic(semi_det);
      if (semi_det && and_determinism)
        {
          bool det = true;
          nscc = si.scc_count();
          do // iterator of SCCs in reverse topological order
            {
              --nscc;
              if (!si.is_accepting_scc(nscc) && !reachable_from_acc[nscc])
                {
                  for (unsigned src: si.states_of(nscc))
                    {
                      bdd available = bddtrue;
                      for (auto& t: aut->out(src))
                        if (!bdd_implies(t.cond, available))
                          {
                            det = false;
                            goto done2;
                          }
                        else
                          {
                            available -= t.cond;
                          }
                    }
                }
            }
          while (nscc);
        done2:
          std::const_pointer_cast<twa_graph>(aut)->prop_universal(det);
        }
      return semi_det;
    }
  }

  std::vector<bool> semidet_sccs(scc_info& si)
  {
    const_twa_graph_ptr aut = si.get_aut();
    trival sd = aut->prop_semi_deterministic();
    if (sd.is_known() && sd.is_false())
      return std::vector<bool>();
    si.determine_unknown_acceptance();
    unsigned nscc = si.scc_count();
    assert(nscc);
    std::vector<bool> reachable_from_acc(nscc);
    std::vector<bool> res(nscc);
    bool semi_det = true;
    do // iterator of SCCs in reverse topological order
      {
        --nscc;
        if (si.is_accepting_scc(nscc) || reachable_from_acc[nscc])
          {
            for (unsigned succ: si.succ(nscc))
              reachable_from_acc[succ] = true;
            for (unsigned src: si.states_of(nscc))
              {
                bdd available = bddtrue;
                for (auto& t: aut->out(src))
                  if (!bdd_implies(t.cond, available))
                    {
                      semi_det = false;
                      goto done;
                    }
                  else
                    {
                      available -= t.cond;
                    }
              }
            res[nscc] = true;
          }
      }
    while (nscc);
  done:
    if (!semi_det)
      return std::vector<bool>();
    return res;
  }

  bool
  is_semi_deterministic(const const_twa_graph_ptr& aut)
  {
    return check_semi_determism(aut, false);
  }

  void check_determinism(twa_graph_ptr aut)
  {
    check_semi_determism(aut, true);
  }

  unsigned
  count_univbranch_states(const const_twa_graph_ptr& aut)
  {
    if (aut->is_existential())
      return 0;
    unsigned res = 0;
    unsigned ns = aut->num_states();
    for (unsigned s = 0; s < ns; ++s)
      for (auto& e: aut->out(s))
        if (aut->is_univ_dest(e))
          {
            ++res;
            break;
          }
    return res;
  }

  unsigned
  count_univbranch_edges(const const_twa_graph_ptr& aut)
  {
    if (aut->is_existential())
      return 0;
    unsigned res = aut->is_univ_dest(aut->get_init_state_number());
    for (auto& e: aut->edges())
      if (aut->is_univ_dest(e))
        ++res;
    return res;
  }

}
