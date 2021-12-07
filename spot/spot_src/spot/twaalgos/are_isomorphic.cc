// -*- coding: utf-8 -*-
// Copyright (C) 2014-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/are_isomorphic.hh>
#include <spot/twaalgos/canonicalize.hh>
#include <spot/twaalgos/isdet.hh>
#include <vector>
#include <queue>

namespace
{
  typedef spot::twa_graph::graph_t::edge_storage_t tr_t;
  static bool
  tr_t_less_than(const tr_t& t1, const tr_t& t2)
  {
    return t1.cond.id() < t2.cond.id();
  }

  static bool
  operator!=(const tr_t& t1, const tr_t& t2)
  {
    return t1.cond.id() != t2.cond.id();
  }

  static bool
  are_isomorphic_det(const spot::const_twa_graph_ptr aut1,
                     const spot::const_twa_graph_ptr aut2)
  {
    typedef std::pair<unsigned, unsigned> state_pair_t;
    std::queue<state_pair_t> workqueue;
    workqueue.emplace(aut1->get_init_state_number(),
                      aut2->get_init_state_number());
    std::vector<unsigned> map(aut1->num_states(), -1U);
    map[aut1->get_init_state_number()] = aut2->get_init_state_number();
    std::vector<tr_t> trans1;
    std::vector<tr_t> trans2;
    state_pair_t current_state;
    while (!workqueue.empty())
      {
        current_state = workqueue.front();
        workqueue.pop();

        for (auto& t : aut1->out(current_state.first))
          trans1.emplace_back(t);
        for (auto& t : aut2->out(current_state.second))
          trans2.emplace_back(t);

        if (trans1.size() != trans2.size())
          return false;

        std::sort(trans1.begin(), trans1.end(), tr_t_less_than);
        std::sort(trans2.begin(), trans2.end(), tr_t_less_than);

        for (auto t1 = trans1.begin(), t2 = trans2.begin();
             t1 != trans1.end() && t2 != trans2.end();
             ++t1, ++t2)
          {
            if (*t1 != *t2)
              {
                return false;
              }
            if (map[t1->dst] == -1U)
              {
                map[t1->dst] = t2->dst;
                workqueue.emplace(t1->dst, t2->dst);
              }
            else if (map[t1->dst] != t2->dst)
              {
                return false;
              }
          }

        trans1.clear();
        trans2.clear();
      }
    return true;
  }

  bool
  trivially_different(const spot::const_twa_graph_ptr aut1,
                      const spot::const_twa_graph_ptr aut2)
  {
    return aut1->num_states() != aut2->num_states() ||
      aut1->num_edges() != aut2->num_edges() ||
      // FIXME: At some point, it would be nice to support reordering
      // of acceptance sets (issue #58).
      aut1->acc().get_acceptance() != aut2->acc().get_acceptance();
  }
}

namespace spot
{
  isomorphism_checker::isomorphism_checker(const const_twa_graph_ptr ref)
  {
    ref_ = make_twa_graph(ref, twa::prop_set::all());
    trival prop_det = ref_->prop_universal();
    if (prop_det)
      {
        ref_deterministic_ = true;
      }
    else
      {
        // Count the number of state even if we know that the
        // automaton is non-deterministic, as this can be used to
        // decide if two automata are non-isomorphic.
        nondet_states_ = spot::count_nondet_states(ref_);
        ref_deterministic_ = (nondet_states_ == 0);
      }
    canonicalize(ref_);
  }

  bool
  isomorphism_checker::is_isomorphic_(const const_twa_graph_ptr aut)
  {
    if (!aut->is_existential())
      throw std::runtime_error
        ("isomorphism_checker does not yet support alternation");
    trival autdet = aut->prop_universal();
    if (ref_deterministic_)
      {
        if (!spot::is_universal(aut))
          return false;
        return are_isomorphic_det(ref_, aut);
      }
    if (autdet || nondet_states_ != spot::count_nondet_states(aut))
      return false;

    auto tmp = make_twa_graph(aut, twa::prop_set::all());
    spot::canonicalize(tmp);
    return *tmp == *ref_;
  }

  bool
  isomorphism_checker::is_isomorphic(const const_twa_graph_ptr aut)
  {
    if (trivially_different(ref_, aut))
      return false;
    return is_isomorphic_(aut);
  }

  bool
  isomorphism_checker::are_isomorphic(const const_twa_graph_ptr ref,
                                      const const_twa_graph_ptr aut)
  {
    if (trivially_different(ref, aut))
      return false;
    isomorphism_checker c(ref);
    return c.is_isomorphic_(aut);
  }
}
