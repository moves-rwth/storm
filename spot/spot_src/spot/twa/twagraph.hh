// -*- coding: utf-8 -*-
// Copyright (C) 2014-2020 Laboratoire de Recherche et Développement
// de l'Epita.
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

#include <spot/twa/fwd.hh>
#include <spot/graph/graph.hh>
#include <spot/graph/ngraph.hh>
#include <spot/twa/bdddict.hh>
#include <spot/twa/twa.hh>
#include <spot/tl/formula.hh>
#include <sstream>

namespace spot
{

  /// \ingroup twa_representation
  /// \brief Graph-based representation of a TωA
  ///
  /// In a twa_graph, states are usually denoted by their number.  However
  /// if the on-the-fly interface is used, it returns pointer to instances
  /// of the twa_graph_state class.
  struct SPOT_API twa_graph_state: public spot::state
  {
  public:
    twa_graph_state() noexcept
    {
    }

    twa_graph_state(const twa_graph_state&) noexcept
    {
    }

    twa_graph_state& operator=(const twa_graph_state&) noexcept
    {
      return *this;
    }

    virtual ~twa_graph_state() noexcept
    {
    }

    virtual int compare(const spot::state* other) const override
    {
      auto o = down_cast<const twa_graph_state*>(other);

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

    virtual twa_graph_state*
    clone() const override
    {
      return const_cast<twa_graph_state*>(this);
    }

    virtual void destroy() const override
    {
    }
  };

  /// \ingroup twa_representation
  /// \brief Data attached to edges of a twa_graph
  ///
  /// Each edge of the graph has to additional data that are \a cond
  /// (a BDD representing the Boolean formula labeling the edge), and
  /// \a acc a set of acceptance marks representing the membership of
  /// the each to each acceptance set.
  struct SPOT_API twa_graph_edge_data
  {
    bdd cond;
    acc_cond::mark_t acc;

    explicit twa_graph_edge_data() noexcept
      : cond(bddfalse), acc({})
    {
    }

    twa_graph_edge_data(
        bdd cond,
        acc_cond::mark_t acc = {}) noexcept
      : cond(cond), acc(acc)
    {
    }

    bool operator<(const twa_graph_edge_data& other) const
    {
      if (cond.id() < other.cond.id())
        return true;
      if (cond.id() > other.cond.id())
        return false;
      return acc < other.acc;
    }

    bool operator==(const twa_graph_edge_data& other) const
    {
      return cond.id() == other.cond.id() &&
        acc == other.acc;
    }
  };



  /// \ingroup twa_representation
  /// \brief Iterator used by the on-the-fly interface of twa_graph.
  ///
  /// Instances of this class are returned by twa_graph::succ_iter().
  template<class Graph>
  class SPOT_API twa_graph_succ_iterator final:
    public twa_succ_iterator
  {
  private:
    typedef typename Graph::edge edge;
    typedef typename Graph::state_data_t state;
    const Graph* g_;
    edge t_;
    edge p_;

  public:
    twa_graph_succ_iterator(const Graph* g, edge t)
      : g_(g), t_(t)
    {
    }

    void recycle(edge t)
    {
      t_ = t;
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

    virtual const twa_graph_state* dst() const override
    {
      SPOT_ASSERT(!done());
      return &g_->state_data(g_->edge_storage(p_).dst);
    }

    virtual bdd cond() const override
    {
      SPOT_ASSERT(!done());
      return g_->edge_data(p_).cond;
    }

    virtual acc_cond::mark_t acc() const override
    {
      SPOT_ASSERT(!done());
      return g_->edge_data(p_).acc;
    }

    edge pos() const
    {
      return p_;
    }

  };

  /// \ingroup twa_representation
  /// \brief Graph-based representation of a TωA
  class SPOT_API twa_graph final: public twa
  {
  public:
    typedef digraph<twa_graph_state, twa_graph_edge_data> graph_t;
    // We avoid using graph_t::edge_storage_t because graph_t is not
    // instantiated in the SWIG bindings, and SWIG would therefore
    // handle graph_t::edge_storage_t as an abstract type.
    typedef spot::internal::edge_storage<unsigned, unsigned, unsigned,
                                         internal::boxed_label
                                         <twa_graph_edge_data, false>>
      edge_storage_t;
    static_assert(std::is_same<typename graph_t::edge_storage_t,
                  edge_storage_t>::value, "type mismatch");
    // We avoid using graph_t::state for the very same reason.
    typedef unsigned state_num;
    static_assert(std::is_same<typename graph_t::state, state_num>::value,
                  "type mismatch");

  protected:
    graph_t g_;
    mutable unsigned init_number_;

  public:

    void apply_permutation(std::vector<unsigned> permut);

    twa_graph(const bdd_dict_ptr& dict)
      : twa(dict),
        init_number_(0)
    {
    }

    explicit twa_graph(const const_twa_graph_ptr& other, prop_set p)
      : twa(other->get_dict()),
        g_(other->g_), init_number_(other->init_number_)
      {
        copy_acceptance_of(other);
        copy_ap_of(other);
        prop_copy(other, p);
      }

    virtual ~twa_graph()
    {
    }

#ifndef SWIG
    template <typename State_Name,
              typename Name_Hash = std::hash<State_Name>,
              typename Name_Equal = std::equal_to<State_Name>>
    using namer = named_graph<graph_t, State_Name, Name_Hash, Name_Equal>;

    template <typename State_Name,
              typename Name_Hash = std::hash<State_Name>,
              typename Name_Equal = std::equal_to<State_Name>>
    namer<State_Name, Name_Hash, Name_Equal>*
    create_namer()
    {
      return new named_graph<graph_t, State_Name, Name_Hash, Name_Equal>(g_);
    }

    namer<formula>*
    create_formula_namer()
    {
      return create_namer<formula>();
    }

    void
    release_formula_namer(namer<formula>* namer, bool keep_names);
#endif

    graph_t& get_graph()
    {
      return g_;
    }

    const graph_t& get_graph() const
    {
      return g_;
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
      bool univ = is_univ_dest(s);
      if (SPOT_UNLIKELY((!univ && s >= num_states())
                        // univ destinations have at least length 2.
                        || (univ && 2 + ~s >= g_.dests_vector().size())))
        throw std::invalid_argument
          ("set_init_state() called with nonexisting state");
      init_number_ = s;
    }

    template<class I>
    void set_univ_init_state(I dst_begin, I dst_end)
    {
      auto ns = num_states();
      for (I i = dst_begin; i != dst_end; ++i)
        if (SPOT_UNLIKELY(*i >= ns))
          throw std::invalid_argument
            ("set_univ_init_state() called with nonexisting state");
      init_number_ = g_.new_univ_dests(dst_begin, dst_end);
    }

    void set_univ_init_state(const std::initializer_list<state_num>& il)
    {
      set_univ_init_state(il.begin(), il.end());
    }

    state_num get_init_state_number() const
    {
      // If the automaton has no state, it has no initial state.
      if (num_states() == 0)
        throw std::runtime_error("automaton has no state at all");
      return init_number_;
    }

    virtual const twa_graph_state* get_init_state() const override
    {
      unsigned n = get_init_state_number();
      if (SPOT_UNLIKELY(!is_existential()))
        throw std::runtime_error
          ("the abstract interface does not support alternating automata");
      return state_from_number(n);
    }

    virtual twa_succ_iterator*
    succ_iter(const state* st) const override
    {
      auto s = down_cast<const typename graph_t::state_storage_t*>(st);
      SPOT_ASSERT(!s->succ || g_.is_valid_edge(s->succ));

      if (this->iter_cache_)
        {
          auto it =
            down_cast<twa_graph_succ_iterator<graph_t>*>(this->iter_cache_);
          it->recycle(s->succ);
          this->iter_cache_ = nullptr;
          return it;
        }
      return new twa_graph_succ_iterator<graph_t>(&g_, s->succ);
    }

    static constexpr bool is_univ_dest(const edge_storage_t& e)
    {
      return is_univ_dest(e.dst);
    }

    static constexpr bool is_univ_dest(unsigned s)
    {
      // Universal destinations are stored with their most-significant
      // bit set.
      return (int) s < 0;
    }

    state_num
    state_number(const state* st) const
    {
      auto s = down_cast<const typename graph_t::state_storage_t*>(st);
      return s - &g_.state_storage(0);
    }

    const twa_graph_state*
    state_from_number(state_num n) const
    {
      return &g_.state_data(n);
    }

    std::string format_state(unsigned n) const;

    virtual std::string format_state(const state* st) const override
    {
      return format_state(state_number(st));
    }

    unsigned edge_number(const twa_succ_iterator* it) const
    {
      auto* i = down_cast<const twa_graph_succ_iterator<graph_t>*>(it);
      return i->pos();
    }

    unsigned edge_number(const edge_storage_t& e) const
    {
      return g_.index_of_edge(e);
    }

    twa_graph_edge_data& edge_data(const twa_succ_iterator* it)
    {
      return g_.edge_data(edge_number(it));
    }

    twa_graph_edge_data& edge_data(unsigned t)
    {
      return g_.edge_data(t);
    }

    const twa_graph_edge_data& edge_data(const twa_succ_iterator* it) const
    {
      return g_.edge_data(edge_number(it));
    }

    const twa_graph_edge_data& edge_data(unsigned t) const
    {
      return g_.edge_data(t);
    }

    edge_storage_t& edge_storage(const twa_succ_iterator* it)
    {
      return g_.edge_storage(edge_number(it));
    }

    edge_storage_t& edge_storage(unsigned t)
    {
      return g_.edge_storage(t);
    }

    const edge_storage_t
      edge_storage(const twa_succ_iterator* it) const
    {
      return g_.edge_storage(edge_number(it));
    }

    const edge_storage_t edge_storage(unsigned t) const
    {
      return g_.edge_storage(t);
    }

    unsigned new_state()
    {
      return g_.new_state();
    }

    unsigned new_states(unsigned n)
    {
      return g_.new_states(n);
    }

    unsigned new_edge(unsigned src, unsigned dst,
                      bdd cond,
                      acc_cond::mark_t acc = {})
    {
      return g_.new_edge(src, dst, cond, acc);
    }

    unsigned new_acc_edge(unsigned src, unsigned dst,
                          bdd cond, bool acc = true)
    {
      if (acc)
        return g_.new_edge(src, dst, cond, this->acc().all_sets());
      else
        return g_.new_edge(src, dst, cond);
    }

    template<class I>
    unsigned new_univ_edge(unsigned src, I begin, I end,
                           bdd cond,
                           acc_cond::mark_t acc = {})
    {
      return g_.new_univ_edge(src, begin, end, cond, acc);
    }

    unsigned new_univ_edge(unsigned src, std::initializer_list<unsigned> dst,
                           bdd cond,
                           acc_cond::mark_t acc = {})
    {
      return g_.new_univ_edge(src, dst.begin(), dst.end(), cond, acc);
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

    internal::killer_edge_iterator<graph_t>
    out_iteraser(unsigned src)
    {
      return g_.out_iteraser(src);
    }

    internal::const_universal_dests
    univ_dests(unsigned d) const noexcept
    {
      return g_.univ_dests(d);
    }

    internal::const_universal_dests
    univ_dests(const edge_storage_t& e) const noexcept
    {
      return g_.univ_dests(e);
    }

    /// Whether the automaton uses only existential branching.
    bool is_existential() const
    {
      return g_.is_existential();
    }

#ifndef SWIG
    auto states() const
      SPOT_RETURN(g_.states());
    auto states()
      SPOT_RETURN(g_.states());

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
    auto edge_vector() const
      SPOT_RETURN(g_.edge_vector());
    auto edge_vector()
      SPOT_RETURN(g_.edge_vector());
#endif

    bool is_dead_edge(unsigned t) const
    {
      return g_.is_dead_edge(t);
    }

    bool is_dead_edge(const graph_t::edge_storage_t& t) const
    {
      return g_.is_dead_edge(t);
    }

    /// \brief Merge edges that can be merged.
    ///
    /// This makes two passes over the automaton to reduce the number
    /// of edges with an identical pair of source and destination.
    ///
    /// In the first pass, edges that share their source, destination,
    /// and acceptance marks are merged into a single edge whose condition
    /// is the conjunction of the conditions of the original edges.
    ///
    /// In the second pass, which is performed only on automata with
    /// Fin-less acceptance, edges with the same source, destination,
    /// and conditions are merged into a single edge whose set of
    /// acceptance marks is the intersection of the sets of the edges.
    ///
    /// If the automaton uses some universal edges, the method
    /// merge_univ_dests() is also called.
    void merge_edges();

    /// \brief Merge common universal destinations.
    ///
    /// This is already called by merge_edges().
    void merge_univ_dests();

    /// \brief Merge states that can be merged.
    ///
    /// This merges states that have the exact same outgoing edges. This method
    /// compares the successors of states in the order in which they are stored.
    /// Therefore, it is better to call it when you know that the edges are
    /// stored, e.g. after a call to merge_edges().
    void merge_states();

    /// \brief Remove all dead states
    ///
    /// Dead states are all the states that cannot be part of
    /// an infinite run of the automaton.  This includes
    /// states without successors, unreachable states, and states
    /// that only have dead successors.
    ///
    /// This function runs in linear time on non-alternating automata,
    /// but its current implementation can be quadratic when removing
    /// dead states from alternating automata.  (In the latter case, a
    /// universal edge has to be remove when any of its destination is
    /// dead, but this might cause the other destinations to become
    /// dead or unreachable themselves.)
    ///
    /// \see purge_unreachable_states
    void purge_dead_states();

    /// \brief Remove all unreachable states.
    ///
    /// A state is unreachable if it cannot be reached from the
    /// initial state.
    ///
    /// Use this function if you have declared more states than you
    /// actually need in the automaton.  It runs in linear time.
    ///
    /// purge_dead_states() will remove more states than
    /// purge_unreachable_states(), but it is more costly.
    ///
    /// You can pass a function to this method, which will be invoked
    /// with a vector indicating the renumbering of states.
    /// newst[i] == -1U means that state i is unreachable and thus deleted.
    /// Otherwise, state i is renumbered newst[i].
    ///
    /// \see purge_dead_states
    typedef void (*shift_action)(const std::vector<unsigned>& newst,
                                 void* action_data);
    void purge_unreachable_states(shift_action* f = nullptr,
                                  void* action_data = nullptr);

    /// \brief Remove unused atomic propositions
    ///
    /// Remove, from the list of atomic propositions registered by the
    /// automaton, those that are not actually used by its labels.
    void remove_unused_ap();

    /// \brief Define the state names of this automaton using
    /// the names from \a other.
    ///
    /// If the "original-states" named property is set, it is used to
    /// map the state numbers, otherwise an identity mapping is
    /// assumed.
    void copy_state_names_from(const const_twa_graph_ptr& other);

    /// \brief Return the marks associated to a state if the
    /// acceptance is state-based.
    acc_cond::mark_t state_acc_sets(unsigned s) const
    {
      if (SPOT_UNLIKELY(!(bool)prop_state_acc()))
        throw std::runtime_error
          ("state_acc_sets() should only be called on "
           "automata with state-based acceptance");
      for (auto& t: g_.out(s))
        // Stop at the first edge, since the remaining should be
        // labeled identically.
        return t.acc;
      return {};
    }

    /// \brief Tell if a state is accepting.
    ///
    /// This makes only sense for automata using state-based
    /// acceptance, and a simple acceptance condition like Büchi or
    /// co-Büchi.
    ///@{
    bool state_is_accepting(unsigned s) const
    {
      if (SPOT_UNLIKELY(!(bool)prop_state_acc()))
        throw std::runtime_error
          ("state_is_accepting() should only be called on "
           "automata with state-based acceptance");
      for (auto& t: g_.out(s))
        // Stop at the first edge, since the remaining should be
        // labeled identically.
        return acc().accepting(t.acc);
      return false;
    }

    bool state_is_accepting(const state* s) const
    {
      return state_is_accepting(state_number(s));
    }
    ///@}

    bool operator==(const twa_graph& aut) const
    {
      auto& dests1 = g_.dests_vector();
      auto& dests2 = aut.get_graph().dests_vector();
      if (num_states() != aut.num_states() ||
          num_edges() != aut.num_edges() ||
          num_sets() != aut.num_sets() ||
          dests1.size() != dests2.size())
        return false;
      auto& trans1 = edge_vector();
      auto& trans2 = aut.edge_vector();
      if (!std::equal(trans1.begin() + 1, trans1.end(),
                      trans2.begin() + 1))
        return false;
      return std::equal(dests1.begin(), dests1.end(),
                        dests2.begin());
    }

    /// \brief Renumber all states, and drop some.
    ///
    /// This semi-internal function is a wrapper around
    /// digraph::defrag_state() that additionally deals with universal
    /// branching.
    ///
    /// \param newst A vector indicating how each state should be renumbered.
    /// Use -1U to erase a state.
    /// \param used_states the number of states used (after renumbering)
    void defrag_states(std::vector<unsigned>&& newst, unsigned used_states);

    /// \brief Print the data structures used to represent the
    /// automaton in dot's format.
    ///
    /// \a opt should be a substring of "vdp" if you want to print
    /// only the vectors, data, or properties.
    void dump_storage_as_dot(std::ostream& out,
                             const char* opt = nullptr) const;
  };

  // This is a workaround for
#if __GNUC__ == 8 && __GNUC_MINOR__ == 2
#  define SPOT_make_twa_graph__(...) \
   std::shared_ptr<twa_graph>(new twa_graph(__VA_ARGS__))
#else
#  define SPOT_make_twa_graph__(...) \
   std::make_shared<twa_graph>(__VA_ARGS__)
#endif

  /// \ingroup twa_representation
  /// \brief Build an explicit automaton from all states of \a aut,
  inline twa_graph_ptr make_twa_graph(const bdd_dict_ptr& dict)
  {
    return SPOT_make_shared_enabled__(twa_graph, dict);
  }

  /// \ingroup twa_representation
  /// \brief Build an explicit automaton from all states of \a aut,
  inline twa_graph_ptr make_twa_graph(const twa_graph_ptr& aut,
                                      twa::prop_set p)
  {
    return SPOT_make_shared_enabled__(twa_graph, aut, p);
  }

  /// \ingroup twa_representation
  /// \brief Build an explicit automaton from all states of \a aut,
  inline twa_graph_ptr make_twa_graph(const const_twa_graph_ptr& aut,
                                      twa::prop_set p)
  {
    return SPOT_make_shared_enabled__(twa_graph, aut, p);
  }

  /// \ingroup twa_representation
  /// \brief Build an explicit automaton from all states of \a aut,
  ///
  /// This overload works using the abstract interface for automata.
  ///
  /// Set \a preserve_names to preserve state names, and set \a max_states
  /// to a maximum number of states to keep.  States with successors that
  /// have not been kept will be marked as incomplete; this is mostly useful
  /// to display a subset of a large state space.
  SPOT_API twa_graph_ptr
  make_twa_graph(const const_twa_ptr& aut, twa::prop_set p,
                 bool preserve_names = false,
                 // parentheses for SWIG, see
                 // https://github.com/swig/swig/issues/993
                 unsigned max_states = -(1U));
}
