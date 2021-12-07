// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/sum.hh>
#include <spot/twa/twagraph.hh>
#include <vector>

namespace spot
{
  namespace
  {
    // Helper function that connects a new initial state to the automaton,
    // in replacement of graph_init
    static
    void connect_init_state(const twa_graph_ptr& res,
                            unsigned init,
                            const const_twa_graph_ptr& left,
                            const const_twa_graph_ptr& right,
                            unsigned left_init,
                            unsigned right_init,
                            unsigned offset)
    {
      bdd comb_left = bddtrue;
      outedge_combiner oe(res);
      for (unsigned c : left->univ_dests(left_init))
        comb_left &= oe(c);

      bdd comb_right = bddtrue;
      for (unsigned c : right->univ_dests(right_init))
        comb_right &= oe(c + offset);

      oe.new_dests(init, comb_left | comb_right);
    }

    // Helper function that copies the states of graph into res, adding
    // offset to the state ID, mark to the current mark of the edges,
    // and setting the number of states to num_sets
    static
    void copy_union(const twa_graph_ptr& res,
                    const const_twa_graph_ptr& graph,
                    acc_cond::mark_t mark = acc_cond::mark_t({}),
                    unsigned offset = 0U)
    {
      unsigned state_offset = res->num_states();

      res->new_states(graph->num_states());

      std::vector<unsigned> dst;
      for (auto& e : graph->edges())
        {
          for (unsigned d : graph->univ_dests(e))
            dst.push_back(d + state_offset);

          res->new_univ_edge(e.src + state_offset,
                             dst.begin(), dst.end(),
                             e.cond,
                             (e.acc << offset)| mark);
          dst.clear();
        }
    }

    // Helper function that perform the sum of the automaton in left and the
    // automaton in right, using is_sum true for sum_or and is_sum false
    // as sum_and
    static
    twa_graph_ptr sum_aux(const const_twa_graph_ptr& left,
                          const const_twa_graph_ptr& right,
                          unsigned left_state,
                          unsigned right_state,
                          bool is_sum)
    {
      if (left->get_dict() != right->get_dict())
        throw std::runtime_error("sum: left and right automata should "
                                 "share their bdd_dict");
      auto res = make_twa_graph(left->get_dict());
      res->copy_ap_of(left);
      res->copy_ap_of(right);

      auto unsatl = left->acc().unsat_mark();
      acc_cond::mark_t markl = {};
      acc_cond::mark_t markr = {};
      auto left_acc = left->get_acceptance();
      unsigned left_num_sets = left->num_sets();
      if (!unsatl.first)
        {
          markl.set(0);
          left_num_sets = 1;
          left_acc = acc_cond::acc_code::buchi();
        }
      else
        {
          markr |= unsatl.second;
        }

      auto unsatr = right->acc().unsat_mark();
      auto right_acc = right->get_acceptance();
      unsigned right_num_sets = right->num_sets();
      if (!unsatr.first)
        {
          markr.set(left_num_sets);
          right_num_sets = 1;
          right_acc = acc_cond::acc_code::buchi();
        }
      else
        {
          markl |= (unsatr.second << left_num_sets);
        }
      res->set_acceptance(left_num_sets + right_num_sets,
                         (right_acc << left_num_sets) |= left_acc);
      copy_union(res, left, markl);
      copy_union(res, right, markr, left_num_sets);

      if (is_sum)
        {
          unsigned init = res->new_state();
          res->set_init_state(init);

          connect_init_state(res,
                             init,
                             left,
                             right,
                             left_state,
                             right_state,
                             left->num_states());
        }
      else
        {
          std::vector<unsigned> stl;
          for (unsigned d : left->univ_dests(left_state))
            stl.push_back(d);

          for (unsigned d : right->univ_dests(right_state))
            stl.push_back(d + left->num_states());

          res->set_univ_init_state(stl.begin(), stl.end());
        }
      return res;
   }


 }
  twa_graph_ptr sum(const const_twa_graph_ptr& left,
                    const const_twa_graph_ptr& right,
                    unsigned left_state,
                    unsigned right_state)
  {
    return sum_aux(left, right, left_state, right_state, true);
  }

  twa_graph_ptr sum(const const_twa_graph_ptr& left,
                    const const_twa_graph_ptr& right)
  {
    return  sum(left, right,
                left->get_init_state_number(),
                right->get_init_state_number());
  }

  twa_graph_ptr sum_and(const const_twa_graph_ptr& left,
                    const const_twa_graph_ptr& right,
                    unsigned left_state,
                    unsigned right_state)
  {
    return sum_aux(left, right, left_state, right_state, false);
  }
  twa_graph_ptr sum_and(const const_twa_graph_ptr& left,
                        const const_twa_graph_ptr& right)
  {
    return sum_and(left,
                   right,
                   left->get_init_state_number(),
                   right->get_init_state_number());
  }

}
