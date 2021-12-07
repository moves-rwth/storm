// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2013-2016, 2018-2020 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005, 2006 Laboratoire d'Informatique de
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

#pragma once

#include <stack>
#include <spot/twaalgos/gtec/status.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>

namespace spot
{
  /// \addtogroup emptiness_check_algorithms
  /// @{

  /// \brief Check whether the language of an automate is empty.
  ///
  /// This is based on \cite couvreur.99.fm .
  ///
  /// A recursive definition of the algorithm would look as follows,
  /// but the implementation is of course not recursive.
  /// (<code>&lt;Sigma, Q, delta, q, F&gt;</code> is the automaton to
  /// check, H is an associative array mapping each state to its
  /// positive DFS order or 0 if it is dead, SCC is and ACC are two
  /// stacks.)
  ///
  /** \verbatim
      check(<Sigma, Q, delta, q, F>, H, SCC, ACC)
        if q is not in H   // new state
            H[q] = H.size + 1
            SCC.push(<H[q], {}>)
            forall <a, s> : <q, _, a, s> in delta
                ACC.push(a)
                res = check(<Sigma, Q, delta, s, F>, H, SCC, ACC)
                if res
                    return res
            <n, _> = SCC.top()
            if n = H[q]
                SCC.pop()
                mark_reachable_states_as_dead(<Sigma, Q, delta, q, F>, H$)
            return 0
        else
            if H[q] = 0 // dead state
                ACC.pop()
                return true
            else // state in stack: merge SCC
                all = {}
                do
                    <n, a> = SCC.pop()
                    all = all union a union { ACC.pop() }
                until n <= H[q]
                SCC.push(<n, all>)
                if all != F
                    return 0
                return new emptiness_check_result(necessary data)
      \endverbatim */
  ///
  /// check() returns 0 iff the automaton's language is empty.  It
  /// returns an instance of emptiness_check_result.  If the automaton
  /// accept a word.  (Use emptiness_check_result::accepting_run() to
  /// extract an accepting run.)
  ///
  /// There are two variants of this algorithm: spot::couvreur99_check and
  /// spot::couvreur99_check_shy.  They differ in their memory usage, the
  /// number for successors computed before they are used and the way
  /// the depth first search is directed.
  ///
  /// spot::couvreur99_check performs a straightforward depth first search.
  /// The DFS stacks store twa_succ_iterators, so that only the
  /// iterators which really are explored are computed.
  ///
  /// spot::couvreur99_check_shy tries to explore successors which are
  /// visited states first.  this helps to merge SCCs and generally
  /// helps to produce shorter counter-examples.  However this
  /// algorithm cannot stores unprocessed successors as
  /// twa_succ_iterators: it must compute all successors of a state
  /// at once in order to decide which to explore first, and must keep
  /// a list of all unexplored successors in its DFS stack.
  ///
  /// The couvreur99() function is a wrapper around these two flavors
  /// of the algorithm.  \a options is an option map that specifies
  /// which algorithms should be used, and how.
  ///
  /// The following options are available.
  /// \li \c "shy" : if non zero, then spot::couvreur99_check_shy is used,
  ///                otherwise (and by default) spot::couvreur99_check is used.
  ///
  /// \li \c "poprem" : specifies how the algorithm should handle the
  /// destruction of non-accepting maximal strongly connected
  /// components.  If \c poprem is non null, the algorithm will keep a
  /// list of all states of a SCC that are fully processed and should
  /// be removed once the MSCC is popped.  If \c poprem is null (the
  /// default), the MSCC will be traversed again (i.e. generating the
  /// successors of the root recursively) for deletion.  This is a
  /// choice between memory and speed.
  ///
  /// \li \c "group" : this options is used only by spot::couvreur99_check_shy.
  /// If non null (the default), the successors of all the
  /// states that belong to the same SCC will be considered when
  /// choosing a successor.  Otherwise, only the successor of the
  /// topmost state on the DFS stack are considered.
  SPOT_API emptiness_check_ptr
  couvreur99(const const_twa_ptr& a, option_map options = option_map());

#ifndef SWIG
  /// \brief An implementation of the Couvreur99 emptiness-check algorithm.
  ///
  /// See the documentation for spot::couvreur99.
  class SPOT_API couvreur99_check:
    // We inherit from ec_statistics first to work around GCC bug #90309.
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90309#c6
    public ec_statistics, public emptiness_check
  {
  public:
    couvreur99_check(const const_twa_ptr& a, option_map o = option_map());

    virtual ~couvreur99_check();

    /// Check whether the automaton's language is empty.
    virtual emptiness_check_result_ptr check() override;

    virtual std::ostream& print_stats(std::ostream& os) const override;

    /// \brief Return the status of the emptiness-check.
    ///
    /// When check() succeed, the status should be passed along
    /// to spot::counter_example.
    ///
    /// This status should not be deleted, it is a pointer
    /// to a member of this class that will be deleted when
    /// the couvreur99 object is deleted.
    std::shared_ptr<const couvreur99_check_status> result() const;

  protected:
    std::shared_ptr<couvreur99_check_status> ecs_;
    /// \brief Remove a strongly component from the hash.
    ///
    /// This function remove all accessible state from a given
    /// state. In other words, it removes the strongly connected
    /// component that contains this state.
    void remove_component(const state* start_delete);

    /// Whether to store the state to be removed.
    bool poprem_;
    /// Number of dead SCC removed by the algorithm.
    unsigned removed_components;

    unsigned get_removed_components() const;
    unsigned get_vmsize() const;
  };

  /// \brief A version of spot::couvreur99_check that tries to visit
  /// known states first.
  ///
  /// See the documentation for spot::couvreur99.
  class SPOT_API couvreur99_check_shy final: public couvreur99_check
  {
  public:
    couvreur99_check_shy(const const_twa_ptr& a, option_map o = option_map());
    virtual ~couvreur99_check_shy();

    virtual emptiness_check_result_ptr check() override;

  protected:
    struct successor {
      acc_cond::mark_t acc;
      const spot::state* s;
      successor(acc_cond::mark_t acc, const spot::state* s) noexcept
        : acc(acc), s(s) {}
    };

    // We use five main data in this algorithm:
    // * couvreur99_check::root, a stack of strongly connected components (SCC),
    // * couvreur99_check::h, a hash of all visited nodes, with their order,
    //   (it is called "Hash" in Couvreur's paper)
    // * arc, a stack of acceptance conditions between each of these SCC,
    std::stack<acc_cond::mark_t> arc;
    // * num, the number of visited nodes.  Used to set the order of each
    //   visited node,
    int num;
    // * todo, the depth-first search stack.  This holds pairs of the
    //   form (STATE, SUCCESSORS) where SUCCESSORS is a list of
    //   (ACCEPTANCE_CONDITIONS, STATE) pairs.
    typedef std::list<successor> succ_queue;

    // Position in the loop seeking known successors.
    succ_queue::iterator pos;

    struct todo_item
    {
      const state* s;
      int n;
      succ_queue q;                // Unprocessed successors of S
      todo_item(const state* s, int n, couvreur99_check_shy* shy);
    };

    typedef std::list<todo_item> todo_list;
    todo_list todo;

    void clear_todo();

    /// Whether successors should be grouped for states in the same SCC.
    bool group_;
    // If the "group2" option is set (it implies "group"), we
    // reprocess the successor states of SCC that have been merged.
    bool group2_;
  };
#endif

  /// @}
}
