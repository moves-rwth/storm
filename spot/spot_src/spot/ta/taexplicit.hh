// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2018 Laboratoire
// de Recherche et Développement de l'Epita (LRDE).
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

#pragma once

#include <spot/misc/hash.hh>
#include <list>
#include <spot/twa/twa.hh>
#include <set>
#include <spot/tl/formula.hh>
#include <cassert>
#include <spot/misc/bddlt.hh>
#include <spot/ta/ta.hh>

namespace spot
{
  // Forward declarations.  See below.
  class state_ta_explicit;
  class ta_explicit;

  /// Explicit representation of a spot::ta.
  /// \ingroup ta_representation
  class SPOT_API ta_explicit : public ta
  {
  public:
    ta_explicit(const const_twa_ptr& tgba,
                unsigned n_acc,
                state_ta_explicit* artificial_initial_state = nullptr);

    const_twa_ptr
    get_tgba() const;

    state_ta_explicit*
    add_state(state_ta_explicit* s);

    void
    add_to_initial_states_set(state* s, bdd condition = bddfalse);

    void
    create_transition(state_ta_explicit* source, bdd condition,
                      acc_cond::mark_t acceptance_conditions,
                      const state_ta_explicit* dest,
                      bool add_at_beginning = false);

    void
    delete_stuttering_transitions();
    // ta interface
    virtual
    ~ta_explicit();
    virtual const_states_set_t get_initial_states_set() const override;

    virtual ta_succ_iterator* succ_iter(const spot::state* s) const override;

    virtual ta_succ_iterator*
    succ_iter(const spot::state* s, bdd condition) const override;

    bdd_dict_ptr get_dict() const;

    virtual std::string
    format_state(const spot::state* s) const override;

    virtual bool
    is_accepting_state(const spot::state* s) const override;

    virtual bool
    is_livelock_accepting_state(const spot::state* s) const override;

    virtual bool
    is_initial_state(const spot::state* s) const override;

    virtual bdd
    get_state_condition(const spot::state* s) const override;

    virtual void
    free_state(const spot::state* s) const override;

    virtual spot::state*
    get_artificial_initial_state() const override
    {
      return (spot::state*) artificial_initial_state_;
    }

    void
    set_artificial_initial_state(state_ta_explicit* s)
    {
      artificial_initial_state_ = s;

    }

    void
    delete_stuttering_and_hole_successors(const spot::state* s);

    ta::states_set_t
    get_states_set()
    {
      return states_set_;
    }

  private:
    // Disallow copy.
    ta_explicit(const ta_explicit& other) = delete;
    ta_explicit& operator=(const ta_explicit& other) = delete;

    const_twa_ptr tgba_;
    state_ta_explicit* artificial_initial_state_;
    ta::states_set_t states_set_;
    ta::const_states_set_t initial_states_set_;
  };

  /// states used by spot::ta_explicit.
  /// \ingroup ta_representation
  class SPOT_API state_ta_explicit final: public spot::state
  {
#ifndef SWIG
  public:

    /// Explicit transitions.
    struct transition
    {
      bdd condition;
      acc_cond::mark_t acceptance_conditions;
      const state_ta_explicit* dest;
    };

    typedef std::list<transition*> transitions;

    state_ta_explicit(const state* tgba_state, const bdd tgba_condition,
                      bool is_initial_state = false,
                      bool is_accepting_state = false,
                      bool is_livelock_accepting_state = false,
                      transitions* trans = nullptr) :
      tgba_state_(tgba_state), tgba_condition_(tgba_condition),
          is_initial_state_(is_initial_state), is_accepting_state_(
              is_accepting_state), is_livelock_accepting_state_(
              is_livelock_accepting_state), transitions_(trans)
    {
    }

    virtual int compare(const spot::state* other) const override;
    virtual size_t hash() const override;
    virtual state_ta_explicit* clone() const override;

    virtual void destroy() const override
    {
    }

    virtual
    ~state_ta_explicit()
    {
    }

    transitions*
    get_transitions() const;

    // return transitions filtred by condition
    transitions*
    get_transitions(bdd condition) const;

    void
    add_transition(transition* t, bool add_at_beginning = false);

    const state*
    get_tgba_state() const;
    const bdd
    get_tgba_condition() const;
    bool
    is_accepting_state() const;
    void
    set_accepting_state(bool is_accepting_state);
    bool
    is_livelock_accepting_state() const;
    void
    set_livelock_accepting_state(bool is_livelock_accepting_state);

    bool
    is_initial_state() const;
    void
    set_initial_state(bool is_initial_state);

    /// \brief Return true if the state has no successors
    bool
    is_hole_state() const;

    /// \brief Remove stuttering transitions
    /// and transitions leading to states having no successors
    void
    delete_stuttering_and_hole_successors();

    void
    free_transitions();

    state_ta_explicit* stuttering_reachable_livelock;
  private:
    const state* tgba_state_;
    const bdd tgba_condition_;
    bool is_initial_state_;
    bool is_accepting_state_;
    bool is_livelock_accepting_state_;
    transitions* transitions_;
    std::unordered_map<int, transitions*, std::hash<int>>
      transitions_by_condition;
#endif // !SWIG
  };

  /// Successor iterators used by spot::ta_explicit.
  class SPOT_API ta_explicit_succ_iterator final: public ta_succ_iterator
  {
  public:
    ta_explicit_succ_iterator(const state_ta_explicit* s);

    ta_explicit_succ_iterator(const state_ta_explicit* s, bdd condition);

    virtual bool first() override;
    virtual bool next() override;
    virtual bool done() const override;

    virtual const state* dst() const override;
    virtual bdd cond() const override;

    virtual acc_cond::mark_t acc() const override;

  private:
    state_ta_explicit::transitions* transitions_;
    state_ta_explicit::transitions::const_iterator i_;
  };

  typedef std::shared_ptr<ta_explicit> ta_explicit_ptr;
  typedef std::shared_ptr<const ta_explicit> const_ta_explicit_ptr;

  inline ta_explicit_ptr
  make_ta_explicit(const const_twa_ptr& tgba,
                   unsigned n_acc,
                   state_ta_explicit* artificial_initial_state = nullptr)
  {
    return std::make_shared<ta_explicit>(tgba, n_acc, artificial_initial_state);
  }
}
