// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2012-2017 Laboratoire de Recherche et
// Developpement de l Epita (LRDE).
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

#include <set>

#include <cassert>
#include <spot/misc/bddlt.hh>
#include <spot/twa/twa.hh>

namespace spot
{

  // Forward declarations.  See below.
  class ta_succ_iterator;

  /// \defgroup ta TA (Testing Automata)
  ///
  /// This type and its cousins are listed \ref ta_essentials "here".
  /// This is an abstract interface.  Its implementations are \ref
  /// ta_representation "concrete representations".  The
  /// algorithms that work on spot::ta are \ref ta_algorithms
  /// "listed separately".

  /// \addtogroup ta_essentials Essential TA types
  /// \ingroup ta

  /// \ingroup ta_essentials
  /// \brief A Testing Automaton.
  ///
  /// The Testing Automata (TA) were introduced by
  /// Henri Hansen, Wojciech Penczek and Antti Valmari
  /// in "Stuttering-insensitive automata for on-the-fly detection of livelock
  /// properties" In Proc. of FMICSÕ02, vol. 66(2) of Electronic Notes in
  /// Theoretical Computer Science.Elsevier.
  ///
  /// While a TGBA automaton observes the value of the atomic propositions, the
  /// basic idea of TA is to detect the changes in these values; if a valuation
  ///  does not change between two consecutive valuations of an execution,
  /// the TA stay in the same state. A TA transition \c (s,k,d) is labeled by a
  /// "changeset" \c k: i.e. the set of atomic propositions that change between
  /// states \c s and \c d, if the changeset is empty then the transition is
  /// called stuttering transition.
  /// To detect execution that ends by stuttering in the same TA state, a
  /// new kind of acceptance states is introduced: "livelock-acceptance states"
  /// (in addition to the standard Buchi-acceptance states).
  ///
  /// Browsing such automaton can be achieved using two functions:
  /// \c get_initial_states_set or \c get_artificial_initial_state, and \c
  /// succ_iter. The former returns the initial state(s) while the latter lists
  /// the successor states of any state (filtred by "changeset").
  ///
  /// Note that although this is a transition-based automata,
  /// we never represent transitions!  Transition informations are
  /// obtained by querying the iterator over the successors of
  /// a state.

  class SPOT_API ta
  {
  protected:
    acc_cond acc_;
    bdd_dict_ptr dict_;

  public:
    ta(const bdd_dict_ptr& d)
      : dict_(d)
    {
    }

    virtual
    ~ta()
    {
    }

    typedef std::set<state*, state_ptr_less_than> states_set_t;
    typedef std::set<const state*, state_ptr_less_than> const_states_set_t;

    /// \brief Get the initial states set of the automaton.
    virtual const_states_set_t
    get_initial_states_set() const = 0;

    /// \brief Get the artificial initial state set of the automaton.
    /// Return 0 if this artificial state is not implemented
    /// (in this case, use \c get_initial_states_set)
    /// The aim of adding this state is to have a unique initial state. This
    /// artificial initial state have one transition to each real initial state,
    /// and this transition is labeled by the corresponding initial condition.
    /// (For more details, see the paper cited above)
    virtual const spot::state*
    get_artificial_initial_state() const
    {
      return nullptr;
    }

    /// \brief Get an iterator over the successors of \a state.
    ///
    /// The iterator has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    ///
    virtual ta_succ_iterator*
    succ_iter(const spot::state* state) const = 0;

    /// \brief Get an iterator over the successors of \a state
    /// filtred by the changeset on transitions
    ///
    /// The iterator has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    ///
    virtual ta_succ_iterator*
    succ_iter(const spot::state* state, bdd changeset) const = 0;

    /// \brief Get the dictionary associated to the automaton.
    ///
    /// State are represented as BDDs.  The dictionary allows
    /// to map BDD variables back to formulae, and vice versa.
    /// This is useful when dealing with several automata (which
    /// may use the same BDD variable for different formula),
    /// or simply when printing.
    bdd_dict_ptr
    get_dict() const
    {
      return dict_;
    }

    /// \brief Format the state as a string for printing.
    ///
    /// This formating is the responsability of the automata
    /// that owns the state.
    virtual std::string
    format_state(const spot::state* s) const = 0;

    /// \brief Return true if \a s is a Buchi-accepting state, otherwise false
    virtual bool
    is_accepting_state(const spot::state* s) const = 0;

    /// \brief Return true if \a s is a livelock-accepting state
    /// , otherwise false
    virtual bool
    is_livelock_accepting_state(const spot::state* s) const = 0;

    /// \brief Return true if \a s is an initial state, otherwise false
    virtual bool
    is_initial_state(const spot::state* s) const = 0;

    /// \brief Return a BDD condition that represents the valuation
    /// of atomic propositions in the state \a s
    virtual bdd
    get_state_condition(const spot::state* s) const = 0;

    /// \brief Release a state \a s
    virtual void
    free_state(const spot::state* s) const = 0;


    const acc_cond& acc() const
    {
      return acc_;
    }

    acc_cond& acc()
    {
      return acc_;
    }

  };

  typedef std::shared_ptr<ta> ta_ptr;
  typedef std::shared_ptr<const ta> const_ta_ptr;

  /// \ingroup ta_essentials
  /// \brief Iterate over the successors of a state.
  ///
  /// This class provides the basic functionalities required to
  /// iterate over the successors of a state, as well as querying
  /// transition labels.  Because transitions are never explicitely
  /// encoded, labels (conditions and acceptance conditions) can only
  /// be queried while iterating over the successors.
  class ta_succ_iterator : public twa_succ_iterator
  {
  public:
    virtual
    ~ta_succ_iterator()
    {
    }
  };

#ifndef SWIG
  // A stack of Strongly-Connected Components
  class scc_stack_ta
  {
  public:
    struct connected_component
    {
    public:
      connected_component(int index = -1) noexcept;

      /// Index of the SCC.
      int index;

      bool is_accepting;

      /// The bdd condition is the union of all acceptance conditions of
      /// transitions which connect the states of the connected component.
      acc_cond::mark_t condition;

      std::list<const state*> rem;
    };

    /// Stack a new SCC with index \a index.
    void
    push(int index);

    /// Access the top SCC.
    connected_component&
    top();

    /// Access the top SCC.
    const connected_component&
    top() const;

    /// Pop the top SCC.
    void
    pop();

    /// How many SCC are in stack.
    size_t
    size() const;

    /// The \c rem member of the top SCC.
    std::list<const state*>&
    rem();

    /// Is the stack empty?
    bool
    empty() const;

    typedef std::list<connected_component> stack_type;
    stack_type s;
  };
#endif // !SWIG

/// \addtogroup ta_representation TA representations
/// \ingroup ta

/// \addtogroup ta_algorithms TA algorithms
/// \ingroup ta

/// \addtogroup ta_io Input/Output of TA
/// \ingroup ta_algorithms

/// \addtogroup tgba_ta Transforming TGBA into TA
/// \ingroup ta_algorithms


/// \addtogroup ta_generic Algorithm patterns
/// \ingroup ta_algorithms

/// \addtogroup ta_reduction TA simplifications
/// \ingroup ta_algorithms

/// \addtogroup ta_misc Miscellaneous algorithms on TA
/// \ingroup ta_algorithms
}
