// -*- coding: utf-8 -*-
// Copyright (C) 2009-2018 Laboratoire de Recherche et Développement de
// l'Epita (LRDE)
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
#include <map>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <spot/twa/formula2bdd.hh>
#include <spot/tl/print.hh>
#include <spot/twa/taatgba.hh>

namespace spot
{
  /*--------.
  | taa_tgba |
  `--------*/

  taa_tgba::taa_tgba(const bdd_dict_ptr& dict)
    : twa(dict),
      init_(nullptr), state_set_vec_()
  {
  }

  taa_tgba::~taa_tgba()
  {
    ss_vec::iterator j;
    for (j = state_set_vec_.begin(); j != state_set_vec_.end(); ++j)
      delete *j;
    get_dict()->unregister_all_my_variables(this);
  }

  void
  taa_tgba::add_condition(transition* t, formula f)
  {
    t->condition &= formula_to_bdd(f, get_dict(), this);
  }

  state*
  taa_tgba::get_init_state() const
  {
    assert(init_);
    return new spot::set_state(init_);
  }

  twa_succ_iterator*
  taa_tgba::succ_iter(const spot::state* state) const
  {
    const spot::set_state* s = down_cast<const spot::set_state*>(state);
    return new taa_succ_iterator(s->get_state(), acc());
  }

  /*----------.
  | state_set |
  `----------*/

  const taa_tgba::state_set*
  set_state::get_state() const
  {
    return s_;
  }

  int
  set_state::compare(const spot::state* other) const
  {
    const set_state* o = down_cast<const set_state*>(other);

    const taa_tgba::state_set* s1 = get_state();
    const taa_tgba::state_set* s2 = o->get_state();

    if (s1->size() != s2->size())
      return s1->size() - s2->size();

    taa_tgba::state_set::const_iterator it1 = s1->begin();
    taa_tgba::state_set::const_iterator it2 = s2->begin();
    while (it2 != s2->end())
    {
      int i = *it1++ - *it2++;
      if (i != 0)
        return i;
    }
    return 0;
  }

  size_t
  set_state::hash() const
  {
    size_t res = 0;
    taa_tgba::state_set::const_iterator it = s_->begin();
    while (it != s_->end())
    {
      res ^= reinterpret_cast<const char*>(*it++) - static_cast<char*>(nullptr);
      res = wang32_hash(res);
    }
    return res;
  }

  set_state*
  set_state::clone() const
  {
    if (delete_me_ && s_)
      return new spot::set_state(new taa_tgba::state_set(*s_), true);
    else
      return new spot::set_state(s_, false);
  }

  /*--------------.
  | taa_succ_iter |
  `--------------*/

  taa_succ_iterator::taa_succ_iterator(const taa_tgba::state_set* s,
                                       const acc_cond& acc)
    : seen_(), acc_(acc)
  {
    if (s->empty())
    {
      taa_tgba::transition* t = new taa_tgba::transition;
      t->condition = bddtrue;
      t->acceptance_conditions = {};
      t->dst = new taa_tgba::state_set;
      succ_.emplace_back(t);
      return;
    }

    bounds_t bounds;
    for (auto& i: *s)
      bounds.emplace_back(i->begin(), i->end());

    /// Sorting might make the cartesian product faster by not
    /// exploring all possibilities.
    std::sort(bounds.begin(), bounds.end(), distance_sort());

    std::vector<iterator> pos;
    pos.reserve(bounds.size());
    for (auto i: bounds)
      pos.emplace_back(i.first);

    while (pos[0] != bounds[0].second)
    {
      taa_tgba::transition* t = new taa_tgba::transition;
      t->condition = bddtrue;
      t->acceptance_conditions = {};
      taa_tgba::state_set* ss = new taa_tgba::state_set;

      unsigned p;
      for (p = 0; p < pos.size() && t->condition != bddfalse; ++p)
      {
        taa_tgba::state_set::const_iterator j;
        for (j = (*pos[p])->dst->begin(); j != (*pos[p])->dst->end(); ++j)
          if ((*j)->size() > 0) // Remove sink states.
            ss->insert(*j);

        // Fill the new transition.
        t->condition &= (*pos[p])->condition;
        t->acceptance_conditions |= (*pos[p])->acceptance_conditions;
      } // If p != pos.size() we have found a contradiction
      assert(p > 0);
      t->dst = ss;
      // Boxing to be able to insert ss in the map directly.
      spot::set_state* b = new spot::set_state(ss);

      // If no contradiction, then look for another transition to
      // merge with the new one.
      seen_map::iterator i = seen_.end(); // Initialize to silent a g++ warning.
      std::vector<taa_tgba::transition*>::iterator j;
      if (t->condition != bddfalse)
      {
        i = seen_.find(b);
        if (i != seen_.end())
          for (j = i->second.begin(); j != i->second.end(); ++j)
          {
            taa_tgba::transition* current = *j;
            if (*current->dst == *t->dst
                && current->condition == t->condition)
            {
              current->acceptance_conditions &= t->acceptance_conditions;
              break;
            }
            if (*current->dst == *t->dst
                && current->acceptance_conditions == t->acceptance_conditions)
            {
              current->condition |= t->condition;
              break;
            }
        }
      }
      // Mark the new transition as seen and keep it if we have not
      // found any contradiction and no other transition to merge
      // with, or delete it otherwise.
      if (t->condition != bddfalse
          && (i == seen_.end() || j == i->second.end()))
      {
        seen_[b].emplace_back(t);
        if (i != seen_.end())
          delete b;
        succ_.emplace_back(t);
      }
      else
      {
        delete t->dst;
        delete t;
        delete b;
      }

      for (int i = pos.size() - 1; i >= 0; --i)
      {
        if ((i < int(p))
            && (std::distance(pos[i], bounds[i].second) > 1
                || (i == 0 && std::distance(pos[i], bounds[i].second) == 1)))
        {
          ++pos[i];
          break;
        }
        else
          pos[i] = bounds[i].first;
      }
    }
  }

  taa_succ_iterator::~taa_succ_iterator()
  {
    for (seen_map::iterator i = seen_.begin(); i != seen_.end();)
    {
      // Advance the iterator before deleting the state set.
      const spot::set_state* s = i->first;
      ++i;
      delete s;
    }
    for (unsigned i = 0; i < succ_.size(); ++i)
    {
      delete succ_[i]->dst;
      delete succ_[i];
    }
  }

  bool
  taa_succ_iterator::first()
  {
    i_ = succ_.begin();
    return i_ != succ_.end();
  }

  bool
  taa_succ_iterator::next()
  {
    ++i_;
    return i_ != succ_.end();
  }

  bool
  taa_succ_iterator::done() const
  {
    return i_ == succ_.end();
  }

  spot::set_state*
  taa_succ_iterator::dst() const
  {
    assert(!done());
    return new spot::set_state(new taa_tgba::state_set(*(*i_)->dst), true);
  }

  bdd
  taa_succ_iterator::cond() const
  {
    assert(!done());
    return (*i_)->condition;
  }

  acc_cond::mark_t
  taa_succ_iterator::acc() const
  {
    assert(!done());
    return acc_.comp((*i_)->acceptance_conditions);
  }

  /*----------------.
  | taa_tgba_string |
  `----------------*/

  std::string
  taa_tgba_string::label_to_string(const label_t& label) const
  {
    return label;
  }

  /*-----------------.
  | taa_tgba_formula |
  `-----------------*/

  std::string
  taa_tgba_formula::label_to_string(const label_t& label) const
  {
    return str_psl(label);
  }
}
