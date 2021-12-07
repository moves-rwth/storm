// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2014, 2015, 2016, 2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <spot/misc/fixpool.hh>

namespace spot
{

  /// \ingroup twa_on_the_fly_algorithms
  /// \brief A state for spot::twa_product.
  ///
  /// This state is in fact a pair of state: the state from the left
  /// automaton and that of the right.
  class SPOT_API state_product final: public state
  {
  public:
    /// \brief Constructor
    /// \param left The state from the left automaton.
    /// \param right The state from the right automaton.
    /// \param pool The pool from which the state was allocated.
    /// These states are acquired by spot::state_product, and will
    /// be destroyed on destruction.
    state_product(const state* left,
                  const state* right,
                  fixed_size_pool* pool)
      :        left_(left), right_(right), count_(1), pool_(pool)
    {
    }

    virtual void destroy() const override;

    const state*
    left() const
    {
      return left_;
    }

    const state*
    right() const
    {
      return right_;
    }

    virtual int compare(const state* other) const override;
    virtual size_t hash() const override;
    virtual state_product* clone() const override;

  private:
    const state* left_;                ///< State from the left automaton.
    const state* right_;        ///< State from the right automaton.
    mutable unsigned count_;
    fixed_size_pool* pool_;

    virtual ~state_product();
    state_product(const state_product& o) = delete;
  };


  /// \brief A lazy product.  (States are computed on the fly.)
  class SPOT_API twa_product: public twa
  {
  public:
    /// \brief Constructor.
    /// \param left The left automata in the product.
    /// \param right The right automata in the product.
    /// Do not be fooled by these arguments: a product is commutative.
    twa_product(const const_twa_ptr& left, const const_twa_ptr& right);

    virtual ~twa_product();

    virtual const state* get_init_state() const override;

    virtual twa_succ_iterator*
    succ_iter(const state* state) const override;

    virtual std::string format_state(const state* state) const override;

    virtual state* project_state(const state* s, const const_twa_ptr& t)
      const override;

    const acc_cond& left_acc() const;
    const acc_cond& right_acc() const;

  protected:
    const_twa_ptr left_;
    const_twa_ptr right_;
    bool left_kripke_;
    fixed_size_pool pool_;

  private:
    // Disallow copy.
    twa_product(const twa_product&) = delete;
    twa_product& operator=(const twa_product&) = delete;
  };

  /// \brief A lazy product with different initial states.
  class SPOT_API twa_product_init final: public twa_product
  {
  public:
    twa_product_init(const const_twa_ptr& left, const const_twa_ptr& right,
                      const state* left_init, const state* right_init);
    virtual const state* get_init_state() const override;
  protected:
    const state* left_init_;
    const state* right_init_;
  };

  /// \brief on-the-fly TGBA product
  inline twa_product_ptr otf_product(const const_twa_ptr& left,
                                      const const_twa_ptr& right)
  {
    return SPOT_make_shared_enabled__(twa_product, left, right);
  }

  /// \brief on-the-fly TGBA product with forced initial states
  inline twa_product_ptr otf_product_at(const const_twa_ptr& left,
                                        const const_twa_ptr& right,
                                        const state* left_init,
                                        const state* right_init)
  {
    return SPOT_make_shared_enabled__(twa_product_init,
                                      left, right, left_init, right_init);
  }
}
