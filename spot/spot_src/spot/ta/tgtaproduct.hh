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

#include <spot/twa/twa.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/misc/fixpool.hh>
#include <spot/kripke/kripke.hh>
#include <spot/ta/tgta.hh>

namespace spot
{

  /// \brief A lazy product.  (States are computed on the fly.)
  class SPOT_API tgta_product : public twa_product
  {
  public:
    tgta_product(const const_kripke_ptr& left,
                 const const_tgta_ptr& right);

    virtual const state* get_init_state() const override;

    virtual twa_succ_iterator*
    succ_iter(const state* local_state) const override;
  };

  inline twa_ptr product(const const_kripke_ptr& left,
                          const const_tgta_ptr& right)
  {
    return std::make_shared<tgta_product>(left, right);
  }

  /// \brief Iterate over the successors of a product computed on the fly.
  class SPOT_API tgta_succ_iterator_product final : public twa_succ_iterator
  {
  public:
    tgta_succ_iterator_product(const state_product* s,
                               const const_kripke_ptr& k,
                               const const_tgta_ptr& tgta,
                               fixed_size_pool* pool);

    virtual
    ~tgta_succ_iterator_product();

    // iteration
    bool first() override;
    bool next() override;
    bool done() const override;

    // inspection
    state_product* dst() const override;
    bdd cond() const override;
    acc_cond::mark_t acc() const override;

  private:
    //@{
    /// Internal routines to advance to the next successor.
    void
    step_();
    bool find_next_succ_();

    void
    next_kripke_dest();

    //@}

  protected:
    const state_product* source_;
    const_tgta_ptr tgta_;
    const_kripke_ptr kripke_;
    fixed_size_pool* pool_;
    twa_succ_iterator* tgta_succ_it_;
    twa_succ_iterator* kripke_succ_it_;
    state_product* current_state_;
    bdd current_condition_;
    acc_cond::mark_t current_acceptance_conditions_;
    bdd kripke_source_condition;
    const state* kripke_current_dest_state;
  };
}
