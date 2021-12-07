// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2018, 2020 Laboratoire de
// Recherche et Developpement de l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

#include <map>
#include <list>
#include <iosfwd>
#include <bddx.h>
#include <spot/misc/optionmap.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/emptiness_stats.hh>

namespace spot
{
  struct twa_run;
  typedef std::shared_ptr<twa_run> twa_run_ptr;
  typedef std::shared_ptr<const twa_run> const_twa_run_ptr;

  /// \addtogroup emptiness_check Emptiness-checks
  /// \ingroup twa_algorithms
  ///
  /// You can create an emptiness check either by instantiating it
  /// explicitly (calling one of the functions of \ref
  /// emptiness_check_algorithms "this list"), or indirectly via
  /// spot::make_emptiness_check_instantiator().  The latter function
  /// allows user-options to influence the choice of the
  /// emptiness-check algorithm used, and the intermediate
  /// instantiator object can be used to query to properties of the
  /// emptiness check selected.
  ///
  /// All emptiness-check algorithms follow the same interface.
  /// Basically once you have constructed an instance of
  /// spot::emptiness_check, you should call
  /// spot::emptiness_check::check() to check the automaton.
  ///
  /// If spot::emptiness_check::check() returns 0, then the automaton
  /// was found empty.  Otherwise the automaton accepts some run.
  /// (Beware that some algorithms---those using bit-state
  /// hashing---may found the automaton to be empty even if it is not
  /// actually empty.)
  ///
  /// When spot::emptiness_check::check() does not return 0, it
  /// returns an instance of spot::emptiness_check_result.  You can
  /// try to call spot::emptiness_check_result::accepting_run() to
  /// obtain an accepting run.  For some emptiness-check algorithms,
  /// spot::emptiness_check_result::accepting_run() will require some
  /// extra computation.  Most emptiness-check algorithms are able to
  /// return such an accepting run, however this is not mandatory and
  /// spot::emptiness_check_result::accepting_run() can return 0 (this
  /// does not means by anyway that no accepting run exist).
  ///
  /// The acceptance run returned by
  /// spot::emptiness_check_result::accepting_run(), if any, is of
  /// type spot::twa_run.  \ref twa_run "This page" gathers existing
  /// operations on these objects.
  ///
  /// @{

  /// \brief The result of an emptiness check.
  ///
  /// Instances of these class should not last longer than the
  /// instances of emptiness_check that produced them as they
  /// may reference data internal to the check.
  class SPOT_API emptiness_check_result
  {
  public:
    emptiness_check_result(const const_twa_ptr& a,
                           option_map o = option_map())
      : a_(a), o_(o)
    {
    }

    virtual
    ~emptiness_check_result()
    {
    }

    /// \brief Return a run accepted by the automata passed to
    /// the emptiness check.
    ///
    /// This method might actually compute the acceptance run.  (Not
    /// all emptiness check algorithms actually produce a
    /// counter-example as a side-effect of checking emptiness, some
    /// need some post-processing.)
    ///
    /// This can also return 0 if the emptiness check algorithm
    /// cannot produce a counter example (that does not mean there
    /// is no counter-example; the mere existence of an instance of
    /// this class asserts the existence of a counter-example).
    virtual twa_run_ptr accepting_run();

    /// The automaton on which an accepting_run() was found.
    const const_twa_ptr&
    automaton() const
    {
      return a_;
    }

    /// Return the options parametrizing how the accepting run is computed.
    const option_map&
    options() const
    {
      return o_;
    }

    /// Modify the algorithm options.
    const char* parse_options(char* options);

    /// Return statistics, if available.
    virtual const unsigned_statistics* statistics() const;

  protected:
    /// Notify option updates.
    virtual void options_updated(const option_map& old);

    const_twa_ptr a_;                ///< The automaton.
    option_map o_;                ///< The options.
  };

  typedef std::shared_ptr<emptiness_check_result> emptiness_check_result_ptr;

  /// Common interface to emptiness check algorithms.
  class SPOT_API emptiness_check:
    public std::enable_shared_from_this<emptiness_check>
  {
  public:
    emptiness_check(const const_twa_ptr& a, option_map o = option_map())
      : a_(a), o_(o)
    {
    }
    virtual ~emptiness_check();

    /// The automaton that this emptiness-check inspects.
    const const_twa_ptr&
    automaton() const
    {
      return a_;
    }

    /// Return the options parametrizing how the emptiness check is realized.
    const option_map&
    options() const
    {
      return o_;
    }

    /// Modify the algorithm options.
    const char* parse_options(char* options);

    /// Return false iff accepting_run() can return 0 for non-empty automata.
    virtual bool safe() const;

    /// \brief Check whether the automaton contain an accepting run.
    ///
    /// Return 0 if the automaton accepts no run.  Return an instance
    /// of emptiness_check_result otherwise.  This instance might
    /// allow to obtain one sample acceptance run.  The result has to
    /// be destroyed before the emptiness_check instance that
    /// generated it.
    ///
    /// Some emptiness_check algorithms may allow check() to be called
    /// several time, but generally you should not assume that.
    ///
    /// Some emptiness_check algorithms, especially those using bit state
    /// hashing may return 0 even if the automaton is not empty.
    /// \see safe()
    virtual emptiness_check_result_ptr check() = 0;

    /// Return statistics, if available.
    virtual const unsigned_statistics* statistics() const;

    /// Return emptiness check statistics, if available.
    virtual const ec_statistics* emptiness_check_statistics() const;

    /// Print statistics, if any.
    virtual std::ostream& print_stats(std::ostream& os) const;

    /// Notify option updates.
    virtual void options_updated(const option_map& old);

  protected:
    const_twa_ptr a_;                ///< The automaton.
    option_map o_;                ///< The options
  };

  typedef std::shared_ptr<emptiness_check> emptiness_check_ptr;

  class emptiness_check_instantiator;
  typedef std::shared_ptr<emptiness_check_instantiator>
    emptiness_check_instantiator_ptr;

  /// Dynamically create emptiness checks.  Given their name and options.
  class SPOT_API emptiness_check_instantiator
  {
  public:
    /// Actually instantiate the emptiness check, for \a a.
    emptiness_check_ptr instantiate(const const_twa_ptr& a) const;

    /// Accessor to the options.
    /// @{
    const option_map&
    options() const
    {
      return o_;
    }

    option_map&
    options()
    {
      return o_;
    }
    /// @}

    /// \brief Minimum number of acceptance sets supported by
    /// the emptiness check.
    unsigned int min_sets() const;

    /// \brief Maximum number of acceptance conditions supported by
    /// the emptiness check.
    ///
    /// \return \c -1U if no upper bound exists.
    unsigned int max_sets() const;
  protected:
    emptiness_check_instantiator(option_map o, void* i);

    option_map o_;
    void *info_;
  };

  /// \brief Create an emptiness-check instantiator, given the name
  /// of an emptiness check.
  ///
  /// This is a convenient entry point to instantiate an emptiness
  /// check with user-supplied options.
  ///
  /// \param name should have the form \c "name" or \c "name(options)".
  ///
  /// \return Return an emptiness-check instantiator.  On error, the
  /// function returns \c nullptr.  If the name of the algorithm was
  /// unknown, \c *err will be set to \c name.  If some fragment of
  /// the options could not be parsed, \c *err will point to that
  /// fragment.
  ///
  /// The following names supported and correspond to different emptiness
  /// check algorithms:
  /// - `Cou99` uses `spot::couvreur99()`, that works with Fin-less
  ///       acceptance conditions, with any number of acceptance sets.
  ///       The following options can be used:
  ///     - `shy` Compute all successors of each state, then explore already
  ///       visited states first. This usually helps to merge SCCs, and
  ///       thus exit sooner. However because all successors have to be
  ///       computed and stored, it often consume more memory.
  ///     - `group` Setting this option is meaningful only when shy is
  ///       used. If set (the default), the successors of all the states
  ///       that belong to the same SCC will be considered when choosing
  ///       a successor. Otherwise, only the successor of the topmost
  ///       state on the DFS stack are considered.
  ///     - `poprem` Specifies how the algorithm should handle the
  ///       destruction of non-accepting maximal strongly connected
  ///       components. If poprem is set, the algorithm will keep a list
  ///       of all states of a SCC that are fully processed and should be
  ///       removed once the MSCC is popped. If poprem is unset (the
  ///       default), the MSCC will be traversed again (i.e. generating
  ///       the successors of the root recursively) for deletion. This
  ///       is a choice between memory and speed.
  ///
  ///   Examples:
  ///   \code
  ///   Cou99
  ///   Cou99(shy !group)
  ///   Cou99(shy group)
  ///   Cou99(poprem)
  ///   Cou99(poprem shy !group)
  ///   Cou99(poprem shy group)
  ///   \endcode
  ///
  /// - `GC04` uses `spot::explicit_gv04_check()` and works on automata
  ///   with Fin-less acceptance conditions using at most one acceptance
  ///   set.  No options are supported.
  ///
  ///   Example:
  ///   \code
  ///   GC04
  ///   \endcode
  ///
  /// - `CVWY90` uses `spot::magic_search()` and work on automata with
  ///   Fin-less acceptance conditions using at most one acceptance
  ///   set.  Set option `bsh` to the size of a hash-table if you want
  ///   to activate bit-state hashing.
  ///
  ///   Examples:
  ///   \code
  ///   CVWY90
  ///   CVWY90(bsh=4M)
  ///   \endcode
  ///
  /// - `SE05` uses `spot::se05()` and works on work on automata with
  ///   Fin-less acceptance conditions using at most one acceptance
  ///   set.  Set option `bsh` to the size of a hash-table if you want
  ///   to activate bit-state hashing.
  ///
  ///   Examples:
  ///   \code
  ///   SE05
  ///   SE05(bsh=4M)
  ///   \endcode
  ///
  /// - `Tau03` uses `spot::explicit_tau03_search()` and work on automata with
  ///   Fin-less acceptance conditions using at least one acceptance
  ///   set.  No options are supported.
  ///
  ///   Example:
  ///   \code
  ///   Tau03
  ///   \endcode
  ///
  /// - `Tau03_opt` uses `spot::explicit_tau03_opt_search()` and work on
  /// automata with any Fin-less acceptance.  The following options are
  /// supported:
  ///   - `weights` This option is set by default and activates the usage
  ///     of weights in the top-level DFS as well as in nested DFSs.
  ///   - `redweights` This option is set by default, and activates the
  ///     usage of weights in nested DFSs. It is meaningful only if
  ///     weights is set.
  ///   - `condstack` This option is unset by default, and activates
  ///     the use of the "conditional stack" optimization described by
  ///     Heikki Tauriainen.
  ///   - ordering This option is unset by default, and activates the
  ///     use of the "ordering" heuristic described by Heikki Tauriainen
  ///     in Research Report A96 from the Helsinki University of Technology.
  ///
  ///   Example:
  ///   \code
  ///   Tau03_opt
  ///   Tau03_opt(!weights)
  ///   Tau03_opt(!redweights)
  ///   Tau03_opt(condstack)
  ///   Tau03_opt(condstack !weights)
  ///   Tau03_opt(condstack !redweights)
  ///   \endcode
  SPOT_API emptiness_check_instantiator_ptr
  make_emptiness_check_instantiator(const char* name, const char** err);

  /// @}


  /// \addtogroup emptiness_check_algorithms Emptiness-check algorithms
  /// \ingroup emptiness_check

  /// \addtogroup twa_run TωA runs and supporting functions
  /// \ingroup emptiness_check
  /// @{

  /// An accepted run, for a twa.
  struct SPOT_API twa_run final
  {
    struct step {
      const state* s;
      bdd label;
      acc_cond::mark_t acc;

      step(const state* s, bdd label, acc_cond::mark_t acc) noexcept
        : s(s), label(label), acc(acc)
      {
      }
      step() = default;
    };

    typedef std::list<step> steps;

    steps prefix;
    steps cycle;
    const_twa_ptr aut;

    ~twa_run();
    twa_run(const const_twa_ptr& aut) noexcept
      : aut(aut)
    {
    }
    twa_run(const twa_run& run);
    twa_run& operator=(const twa_run& run);

    /// \brief Raise an exception of the cycle is empty.
    ///
    /// It is OK for a twa_run to have an empty cycle while the run is
    /// being filled by some procedure.  But after that, we expect
    /// cycles to be non-empty.  Calling this function will raise an
    /// std::runtime_error if the cycle is empty, giving \a where
    /// (usually the name of the calling function) as context.
    void ensure_non_empty_cycle(const char* where) const;

    /// \brief Reduce an accepting run.
    ///
    /// Return a run which is still accepting for <code>aut</code>,
    /// but is no longer than this one.
    ///
    /// This is done by trying to find a fragment of the accepting
    /// single that is accepting, and trying to close a cycle around
    /// this fragment with fewer edges than in the original cycle.
    /// (This step works best in Fin-less automata.)  And then trying
    /// to find a shorter prefix leading to any state of the cycle.
    twa_run_ptr reduce() const;

    /// \brief Project an accepting run
    ///
    /// This only works if the automaton associated to this run has
    /// been created with otf_product() or product(), and \a other is
    /// one of the two operands of the product.
    ///
    /// Use the \a right Boolean to specify whether \a other was a
    /// left or right operand.
    twa_run_ptr project(const const_twa_ptr& other, bool right = false);

    /// \brief Replay a run.
    ///
    /// This is similar to <code>os << run;</code>, except that the
    /// run is actually replayed on the automaton while it is printed.
    /// The output will stop if the run cannot be completed.
    ///
    /// \param os the stream on which the replay should be traced
    /// \param debug if set the output will be more verbose and extra
    ///              debugging informations will be output on failure
    /// \return true iff the run could be completed
    bool replay(std::ostream& os, bool debug = false) const;

    /// \brief Highlight the accepting run on the automaton.
    ///
    /// Note that this works only if the automaton is a twa_graph_ptr.
    void highlight(unsigned color);

    /// \brief Return a twa_graph_ptr corresponding to \a run
    ///
    /// Identical states are merged.
    ///
    /// If \a preserve_names is set, the created states are named
    /// using the format_state() result from the original state.
    twa_graph_ptr as_twa(bool preserve_names = false) const;

    /// \brief Display a twa_run.
    ///
    /// Output the prefix and cycle parts of the twa_run \a run on \a os.
    ///
    /// The automaton object (stored by \a run) is used only to format
    /// the states, and to know how to print the BDDs describing the
    /// conditions and acceptance conditions of the run; it is
    /// <b>not</b> used to replay the run.  In other words this
    /// function will work even if the twa_run you are trying to print
    /// appears to connect states that are not connected.
    ///
    /// This is unlike replay_twa_run(), which will ensure the run
    /// actually exists in the automaton (and will also display any
    /// transition annotation).
    SPOT_API
    friend std::ostream& operator<<(std::ostream& os, const twa_run& run);
  };
  /// @}

  /// \addtogroup emptiness_check_stats Emptiness-check statistics
  /// \ingroup emptiness_check
}
