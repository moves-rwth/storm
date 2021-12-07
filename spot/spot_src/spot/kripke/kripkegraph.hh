// -*- coding: utf-8 -*-
// Copyright (C) 2011-2019 Laboratoire de Recherche et Développement de
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

#pragma once

#include <iosfwd>
#include <spot/kripke/kripke.hh>
#include <spot/graph/graph.hh>
#include <spot/tl/formula.hh>

namespace spot
{
  /// \brief Concrete class for kripke_graph states.
  struct SPOT_API kripke_graph_state: public spot::state
  {
  public:
    kripke_graph_state(bdd cond = bddfalse) noexcept
      : cond_(cond)
    {
    }

    kripke_graph_state(const kripke_graph_state& other) noexcept
      : cond_(other.cond_)
    {
    }

    kripke_graph_state& operator=(const kripke_graph_state& other) noexcept
    {
      cond_ = other.cond_;
      return *this;
    }

    virtual ~kripke_graph_state() noexcept
    {
    }

    virtual int compare(const spot::state* other) const override
    {
      auto o = down_cast<const kripke_graph_state*>(other);

      // Do not simply return "other - this", it might not fit in an int.
      if (o < this)
        return -1;
      if (o > this)
        return 1;
      return 0;
    }

    virtual size_t hash() const override
    {
      return
        reinterpret_cast<const char*>(this) - static_cast<const char*>(nullptr);
    }

    virtual kripke_graph_state*
    clone() const override
    {
      return const_cast<kripke_graph_state*>(this);
    }

    virtual void destroy() const override
    {
    }

    bdd cond() const
    {
      return cond_;
    }

    void cond(bdd c)
    {
      cond_ = c;
    }

  private:
    bdd cond_;
  };

  template<class Graph>
  class SPOT_API kripke_graph_succ_iterator final: public kripke_succ_iterator
  {
  private:
    typedef typename Graph::edge edge;
    typedef typename Graph::state_data_t state;
    const Graph* g_;
    edge t_;
    edge p_;
  public:
    kripke_graph_succ_iterator(const Graph* g,
                               const typename Graph::state_storage_t* s):
      kripke_succ_iterator(s->cond()),
      g_(g),
      t_(s->succ)
    {
    }

    ~kripke_graph_succ_iterator()
    {
    }

    void recycle(const typename Graph::state_storage_t* s)
    {
      cond_ = s->cond();
      t_ = s->succ;
    }

    virtual bool first() override
    {
      p_ = t_;
      return p_;
    }

    virtual bool next() override
    {
      p_ = g_->edge_storage(p_).next_succ;
      return p_;
    }

    virtual bool done() const override
    {
      return !p_;
    }

    virtual kripke_graph_state* dst() const override
    {
      SPOT_ASSERT(!done());
      return const_cast<kripke_graph_state*>
        (&g_->state_data(g_->edge_storage(p_).dst));
    }
  };


  /// \class kripke_graph
  /// \brief Kripke Structure.
  class SPOT_API kripke_graph final : public kripke
  {
  public:
    typedef digraph<kripke_graph_state, void> graph_t;
    // We avoid using graph_t::edge_storage_t because graph_t is not
    // instantiated in the SWIG bindings, and SWIG would therefore
    // handle graph_t::edge_storage_t as an abstract type.
    typedef internal::edge_storage<unsigned, unsigned, unsigned,
                                         internal::boxed_label
                                         <void, true>>
      edge_storage_t;
    static_assert(std::is_same<typename graph_t::edge_storage_t,
                  edge_storage_t>::value, "type mismatch");

    typedef internal::distate_storage<unsigned,
            internal::boxed_label<kripke_graph_state, false>>
      state_storage_t;
    static_assert(std::is_same<typename graph_t::state_storage_t,
                  state_storage_t>::value, "type mismatch");
    typedef std::vector<state_storage_t> state_vector;

    // We avoid using graph_t::state for the very same reason.
    typedef unsigned state_num;
    static_assert(std::is_same<typename graph_t::state, state_num>::value,
                  "type mismatch");

  protected:
    graph_t g_;
    mutable unsigned init_number_;
  public:
    kripke_graph(const bdd_dict_ptr& d)
      : kripke(d), init_number_(0)
    {
    }

    virtual ~kripke_graph()
    {
      get_dict()->unregister_all_my_variables(this);
    }

    unsigned num_states() const
    {
      return g_.num_states();
    }

    unsigned num_edges() const
    {
      return g_.num_edges();
    }

    void set_init_state(state_num s)
    {
      if (SPOT_UNLIKELY(s >= num_states()))
        throw std::invalid_argument
          ("set_init_state() called with nonexisiting state");
      init_number_ = s;
    }

    state_num get_init_state_number() const
    {
      // If the kripke has no state, it has no initial state.
      if (num_states() == 0)
        throw std::runtime_error("kripke has no state at all");
      return init_number_;
    }

    virtual const kripke_graph_state* get_init_state() const override
    {
      return state_from_number(get_init_state_number());
    }

    /// \brief Allow to get an iterator on the state we passed in
    /// parameter.
    virtual kripke_graph_succ_iterator<graph_t>*
    succ_iter(const spot::state* st) const override
    {
      auto s = down_cast<const typename graph_t::state_storage_t*>(st);
      SPOT_ASSERT(!s->succ || g_.is_valid_edge(s->succ));

      if (this->iter_cache_)
        {
          auto it =
            down_cast<kripke_graph_succ_iterator<graph_t>*>(this->iter_cache_);
          it->recycle(s);
          this->iter_cache_ = nullptr;
          return it;
        }
      return new kripke_graph_succ_iterator<graph_t>(&g_, s);

    }

    state_num
    state_number(const state* st) const
    {
      auto s = down_cast<const typename graph_t::state_storage_t*>(st);
      return s - &g_.state_storage(0);
    }

    const kripke_graph_state*
    state_from_number(state_num n) const
    {
      return &g_.state_data(n);
    }

    kripke_graph_state*
    state_from_number(state_num n)
    {
      return &g_.state_data(n);
    }

    std::string format_state(unsigned n) const
    {
      auto named = get_named_prop<std::vector<std::string>>("state-names");
      if (named && n < named->size())
        return (*named)[n];

      return std::to_string(n);
    }

    virtual std::string format_state(const state* st) const override
    {
      return format_state(state_number(st));
    }

    /// \brief Get the condition on the state
    virtual bdd state_condition(const state* s) const override
    {
      auto gs = down_cast<const kripke_graph_state*>(s);
      return gs->cond();
    }

    edge_storage_t& edge_storage(unsigned t)
    {
      return g_.edge_storage(t);
    }

    const edge_storage_t edge_storage(unsigned t) const
    {
      return g_.edge_storage(t);
    }

    unsigned new_state(bdd cond)
    {
      return g_.new_state(cond);
    }

    unsigned new_states(unsigned n, bdd cond)
    {
      return g_.new_states(n, cond);
    }

    unsigned new_edge(unsigned src, unsigned dst)
    {
      return g_.new_edge(src, dst);
    }


#ifndef SWIG
    const state_vector& states() const
    {
      return g_.states();
    }
#endif

    state_vector& states()
    {
      return g_.states();
    }

#ifndef SWIG
    internal::all_trans<const graph_t>
    edges() const noexcept
    {
      return g_.edges();
    }
#endif

    internal::all_trans<graph_t>
    edges() noexcept
    {
      return g_.edges();
    }

#ifndef SWIG
    internal::state_out<const graph_t>
    out(unsigned src) const
    {
      return g_.out(src);
    }
#endif

    internal::state_out<graph_t>
    out(unsigned src)
    {
      return g_.out(src);
    }

  };

  typedef std::shared_ptr<kripke_graph> kripke_graph_ptr;

  inline kripke_graph_ptr
  make_kripke_graph(const bdd_dict_ptr& d)
  {
    return std::make_shared<kripke_graph>(d);
  }
}
