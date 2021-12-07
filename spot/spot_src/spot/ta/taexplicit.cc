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

#include <spot/ta/taexplicit.hh>
#include <spot/twa/formula2bdd.hh>
#include <cassert>
#include <spot/twa/bddprint.hh>

namespace spot
{

  ////////////////////////////////////////
  // ta_explicit_succ_iterator

  ta_explicit_succ_iterator::ta_explicit_succ_iterator(
      const state_ta_explicit* s)
  {
    transitions_ = s->get_transitions();
  }

  ta_explicit_succ_iterator::ta_explicit_succ_iterator(
      const state_ta_explicit* s, bdd condition)
  {
    transitions_ = s->get_transitions(condition);
  }

  bool
  ta_explicit_succ_iterator::first()
  {
    if (!transitions_)
      return false;
    i_ = transitions_->begin();
    return i_ != transitions_->end();
  }

  bool
  ta_explicit_succ_iterator::next()
  {
    ++i_;
    return i_ != transitions_->end();
  }

  bool
  ta_explicit_succ_iterator::done() const
  {
    return !transitions_ || i_ == transitions_->end();
  }

  const state*
  ta_explicit_succ_iterator::dst() const
  {
    trace
      << "***ta_explicit_succ_iterator::dst()  if(done()) =***"
          << done() << std::endl;
    assert(!done());
    trace
      << "***ta_explicit_succ_iterator::dst() (*i_)->condition =***"
          << (*i_)->condition << std::endl;
    return (*i_)->dest;
  }

  bdd
  ta_explicit_succ_iterator::cond() const
  {
    assert(!done());
    return (*i_)->condition;
  }

  acc_cond::mark_t
  ta_explicit_succ_iterator::acc() const
  {
    assert(!done());
    return (*i_)->acceptance_conditions;
  }

  ////////////////////////////////////////
  // state_ta_explicit

  state_ta_explicit::transitions*
  state_ta_explicit::get_transitions() const
  {
    return transitions_;
  }

  // return transitions filtred by condition
  state_ta_explicit::transitions*
  state_ta_explicit::get_transitions(bdd condition) const
  {

    std::unordered_map<int, transitions*, std::hash<int> >::const_iterator i =
        transitions_by_condition.find(condition.id());

    if (i == transitions_by_condition.end())
      return nullptr;
    else
      return i->second;
  }

  void
  state_ta_explicit::add_transition(state_ta_explicit::transition* t,
      bool add_at_beginning)
  {
    if (!transitions_)
      transitions_ = new transitions;

    transitions* trans_by_condition = get_transitions(t->condition);

    if (!trans_by_condition)
      {
        trans_by_condition = new transitions;
        transitions_by_condition[(t->condition).id()] = trans_by_condition;
      }

    state_ta_explicit::transitions::iterator it_trans;
    bool transition_found = false;

    for (it_trans = trans_by_condition->begin(); (it_trans
        != trans_by_condition->end() && !transition_found); ++it_trans)
      {
        transition_found = ((*it_trans)->dest == t->dest);
        if (transition_found)
          {
            (*it_trans)->acceptance_conditions |= t->acceptance_conditions;
          }
      }

    if (!transition_found)
      {
        if (add_at_beginning)
          {
            trans_by_condition->push_front(t);
            transitions_->push_front(t);
          }
        else
          {
            trans_by_condition->emplace_back(t);
            transitions_->emplace_back(t);
          }

      }
    else
      {
        delete t;
      }

  }

  const state*
  state_ta_explicit::get_tgba_state() const
  {
    return tgba_state_;
  }

  const bdd
  state_ta_explicit::get_tgba_condition() const
  {
    return tgba_condition_;
  }

  bool
  state_ta_explicit::is_accepting_state() const
  {
    return is_accepting_state_;
  }

  bool
  state_ta_explicit::is_initial_state() const
  {
    return is_initial_state_;
  }

  void
  state_ta_explicit::set_accepting_state(bool is_accepting_state)
  {
    is_accepting_state_ = is_accepting_state;
  }

  bool
  state_ta_explicit::is_livelock_accepting_state() const
  {
    return is_livelock_accepting_state_;
  }

  void
  state_ta_explicit::set_livelock_accepting_state(
      bool is_livelock_accepting_state)
  {
    is_livelock_accepting_state_ = is_livelock_accepting_state;
  }

  void
  state_ta_explicit::set_initial_state(bool is_initial_state)
  {
    is_initial_state_ = is_initial_state;
  }

  bool
  state_ta_explicit::is_hole_state() const
  {
    state_ta_explicit::transitions* trans = get_transitions();
    return !trans || trans->empty();
  }

  int
  state_ta_explicit::compare(const spot::state* other) const
  {
    const state_ta_explicit* o = down_cast<const state_ta_explicit*>(other);

    int compare_value = tgba_state_->compare(o->tgba_state_);

    if (compare_value)
      return compare_value;

    compare_value = tgba_condition_.id() - o->tgba_condition_.id();

    //    if (compare_value != 0)
    //      return compare_value;
    //
    //    //unique artificial_livelock_accepting_state
    //    if (o->is_the_artificial_livelock_accepting_state())
    //      return is_the_artificial_livelock_accepting_state();

    return compare_value;
  }

  size_t
  state_ta_explicit::hash() const
  {
    //return wang32_hash(tgba_state_->hash());
    return wang32_hash(tgba_state_->hash()) ^ wang32_hash(tgba_condition_.id());

  }

  state_ta_explicit*
  state_ta_explicit::clone() const
  {
    return new state_ta_explicit(tgba_state_, tgba_condition_,
                                 is_initial_state_,
                                 is_accepting_state_,
                                 is_livelock_accepting_state_, transitions_);
  }

  void
  state_ta_explicit::delete_stuttering_and_hole_successors()
  {
    state_ta_explicit::transitions* trans = get_transitions();
    state_ta_explicit::transitions::iterator it_trans;

    if (trans)
      for (it_trans = trans->begin(); it_trans != trans->end();)
        {
          auto dest = (*it_trans)->dest;
          bool is_stuttering_transition = (get_tgba_condition()
              == (dest)->get_tgba_condition());
          bool dest_is_livelock_accepting =
            dest->is_livelock_accepting_state();

          //Before deleting stuttering transitions, propaged back livelock
          //and initial state's properties
          if (is_stuttering_transition)
            {
              if (!is_livelock_accepting_state() && dest_is_livelock_accepting)
                {
                  set_livelock_accepting_state(true);
                  stuttering_reachable_livelock
                      = dest->stuttering_reachable_livelock;
                }
              if (dest->is_initial_state())
                set_initial_state(true);
            }

          //remove hole successors states
          state_ta_explicit::transitions* dest_trans =
              (dest)->get_transitions();
          bool dest_trans_empty = !dest_trans || dest_trans->empty();
          if (is_stuttering_transition || (dest_trans_empty
              && (!dest_is_livelock_accepting)))
            {
              get_transitions((*it_trans)->condition)->remove(*it_trans);
              delete *it_trans;
              it_trans = trans->erase(it_trans);
            }
          else
            {
              ++it_trans;
            }
        }

  }

  void
  state_ta_explicit::free_transitions()
  {
    state_ta_explicit::transitions* trans = transitions_;
    // We don't destroy the transitions in the state's destructor because
    // they are not cloned.
    if (trans)
      for (auto& t: *trans)
        delete t;
    delete trans;

    std::unordered_map<int, transitions*, std::hash<int> >::iterator i =
      transitions_by_condition.begin();
    while (i != transitions_by_condition.end())
      {
        delete i->second;
        ++i;
      }

    transitions_ = nullptr;
  }

  ////////////////////////////////////////
  // ta_explicit


  ta_explicit::ta_explicit(const const_twa_ptr& tgba,
                           unsigned n_acc,
                           state_ta_explicit* artificial_initial_state):
    ta(tgba->get_dict()),
    tgba_(tgba),
    artificial_initial_state_(artificial_initial_state)
  {
    get_dict()->register_all_variables_of(&tgba_, this);
    acc().add_sets(n_acc);
    acc().set_generalized_buchi();
    if (artificial_initial_state)
      {
        auto is = add_state(artificial_initial_state);
        assert(is == artificial_initial_state);
        (void)is;
      }
  }

  ta_explicit::~ta_explicit()
  {
    ta::states_set_t::iterator it;
    for (it = states_set_.begin(); it != states_set_.end(); ++it)
      {
        auto* s = const_cast<state_ta_explicit*>
          (down_cast<const state_ta_explicit*>(*it));
        s->free_transitions();
        s->get_tgba_state()->destroy();
        delete s;
      }
    get_dict()->unregister_all_my_variables(this);
  }

  state_ta_explicit*
  ta_explicit::add_state(state_ta_explicit* s)
  {
    std::pair<ta::states_set_t::iterator, bool> add_state_to_ta =
        states_set_.insert(s);

    return down_cast<state_ta_explicit*>(*add_state_to_ta.first);
  }

  void
  ta_explicit::add_to_initial_states_set(state* state, bdd condition)
  {
    state_ta_explicit* s = down_cast<state_ta_explicit*>(state);
    s->set_initial_state(true);
    if (condition == bddfalse)
      condition = get_state_condition(s);
    auto add_state = initial_states_set_.insert(s);
    if (get_artificial_initial_state())
      if (add_state.second)
        {
          auto i =
            down_cast<state_ta_explicit*>(get_artificial_initial_state());
          create_transition(i, condition, {}, s);
        }
  }

  void
  ta_explicit::delete_stuttering_and_hole_successors(const spot::state* s)
  {
    auto state =
      const_cast<state_ta_explicit*>(down_cast<const state_ta_explicit*>(s));
    state->delete_stuttering_and_hole_successors();
    if (state->is_initial_state())
      add_to_initial_states_set(state);

  }

  void
  ta_explicit::create_transition(state_ta_explicit* source, bdd condition,
                                 acc_cond::mark_t acceptance_conditions,
                                 const state_ta_explicit* dest,
                                 bool add_at_beginning)
  {
    state_ta_explicit::transition* t = new state_ta_explicit::transition;
    t->dest = dest;
    t->condition = condition;
    t->acceptance_conditions = acceptance_conditions;
    source->add_transition(t, add_at_beginning);

  }

  ta::const_states_set_t
  ta_explicit::get_initial_states_set() const
  {
    return initial_states_set_;

  }

  bdd
  ta_explicit::get_state_condition(const spot::state* initial_state) const
  {
    const state_ta_explicit* sta =
        down_cast<const state_ta_explicit*>(initial_state);
    return sta->get_tgba_condition();
  }

  bool
  ta_explicit::is_accepting_state(const spot::state* s) const
  {
    const state_ta_explicit* sta = down_cast<const state_ta_explicit*>(s);
    return sta->is_accepting_state();
  }

  bool
  ta_explicit::is_initial_state(const spot::state* s) const
  {
    const state_ta_explicit* sta = down_cast<const state_ta_explicit*>(s);
    return sta->is_initial_state();
  }

  bool
  ta_explicit::is_livelock_accepting_state(const spot::state* s) const
  {
    const state_ta_explicit* sta = down_cast<const state_ta_explicit*>(s);
    return sta->is_livelock_accepting_state();
  }

  ta_succ_iterator*
  ta_explicit::succ_iter(const spot::state* state) const
  {
    const state_ta_explicit* s = down_cast<const state_ta_explicit*>(state);
    return new ta_explicit_succ_iterator(s);
  }

  ta_succ_iterator*
  ta_explicit::succ_iter(const spot::state* state, bdd condition) const
  {
    const state_ta_explicit* s = down_cast<const state_ta_explicit*>(state);
    return new ta_explicit_succ_iterator(s, condition);
  }

  bdd_dict_ptr
  ta_explicit::get_dict() const
  {
    return tgba_->get_dict();
  }

  const_twa_ptr
  ta_explicit::get_tgba() const
  {
    return tgba_;
  }

  std::string
  ta_explicit::format_state(const spot::state* s) const
  {
    const state_ta_explicit* sta = down_cast<const state_ta_explicit*>(s);

    if (sta->get_tgba_condition() == bddtrue)
      return tgba_->format_state(sta->get_tgba_state());

    return tgba_->format_state(sta->get_tgba_state()) + "\n"
        + bdd_format_formula(get_dict(), sta->get_tgba_condition());

  }

  void
  ta_explicit::delete_stuttering_transitions()
  {
    ta::states_set_t::iterator it;
    for (it = states_set_.begin(); it != states_set_.end(); ++it)
      {

        const state_ta_explicit* source =
            static_cast<const state_ta_explicit*>(*it);

        state_ta_explicit::transitions* trans = source->get_transitions();
        state_ta_explicit::transitions::iterator it_trans;

        if (trans)
          for (it_trans = trans->begin(); it_trans != trans->end();)
            {
              if (source->get_tgba_condition()
                  == ((*it_trans)->dest)->get_tgba_condition())
                {
                  delete *it_trans;
                  it_trans = trans->erase(it_trans);
                }
              else
                {
                  ++it_trans;
                }
            }
      }

  }

  void
  ta_explicit::free_state(const spot::state*) const
  {
  }

}
