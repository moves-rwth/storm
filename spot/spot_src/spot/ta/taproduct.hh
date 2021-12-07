// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2012, 2013, 2014, 2016 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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

#include <spot/ta/ta.hh>
#include <spot/kripke/kripke.hh>

namespace spot
{

  /// \ingroup ta_emptiness_check
  /// \brief A state for spot::ta_product.
  ///
  /// This state is in fact a pair of state: the state from the TA
  /// automaton and that of Kripke structure.
  class SPOT_API state_ta_product : public state
  {
  public:
    /// \brief Constructor
    /// \param ta_state The state from the ta automaton.
    /// \param kripke_state The state from Kripke structure.
    state_ta_product(const state* ta_state, const state* kripke_state) :
      ta_state_(ta_state), kripke_state_(kripke_state)
    {
    }

    state_ta_product(const state_ta_product& o) = delete;

    virtual
    ~state_ta_product();

    const state*
    get_ta_state() const
    {
      return ta_state_;
    }

    const state*
    get_kripke_state() const
    {
      return kripke_state_;
    }

    virtual int
    compare(const state* other) const override;
    virtual size_t
    hash() const override;
    virtual state_ta_product*
    clone() const override;

  private:
    const state* ta_state_; ///< State from the ta automaton.
    const state* kripke_state_; ///< State from the kripke structure.
  };

  /// \brief Iterate over the successors of a product computed on the fly.
  class SPOT_API ta_succ_iterator_product : public ta_succ_iterator
  {
  public:
    ta_succ_iterator_product(const state_ta_product* s, const ta* t,
        const kripke* k);

    virtual
    ~ta_succ_iterator_product();

    // iteration
    virtual bool first() override;
    virtual bool next() override;
    virtual bool done() const override;

    // inspection
    virtual state_ta_product* dst() const override;
    virtual bdd cond() const override;
    virtual acc_cond::mark_t acc() const override;

    /// \brief Return true if the changeset of the current transition is empty
    bool is_stuttering_transition() const;

  protected:
    //@{
    /// Internal routines to advance to the next successor.
    void step_();
    bool next_non_stuttering_();

    /// \brief Move to the next successor in the kripke structure
    void
    next_kripke_dest();

    //@}

  protected:
    const state_ta_product* source_;
    const ta* ta_;
    const kripke* kripke_;
    ta_succ_iterator* ta_succ_it_;
    twa_succ_iterator* kripke_succ_it_;
    const state_ta_product* current_state_;
    bdd current_condition_;
    acc_cond::mark_t current_acceptance_conditions_;
    bool is_stuttering_transition_;
    bdd kripke_source_condition;
    const state* kripke_current_dest_state;

  };

  /// \ingroup ta_emptiness_check
  /// \brief A lazy product between a Testing automaton and a Kripke structure.
  /// (States are computed on the fly.)
  class SPOT_API ta_product final: public ta
  {
  public:
    /// \brief Constructor.
    /// \param testing_automaton The TA component in the product.
    /// \param kripke_structure The Kripke component in the product.
    ta_product(const const_ta_ptr& testing_automaton,
               const const_kripke_ptr& kripke_structure);

    virtual
    ~ta_product();

    virtual ta::const_states_set_t
    get_initial_states_set() const override;

    virtual ta_succ_iterator_product*
    succ_iter(const spot::state* s) const override;

    virtual ta_succ_iterator_product*
    succ_iter(const spot::state* s, bdd changeset) const override;

    bdd_dict_ptr
    get_dict() const;

    virtual std::string
    format_state(const spot::state* s) const override;

    virtual bool
    is_accepting_state(const spot::state* s) const override;

    virtual bool
    is_livelock_accepting_state(const spot::state* s) const override;

    virtual bool
    is_initial_state(const spot::state* s) const override;

    /// \brief Return true if the state \a s has no succeseurs
    /// in the TA automaton (the TA component of the product automaton)
    bool
    is_hole_state_in_ta_component(const spot::state* s) const;

    virtual bdd
    get_state_condition(const spot::state* s) const override;

    virtual void
    free_state(const spot::state* s) const override;

    const const_ta_ptr&
    get_ta() const
    {
      return ta_;
    }

    const const_kripke_ptr&
    get_kripke() const
    {
      return kripke_;
    }

  private:
    bdd_dict_ptr dict_;
    const_ta_ptr ta_;
    const_kripke_ptr kripke_;

    // Disallow copy.
    ta_product(const ta_product&) = delete;
    ta_product& operator=(const ta_product&) = delete;
  };


  typedef std::shared_ptr<ta_product> ta_product_ptr;
  typedef std::shared_ptr<const ta_product> const_ta_product_ptr;
  inline ta_product_ptr product(const const_ta_ptr& testing_automaton,
                                const const_kripke_ptr& kripke_structure)
  {
    return std::make_shared<ta_product>(testing_automaton, kripke_structure);
  }

  class SPOT_API ta_succ_iterator_product_by_changeset :
    public ta_succ_iterator_product
  {
  public:
    ta_succ_iterator_product_by_changeset(const state_ta_product* s,
                                          const ta* t, const kripke* k,
                                          bdd changeset);

    /// \brief Move to the next successor in the Kripke structure
    void next_kripke_dest();
  };
}
