// -*- coding: utf-8 -*-
// Copyright (C) 2010-2018 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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

#include <spot/twa/formula2bdd.hh>
#include <cassert>
#include <spot/twa/bddprint.hh>
#include <stack>
#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/statessetbuilder.hh>
#include <spot/ta/tgtaexplicit.hh>

using namespace std;

namespace spot
{

  namespace
  {
    typedef std::pair<const spot::state*, twa_succ_iterator*> pair_state_iter;

    static void
    transform_to_single_pass_automaton
    (const ta_explicit_ptr& testing_automata,
     state_ta_explicit* artificial_livelock_acc_state = nullptr)
    {

      if (artificial_livelock_acc_state)
        {
          auto artificial_livelock_acc_state_added =
            testing_automata->add_state(artificial_livelock_acc_state);

          // unique artificial_livelock_acc_state
          assert(artificial_livelock_acc_state_added
                 == artificial_livelock_acc_state);
          (void)artificial_livelock_acc_state_added;
          artificial_livelock_acc_state->set_livelock_accepting_state(true);
          artificial_livelock_acc_state->free_transitions();
        }

      ta::states_set_t states_set = testing_automata->get_states_set();
      ta::states_set_t::iterator it;

      state_ta_explicit::transitions* transitions_to_livelock_states =
        new state_ta_explicit::transitions;

      for (it = states_set.begin(); it != states_set.end(); ++it)
        {
          auto source = const_cast<state_ta_explicit*>
            (static_cast<const state_ta_explicit*>(*it));

          transitions_to_livelock_states->clear();

          state_ta_explicit::transitions* trans = source->get_transitions();
          state_ta_explicit::transitions::iterator it_trans;

          if (trans)
            for (it_trans = trans->begin(); it_trans != trans->end();)
              {
                auto dest = const_cast<state_ta_explicit*>((*it_trans)->dest);

                state_ta_explicit::transitions* dest_trans =
                  dest->get_transitions();
                bool dest_trans_empty = !dest_trans || dest_trans->empty();

                //select transitions where a destination is a livelock state
                // which isn't a Buchi accepting state and has successors
                if (dest->is_livelock_accepting_state()
                    && (!dest->is_accepting_state()) && (!dest_trans_empty))
                  transitions_to_livelock_states->push_front(*it_trans);

                // optimization to have, after minimization, a unique
                // livelock state which has no successors
                if (dest->is_livelock_accepting_state() && (dest_trans_empty))
                  dest->set_accepting_state(false);

                ++it_trans;
              }

          for (auto* trans: *transitions_to_livelock_states)
            testing_automata->create_transition
              (source, trans->condition,
               trans->acceptance_conditions,
               artificial_livelock_acc_state ?
               artificial_livelock_acc_state :
               trans->dest->stuttering_reachable_livelock, true);
        }
      delete transitions_to_livelock_states;

      for (it = states_set.begin(); it != states_set.end(); ++it)
        {
          state_ta_explicit* state = static_cast<state_ta_explicit*> (*it);
          state_ta_explicit::transitions* state_trans =
            (state)->get_transitions();
          bool state_trans_empty = !state_trans || state_trans->empty();

          if (state->is_livelock_accepting_state()
              && (!state->is_accepting_state()) && (!state_trans_empty))
            state->set_livelock_accepting_state(false);
        }
    }

    static void
    compute_livelock_acceptance_states(const ta_explicit_ptr& testing_aut,
                                       bool single_pass_emptiness_check,
                                       state_ta_explicit*
                                       artificial_livelock_acc_state)
    {
      // We use five main data in this algorithm:
      // * sscc: a stack of strongly stuttering-connected components (SSCC)
      scc_stack_ta sscc;

      // * arc, a stack of acceptance conditions between each of these SCC,
      std::stack<acc_cond::mark_t> arc;

      // * h: a hash of all visited nodes, with their order,
      //   (it is called "Hash" in Couvreur's paper)
      state_map<int> h; ///< Heap of visited states.

      // * num: the number of visited nodes.  Used to set the order of each
      //   visited node,
      int num = 0;

      // * todo: the depth-first search stack.  This holds pairs of the
      //   form (STATE, ITERATOR) where ITERATOR is a twa_succ_iterator
      //   over the successors of STATE.  In our use, ITERATOR should
      //   always be freed when TODO is popped, but STATE should not because
      //   it is also used as a key in H.
      std::stack<pair_state_iter> todo;

      // * init: the set of the depth-first search initial states
      std::stack<const state*> init_set;

      for (auto s: testing_aut->get_initial_states_set())
        init_set.push(s);

      while (!init_set.empty())
        {
          // Setup depth-first search from initial states.

          {
            auto init = down_cast<const state_ta_explicit*> (init_set.top());
            init_set.pop();

            if (!h.emplace(init, num + 1).second)
              {
                init->destroy();
                continue;
              }

            sscc.push(++num);
            arc.push({});
            sscc.top().is_accepting
              = testing_aut->is_accepting_state(init);
            twa_succ_iterator* iter = testing_aut->succ_iter(init);
            iter->first();
            todo.emplace(init, iter);
          }

          while (!todo.empty())
            {
              auto curr = todo.top().first;

              auto i = h.find(curr);
              // If we have reached a dead component, ignore it.
              if (i != h.end() && i->second == -1)
                {
                  todo.pop();
                  continue;
                }

              // We are looking at the next successor in SUCC.
              twa_succ_iterator* succ = todo.top().second;

              // If there is no more successor, backtrack.
              if (succ->done())
                {
                  // We have explored all successors of state CURR.

                  // Backtrack TODO.
                  todo.pop();

                  // fill rem with any component removed,
                  assert(i != h.end());
                  sscc.rem().push_front(curr);

                  // When backtracking the root of an SSCC, we must also
                  // remove that SSCC from the ROOT stacks.  We must
                  // discard from H all reachable states from this SSCC.
                  assert(!sscc.empty());
                  if (sscc.top().index == i->second)
                    {
                      // removing states
                      bool is_livelock_accepting_sscc = (sscc.rem().size() > 1)
                        && ((sscc.top().is_accepting) ||
                            (testing_aut->acc().
                             accepting(sscc.top().condition)));
                      trace << "*** sscc.size()  = ***" <<  sscc.size() << '\n';
                      for (auto j: sscc.rem())
                        {
                          h[j] = -1;

                          if (is_livelock_accepting_sscc)
                            {
                              // if it is an accepting sscc add the state to
                              // G (=the livelock-accepting states set)
                              trace << "*** sscc.size() > 1: states: ***"
                                    << testing_aut->format_state(j)
                                    << '\n';
                              auto livelock_accepting_state =
                                const_cast<state_ta_explicit*>
                                (down_cast<const state_ta_explicit*>(j));

                              livelock_accepting_state->
                                set_livelock_accepting_state(true);

                              if (single_pass_emptiness_check)
                                {
                                  livelock_accepting_state
                                    ->set_accepting_state(true);
                                  livelock_accepting_state
                                    ->stuttering_reachable_livelock
                                    = livelock_accepting_state;
                                }
                            }
                        }

                      assert(!arc.empty());
                      sscc.pop();
                      arc.pop();
                    }

                  // automata reduction
                  testing_aut->delete_stuttering_and_hole_successors(curr);

                  delete succ;
                  // Do not delete CURR: it is a key in H.
                  continue;
                }

              // Fetch the values destination state we are interested in...
              auto dest = succ->dst();

              auto acc_cond = succ->acc();
              // ... and point the iterator to the next successor, for
              // the next iteration.
              succ->next();
              // We do not need SUCC from now on.

              // Are we going to a new state through a stuttering transition?
              bool is_stuttering_transition =
                testing_aut->get_state_condition(curr)
                == testing_aut->get_state_condition(dest);
              auto id = h.find(dest);

              // Is this a new state?
              if (id == h.end())
                {
                  if (!is_stuttering_transition)
                    {
                      init_set.push(dest);
                      dest->destroy();
                      continue;
                    }

                  // Number it, stack it, and register its successors
                  // for later processing.
                  h[dest] = ++num;
                  sscc.push(num);
                  arc.push(acc_cond);
                  sscc.top().is_accepting =
                    testing_aut->is_accepting_state(dest);

                  twa_succ_iterator* iter = testing_aut->succ_iter(dest);
                  iter->first();
                  todo.emplace(dest, iter);
                  continue;
                }
              dest->destroy();

              // If we have reached a dead component, ignore it.
              if (id->second == -1)
                continue;

              trace << "***compute_livelock_acceptance_states: CYCLE***\n";

              if (!curr->compare(id->first))
                {
                  auto self_loop_state = const_cast<state_ta_explicit*>
                    (down_cast<const state_ta_explicit*>(curr));

                  if (testing_aut->is_accepting_state(self_loop_state)
                      || (testing_aut->acc().accepting(acc_cond)))
                    {
                      self_loop_state->set_livelock_accepting_state(true);
                      if (single_pass_emptiness_check)
                        {
                          self_loop_state->set_accepting_state(true);
                          self_loop_state->stuttering_reachable_livelock
                            = self_loop_state;
                        }
                    }

                  trace
                    << "***compute_livelock_acceptance_states: CYCLE: "
                    << "self_loop_state***\n";
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
              int threshold = id->second;
              std::list<const state*> rem;
              bool acc = false;

              while (threshold < sscc.top().index)
                {
                  assert(!sscc.empty());
                  assert(!arc.empty());
                  acc |= sscc.top().is_accepting;
                  acc_cond |= sscc.top().condition;
                  acc_cond |= arc.top();
                  rem.splice(rem.end(), sscc.rem());
                  sscc.pop();
                  arc.pop();
                }

              // Note that we do not always have
              //  threshold == sscc.top().index
              // after this loop, the SSCC whose index is threshold might have
              // been merged with a lower SSCC.

              // Accumulate all acceptance conditions into the merged SSCC.
              sscc.top().is_accepting |= acc;
              sscc.top().condition |= acc_cond;

              sscc.rem().splice(sscc.rem().end(), rem);

            }

        }

      if (artificial_livelock_acc_state || single_pass_emptiness_check)
        transform_to_single_pass_automaton(testing_aut,
                                           artificial_livelock_acc_state);
    }

    ta_explicit_ptr
    build_ta(const ta_explicit_ptr& ta, bdd atomic_propositions_set_,
             bool degeneralized,
             bool single_pass_emptiness_check,
             bool artificial_livelock_state_mode,
             bool no_livelock)
    {

      std::stack<state_ta_explicit*> todo;
      const_twa_ptr tgba_ = ta->get_tgba();

      // build Initial states set:
      auto tgba_init_state = tgba_->get_init_state();

      bdd tgba_condition = [&]()
        {
          bdd cond = bddfalse;
          for (auto i: tgba_->succ(tgba_init_state))
            cond |= i->cond();
          return cond;
        }();

      bool is_acc = false;
      if (degeneralized)
        {
          twa_succ_iterator* it = tgba_->succ_iter(tgba_init_state);
          it->first();
          if (!it->done())
            is_acc = !!it->acc();
          delete it;
        }

      bdd satone_tgba_condition;
      while ((satone_tgba_condition = bdd_satoneset(tgba_condition,
                                                    atomic_propositions_set_,
                                                    bddtrue)) != bddfalse)
        {
          tgba_condition -= satone_tgba_condition;
          state_ta_explicit* init_state = new
            state_ta_explicit(tgba_init_state->clone(),
                              satone_tgba_condition, true, is_acc);
          state_ta_explicit* s = ta->add_state(init_state);
          assert(s == init_state);
          ta->add_to_initial_states_set(s);

          todo.push(init_state);
        }
      tgba_init_state->destroy();

      while (!todo.empty())
        {
          state_ta_explicit* source = todo.top();
          todo.pop();

          twa_succ_iterator* twa_succ_it =
            tgba_->succ_iter(source->get_tgba_state());
          for (twa_succ_it->first(); !twa_succ_it->done();
               twa_succ_it->next())
            {
              const state* tgba_state = twa_succ_it->dst();
              bdd tgba_condition = twa_succ_it->cond();
              acc_cond::mark_t tgba_acceptance_conditions =
                twa_succ_it->acc();
              bdd satone_tgba_condition;
              while ((satone_tgba_condition =
                      bdd_satoneset(tgba_condition,
                                    atomic_propositions_set_, bddtrue))
                     != bddfalse)
                {
                  tgba_condition -= satone_tgba_condition;

                  bdd all_props = bddtrue;
                  bdd dest_condition;

                  bool is_acc = false;
                  if (degeneralized)
                  {
                    twa_succ_iterator* it = tgba_->succ_iter(tgba_state);
                    it->first();
                    if (!it->done())
                      is_acc = !!it->acc();
                    delete it;
                  }

                  if (satone_tgba_condition == source->get_tgba_condition())
                    while ((dest_condition =
                            bdd_satoneset(all_props,
                                          atomic_propositions_set_, bddtrue))
                           != bddfalse)
                      {
                        all_props -= dest_condition;
                        state_ta_explicit* new_dest =
                          new state_ta_explicit(tgba_state->clone(),
                                                dest_condition, false, is_acc);
                        state_ta_explicit* dest = ta->add_state(new_dest);

                        if (dest != new_dest)
                          {
                            // the state dest already exists in the automaton
                            new_dest->get_tgba_state()->destroy();
                            delete new_dest;
                          }
                        else
                          {
                            todo.push(dest);
                          }

                        bdd cs = bdd_setxor(source->get_tgba_condition(),
                                            dest->get_tgba_condition());
                        ta->create_transition(source, cs,
                                              tgba_acceptance_conditions, dest);
                      }
                }
              tgba_state->destroy();
            }
          delete twa_succ_it;
        }

      if (no_livelock)
        return ta;

      state_ta_explicit* artificial_livelock_acc_state = nullptr;

      trace << "*** build_ta: artificial_livelock_acc_state_mode = ***"
            << artificial_livelock_state_mode << std::endl;

      if (artificial_livelock_state_mode)
        {
          single_pass_emptiness_check = true;
          artificial_livelock_acc_state =
            new state_ta_explicit(ta->get_tgba()->get_init_state(), bddtrue,
                                  false, false, true, nullptr);
          trace
            << "*** build_ta: artificial_livelock_acc_state = ***"
            << artificial_livelock_acc_state << std::endl;
        }

      compute_livelock_acceptance_states(ta, single_pass_emptiness_check,
                                         artificial_livelock_acc_state);
      return ta;
    }
  }

  ta_explicit_ptr
  tgba_to_ta(const const_twa_ptr& tgba_, bdd atomic_propositions_set_,
             bool degeneralized, bool artificial_initial_state_mode,
             bool single_pass_emptiness_check,
             bool artificial_livelock_state_mode,
             bool no_livelock)
  {
    ta_explicit_ptr ta;

    auto tgba_init_state = tgba_->get_init_state();
    if (artificial_initial_state_mode)
      {
        state_ta_explicit* artificial_init_state =
          new state_ta_explicit(tgba_init_state->clone(), bddfalse, true);

        ta = make_ta_explicit(tgba_, tgba_->acc().num_sets(),
                              artificial_init_state);
      }
    else
      {
        ta = make_ta_explicit(tgba_, tgba_->acc().num_sets());
      }
    tgba_init_state->destroy();

    // build ta automaton
    build_ta(ta, atomic_propositions_set_, degeneralized,
             single_pass_emptiness_check, artificial_livelock_state_mode,
             no_livelock);

    // (degeneralized=true) => TA
    if (degeneralized)
      return ta;

    // (degeneralized=false) => GTA
    // adapt a GTA to remove acceptance conditions from states
    ta::states_set_t states_set = ta->get_states_set();
    ta::states_set_t::iterator it;
    for (it = states_set.begin(); it != states_set.end(); ++it)
      {
        state_ta_explicit* state = static_cast<state_ta_explicit*> (*it);

        if (state->is_accepting_state())
          {
            state_ta_explicit::transitions* trans = state->get_transitions();
            state_ta_explicit::transitions::iterator it_trans;

            for (it_trans = trans->begin(); it_trans != trans->end();
                 ++it_trans)
              (*it_trans)->acceptance_conditions = ta->acc().all_sets();

            state->set_accepting_state(false);
          }
      }

    return ta;
  }

  tgta_explicit_ptr
  tgba_to_tgta(const const_twa_ptr& tgba_, bdd atomic_propositions_set_)
  {
    auto tgba_init_state = tgba_->get_init_state();
    auto artificial_init_state = new state_ta_explicit(tgba_init_state->clone(),
                                                       bddfalse, true);
    tgba_init_state->destroy();

    auto tgta = make_tgta_explicit(tgba_, tgba_->acc().num_sets(),
                                   artificial_init_state);

    // build a Generalized TA automaton involving a single_pass_emptiness_check
    // (without an artificial livelock state):
    auto ta = tgta->get_ta();
    build_ta(ta, atomic_propositions_set_, false, true, false, false);

    trace << "***tgba_to_tgbta: POST build_ta***" << std::endl;

    // adapt a ta automata to build tgta automata :
    ta::states_set_t states_set = ta->get_states_set();
    ta::states_set_t::iterator it;
    twa_succ_iterator* initial_states_iter =
      ta->succ_iter(ta->get_artificial_initial_state());
    initial_states_iter->first();
    if (initial_states_iter->done())
      {
        delete initial_states_iter;
        return tgta;
      }
    bdd first_state_condition = initial_states_iter->cond();
    delete initial_states_iter;

    bdd bdd_stutering_transition = bdd_setxor(first_state_condition,
                                              first_state_condition);

    for (it = states_set.begin(); it != states_set.end(); ++it)
      {
        state_ta_explicit* state = static_cast<state_ta_explicit*> (*it);

        state_ta_explicit::transitions* trans = state->get_transitions();
        if (state->is_livelock_accepting_state())
          {
            bool trans_empty = !trans || trans->empty();
            if (trans_empty || state->is_accepting_state())
              {
                ta->create_transition(state, bdd_stutering_transition,
                                      ta->acc().all_sets(), state);
              }
          }

        if (state->compare(ta->get_artificial_initial_state()))
          ta->create_transition(state, bdd_stutering_transition,
                                {}, state);

        state->set_livelock_accepting_state(false);
        state->set_accepting_state(false);
        trace << "***tgba_to_tgbta: POST create_transition ***" << std::endl;
      }

    return tgta;
  }
}
