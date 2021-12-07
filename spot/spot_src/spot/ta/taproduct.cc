// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2012, 2014-2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
//
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
#include <spot/ta/taproduct.hh>
#include <cassert>
#include <spot/misc/hashfunc.hh>

namespace spot
{

  ////////////////////////////////////////////////////////////
  // state_ta_product

  state_ta_product::~state_ta_product()
  {
    //see ta_product::free_state() method
    kripke_state_->destroy();
  }

  int
  state_ta_product::compare(const state* other) const
  {
    const state_ta_product* o = down_cast<const state_ta_product*> (other);
    int res = ta_state_->compare(o->get_ta_state());
    if (res != 0)
      return res;
    return kripke_state_->compare(o->get_kripke_state());
  }

  size_t
  state_ta_product::hash() const
  {
    // We assume that size_t is 32-bit wide.
    return wang32_hash(ta_state_->hash()) ^ wang32_hash(kripke_state_->hash());
  }

  state_ta_product*
  state_ta_product::clone() const
  {
    return new state_ta_product(ta_state_, kripke_state_);
  }

  ////////////////////////////////////////////////////////////
  // ta_succ_iterator_product
  ta_succ_iterator_product::ta_succ_iterator_product(const state_ta_product* s,
                                                     const ta* t,
                                                     const kripke* k)
    : source_(s), ta_(t), kripke_(k)
  {
    kripke_source_condition = kripke_->state_condition(s->get_kripke_state());

    kripke_succ_it_ = kripke_->succ_iter(s->get_kripke_state());
    kripke_current_dest_state = nullptr;
    ta_succ_it_ = nullptr;
    current_state_ = nullptr;
  }

  ta_succ_iterator_product::~ta_succ_iterator_product()
  {
    delete current_state_;
    current_state_ = nullptr;
    delete ta_succ_it_;
    delete kripke_succ_it_;
    if (kripke_current_dest_state)
      kripke_current_dest_state->destroy();
  }

  void
  ta_succ_iterator_product::step_()
  {
    if (!ta_succ_it_->done())
      ta_succ_it_->next();
    if (ta_succ_it_->done())
      {
        delete ta_succ_it_;
        ta_succ_it_ = nullptr;
        next_kripke_dest();
      }
  }

  void
  ta_succ_iterator_product::next_kripke_dest()
  {
    if (!kripke_succ_it_)
      return;

    if (!kripke_current_dest_state)
      {
        kripke_succ_it_->first();
      }
    else
      {
        kripke_current_dest_state->destroy();
        kripke_current_dest_state = nullptr;
        kripke_succ_it_->next();
      }

    // If one of the two successor sets is empty initially, we reset
    // kripke_succ_it_, so that done() can detect this situation easily.  (We
    // choose to reset kripke_succ_it_ because this variable is already used by
    // done().)
    if (kripke_succ_it_->done())
      {
        delete kripke_succ_it_;
        kripke_succ_it_ = nullptr;
        return;
      }

    kripke_current_dest_state = kripke_succ_it_->dst();
    bdd kripke_current_dest_condition = kripke_->state_condition(
        kripke_current_dest_state);
    is_stuttering_transition_ = (kripke_source_condition
        == kripke_current_dest_condition);
    if (is_stuttering_transition_)
      {
        current_condition_ = bddfalse;
      }
    else
      {
        current_condition_ = bdd_setxor(kripke_source_condition,
            kripke_current_dest_condition);
        ta_succ_it_ = ta_->succ_iter(source_->get_ta_state(),
            current_condition_);
        ta_succ_it_->first();
      }

  }

  bool
  ta_succ_iterator_product::first()
  {

    next_kripke_dest();

    if (!done())
      return next_non_stuttering_();
    return false;
  }

  bool
  ta_succ_iterator_product::next()
  {
    delete current_state_;
    current_state_ = nullptr;
    if (is_stuttering_transition())
      {
        next_kripke_dest();
      }
    else
      step_();

    if (!done())
      return next_non_stuttering_();
    return false;
  }

  bool
  ta_succ_iterator_product::next_non_stuttering_()
  {

    while (!done())
      {

        if (is_stuttering_transition_)
          {
            //if stuttering transition, the TA automata stays in the same state
            current_state_ = new state_ta_product(source_->get_ta_state(),
                kripke_current_dest_state->clone());
            current_acceptance_conditions_ = {};
            return true;
          }

        if (!ta_succ_it_->done())
          {
            current_state_ = new state_ta_product(ta_succ_it_->dst(),
                kripke_current_dest_state->clone());
            current_acceptance_conditions_
                = ta_succ_it_->acc();
            return true;
          }

        step_();
      }
    return false;
  }

  bool
  ta_succ_iterator_product::done() const
  {
    return !kripke_succ_it_ || kripke_succ_it_->done();
  }

  state_ta_product*
  ta_succ_iterator_product::dst() const
  {
    return current_state_->clone();
  }

  bool
  ta_succ_iterator_product::is_stuttering_transition() const
  {
    return is_stuttering_transition_;
  }

  bdd
  ta_succ_iterator_product::cond() const
  {
    return current_condition_;
  }

  acc_cond::mark_t
  ta_succ_iterator_product::acc() const
  {
    return current_acceptance_conditions_;
  }

  ////////////////////////////////////////////////////////////
  // ta_product


  ta_product::ta_product(const const_ta_ptr& testing_automata,
                         const const_kripke_ptr& kripke_structure):
    ta(testing_automata->get_dict()),
    dict_(testing_automata->get_dict()),
    ta_(testing_automata),
    kripke_(kripke_structure)
  {
    assert(dict_ == kripke_structure->get_dict());
    dict_->register_all_variables_of(ta_, this);
    dict_->register_all_variables_of(kripke_, this);
  }

  ta_product::~ta_product()
  {
    dict_->unregister_all_my_variables(this);
  }

  ta::const_states_set_t
  ta_product::get_initial_states_set() const
  {
    //build initial states set

    ta::const_states_set_t ta_init_states_set;
    ta::const_states_set_t::const_iterator it;

    ta::const_states_set_t initial_states_set;
    const state* kripke_init = kripke_->get_init_state();
    bdd kripke_init_condition = kripke_->state_condition(kripke_init);

    const spot::state* artificial_initial_state =
      ta_->get_artificial_initial_state();

    if (artificial_initial_state)
      {
        ta_succ_iterator* ta_init_it_ = ta_->succ_iter(
            artificial_initial_state, kripke_init_condition);
        for (ta_init_it_->first(); !ta_init_it_->done(); ta_init_it_->next())
          {
            ta_init_states_set.insert(ta_init_it_->dst());
          }
        delete ta_init_it_;

      }
    else
      {
        ta_init_states_set = ta_->get_initial_states_set();
      }

    for (auto s: ta_init_states_set)
      if (artificial_initial_state ||
          (kripke_init_condition == ta_->get_state_condition(s)))
        initial_states_set.insert(new state_ta_product(s,
                                                       kripke_init->clone()));

    kripke_init->destroy();
    return initial_states_set;
  }

  ta_succ_iterator_product*
  ta_product::succ_iter(const state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*>(s);
    return new ta_succ_iterator_product(stp, ta_.get(), kripke_.get());
  }


  ta_succ_iterator_product*
  ta_product::succ_iter(const spot::state* s, bdd changeset) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*>(s);
    return new ta_succ_iterator_product_by_changeset(stp,
                                                     ta_.get(), kripke_.get(),
                                                     changeset);

  }

  bdd_dict_ptr
  ta_product::get_dict() const
  {
    return dict_;
  }

  std::string
  ta_product::format_state(const state* state) const
  {
    const state_ta_product* s = down_cast<const state_ta_product*> (state);
    return kripke_->format_state(s->get_kripke_state()) + " * \n"
        + ta_->format_state(s->get_ta_state());
  }

  bool
  ta_product::is_accepting_state(const spot::state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*> (s);
    return ta_->is_accepting_state(stp->get_ta_state());
  }

  bool
  ta_product::is_livelock_accepting_state(const spot::state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*> (s);
    return ta_->is_livelock_accepting_state(stp->get_ta_state());
  }

  bool
  ta_product::is_initial_state(const spot::state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*> (s);

    const state* ta_s = stp->get_ta_state();
    const state* kr_s = stp->get_kripke_state();

    return (ta_->is_initial_state(ta_s))
        && ((kripke_->get_init_state())->compare(kr_s) == 0)
        && ((kripke_->state_condition(kr_s))
            == (ta_->get_state_condition(ta_s)));
  }

  bool
  ta_product::is_hole_state_in_ta_component(const spot::state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*> (s);
    ta_succ_iterator* ta_succ_iter = get_ta()->succ_iter(stp->get_ta_state());
    bool is_hole_state = ta_succ_iter->done();
    delete ta_succ_iter;
    return is_hole_state;
  }

  bdd
  ta_product::get_state_condition(const spot::state* s) const
  {
    const state_ta_product* stp = down_cast<const state_ta_product*> (s);
    const state* ta_s = stp->get_ta_state();
    return ta_->get_state_condition(ta_s);
  }

  void
  ta_product::free_state(const spot::state* s) const
  {

    const state_ta_product* stp = down_cast<const state_ta_product*> (s);
    ta_->free_state(stp->get_ta_state());
    delete stp;

  }

  ta_succ_iterator_product_by_changeset::
  ta_succ_iterator_product_by_changeset(const state_ta_product* s, const ta* t,
                                        const kripke* k, bdd changeset)
    : ta_succ_iterator_product(s, t, k)
  {
    current_condition_ = changeset;
  }

  void
  ta_succ_iterator_product_by_changeset::next_kripke_dest()
  {
    if (!kripke_succ_it_)
      return;

    if (!kripke_current_dest_state)
      {
        kripke_succ_it_->first();
      }
    else
      {
        kripke_current_dest_state->destroy();
        kripke_current_dest_state = nullptr;
        kripke_succ_it_->next();
      }

    // If one of the two successor sets is empty initially, we reset
    // kripke_succ_it_, so that done() can detect this situation easily.  (We
    // choose to reset kripke_succ_it_ because this variable is already used by
    // done().)
    if (kripke_succ_it_->done())
      {
        delete kripke_succ_it_;
        kripke_succ_it_ = nullptr;
        return;
      }

    kripke_current_dest_state = kripke_succ_it_->dst();
    bdd kripke_current_dest_condition = kripke_->state_condition(
        kripke_current_dest_state);

    if (current_condition_ != bdd_setxor(kripke_source_condition,
        kripke_current_dest_condition))
      next_kripke_dest();
    is_stuttering_transition_ = (kripke_source_condition
        == kripke_current_dest_condition);
    if (!is_stuttering_transition_)
      {
        ta_succ_it_ = ta_->succ_iter(source_->get_ta_state(),
            current_condition_);
        ta_succ_it_->first();
      }
  }
}
