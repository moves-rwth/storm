// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018 Laboratoire de Recherche et Développement
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
#include <vector>
#include <map>
#include <utility>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/stripacc.hh>

namespace spot
{
  twa_graph_ptr sbacc(twa_graph_ptr old)
  {
    if (old->prop_state_acc())
      return old;

    // We will need a mark that is rejecting to mark rejecting states.
    // If no such mark exist, our work is actually quite simple: we
    // just have to copy the automaton and give it "t" as acceptance
    // condition.
    auto unsat_mark = old->acc().unsat_mark();
    if (!unsat_mark.first)
      {
        auto res = make_twa_graph(old, twa::prop_set::all());
        strip_acceptance_here(res);
        return res;
      }

    scc_info si(old, scc_info_options::NONE);

    unsigned ns = old->num_states();
    acc_cond::mark_t all = old->acc().all_sets();
    // Marks that are common to all ingoing or outgoing transitions.
    std::vector<acc_cond::mark_t> common_in(ns, all);
    std::vector<acc_cond::mark_t> common_out(ns, all);
    // Marks that label one incoming transition from the same SCC.
    std::vector<acc_cond::mark_t> one_in(ns, acc_cond::mark_t({}));
    std::vector<bool> true_state(ns, false);
    acc_cond::mark_t true_state_acc = {};
    unsigned true_state_last;
    for (auto& e: old->edges())
      for (unsigned d: old->univ_dests(e.dst))
        if (si.scc_of(e.src) == si.scc_of(d))
          {
            common_in[d] &= e.acc;
            common_out[e.src] &= e.acc;
            if (e.src == e.dst && e.cond == bddtrue
                && old->acc().accepting(e.acc))
              {
                true_state[d] = true;
                true_state_acc = e.acc;
                true_state_last = e.src;
              }
          }
    for (unsigned s = 0; s < ns; ++s)
      common_out[s] |= common_in[s];
    for (auto& e: old->edges())
      for (unsigned d: old->univ_dests(e.dst))
        if (si.scc_of(e.src) == si.scc_of(d))
          one_in[d] = e.acc - common_out[e.src];

    auto res = make_twa_graph(old->get_dict());
    res->copy_ap_of(old);
    res->copy_acceptance_of(old);
    res->prop_copy(old, {false, true, true, true, true, true});
    res->prop_state_acc(true);

    typedef std::pair<unsigned, acc_cond::mark_t> pair_t;
    std::map<pair_t, unsigned> s2n;

    std::vector<std::pair<pair_t, unsigned>> todo;

    auto new_state =
      [&](unsigned state, acc_cond::mark_t m) -> unsigned
      {
        bool ts = true_state[state];
        if (ts)
          {
            state = true_state_last; // Merge all true states.
            m = {};
          }
        pair_t x(state, m);
        auto p = s2n.emplace(x, 0);
        if (p.second)                // This is a new state
          {
            unsigned s = res->new_state();
            p.first->second = s;
            if (ts)
              {
                res->new_edge(s, s, bddtrue, true_state_acc);
                // As we do not process all outgoing transition of
                // STATE, it is possible that a non-deterministic
                // automaton becomes deterministic.
                if (res->prop_universal().is_false())
                  res->prop_universal(trival::maybe());
              }
            else
              todo.emplace_back(x, s);
          }
        return p.first->second;
      };

    std::vector<unsigned> old_init;
    for (unsigned d: old->univ_dests(old->get_init_state_number()))
      old_init.push_back(d);

    std::vector<unsigned> old_st;
    internal::univ_dest_mapper<twa_graph::graph_t> uniq(res->get_graph());
    for (unsigned s: old_init)
      {
        acc_cond::mark_t init_acc = {};
        if (!si.is_rejecting_scc(si.scc_of(s)))
          // Use any edge going into the initial state to set the first
          // acceptance mark.
          init_acc = one_in[s] | common_out[s];

        old_st.push_back(new_state(s, init_acc));
      }
    res->set_init_state(uniq.new_univ_dests(old_st.begin(), old_st.end()));

    while (!todo.empty())
      {
        auto one = todo.back();
        todo.pop_back();
        unsigned scc_src = si.scc_of(one.first.first);
        bool maybe_accepting = !si.is_rejecting_scc(scc_src);
        for (auto& t: old->out(one.first.first))
          {
            std::vector<unsigned> dests;
            for (unsigned d: old->univ_dests(t.dst))
              {
                unsigned scc_dst = si.scc_of(d);
                acc_cond::mark_t acc = {};
                bool dst_acc = !si.is_rejecting_scc(scc_dst);
                if (maybe_accepting && scc_src == scc_dst)
                  acc = t.acc - common_out[t.src];
                else if (dst_acc)
                  // We enter a new accepting SCC. Use any edge going into
                  // t.dst from this SCC to set the initial acceptance mark.
                  acc = one_in[d];
                if (dst_acc)
                  acc |= common_out[d];
                else
                  acc = unsat_mark.second;

                dests.push_back(new_state(d, acc));
              }
            res->new_edge(one.second,
                          uniq.new_univ_dests(dests.begin(), dests.end()),
                          t.cond, one.first.second);
          }
      }
    res->merge_edges();

    // If the automaton was marked as not complete or not universal,
    // and we have ignored some unreachable state, then it is possible
    // that the result becomes complete or universal.
    if (res->prop_complete().is_false() || res->prop_universal().is_false())
      for (unsigned i = 0; i < ns; ++i)
        if (!si.reachable_state(i))
          {
            if (res->prop_complete().is_false())
              res->prop_complete(trival::maybe());
            if (res->prop_universal().is_false())
              res->prop_universal(trival::maybe());
            break;
          }
    return res;
  }
}
