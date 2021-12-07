// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011, 2013-2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003-2005 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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

#include <cstddef>
#include <spot/twa/fwd.hh>
#include <spot/twa/acc.hh>
#include <spot/twa/bdddict.hh>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <functional>
#include <array>
#include <vector>
#include <spot/misc/casts.hh>
#include <spot/misc/hash.hh>
#include <spot/tl/formula.hh>
#include <spot/misc/trival.hh>

namespace spot
{
  struct twa_run;
  typedef std::shared_ptr<twa_run> twa_run_ptr;

  struct twa_word;
  typedef std::shared_ptr<twa_word> twa_word_ptr;

  /// \ingroup twa_essentials
  /// \brief Abstract class for states.
  class SPOT_API state
  {
  public:
    /// \brief Compares two states (that come from the same automaton).
    ///
    /// This method returns an integer less than, equal to, or greater
    /// than zero if \a this is found, respectively, to be less than, equal
    /// to, or greater than \a other according to some implicit total order.
    ///
    /// This method should not be called to compare states from
    /// different automata.
    ///
    /// \sa spot::state_ptr_less_than
    virtual int compare(const state* other) const = 0;

    /// \brief Hash a state.
    ///
    /// This method returns an integer that can be used as a
    /// hash value for this state.
    ///
    /// Note that the hash value is guaranteed to be unique for all
    /// equal states (in compare()'s sense) for only as long as one
    /// of these states exists.  So it's OK to use a spot::state as a
    /// key in a \c hash_map because the mere use of the state as a
    /// key in the hash will ensure the state continues to exist.
    ///
    /// However if you create the state, get its hash key, delete the
    /// state, recreate the same state, and get its hash key, you may
    /// obtain two different hash keys if the same state were not
    /// already used elsewhere.  In practice this weird situation can
    /// occur only when the state is BDD-encoded, because BDD numbers
    /// (used to build the hash value) can be reused for other
    /// formulas.  That probably doesn't matter, since the hash value
    /// is meant to be used in a \c hash_map, but it had to be noted.
    virtual size_t hash() const = 0;

    /// Duplicate a state.
    virtual state* clone() const = 0;

    /// \brief Release a state.
    ///
    /// Methods from the tgba or twa_succ_iterator always return a
    /// new state that you should deallocate with this function.
    /// Before Spot 0.7, you had to "delete" your state directly.
    /// Starting with Spot 0.7, you should update your code to use
    /// this function instead. destroy() usually calls delete, except
    /// in subclasses that destroy() to allow better memory management
    /// (e.g., no memory allocation for explicit automata).
    virtual void destroy() const
    {
      delete this;
    }

  protected:
    /// \brief Destructor.
    ///
    /// Note that client code should call
    /// \code s->destroy(); \endcode
    /// instead of
    /// \code delete s; \endcode .
    virtual ~state()
    {
    }
  };

  /// \ingroup twa_essentials
  /// \brief Strict Weak Ordering for \c state*.
  ///
  /// This is meant to be used as a comparison functor for
  /// STL \c map whose keys are of type \c state*.
  ///
  /// For instance here is how one could declare
  /// a map of \c state*.
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::map<spot::state*, int, spot::state_ptr_less_than> seen;
  /// \endcode
  struct state_ptr_less_than
  {
    bool
    operator()(const state* left, const state* right) const
    {
      SPOT_ASSERT(left);
      return left->compare(right) < 0;
    }
  };

  /// \ingroup twa_essentials
  /// \brief An Equivalence Relation for \c state*.
  ///
  /// This is meant to be used as a comparison functor for
  /// an \c unordered_map whose keys are of type \c state*.
  ///
  /// For instance here is how one could declare
  /// a map of \c state*.
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::unordered_map<spot::state*, int, spot::state_ptr_hash,
  ///                                    spot::state_ptr_equal> seen;
  /// \endcode
  struct state_ptr_equal
  {
    bool
    operator()(const state* left, const state* right) const
    {
      SPOT_ASSERT(left);
      return 0 == left->compare(right);
    }
  };

  /// \ingroup twa_essentials
  /// \ingroup hash_funcs
  /// \brief Hash Function for \c state*.
  ///
  /// This is meant to be used as a hash functor for
  /// an \c unordered_map whose keys are of type \c state*.
  ///
  /// For instance here is how one could declare
  /// a map of \c state*.
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::unordered_map<spot::state*, int, spot::state_ptr_hash,
  ///                                    spot::state_ptr_equal> seen;
  /// \endcode
  struct state_ptr_hash
  {
    size_t
    operator()(const state* that) const
    {
      SPOT_ASSERT(that);
      return that->hash();
    }
  };

  /// \brief Unordered set of abstract states
  ///
  /// Destroying each state if needed is the user's responsibility.
  ///
  /// \see state_unicity_table
  typedef std::unordered_set<const state*,
                             state_ptr_hash, state_ptr_equal> state_set;

  /// \brief Unordered map of abstract states
  ///
  /// Destroying each state if needed is the user's responsibility.
  template<class val>
  using state_map = std::unordered_map<const state*, val,
                                       state_ptr_hash, state_ptr_equal>;

  /// \ingroup twa_essentials
  /// \brief Render state pointers unique via a hash table.
  class SPOT_API state_unicity_table
  {
    state_set m;
  public:

    /// \brief Canonicalize state pointer.
    ///
    /// If this is the first time a state is seen, this returns the
    /// state pointer as-is, otherwise it frees the state and returns
    /// a pointer to the previously seen copy.
    ///
    /// States are owned by the table and will be freed on
    /// destruction.
    const state* operator()(const state* s)
    {
      auto p = m.insert(s);
      if (!p.second)
        s->destroy();
      return *p.first;
    }

    /// \brief Canonicalize state pointer.
    ///
    /// Same as operator(), except that a nullptr
    /// is returned if the state is not new.
    const state* is_new(const state* s)
    {
      auto p = m.insert(s);
      if (!p.second)
        {
          s->destroy();
          return nullptr;
        }
      return *p.first;
    }

    ~state_unicity_table()
    {
      for (state_set::iterator i = m.begin(); i != m.end();)
        {
          // Advance the iterator before destroying its key.  This
          // avoids issues with old g++ implementations.
          state_set::iterator old = i++;
          (*old)->destroy();
        }
    }

    size_t
    size()
    {
      return m.size();
    }
  };



  // Functions related to shared_ptr.
  //////////////////////////////////////////////////

  typedef std::shared_ptr<const state> shared_state;

  inline void shared_state_deleter(state* s) { s->destroy(); }

  /// \ingroup twa_essentials
  /// \brief Strict Weak Ordering for \c shared_state
  /// (shared_ptr<const state*>).
  ///
  /// This is meant to be used as a comparison functor for
  /// STL \c map whose keys are of type \c shared_state.
  ///
  /// For instance here is how one could declare
  /// a map of \c shared_state.
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::map<shared_state, int, spot::state_shared_ptr_less_than> seen;
  /// \endcode
  struct state_shared_ptr_less_than
  {
    bool
    operator()(shared_state left,
               shared_state right) const
    {
      SPOT_ASSERT(left);
      return left->compare(right.get()) < 0;
    }
  };

  /// \ingroup twa_essentials
  /// \brief An Equivalence Relation for \c shared_state
  /// (shared_ptr<const state*>).
  ///
  /// This is meant to be used as a comparison functor for
  /// an \c unordered_map whose keys are of type \c shared_state.
  ///
  /// For instance here is how one could declare
  /// a map of \c shared_state
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::unordered_map<shared_state, int,
  ///                      state_shared_ptr_hash,
  ///                      state_shared_ptr_equal> seen;
  /// \endcode
  ///
  /// \see shared_state_set
  struct state_shared_ptr_equal
  {
    bool
    operator()(shared_state left,
               shared_state right) const
    {
      SPOT_ASSERT(left);
      return 0 == left->compare(right.get());
    }
  };

  /// \ingroup twa_essentials
  /// \ingroup hash_funcs
  /// \brief Hash Function for \c shared_state (shared_ptr<const state*>).
  ///
  /// This is meant to be used as a hash functor for
  /// an \c unordered_map whose keys are of type
  /// \c shared_state.
  ///
  /// For instance here is how one could declare
  /// a map of \c shared_state.
  /// \code
  ///   // Remember how many times each state has been visited.
  ///   std::unordered_map<shared_state, int,
  ///                      state_shared_ptr_hash,
  ///                      state_shared_ptr_equal> seen;
  /// \endcode
  ///
  /// \see shared_state_set
  struct state_shared_ptr_hash
  {
    size_t
    operator()(shared_state that) const
    {
      SPOT_ASSERT(that);
      return that->hash();
    }
  };

  /// Unordered set of shared states
  typedef std::unordered_set<shared_state,
                             state_shared_ptr_hash,
                             state_shared_ptr_equal> shared_state_set;

  /// \ingroup twa_essentials
  /// \brief Iterate over the successors of a state.
  ///
  /// This class provides the basic functionality required to iterate
  /// over the set of edges leaving a given state.  Instance of
  /// twa_succ_iterator should normally not be created directly.
  /// Instead, they are created by passing a "source" state to
  /// twa::succ_iter(), which will create the instance of
  /// twa_succ_iterator to iterate over the successors of that state.
  ///
  /// This twa_succ_iterator class offers two types of services,
  /// offered by two groups of methods.  The methods first(), next(),
  /// and done() allow iteration over the set of outgoing edges.
  /// The methods cond(), acc(), dst(), allow inspecting the current
  /// edge.
  ///
  /// The twa_succ_iterator is usually subclassed so that iteration
  /// methods and accessor methods can be implemented differently in
  /// different automata.  In particular, this interface allows
  /// computing the set of successors on the fly if needed.
  ///
  /// The iterator can be used to iterate over all successors in a
  /// loop as follows:
  ///
  /// \code
  ///     for (i->first(); !i->done(); i->next())
  ///       {
  ///         // use i->cond(), i->acc(), i->dst()
  ///       }
  /// \endcode
  ///
  /// If there are n successors, there will be 1 call to first(), n
  /// calls to next() and n+1 calls to done(), so a total of 2(n+1)
  /// calls to virtual methods just to handle the iteration.  For this
  /// reason, we usually favor the following more efficient way of
  /// performing the same loop:
  ///
  /// \code
  ///     if (i->first())
  ///       do
  ///         {
  ///           // use i->cond(), i->acc(), i->dst()
  ///         }
  ///       while(i->next());
  /// \endcode
  ///
  /// This loop uses the return value of first() and next() to save
  /// n+1 calls to done().
  class SPOT_API twa_succ_iterator
  {
  public:
    virtual
    ~twa_succ_iterator()
    {
    }

    /// \name Iteration
    ///@{

    /// \brief Position the iterator on the first successor (if any).
    ///
    /// This method can be called several times in order to make
    /// multiple passes over successors.
    ///
    /// \warning One should always call \c done() (or better: check
    /// the return value of first()) to ensure there is a successor,
    /// even after \c first().  A common trap is to assume that there
    /// is at least one successor: this is wrong.
    ///
    /// \return true iff there is at least one successor
    ///
    /// If first() returns false, it is invalid to call next(),
    /// cond(), acc(), or dst().
    virtual bool first() = 0;

    /// \brief Jump to the next successor (if any).
    ///
    /// \warning Again, one should always call \c done() (or better:
    /// check the return value of next()) to ensure there is a successor.
    ///
    /// \return true if the iterator moved to a new successor, false
    /// if the iterator could not move to a new successor.
    ///
    /// If next() returns false, it is invalid to call next() again,
    /// or to call cond(), acc() or dst().
    virtual bool next() = 0;

    /// \brief Check whether the iteration is finished.
    ///
    /// This function should be called after any call to \c first()
    /// or \c next() and before any enquiry about the current state.
    ///
    /// The typical use case of done() is in a \c for loop such as:
    ///
    ///     for (s->first(); !s->done(); s->next())
    ///       ...
    ///
    /// \return false iff the iterator is pointing to a successor.
    ///
    /// It is incorrect to call done() if first() hasn't been called
    /// before.  If done() returns true, it is invalid to call
    /// next(), cond(), acc(), or dst().
    virtual bool done() const = 0;

    ///@}

    /// \name Inspection
    ///@{

    /// \brief Get the destination state of the current edge.
    ///
    /// Each call to dst() (even several times on the same edge)
    /// creates a new state that has to be destroyed (see
    /// state::destroy()) by the caller after it is no longer used.
    ///
    /// Note that the same state may occur at different points in the
    /// iteration, as different outgoing edges (usually with different
    /// labels or acceptance membership) may go to the same state.
    virtual const state* dst() const = 0;
    /// \brief Get the condition on the edge leading to this successor.
    ///
    /// This is a Boolean function of atomic propositions.
    virtual bdd cond() const = 0;
    /// \brief Get the acceptance mark of the edge leading to this
    /// successor.
    virtual acc_cond::mark_t acc() const = 0;

    ///@}
  };

  namespace internal
  {
    /// \brief Helper structure to iterate over the successors of a
    /// state using the on-the-fly interface.
    ///
    /// This one emulates an STL-like iterator over the
    /// twa_succ_iterator interface.
    struct SPOT_API succ_iterator
    {
    protected:
      twa_succ_iterator* it_;
    public:

      succ_iterator(twa_succ_iterator* it):
        it_(it)
      {
      }

      bool operator==(succ_iterator o) const
      {
        return it_ == o.it_;
      }

      bool operator!=(succ_iterator o) const
      {
        return it_ != o.it_;
      }

      const twa_succ_iterator* operator*() const
      {
        return it_;
      }

      void operator++()
      {
        if (!it_->next())
          it_ = nullptr;
      }
    };

#ifndef SWIG
    /// \brief Helper class to iterate over the successors of a state
    /// using the on-the-fly interface
    ///
    /// This one emulates an STL-like container with begin()/end()
    /// methods so that it can be iterated using a ranged-for.
    class twa_succ_iterable
    {
    protected:
      const twa* aut_;
      twa_succ_iterator* it_;
    public:
      twa_succ_iterable(const twa* aut, twa_succ_iterator* it)
        : aut_(aut), it_(it)
      {
      }

      twa_succ_iterable(twa_succ_iterable&& other) noexcept
        : aut_(other.aut_), it_(other.it_)
      {
        other.it_ = nullptr;
      }

      ~twa_succ_iterable(); // Defined in this file after twa

      internal::succ_iterator begin()
      {
        return it_->first() ? it_ : nullptr;
      }

      internal::succ_iterator end()
      {
        return nullptr;
      }
    };
#endif // SWIG
  }

  /// \defgroup twa TωA (Transition-based ω-Automata)
  ///
  /// Spot is centered around the spot::twa type.  This type and its
  /// cousins are listed \ref twa_essentials "here".  This is an
  /// abstract interface.  Its implementations are either \ref
  /// twa_representation "concrete representations", or \ref
  /// twa_on_the_fly_algorithms "on-the-fly algorithms".  Other
  /// algorithms that work on spot::twa are \ref twa_algorithms
  /// "listed separately".

  /// \addtogroup twa_essentials Essential TωA types
  /// \ingroup twa

  /// \ingroup twa_essentials
  /// \brief A Transition-based ω-Automaton.
  ///
  /// The acronym TωA stands for Transition-based ω-automaton.
  /// We may write it as TwA or twa, but never as TWA as the
  /// w is just a non-utf8 replacement for ω that should not be
  /// capitalized.
  ///
  /// TωAs are transition-based automata, meaning that not-only do
  /// they have labels on edges, but they also have an acceptance
  /// condition defined in terms of sets of transitions.  The
  /// acceptance condition can be anything supported by the HOA format
  /// (http://adl.github.io/hoaf/).  The only restriction w.r.t. the
  /// format is that this class does not support alternating automata.
  ///
  /// Previous versions of Spot supported a type of automata called
  /// TGBA, which are TωA in which the acceptance condition is a set
  /// of sets of transitions that must be visited infinitely often.
  ///
  /// In this version, TGBAs are now represented by TωAs for which
  ///
  ///     aut->acc().is_generalized_buchi()
  ///
  /// returns true.
  ///
  /// Browsing a TωA is usually achieved using two methods: \c
  /// get_init_state(), and succ().  The former returns the initial
  /// state while the latter allows iterating over the outgoing edges
  /// of any given state. A TωA is always assumed to have at least
  /// one state, the initial one.
  ///
  /// Note that although this is a transition-based automaton, we never
  /// represent edges in the API.  Information about edges can be
  /// obtained by querying the iterator over the successors of a
  /// state.
  ///
  /// The interface presented here is what we call the on-the-fly
  /// interface of automata, because the TωA class can be subclassed
  /// to implement an object that computes its successors on-the-fly.
  /// The down-side is that all these methods are virtual, so you
  /// pay the cost of virtual calls when iterating over automata
  /// constructed on-the-fly.  Also the interface assumes that each
  /// successor state is a new object whose memory management is the
  /// responsibility of the caller, who should then call
  /// state::destroy() to release it.
  ///
  /// If you want to work with a TωA that is explicitly stored as a
  /// graph in memory, use the spot::twa_graph subclass instead.  A
  /// twa_graph object can be used as a spot::twa (using the
  /// on-the-fly interface, even though nothing needs to be
  /// constructed), but it also offers a faster interface that does not
  /// use virtual methods.
  class SPOT_API twa: public std::enable_shared_from_this<twa>
  {
  protected:
    twa(const bdd_dict_ptr& d);
    /// Any iterator returned via release_iter.
    mutable twa_succ_iterator* iter_cache_;
    /// BDD dictionary used by the automaton.
    bdd_dict_ptr dict_;
  public:

    virtual ~twa();

    /// \brief Get the initial state of the automaton.
    ///
    /// The state has been allocated with \c new.  It is the
    /// responsability of the caller to \c destroy it when no
    /// longer needed.
    virtual const state* get_init_state() const = 0;

    /// \brief Get an iterator over the successors of \a local_state.
    ///
    /// The iterator has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    ///
    /// \see succ()
    virtual twa_succ_iterator*
    succ_iter(const state* local_state) const = 0;

#ifndef SWIG
    /// \brief Build an iterable over the successors of \a s.
    ///
    /// This is meant to be used as
    ///
    /// \code
    ///    for (auto i: aut->succ(s))
    ///      {
    ///        // use i->cond(), i->acc(), i->dst()
    ///      }
    /// \endcode
    ///
    /// and the above loop is in fact syntactic sugar for
    ///
    /// \code
    ///    twa_succ_iterator* i = aut->succ_iter(s);
    ///    if (i->first())
    ///      do
    ///        {
    ///          // use i->cond(), i->acc(), i->dst()
    ///        }
    ///      while (i->next());
    ///    aut->release_iter(i);
    /// \endcode
    internal::twa_succ_iterable
    succ(const state* s) const
    {
      return {this, succ_iter(s)};
    }
 #endif

    /// \brief Release an iterator after usage.
    ///
    /// This iterator can then be reused by succ_iter() to avoid
    /// memory allocation.
    void release_iter(twa_succ_iterator* i) const
    {
      if (iter_cache_)
        delete i;
      else
        iter_cache_ = i;
    }

    /// \brief Get the dictionary associated to the automaton.
    ///
    /// Automata are labeled by Boolean formulas over atomic
    /// propositions.  These Boolean formulas are represented as BDDs.
    /// The dictionary allows to map BDD variables back to atomic
    /// propositions, and vice versa.
    ///
    /// Usually automata that are involved in the same computations
    /// should share their dictionaries so that operations between
    /// BDDs of the two automata work naturally.
    ///
    /// It is however possible to declare automata that use different
    /// sets of atomic propositions with different dictionaries.  That
    /// way, a BDD variable associated to some atomic proposition in
    /// one automaton might be reused for another atomic proposition
    /// in the other automaton.
    bdd_dict_ptr get_dict() const
    {
      return dict_;
    }

    ///@{
    /// \brief Register an atomic proposition designated by \a ap.
    ///
    /// This is the preferred way to declare that an automaton is using
    /// a given atomic proposition.
    ///
    /// This adds the atomic proposition to the list of atomic
    /// proposition of the automaton, and also registers it to the
    /// bdd_dict.
    ///
    /// \return The BDD variable number assigned for this atomic
    /// proposition.
    int register_ap(formula ap)
    {
      int res = dict_->has_registered_proposition(ap, this);
      if (res < 0)
        {
          aps_.emplace_back(ap);
          res = dict_->register_proposition(ap, this);
          bddaps_ &= bdd_ithvar(res);
        }
      return res;
    }

    int register_ap(std::string ap)
    {
      return register_ap(formula::ap(ap));
    }
    ///@}

    /// \brief Unregister an atomic proposition.
    ///
    /// \param num the BDD variable number returned by register_ap().
    void unregister_ap(int num);

    /// \brief Register all atomic propositions that have
    /// already been registered by the bdd_dict for this automaton.
    ///
    /// This method may only be called on an automaton with an empty
    /// list of AP.  It will fetch all atomic propositions that have
    /// been set in the bdd_dict for this particular automaton.
    ///
    /// The typical use-case for this function is when the labels of
    /// an automaton are created by functions such as
    /// formula_to_bdd().  This is for instance done in the parser
    /// for never claims or LBTT.
    void register_aps_from_dict()
    {
      if (!aps_.empty())
        throw std::runtime_error("register_ap_from_dict() may not be"
                                 " called on an automaton that has already"
                                 " registered some AP");
      auto& m = get_dict()->bdd_map;
      unsigned s = m.size();
      for (unsigned n = 0; n < s; ++n)
        if (m[n].refs.find(this) != m[n].refs.end())
          {
            aps_.emplace_back(m[n].f);
            bddaps_ &= bdd_ithvar(n);
          }
    }

    /// \brief The vector of atomic propositions registered by this
    /// automaton.
    const std::vector<formula>& ap() const
    {
      return aps_;
    }

    /// \brief The set of atomic propositions as a conjunction.
    bdd ap_vars() const
    {
      return bddaps_;
    }

    /// \brief Format the state as a string for printing.
    ///
    /// Formating is the responsability of the automata that owns the
    /// state, so that state objects could be implemented as very
    /// small objects, maybe sharing data with other state objects via
    /// data structure stored in the automaton.
    virtual std::string format_state(const state* s) const = 0;

    /// \brief Project a state on an automaton.
    ///
    /// This converts \a s, into that corresponding spot::state for \a
    /// t.  This is useful when you have the state of a product, and
    /// want to restrict this state to a specific automata occuring in
    /// the product.
    ///
    /// It goes without saying that \a s and \a t should be compatible
    /// (i.e., \a s is a state of \a t).
    ///
    /// \return 0 if the projection fails (\a s is unrelated to \a t),
    ///    or a new \c state* (the projected state) that must be
    ///    destroyed by the caller.
    virtual state* project_state(const state* s,
                                 const const_twa_ptr& t) const;

    ///@{
    /// \brief The acceptance condition of the automaton.
    const acc_cond& acc() const
    {
      return acc_;
    }

    acc_cond& acc()
    {
      return acc_;
    }
    ///@}

    /// \brief Check whether the language of the automaton is empty.
    ///
    /// If you are calling this method on a product of two automata,
    /// consider using intersects() instead.
    ///
    /// Note that if the input automaton uses Fin-acceptance, the
    /// emptiness check is not performed on-the-fly.  Any on-the-fly
    /// automaton would be automatically copied into a twa_graph_ptr
    /// first.
    virtual bool is_empty() const;

    /// \brief Return an accepting run if one exists.
    ///
    /// Return nullptr if no accepting run were found.
    ///
    /// If you are calling this method on a product of two automata,
    /// consider using intersecting_run() instead.
    ///
    /// Note that if the input automaton uses Fin-acceptance, this
    /// computation is not performed on-the-fly.  Any on-the-fly
    /// automaton would be automatically copied into a twa_graph_ptr
    /// first.
    virtual twa_run_ptr accepting_run() const;

    /// \brief Return an accepting word if one exists.
    ///
    /// Return nullptr if no accepting word were found.
    ///
    /// If you are calling this method on a product of two automata,
    /// consider using intersecting_word() instead.
    ///
    /// Note that if the input automaton uses Fin-acceptance, this
    /// computation is not performed on-the-fly.  Any on-the-fly
    /// automaton would be automatically copied into a twa_graph_ptr
    /// first.
    virtual twa_word_ptr accepting_word() const;

    /// \brief Check whether the language of this automaton intersects
    /// that of the \a other automaton.
    ///
    /// An emptiness check is performed on a product computed
    /// on-the-fly, unless some of the operands use Fin-acceptance: in
    /// this case an explicit product is performed.
    virtual bool intersects(const_twa_ptr other) const;

    /// \brief Return an accepting run recognizing a word accepted by
    /// two automata.
    ///
    /// The run returned is a run from automaton this.
    ///
    /// Return nullptr if no accepting run were found.
    ///
    /// An emptiness check is performed on a product computed
    /// on-the-fly, unless some of the operands use Fin-acceptance: in
    /// this case an explicit product is performed.
    virtual twa_run_ptr intersecting_run(const_twa_ptr other) const;

    // (undocumented)
    //
    // If \a from_other is true, the returned run will be over the
    // \a other automaton.  Otherwise, the run will be over this
    // automaton.
    //
    // This function was deprecated in Spot 2.8.
    SPOT_DEPRECATED("replace a->intersecting_run(b, true) "
                    "by b->intersecting_run(a).")
    twa_run_ptr intersecting_run(const_twa_ptr other,
                                 bool from_other) const
    {
      if (from_other)
        return other->intersecting_run(shared_from_this());
      else
        return this->intersecting_run(other);
    }

    /// \brief Return a word accepted by two automata.
    ///
    /// Return nullptr if no accepting word were found.
    virtual twa_word_ptr intersecting_word(const_twa_ptr other) const;

    /// \brief Return an accepting run recognizing a word accepted by
    /// exactly one of the two automata.
    ///
    /// Return nullptr iff the two automata recognize the same
    /// language.
    ///
    /// This methods needs to complement at least one automaton (if
    /// lucky) or maybe both automata.  It will therefore be more
    /// efficient on deterministic automata.
    virtual twa_run_ptr exclusive_run(const_twa_ptr other) const;

    /// \brief Return a word accepted by exactly one of the two
    /// automata.
    ///
    /// Return nullptr iff the two automata recognize the same
    /// language.
    ///
    /// This methods needs to complement at least one automaton (if
    /// lucky) or maybe both automata.  It will therefore be more
    /// efficient on deterministic automata.
    virtual twa_word_ptr exclusive_word(const_twa_ptr other) const;

  private:
    acc_cond acc_;

  public:
    /// Number of acceptance sets used by the automaton.
    unsigned num_sets() const
    {
      return acc_.num_sets();
    }

    /// Acceptance formula used by the automaton.
    const acc_cond::acc_code& get_acceptance() const
    {
      return acc_.get_acceptance();
    }

    /// \brief Set the acceptance condition of the automaton.
    ///
    /// \param num the number of acceptance sets used
    /// \param c the acceptance formula
    void set_acceptance(unsigned num, const acc_cond::acc_code& c)
    {
      acc_ = acc_cond(num, c);
    }

    /// \brief Set the acceptance condition of the automaton.
    ///
    /// \param c another acceptance condition
    void set_acceptance(const acc_cond& c)
    {
      acc_ = c;
    }

    /// Copy the acceptance condition of another TωA.
    void copy_acceptance_of(const const_twa_ptr& a)
    {
      acc_ = a->acc();
    }

    /// Copy the atomic propositions of another TωA
    void copy_ap_of(const const_twa_ptr& a)
    {
      for (auto f: a->ap())
        this->register_ap(f);
    }

    /// \brief Set generalized Büchi acceptance
    ///
    /// \param num the number of acceptance sets to use
    ///
    /// The acceptance formula of the form
    /// \code
    /// Inf(0)&Inf(1)&...&Inf(num-1)
    /// \endcode
    /// is generated.
    ///
    /// In the case where \a num is null, the state-acceptance
    /// property is automatically turned on.
    void set_generalized_buchi(unsigned num)
    {
      acc_ = acc_cond(num, acc_cond::acc_code::generalized_buchi(num));
    }

    /// \brief Set generalized co-Büchi acceptance
    ///
    /// \param num the number of acceptance sets to use
    ///
    /// The acceptance formula of the form
    /// \code
    /// Fin(0)&Fin(1)&...&Fin(num-1)
    /// \endcode
    /// is generated.
    ///
    /// In the case where \a num is null, the state-acceptance
    /// property is automatically turned on.
    void set_generalized_co_buchi(unsigned num)
    {
      acc_ = acc_cond(num, acc_cond::acc_code::generalized_co_buchi(num));
    }

    /// \brief Set Büchi acceptance.
    ///
    /// This declares a single acceptance set, and \c Inf(0)
    /// acceptance.  The returned mark \c {0} can be used to tag
    /// accepting transitions.
    ///
    /// Note that this does not mark the automaton as using
    /// state-based acceptance. If you want to create a Büchi
    /// automaton with state-based acceptance, call
    /// \code
    /// prop_state_acc(true)
    /// \endcode
    /// in addition.
    ///
    /// \see prop_state_acc
    acc_cond::mark_t set_buchi()
    {
      acc_ = acc_cond(1, acc_cond::acc_code::buchi());
      return {0};
    }

    /// \brief Set co-Büchi acceptance.
    ///
    /// This declares a single acceptance set, and \c Fin(0)
    /// acceptance.  The returned mark \c {0} can be used to tag
    /// non-accepting transitions.
    ///
    /// Note that this does not mark the automaton as using
    /// state-based acceptance. If you want to create a co-Büchi
    /// automaton with state-based acceptance, call
    /// \code
    /// prop_state_acc(true)
    /// \endcode
    /// in addition.
    ///
    /// \see prop_state_acc
    acc_cond::mark_t set_co_buchi()
    {
      acc_ = acc_cond(1, acc_cond::acc_code::cobuchi());
      return {0};
    }

  private:
    std::vector<formula> aps_;
    bdd bddaps_;

    /// Helper structure used to store property flags.
    struct bprop
    {
      trival::repr_t state_based_acc:2;     // State-based acceptance.
      trival::repr_t inherently_weak:2;     // Inherently Weak automaton.
      trival::repr_t weak:2;                // Weak automaton.
      trival::repr_t terminal:2;            // Terminal automaton.
      trival::repr_t universal:2;           // Universal automaton.
      trival::repr_t unambiguous:2;         // Unambiguous automaton.
      trival::repr_t stutter_invariant:2;   // Stutter invariant language.
      trival::repr_t very_weak:2;           // very-weak, or 1-weak
      trival::repr_t semi_deterministic:2;  // semi-deterministic automaton.
      trival::repr_t complete:2;            // Complete automaton.
    };
    union
    {
      unsigned props;
      bprop is;
    };

  protected:
#ifndef SWIG
    // Dynamic properties, are given with a name and a destructor function.
    std::unordered_map<std::string,
                       std::pair<void*,
                                 std::function<void(void*)>>> named_prop_;
#endif
    void* get_named_prop_(std::string s) const;

  public:

#ifndef SWIG
    /// \brief Declare a named property
    ///
    /// Arbitrary objects can be attached to automata.  Those are called
    /// named properties.  They are used for instance to name all the
    /// states of an automaton.
    ///
    /// This function attaches the object \a val to the current
    /// automaton, under the name \a s and destroys any previous
    /// property with the same name.
    ///
    /// When the automaton is destroyed, the \a destructor function will
    /// be called to destroy the attached object.
    ///
    /// See https://spot.lrde.epita.fr/concepts.html#named-properties
    /// for a list of named properties used by Spot.
    void set_named_prop(std::string s,
                        void* val, std::function<void(void*)> destructor);

    /// \brief Declare a named property
    ///
    /// Arbitrary objects can be attached to automata.  Those are called
    /// named properties.  They are used for instance to name all the
    /// states of an automaton.
    ///
    /// This function attaches the object \a val to the current
    /// automaton, under the name \a s and destroys any previous
    /// property with the same name.
    ///
    /// When the automaton is destroyed, the attached object will be
    /// destroyed with \c delete.
    ///
    /// See https://spot.lrde.epita.fr/concepts.html#named-properties
    /// for a list of named properties used by Spot.
    template<typename T>
    void set_named_prop(std::string s, T* val)
    {
      set_named_prop(s, val,
                     [](void *p) noexcept { delete static_cast<T*>(p); });
    }

    /// \brief Erase a named property
    ///
    /// Arbitrary objects can be attached to automata.  Those are called
    /// named properties.  They are used for instance to name all the
    /// states of an automaton.
    ///
    /// This function removes the property \a s if it exists.
    ///
    /// See https://spot.lrde.epita.fr/concepts.html#named-properties
    /// for a list of named properties used by Spot.
    void set_named_prop(std::string s, std::nullptr_t);

    /// \brief Retrieve a named property
    ///
    /// Because named property can be object of any type, retrieving
    /// the object requires knowing its type.
    ///
    /// \param s the name of the object to retrieve
    /// \tparam T the type of the object to retrieve
    ///
    /// Note that the return is a pointer to \c T, so the type should
    /// not include the pointer.
    ///
    /// Returns a nullptr if no such named property exists.
    ///
    /// See https://spot.lrde.epita.fr/concepts.html#named-properties
    /// for a list of named properties used by Spot.
    template<typename T>
    T* get_named_prop(std::string s) const
    {
      if (void* p = get_named_prop_(s))
        return static_cast<T*>(p);
      else
        return nullptr;
    }

    /// \brief Create or retrieve a named property
    ///
    /// Arbitrary objects can be attached to automata.  Those are called
    /// named properties.  They are used for instance to name all the
    /// states of an automaton.
    ///
    /// This function creates a property object of a given type, and
    /// attaches it to \a name if no such property exists, or it
    /// returns the found object.
    ///
    /// See https://spot.lrde.epita.fr/concepts.html#named-properties
    /// for a list of named properties used by Spot.
    template<typename T>
    T* get_or_set_named_prop(std::string s)
    {
      if (void* p = get_named_prop_(s))
        return static_cast<T*>(p);

      auto tmp = new T;
      set_named_prop(s, tmp);
      return tmp;
    }

#endif

    /// \brief Destroy all named properties.
    ///
    /// This is used by the automaton destructor, but it could be used
    /// by any algorithm that wants to get rid of all named properties.
    void release_named_properties()
    {
      // Destroy all named properties.
      for (auto& np: named_prop_)
        np.second.second(np.second.first);
      named_prop_.clear();
    }

    /// \brief Whether the automaton uses state-based acceptance.
    ///
    /// From the point of view of Spot, this means that all
    /// transitions leaving a state belong to the same acceptance
    /// sets.  Then it is equivalent to pretend that the state is in
    /// the acceptance set.
    trival prop_state_acc() const
    {
      if (num_sets() == 0)
        return trival(true);
      return trival::from_repr_t(is.state_based_acc);
    }

    /// \brief Set the state-based-acceptance property.
    ///
    /// If this property is set to true, then all transitions leaving
    /// a state must belong to the same acceptance sets.
    void prop_state_acc(trival val)
    {
      is.state_based_acc = val.val();
    }

    /// \brief Whether this is a state-based Büchi automaton.
    ///
    /// An SBA has a Büchi acceptance, and should have its
    /// state-based acceptance property set.
    trival is_sba() const
    {
      return prop_state_acc() && acc().is_buchi();
    }

    /// \brief Whether the automaton is inherently weak.
    ///
    /// An automaton is inherently weak if accepting cycles and
    /// rejecting cycles are never mixed in the same strongly
    /// connected component.
    ///
    /// \see prop_weak()
    /// \see prop_terminal()
    trival prop_inherently_weak() const
    {
      return trival::from_repr_t(is.inherently_weak);
    }

    /// \brief Set the "inherently weak" property.
    ///
    /// Setting "inherently weak" to false automatically
    /// disables "terminal", "very weak", and "weak".
    ///
    /// \see prop_weak()
    /// \see prop_terminal()
    void prop_inherently_weak(trival val)
    {
      is.inherently_weak = val.val();
      if (!val)
        is.very_weak = is.terminal = is.weak = val.val();
    }

    /// \brief Whether the automaton is terminal.
    ///
    /// An automaton is terminal if it is weak, its accepting strongly
    /// connected components are complete, and no accepting edge leads
    /// to a non-accepting SCC.
    ///
    /// This property ensures that a word can be accepted as soon as
    /// one of its prefixes moves through an accepting edge.
    ///
    /// \see prop_weak()
    /// \see prop_inherently_weak()
    trival prop_terminal() const
    {
      return trival::from_repr_t(is.terminal);
    }

    /// \brief Set the terminal property.
    ///
    /// Marking an automaton as "terminal" automatically marks it as
    /// "weak" and "inherently weak".
    ///
    /// \see prop_weak()
    /// \see prop_inherently_weak()
    void prop_terminal(trival val)
    {
      is.terminal = val.val();
      if (val)
        is.inherently_weak = is.weak = val.val();
    }

    /// \brief Whether the automaton is weak.
    ///
    /// An automaton is weak if inside each strongly connected
    /// component, all transitions belong to the same acceptance sets.
    ///
    /// \see prop_terminal()
    /// \see prop_inherently_weak()
    trival prop_weak() const
    {
      return trival::from_repr_t(is.weak);
    }

    /// \brief Set the weak property.
    ///
    /// Marking an automaton as "weak" automatically marks it as
    /// "inherently weak".  Marking an automaton as "not weak"
    /// automatically marks it as "not terminal".
    ///
    /// \see prop_terminal()
    /// \see prop_inherently_weak()
    void prop_weak(trival val)
    {
      is.weak = val.val();
      if (val)
        is.inherently_weak = val.val();
      if (!val)
        is.very_weak = is.terminal = val.val();
    }

    /// \brief Whether the automaton is very-weak.
    ///
    /// An automaton is very-weak if it is weak (inside each strongly connected
    /// component, all transitions belong to the same acceptance sets)
    /// and each SCC contains only one state.
    ///
    /// \see prop_terminal()
    /// \see prop_weak()
    trival prop_very_weak() const
    {
      return trival::from_repr_t(is.very_weak);
    }

    /// \brief Set the very-weak property.
    ///
    /// Marking an automaton as "very-weak" automatically marks it as
    /// "weak" and "inherently weak".
    ///
    /// \see prop_terminal()
    /// \see prop_weak()
    void prop_very_weak(trival val)
    {
      is.very_weak = val.val();
      if (val)
        is.weak = is.inherently_weak = val.val();
    }


    /// \brief Whether the automaton is complete.
    ///
    /// An automaton is complete if for each state the union of the
    /// labels of its outgoing transitions is always true.
    ///
    /// Note that this method may return trival::maybe() when it is
    /// unknown whether the automaton is complete or not.  If you
    /// need a true/false answer, prefer the is_complete() function.
    ///
    /// \see prop_complete()
    /// \see is_complete()
    trival prop_complete() const
    {
      return trival::from_repr_t(is.complete);
    }

    /// \brief Set the complete property.
    ///
    /// \see is_complete()
    void prop_complete(trival val)
    {
      is.complete = val.val();
    }

    /// \brief Whether the automaton is universal.
    ///
    /// An automaton is universal if the conjunction between the
    /// labels of two transitions leaving a state is always false.
    ///
    /// Note that this method may return trival::maybe() when it is
    /// unknown whether the automaton is universal or not.  If you
    /// need a true/false answer, prefer the is_universal() function.
    ///
    /// \see prop_unambiguous()
    /// \see is_universal()
    trival prop_universal() const
    {
      return trival::from_repr_t(is.universal);
    }

    /// \brief Set the universal property.
    ///
    /// Setting the "universal" property automatically sets the
    /// "unambiguous" and "semi-deterministic" properties.
    ///
    /// \see prop_unambiguous()
    /// \see prop_semi_deterministic()
    void prop_universal(trival val)
    {
      is.universal = val.val();
      if (val)
        // universal implies unambiguous and semi-deterministic
        is.unambiguous = is.semi_deterministic = val.val();
    }

    // Starting with Spot 2.4, an automaton is deterministic if it is
    // both universal and existential, but as we already have
    // twa::is_existential(), we only need to additionally record the
    // universal property.  Before that, the deterministic property
    // was just a synonym for universal, hence we keep the deprecated
    // function prop_deterministic() with this meaning.
    SPOT_DEPRECATED("use prop_universal() instead")
    void prop_deterministic(trival val)
    {
      prop_universal(val);
    }

    SPOT_DEPRECATED("use prop_universal() instead")
    trival prop_deterministic() const
    {
      return prop_universal();
    }

    /// \brief Whether the automaton is unambiguous
    ///
    /// An automaton is unambiguous if any accepted word is recognized
    /// by exactly one accepting path in the automaton.  Any word
    /// (accepted or not) may be recognized by several rejecting paths
    /// in the automaton.
    ///
    /// Note that this method may return trival::maybe() when it is
    /// unknown whether the automaton is unambiguous or not.  If you
    /// need a true/false answer, prefer the is_unambiguous() function.
    ///
    /// \see prop_universal()
    /// \see is_unambiguous()
    trival prop_unambiguous() const
    {
      return trival::from_repr_t(is.unambiguous);
    }

    /// \brief Set the unambiguous property
    ///
    /// Marking an automaton as "non unambiguous" automatically
    /// marks it as "non universal".
    ///
    /// \see prop_deterministic()
    void prop_unambiguous(trival val)
    {
      is.unambiguous = val.val();
      if (!val)
        is.universal = val.val();
    }

    /// \brief Whether the automaton is semi-deterministic
    ///
    /// An automaton is semi-deterministic if the sub-automaton
    /// reachable from any accepting SCC is universal.
    ///
    /// Note that this method may return trival::maybe() when it is
    /// unknown whether the automaton is semi-deterministic or not.
    /// If you need a true/false answer, prefer the
    /// is_semi_deterministic() function.
    ///
    /// \see prop_universal()
    /// \see is_semi_deterministic()
    trival prop_semi_deterministic() const
    {
      return trival::from_repr_t(is.semi_deterministic);
    }

    /// \brief Set the semi-deterministic property
    ///
    /// Marking an automaton as "non semi-deterministic" automatically
    /// marks it as "non universal".
    ///
    /// \see prop_universal()
    void prop_semi_deterministic(trival val)
    {
      is.semi_deterministic = val.val();
      if (!val)
        is.universal = val.val();
    }

    /// \brief Whether the automaton is stutter-invariant.
    ///
    /// An automaton is stutter-invariant iff any accepted word
    /// remains accepted after removing a finite number of duplicate
    /// letters, or after duplicating a finite number of letters.
    ///
    /// Note that this method may return trival::maybe() when it is
    /// unknown whether the automaton is stutter-invariant or not.  If
    /// you need a true/false answer, prefer the
    /// is_stutter_invariant() function.
    ///
    /// \see is_stutter_invariant
    trival prop_stutter_invariant() const
    {
      return trival::from_repr_t(is.stutter_invariant);
    }

    /// \brief Set the stutter-invariant property
    void prop_stutter_invariant(trival val)
    {
      is.stutter_invariant = val.val();
    }

    /// \brief A structure for selecting a set of automaton properties
    /// to copy.
    ///
    /// When an algorithm copies an automaton before making some
    /// modification on that automaton, it should use a \c prop_set
    /// structure to indicate which properties should be copied from
    /// the original automaton.
    ///
    /// This structure does not list all supported properties, because
    /// properties are copied by groups of related properties.  For
    /// instance if an algorithm breaks the "inherent_weak"
    /// properties, it usually also breaks the "weak" and "terminal"
    /// properties.
    ///
    /// Set the flags to true to copy the value of the original
    /// property, and to false to ignore the original property
    /// (leaving the property of the new automaton to its default
    /// value, which is trival::maybe()).
    ///
    /// This can be used for instance as:
    /// \code
    ///    aut->prop_copy(other_aut, {true, false, false, false, false, true});
    /// \endcode
    /// This would copy the "state-based acceptance" and
    /// "stutter invariant" properties from \c other_aut to \c code.
    ///
    /// There are two flags for the determinism.  If \code
    /// deterministic is set, the universal, semi-deterministic,
    /// and unambiguous properties are copied as-is.  If deterministic
    /// is unset but improve_det is set, then those properties are
    /// only copied if they are positive.
    ///
    /// The reason there is no default value for these flags is that
    /// whenever we add a new property that does not fall into any of
    /// these groups, we will be forced to review all algorithms to
    /// decide if the property should be preserved or not.
    ///
    /// \see make_twa_graph_ptr
    /// \see prop_copy
    struct prop_set
    {
      bool state_based;     ///< preserve state-based acceptance
      bool inherently_weak; ///< preserve inherently weak, weak, & terminal
      bool deterministic;   ///< preserve deterministic, semi-det, unambiguous
      bool improve_det;     ///< improve deterministic, semi-det, unambiguous
      bool complete;        ///< preserve completeness
      bool stutter_inv;     ///< preserve stutter invariance

      prop_set()
      : state_based(false),
        inherently_weak(false),
        deterministic(false),
        improve_det(false),
        complete(false),
        stutter_inv(false)
      {
      }

      prop_set(bool state_based,
               bool inherently_weak,
               bool deterministic,
               bool improve_det,
               bool complete,
               bool stutter_inv)
      : state_based(state_based),
        inherently_weak(inherently_weak),
        deterministic(deterministic),
        improve_det(improve_det),
        complete(complete),
        stutter_inv(stutter_inv)
      {
      }

#ifndef SWIG
      // The "complete" argument was added in Spot 2.4
      SPOT_DEPRECATED("prop_set() now takes 6 arguments")
      prop_set(bool state_based,
               bool inherently_weak,
               bool deterministic,
               bool improve_det,
               bool stutter_inv)
      : state_based(state_based),
        inherently_weak(inherently_weak),
        deterministic(deterministic),
        improve_det(improve_det),
        complete(false),
        stutter_inv(stutter_inv)
      {
      }
#endif

      /// \brief An all-true \c prop_set
      ///
      /// Use that only in algorithms that copy an automaton without
      /// performing any modification.
      ///
      /// If an algorithm X does modifications, but preserves all the
      /// properties currently implemented, use an explicit
      ///
      /// \code
      ///    {true, true, true, true, true, true}
      /// \endcode
      ///
      /// instead of calling \c all().  This way, the day a new
      /// property is added, we will still be forced to review
      /// algorithm X, in case that new property is not preserved.
      static prop_set all()
      {
        return { true, true, true, true, true, true };
      }
    };

    /// \brief Copy the properties of another automaton.
    ///
    /// Copy the property specified with \a p from \a other to the
    /// current automaton.
    ///
    /// There is no default value for \a p on purpose.  This way any
    /// time we add a new property we have to update every call to
    /// prop_copy().
    ///
    /// \see prop_set
    void prop_copy(const const_twa_ptr& other, prop_set p)
    {
      if (p.state_based)
        prop_state_acc(other->prop_state_acc());
      if (p.inherently_weak)
        {
          prop_terminal(other->prop_terminal());
          prop_weak(other->prop_weak());
          prop_very_weak(other->prop_very_weak());
          prop_inherently_weak(other->prop_inherently_weak());
        }
      if (p.deterministic)
        {
          prop_universal(other->prop_universal());
          prop_semi_deterministic(other->prop_semi_deterministic());
          prop_unambiguous(other->prop_unambiguous());
        }
      else if (p.improve_det)
        {
          if (other->prop_universal().is_true())
            {
              prop_universal(true);
            }
          else
            {
              if (other->prop_semi_deterministic().is_true())
                prop_semi_deterministic(true);
              if (other->prop_unambiguous().is_true())
                prop_unambiguous(true);
            }
        }
      if (p.complete)
        prop_complete(other->prop_complete());
      if (p.stutter_inv)
        prop_stutter_invariant(other->prop_stutter_invariant());
    }

    /// \brief Keep only a subset of properties of the current
    /// automaton.
    ///
    /// All properties part of a group set to \c false in \a p are reset
    /// to their default value of trival::maybe().
    void prop_keep(prop_set p)
    {
      if (!p.state_based)
        prop_state_acc(trival::maybe());
      if (!p.inherently_weak)
        {
          prop_terminal(trival::maybe());
          prop_weak(trival::maybe());
          prop_very_weak(trival::maybe());
          prop_inherently_weak(trival::maybe());
        }
      if (!p.deterministic)
        {
          if (!(p.improve_det && prop_universal().is_true()))
            prop_universal(trival::maybe());
          if (!(p.improve_det && prop_semi_deterministic().is_true()))
            prop_semi_deterministic(trival::maybe());
          if (!(p.improve_det && prop_unambiguous().is_true()))
            prop_unambiguous(trival::maybe());
        }
      if (!p.complete)
        prop_complete(trival::maybe());
      if (!p.stutter_inv)
        prop_stutter_invariant(trival::maybe());
    }

    void prop_reset()
    {
      prop_keep({});
    }
  };

#ifndef SWIG
  namespace internal
  {
    inline twa_succ_iterable::~twa_succ_iterable()
    {
      if (it_)
        aut_->release_iter(it_);
    }
  }
#endif // SWIG

  /// \addtogroup twa_representation TωA representations
  /// \ingroup twa

  /// \addtogroup twa_algorithms TωA algorithms
  /// \ingroup twa

  /// \addtogroup twa_on_the_fly_algorithms TωA on-the-fly algorithms
  /// \ingroup twa_algorithms

  /// \addtogroup twa_io Input/Output of TωA
  /// \ingroup twa_algorithms

  /// \addtogroup stutter_inv Stutter-invariance checks
  /// \ingroup twa_algorithms

  /// \addtogroup twa_acc_transform Conversion between acceptance conditions
  /// \ingroup twa_algorithms

  /// \addtogroup twa_ltl Translating LTL formulas into TωA
  /// \ingroup twa_algorithms

  /// \addtogroup twa_generic Algorithm patterns
  /// \ingroup twa_algorithms

  /// \addtogroup twa_reduction TωA simplifications
  /// \ingroup twa_algorithms

  /// \addtogroup twa_misc Miscellaneous algorithms on TωA
  /// \ingroup twa_algorithms
}
