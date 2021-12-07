// -*- coding: utf-8 -*-
// Copyright (C) 2010-2016, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

//#define TRACE

#include "config.h"
#include <iostream>
#ifdef TRACE
#define trace std::clog
#else
#define trace while (0) std::clog
#endif

#include <spot/taalgos/emptinessta.hh>
#include <spot/misc/memusage.hh>
#include <cstdlib>
#include <spot/twa/bddprint.hh>

namespace spot
{

  ta_check::ta_check(const const_ta_product_ptr& a, option_map o) :
    a_(a), o_(o)
  {
    is_full_2_pass_ = o.get("is_full_2_pass", 0);
  }

  ta_check::~ta_check()
  {
  }

  bool
  ta_check::check(bool disable_second_pass,
      bool disable_heuristic_for_livelock_detection)
  {

    // We use five main data in this algorithm:

    // * scc: (attribute) a stack of strongly connected components (SCC)

    // * arc, a stack of acceptance conditions between each of these SCC,
    std::stack<acc_cond::mark_t> arc;

    // * h: a hash of all visited nodes, with their order,
    //   (it is called "Hash" in Couvreur's paper)
    hash_type h;

    // * num: the number of visited nodes.  Used to set the order of each
    //   visited node,
    int num = 1;

    // * todo: the depth-first search stack.  This holds pairs of the
    //   form (STATE, ITERATOR) where ITERATOR is a ta_succ_iterator_product
    //   over the successors of STATE.  In our use, ITERATOR should
    //   always be freed when TODO is popped, but STATE should not because
    //   it is also used as a key in H.
    std::stack<pair_state_iter> todo;

    trace
      << "PASS 1" << std::endl;

    state_map<std::set<const state*, state_ptr_less_than>> liveset;

    std::stack<spot::state*> livelock_roots;

    bool livelock_acceptance_states_not_found = true;

    bool activate_heuristic = !disable_heuristic_for_livelock_detection
        && (is_full_2_pass_ == disable_second_pass);

    // Setup depth-first search from initial states.
    auto& ta_ = a_->get_ta();
    auto& kripke_ = a_->get_kripke();
    auto kripke_init_state = kripke_->get_init_state();
    bdd kripke_init_state_condition = kripke_->state_condition(
        kripke_init_state);

    auto artificial_initial_state = ta_->get_artificial_initial_state();

    ta_succ_iterator* ta_init_it_ = ta_->succ_iter(artificial_initial_state,
        kripke_init_state_condition);
    kripke_init_state->destroy();
    for (ta_init_it_->first(); !ta_init_it_->done(); ta_init_it_->next())
      {

        state_ta_product* init = new state_ta_product(
            (ta_init_it_->dst()), kripke_init_state->clone());

        if (!h.emplace(init, num + 1).second)
          {
            init->destroy();
            continue;
          }

        scc.push(++num);
        arc.push({});

        ta_succ_iterator_product* iter = a_->succ_iter(init);
        iter->first();
        todo.emplace(init, iter);

        inc_depth();

        //push potential root of live-lock accepting cycle
        if (activate_heuristic && a_->is_livelock_accepting_state(init))
          livelock_roots.push(init);

        while (!todo.empty())
          {
            auto curr = todo.top().first;

            // We are looking at the next successor in SUCC.
            ta_succ_iterator_product* succ = todo.top().second;

            // If there is no more successor, backtrack.
            if (succ->done())
              {
                // We have explored all successors of state CURR.


                // Backtrack TODO.
                todo.pop();
                dec_depth();
                trace << "PASS 1 : backtrack\n";

                if (a_->is_livelock_accepting_state(curr)
                    && !a_->is_accepting_state(curr))
                  {
                    livelock_acceptance_states_not_found = false;
                    trace << "PASS 1 : livelock accepting state found\n";
                  }

                // fill rem with any component removed,
                auto i = h.find(curr);
                assert(i != h.end());

                scc.rem().push_front(curr);
                inc_depth();

                // set the h value of the Backtracked state to negative value.
                i->second = -std::abs(i->second);

                // Backtrack livelock_roots.
                if (activate_heuristic && !livelock_roots.empty()
                    && !livelock_roots.top()->compare(curr))
                  livelock_roots.pop();

                // When backtracking the root of an SSCC, we must also
                // remove that SSCC from the ROOT stacks.  We must
                // discard from H all reachable states from this SSCC.
                assert(!scc.empty());
                if (scc.top().index == std::abs(i->second))
                  {
                    // removing states
                    for (auto j: scc.rem())
                      h[j] = -1;
                    dec_depth(scc.rem().size());
                    scc.pop();
                    assert(!arc.empty());
                    arc.pop();

                  }

                delete succ;
                // Do not delete CURR: it is a key in H.
                continue;
              }

            // We have a successor to look at.
            inc_transitions();
            trace << "PASS 1: transition\n";
            // Fetch the values destination state we are interested in...
            state* dest = succ->dst();

            auto acc_cond = succ->acc();

            bool curr_is_livelock_hole_state_in_ta_component =
                (a_->is_hole_state_in_ta_component(curr))
                    && a_->is_livelock_accepting_state(curr);

            // May be Buchi accepting scc or livelock accepting scc
            // (contains a livelock accepting state that have no
            // successors in TA).
            scc.top().is_accepting = (a_->is_accepting_state(curr)
                && (!succ->is_stuttering_transition()
                    || a_->is_livelock_accepting_state(curr)))
                || curr_is_livelock_hole_state_in_ta_component;

            bool is_stuttering_transition = succ->is_stuttering_transition();

            // ... and point the iterator to the next successor, for
            // the next iteration.
            succ->next();
            // We do not need SUCC from now on.

            // Are we going to a new state?
            auto p = h.emplace(dest, num + 1);
            if (p.second)
              {
                // Number it, stack it, and register its successors
                // for later processing.
                scc.push(++num);
                arc.push(acc_cond);

                ta_succ_iterator_product* iter = a_->succ_iter(dest);
                iter->first();
                todo.emplace(dest, iter);
                inc_depth();

                //push potential root of live-lock accepting cycle
                if (activate_heuristic && a_->is_livelock_accepting_state(dest)
                    && !is_stuttering_transition)
                  livelock_roots.push(dest);

                continue;
              }

            // If we have reached a dead component, ignore it.
            if (p.first->second == -1)
              continue;

            // Now this is the most interesting case.  We have reached a
            // state S1 which is already part of a non-dead SSCC.  Any such
            // non-dead SSCC has necessarily been crossed by our path to
            // this state: there is a state S2 in our path which belongs
            // to this SSCC too.  We are going to merge all states between
            // this S1 and S2 into this SSCC.
            //
            // This merge is easy to do because the order of the SSCC in
            // ROOT is ascending: we just have to merge all SSCCs from the
            // top of ROOT that have an index greater to the one of
            // the SSCC of S2 (called the "threshold").
            int threshold = std::abs(p.first->second);
            std::list<const state*> rem;
            bool acc = false;

            trace << "***PASS 1: CYCLE***\n";

            while (threshold < scc.top().index)
              {
                assert(!scc.empty());
                assert(!arc.empty());

                acc |= scc.top().is_accepting;
                acc_cond |= scc.top().condition;
                acc_cond |= arc.top();

                rem.splice(rem.end(), scc.rem());
                scc.pop();
                arc.pop();

              }

            // Note that we do not always have
            //  threshold == scc.top().index
            // after this loop, the SSCC whose index is threshold might have
            // been merged with a lower SSCC.

            // Accumulate all acceptance conditions into the merged SSCC.
            scc.top().is_accepting |= acc;
            scc.top().condition |= acc_cond;

            scc.rem().splice(scc.rem().end(), rem);
            bool is_accepting_sscc = scc.top().is_accepting
              || a_->acc().accepting(scc.top().condition);

            if (is_accepting_sscc)
              {
                trace
                  << "PASS 1: SUCCESS: a_->is_livelock_accepting_state(curr): "
                  << a_->is_livelock_accepting_state(curr) << '\n';
                trace
                  << "PASS 1: scc.top().condition : "
                  << scc.top().condition << '\n';
                trace
                  << "PASS 1: a_->acc().all_sets() : "
                  << (a_->acc().all_sets()) << '\n';
                trace
                  << ("PASS 1 CYCLE and accepting? ")
                  << a_->acc().accepting(scc.top().condition)
                  << std::endl;
                clear(h, todo, ta_init_it_);
                return true;
              }

            //ADDLINKS
            if (activate_heuristic && a_->is_livelock_accepting_state(curr)
                && is_stuttering_transition)
              {
                trace << "PASS 1: heuristic livelock detection \n";
                const state* dest = p.first->first;
                std::set<const state*, state_ptr_less_than> liveset_dest =
                    liveset[dest];

                std::set<const state*, state_ptr_less_than> liveset_curr =
                    liveset[curr];

                int h_livelock_root = 0;
                if (!livelock_roots.empty())
                  h_livelock_root = h[livelock_roots.top()];

                if (heuristic_livelock_detection(dest, h, h_livelock_root,
                                                 liveset_curr))
                  {
                    clear(h, todo, ta_init_it_);
                    return true;
                  }

                for (const state* succ: liveset_dest)
                  if (heuristic_livelock_detection(succ, h, h_livelock_root,
                                                   liveset_curr))
                    {
                      clear(h, todo, ta_init_it_);
                      return true;
                    }
              }
          }

      }

    clear(h, todo, ta_init_it_);

    if (disable_second_pass || livelock_acceptance_states_not_found)
      return false;

    return livelock_detection(a_);
  }

  bool
  ta_check::heuristic_livelock_detection(const state * u,
      hash_type& h, int h_livelock_root, std::set<const state*,
          state_ptr_less_than> liveset_curr)
  {
    int hu = h[u];

    if (hu > 0)
      {

        if (hu >= h_livelock_root)
          {
            trace << "PASS 1: heuristic livelock detection SUCCESS\n";
            return true;
          }

        liveset_curr.insert(u);
      }
    return false;
  }

  bool
  ta_check::livelock_detection(const const_ta_product_ptr& t)
  {
    // We use five main data in this algorithm:

    // * sscc: a stack of strongly stuttering-connected components (SSCC)


    // * h: a hash of all visited nodes, with their order,
    //   (it is called "Hash" in Couvreur's paper)
    hash_type h;

    // * num: the number of visited nodes.  Used to set the order of each
    //   visited node,

    trace
      << "PASS 2" << std::endl;

    int num = 0;

    // * todo: the depth-first search stack.  This holds pairs of the
    //   form (STATE, ITERATOR) where ITERATOR is a twa_succ_iterator
    //   over the successors of STATE.  In our use, ITERATOR should
    //   always be freed when TODO is popped, but STATE should not because
    //   it is also used as a key in H.
    std::stack<pair_state_iter> todo;

    // * init: the set of the depth-first search initial states
    std::queue<const spot::state*> ta_init_it_;

    auto init_states_set = a_->get_initial_states_set();
    for (auto init_state: init_states_set)
      ta_init_it_.push(init_state);

    while (!ta_init_it_.empty())
      {
        // Setup depth-first search from initial states.
          {
            auto init = ta_init_it_.front();
            ta_init_it_.pop();

            if (!h.emplace(init, num + 1).second)
              {
                init->destroy();
                continue;
              }

            sscc.push(num);
            sscc.top().is_accepting = t->is_livelock_accepting_state(init);
            ta_succ_iterator_product* iter = t->succ_iter(init);
            iter->first();
            todo.emplace(init, iter);
            inc_depth();
          }

        while (!todo.empty())
          {
            auto curr = todo.top().first;

            // We are looking at the next successor in SUCC.
            ta_succ_iterator_product* succ = todo.top().second;

            // If there is no more successor, backtrack.
            if (succ->done())
              {
                // We have explored all successors of state CURR.

                // Backtrack TODO.
                todo.pop();
                dec_depth();
                trace << "PASS 2 : backtrack\n";

                // fill rem with any component removed,
                auto i = h.find(curr);
                assert(i != h.end());

                sscc.rem().push_front(curr);
                inc_depth();

                // When backtracking the root of an SSCC, we must also
                // remove that SSCC from the ROOT stacks.  We must
                // discard from H all reachable states from this SSCC.
                assert(!sscc.empty());
                if (sscc.top().index == i->second)
                  {
                    // removing states
                    for (auto j: sscc.rem())
                      h[j] = -1;
                    dec_depth(sscc.rem().size());
                    sscc.pop();
                  }

                delete succ;
                // Do not delete CURR: it is a key in H.

                continue;
              }

            // We have a successor to look at.
            inc_transitions();
            trace << "PASS 2 : transition\n";
            // Fetch the values destination state we are interested in...
            state* dest = succ->dst();

            bool is_stuttering_transition = succ->is_stuttering_transition();
            // ... and point the iterator to the next successor, for
            // the next iteration.
            succ->next();
            // We do not need SUCC from now on.

            auto i = h.find(dest);

            // Is this a new state?
            if (i == h.end())
              {

                // Are we going to a new state through a stuttering transition?

                if (!is_stuttering_transition)
                  {
                    ta_init_it_.push(dest);
                    continue;
                  }

                // Number it, stack it, and register its successors
                // for later processing.
                h[dest] = ++num;
                sscc.push(num);
                sscc.top().is_accepting = t->is_livelock_accepting_state(dest);

                ta_succ_iterator_product* iter = t->succ_iter(dest);
                iter->first();
                todo.emplace(dest, iter);
                inc_depth();
                continue;
              }
            else
              {
                dest->destroy();
              }

            // If we have reached a dead component, ignore it.
            if (i->second == -1)
              continue;

            //self loop state
            if (!curr->compare(i->first))
              if (t->is_livelock_accepting_state(curr))
                {
                  clear(h, todo, ta_init_it_);
                  trace << "PASS 2: SUCCESS\n";
                  return true;
                }

            // Now this is the most interesting case.  We have reached a
            // state S1 which is already part of a non-dead SSCC.  Any such
            // non-dead SSCC has necessarily been crossed by our path to
            // this state: there is a state S2 in our path which belongs
            // to this SSCC too.  We are going to merge all states between
            // this S1 and S2 into this SSCC.
            //
            // This merge is easy to do because the order of the SSCC in
            // ROOT is ascending: we just have to merge all SSCCs from the
            // top of ROOT that have an index greater to the one of
            // the SSCC of S2 (called the "threshold").
            int threshold = i->second;
            std::list<const state*> rem;
            bool acc = false;

            while (threshold < sscc.top().index)
              {
                assert(!sscc.empty());

                acc |= sscc.top().is_accepting;

                rem.splice(rem.end(), sscc.rem());
                sscc.pop();

              }
            // Note that we do not always have
            //  threshold == sscc.top().index
            // after this loop, the SSCC whose index is threshold might have
            // been merged with a lower SSCC.

            // Accumulate all acceptance conditions into the merged SSCC.
            sscc.top().is_accepting |= acc;

            sscc.rem().splice(sscc.rem().end(), rem);
            if (sscc.top().is_accepting)
              {
                clear(h, todo, ta_init_it_);
                trace
                  << "PASS 2: SUCCESS" << std::endl;
                return true;
              }
          }

      }
    clear(h, todo, ta_init_it_);
    return false;
  }

  void
  ta_check::clear(hash_type& h, std::stack<pair_state_iter> todo,
      std::queue<const spot::state*> init_states)
  {

    set_states(states() + h.size());

    while (!init_states.empty())
      {
        a_->free_state(init_states.front());
        init_states.pop();
      }

    // Release all iterators in TODO.
    while (!todo.empty())
      {
        delete todo.top().second;
        todo.pop();
        dec_depth();
      }
  }

  void
  ta_check::clear(hash_type& h, std::stack<pair_state_iter> todo,
      spot::ta_succ_iterator* init_states_it)
  {

    set_states(states() + h.size());

    delete init_states_it;

    // Release all iterators in TODO.
    while (!todo.empty())
      {
        delete todo.top().second;
        todo.pop();
        dec_depth();
      }
  }

  std::ostream&
  ta_check::print_stats(std::ostream& os) const
  {
    //    ecs_->print_stats(os);
    os << states() << " unique states visited" << std::endl;

    //TODO  sscc;
    os << scc.size() << " strongly connected components in search stack"
        << std::endl;
    os << transitions() << " transitions explored" << std::endl;
    os << max_depth() << " items max in DFS search stack" << std::endl;
    return os;
  }

//////////////////////////////////////////////////////////////////////


}
