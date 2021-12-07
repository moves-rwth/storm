// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014-2018 Laboratoire de Recherche et Développement de
// l Epita (LRDE).
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

#include <spot/ta/tgtaproduct.hh>
#include <string>
#include <cassert>
#include <spot/misc/hashfunc.hh>
#include <spot/kripke/kripke.hh>

namespace spot
{

  ////////////////////////////////////////////////////////////
  // tgta_succ_iterator_product


  ////////////////////////////////////////////////////////////
  // tgta_product

  tgta_product::tgta_product(const const_kripke_ptr& left,
                             const const_tgta_ptr& right):
    twa_product(left, right)
  {
  }

  const state*
  tgta_product::get_init_state() const
  {
    fixed_size_pool* p = const_cast<fixed_size_pool*> (&pool_);
    return new (p->allocate()) state_product(left_->get_init_state(),
        right_->get_init_state(), p);
  }

  twa_succ_iterator*
  tgta_product::succ_iter(const state* state) const
  {
    const state_product* s = down_cast<const state_product*> (state);

    fixed_size_pool* p = const_cast<fixed_size_pool*> (&pool_);

    auto l = std::static_pointer_cast<const kripke>(left_);
    auto r = std::static_pointer_cast<const tgta>(right_);
    return new tgta_succ_iterator_product(s, l, r, p);
  }

  ////////////////////////////////////////////////////////////
  // tgbtgta_succ_iterator_product
  tgta_succ_iterator_product::tgta_succ_iterator_product(
      const state_product* s,
      const const_kripke_ptr& k, const const_tgta_ptr& t,
      fixed_size_pool* pool)
    : source_(s), tgta_(t), kripke_(k), pool_(pool)
  {

    const state* tgta_init_state = tgta_->get_init_state();
    if ((s->right())->compare(tgta_init_state) == 0)
      source_ = nullptr;

    if (!source_)
      {
        kripke_succ_it_ = nullptr;
        kripke_current_dest_state = kripke_->get_init_state();
        current_condition_
            = kripke_->state_condition(kripke_current_dest_state);
        tgta_succ_it_ = tgta_->succ_iter_by_changeset(
            tgta_init_state, current_condition_);
        tgta_succ_it_->first();
        trace
          << "*** tgta_succ_it_->done() = ***" << tgta_succ_it_->done()
              << std::endl;

      }
    else
      {
        kripke_source_condition = kripke_->state_condition(s->left());
        kripke_succ_it_ = kripke_->succ_iter(s->left());
        kripke_current_dest_state = nullptr;
        tgta_succ_it_ = nullptr;
      }

    tgta_init_state->destroy();
    current_state_ = nullptr;
  }

  tgta_succ_iterator_product::~tgta_succ_iterator_product()
  {
    // ta_->free_state(current_state_);
    if (current_state_)
      current_state_->destroy();
    current_state_ = nullptr;
    delete tgta_succ_it_;
    delete kripke_succ_it_;
    if (kripke_current_dest_state)
      kripke_current_dest_state->destroy();
  }

  void
  tgta_succ_iterator_product::step_()
  {
    if (!tgta_succ_it_->done())
      tgta_succ_it_->next();
    if (tgta_succ_it_->done())
      {
        delete tgta_succ_it_;
        tgta_succ_it_ = nullptr;
        next_kripke_dest();
      }
  }

  void
  tgta_succ_iterator_product::next_kripke_dest()
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
    // kripke_succ_it_, so that done() can detect this situation
    // easily.  (We choose to reset kripke_succ_it_ because this
    // variable is already used by done().)
    if (kripke_succ_it_->done())
      {
        delete kripke_succ_it_;
        kripke_succ_it_ = nullptr;
        return;
      }

    kripke_current_dest_state = kripke_succ_it_->dst();
    bdd kripke_current_dest_condition = kripke_->state_condition(
        kripke_current_dest_state);

    current_condition_ = bdd_setxor(kripke_source_condition,
        kripke_current_dest_condition);
    tgta_succ_it_ = tgta_->succ_iter_by_changeset(source_->right(),
        current_condition_);
    tgta_succ_it_->first();

  }

  bool
  tgta_succ_iterator_product::first()
  {

    next_kripke_dest();
    trace << "*** first() .... if(done()) = ***" << done() << std::endl;
    return find_next_succ_();
  }

  bool
  tgta_succ_iterator_product::next()
  {
    current_state_->destroy();
    current_state_ = nullptr;

    step_();

    trace << "*** next() .... if(done()) = ***" << done() << std::endl;

    return find_next_succ_();
  }

  bool
  tgta_succ_iterator_product::find_next_succ_()
  {
    while (!done())
      {
        if (!tgta_succ_it_->done())
          {
            current_state_ = new (pool_->allocate()) state_product(
                kripke_current_dest_state->clone(),
                tgta_succ_it_->dst(), pool_);
            current_acceptance_conditions_
              = tgta_succ_it_->acc();
            return true;
          }

        step_();
      }
    return false;
  }

  bool
  tgta_succ_iterator_product::done() const
  {
    if (!source_)
      {
        return !tgta_succ_it_ || tgta_succ_it_->done();
      }
    else
      {
        return !kripke_succ_it_ || kripke_succ_it_->done();
      }

  }

  state_product*
  tgta_succ_iterator_product::dst() const
  {
    trace
      << "*** dst() .... if(done()) = ***" << done() << std::endl;
    return current_state_->clone();
  }

  bdd
  tgta_succ_iterator_product::cond() const
  {
    return current_condition_;
  }

  acc_cond::mark_t
  tgta_succ_iterator_product::acc() const
  {
    return current_acceptance_conditions_;
  }

}
