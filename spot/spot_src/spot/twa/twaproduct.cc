// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011-2012, 2014-2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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

#include "config.h"
#include <spot/twa/twaproduct.hh>
#include <string>
#include <cassert>
#include <spot/misc/hashfunc.hh>
#include <spot/kripke/kripke.hh>

namespace spot
{

  ////////////////////////////////////////////////////////////
  // state_product

  state_product::~state_product()
  {
    left_->destroy();
    right_->destroy();
  }

  void
  state_product::destroy() const
  {
    if (--count_)
      return;
    fixed_size_pool* p = pool_;
    this->~state_product();
    p->deallocate(const_cast<state_product*>(this));
  }

  int
  state_product::compare(const state* other) const
  {
    const state_product* o = down_cast<const state_product*>(other);
    int res = left_->compare(o->left());
    if (res != 0)
      return res;
    return right_->compare(o->right());
  }

  size_t
  state_product::hash() const
  {
    // We assume that size_t is 32-bit wide.
    return wang32_hash(left_->hash()) ^ wang32_hash(right_->hash());
  }

  state_product*
  state_product::clone() const
  {
    ++count_;
    return const_cast<state_product*>(this);
  }

  ////////////////////////////////////////////////////////////
  // twa_succ_iterator_product

  namespace
  {

    class twa_succ_iterator_product_common: public twa_succ_iterator
    {
    public:
      twa_succ_iterator_product_common(twa_succ_iterator* left,
                                        twa_succ_iterator* right,
                                        const twa_product* prod,
                                        fixed_size_pool* pool)
        : left_(left), right_(right), prod_(prod), pool_(pool)
      {
      }

      void recycle(const const_twa_ptr& l, twa_succ_iterator* left,
                   const_twa_ptr r, twa_succ_iterator* right)
      {
        l->release_iter(left_);
        left_ = left;
        r->release_iter(right_);
        right_ = right;
      }

      virtual ~twa_succ_iterator_product_common()
      {
        delete left_;
        delete right_;
      }

      virtual bool next_non_false_() = 0;

      bool first() override
      {
        if (!right_)
          return false;

        // If one of the two successor sets is empty initially, we
        // reset right_, so that done() can detect this situation
        // easily.  (We choose to reset right_ because this variable
        // is already used by done().)
        if (!(left_->first() && right_->first()))
          {
            delete right_;
            right_ = nullptr;
            return false;
          }
        return next_non_false_();
      }

      bool done() const override
      {
        return !right_ || right_->done();
      }

      const state_product* dst() const override
      {
        return new(pool_->allocate()) state_product(left_->dst(),
                                                    right_->dst(),
                                                    pool_);
      }

    protected:
      twa_succ_iterator* left_;
      twa_succ_iterator* right_;
      const twa_product* prod_;
      fixed_size_pool* pool_;
      friend class spot::twa_product;
    };


    /// \brief Iterate over the successors of a product computed on the fly.
    class twa_succ_iterator_product final:
      public twa_succ_iterator_product_common
    {
    public:
      twa_succ_iterator_product(twa_succ_iterator* left,
                                 twa_succ_iterator* right,
                                 const twa_product* prod,
                                 fixed_size_pool* pool)
        : twa_succ_iterator_product_common(left, right, prod, pool)
      {
      }

      virtual ~twa_succ_iterator_product()
      {
      }

      bool step_()
      {
        if (left_->next())
          return true;
        left_->first();
        return right_->next();
      }

      bool next_non_false_() override
      {
        assert(!done());
        do
          {
            bdd l = left_->cond();
            bdd r = right_->cond();
            bdd current_cond = l & r;

            if (current_cond != bddfalse)
              {
                current_cond_ = current_cond;
                return true;
              }
          }
        while (step_());
        return false;
      }

      bool next() override
      {
        if (step_())
          return next_non_false_();
        return false;
      }

      bdd cond() const override
      {
        return current_cond_;
      }

      acc_cond::mark_t acc() const override
      {
        return left_->acc() | (right_->acc() << prod_->left_acc().num_sets());
      }

    protected:
      bdd current_cond_;
    };

    /// Iterate over the successors of a product computed on the fly.
    /// This one assumes that LEFT is an iterator over a Kripke structure
    class twa_succ_iterator_product_kripke final:
      public twa_succ_iterator_product_common
    {
    public:
      twa_succ_iterator_product_kripke(twa_succ_iterator* left,
                                        twa_succ_iterator* right,
                                        const twa_product* prod,
                                        fixed_size_pool* pool)
        : twa_succ_iterator_product_common(left, right, prod, pool)
      {
      }

      virtual ~twa_succ_iterator_product_kripke()
      {
      }

      bool next_non_false_() override
      {
        // All the transitions of left_ iterator have the
        // same label, because it is a Kripke structure.
        bdd l = left_->cond();
        assert(!right_->done());
        do
          {
            bdd r = right_->cond();
            bdd current_cond = l & r;

            if (current_cond != bddfalse)
              {
                current_cond_ = current_cond;
                return true;
              }
          }
        while (right_->next());
        return false;
      }

      bool next() override
      {
        if (left_->next())
          return true;
        left_->first();
        if (right_->next())
          return next_non_false_();
        return false;
      }

      bdd cond() const override
      {
        return current_cond_;
      }

      acc_cond::mark_t acc() const override
      {
        return right_->acc();
      }

    protected:
      bdd current_cond_;
    };

  } // anonymous

  ////////////////////////////////////////////////////////////
  // twa_product

  twa_product::twa_product(const const_twa_ptr& left,
                           const const_twa_ptr& right)
    : twa(left->get_dict()), left_(left), right_(right),
      pool_(sizeof(state_product))
  {
    if (left->get_dict() != right->get_dict())
      throw std::runtime_error("twa_product: left and right automata should "
                               "share their bdd_dict");
    assert(get_dict() == right_->get_dict());

    // If one of the side is a Kripke structure, it is easier to deal
    // with (we don't have to fix the acceptance conditions, and
    // computing the successors can be improved a bit).
    if (dynamic_cast<const kripke*>(left_.get()))
      {
        left_kripke_ = true;
      }
    else if (dynamic_cast<const kripke*>(right_.get()))
      {
        std::swap(left_, right_);
        left_kripke_ = true;
      }
    else
      {
        left_kripke_ = false;
      }

    copy_ap_of(left_);
    copy_ap_of(right_);

    assert(num_sets() == 0);
    auto left_num = left->num_sets();
    auto right_acc = right->get_acceptance() << left_num;
    right_acc &= left->get_acceptance();
    set_acceptance(left_num + right->num_sets(), right_acc);
  }

  twa_product::~twa_product()
  {
    // Make sure we delete the iterator cache before erasing the two
    // automata (by reference counting).
    delete iter_cache_;
    iter_cache_ = nullptr;
  }

  const state*
  twa_product::get_init_state() const
  {
    fixed_size_pool* p = const_cast<fixed_size_pool*>(&pool_);
    return new(p->allocate()) state_product(left_->get_init_state(),
                                            right_->get_init_state(), p);
  }

  twa_succ_iterator*
  twa_product::succ_iter(const state* state) const
  {
    const state_product* s = down_cast<const state_product*>(state);
    twa_succ_iterator* li = left_->succ_iter(s->left());
    twa_succ_iterator* ri = right_->succ_iter(s->right());

    if (iter_cache_)
      {
        twa_succ_iterator_product_common* it =
          down_cast<twa_succ_iterator_product_common*>(iter_cache_);
        it->recycle(left_, li, right_, ri);
        iter_cache_ = nullptr;
        return it;
      }

    fixed_size_pool* p = const_cast<fixed_size_pool*>(&pool_);
    if (left_kripke_)
      return new twa_succ_iterator_product_kripke(li, ri, this, p);
    else
      return new twa_succ_iterator_product(li, ri, this, p);
  }

  const acc_cond& twa_product::left_acc() const
  {
    return left_->acc();
  }

  const acc_cond& twa_product::right_acc() const
  {
    return right_->acc();
  }

  std::string
  twa_product::format_state(const state* state) const
  {
    const state_product* s = down_cast<const state_product*>(state);
    return (left_->format_state(s->left())
            + " * "
            + right_->format_state(s->right()));
  }

  state*
  twa_product::project_state(const state* s, const const_twa_ptr& t) const
  {
    const state_product* s2 = down_cast<const state_product*>(s);
    if (t.get() == this)
      return s2->clone();
    state* res = left_->project_state(s2->left(), t);
    if (res)
      return res;
    return right_->project_state(s2->right(), t);
  }

  //////////////////////////////////////////////////////////////////////
  // twa_product_init

  twa_product_init::twa_product_init(const const_twa_ptr& left,
                                       const const_twa_ptr& right,
                                       const state* left_init,
                                       const state* right_init)
    : twa_product(left, right),
      left_init_(left_init), right_init_(right_init)
  {
    if (left_ != left)
      std::swap(left_init_, right_init_);
  }

  const state*
  twa_product_init::get_init_state() const
  {
    fixed_size_pool* p = const_cast<fixed_size_pool*>(&pool_);
    return new(p->allocate()) state_product(left_init_->clone(),
                                            right_init_->clone(), p);
  }

}
