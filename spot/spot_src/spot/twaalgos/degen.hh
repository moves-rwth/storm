// -*- coding: utf-8 -*-
// Copyright (C) 2012-2015, 2017-2020 Laboratoire de
// Recherche et Développement de l'Epita.
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

#include <spot/twa/twagraph.hh>

namespace spot
{
  class scc_info;

  /// \ingroup twa_acc_transform
  /// \brief Degeneralize a spot::tgba into an equivalent sba with
  /// only one acceptance condition.
  ///
  /// This algorithm will build a new explicit automaton that has
  /// at most (N+1) times the number of states of the original automaton.
  ///
  /// When \a use_z_lvl is set, the level of the degeneralized
  /// automaton is reset everytime an SCC is exited.  If \a
  /// use_cust_acc_orders is set, the degeneralization will compute a
  /// custom acceptance order for each SCC (this option is disabled by
  /// default because our benchmarks show that it usually does more
  /// harm than good).  If \a use_lvl_cache is set, everytime an SCC
  /// is entered on a state that as already been associated to some
  /// level elsewhere, reuse that level (set it to 2 to keep the
  /// smallest number, 3 to keep the largest level, and 1 to keep the
  /// first level found). If \a ignaccsl is set, we do not directly
  /// jump to the accepting level if the entering state has an
  /// accepting self-loop.  If \a remove_extra_scc is set (the default)
  /// we ensure that the output automaton has as many SCCs as the input
  /// by removing superfluous SCCs.
  ///
  /// Any of these three options will cause the SCCs of the automaton
  /// \a a to be computed prior to its actual degeneralization.
  ///
  /// The degeneralize_tba() variant produce a degeneralized automaton
  /// with transition-based acceptance.
  ///
  /// The mapping between each state of the resulting automaton
  /// and the original state of the input automaton is stored in the
  /// "original-states" named property of the produced automaton.  Call
  /// `aut->get_named_prop<std::vector<unsigned>>("original-states")`
  /// to retrieve it.  However be aware that if the input automaton
  /// already defines the "original-states" named property, it will
  /// be composed with the new one, so the "original-states" of the
  /// degeneralized automaton will refer to the same automaton as the
  /// "original-states" of the input automaton.
  ///
  /// Note that these functions may return the original
  /// automaton as-is if it is already degeneralized; in this case
  /// the "original-states" property is not defined (or not changed).
  ///
  /// Similarly, the property "degen-levels" keeps track of the degeneralization
  /// levels.  To retrieve it, call
  /// `aut->get_named_prop<std::vector<unsigned>>("degen-levels")`.
  /// \@{
  SPOT_API twa_graph_ptr
  degeneralize(const const_twa_graph_ptr& a, bool use_z_lvl = true,
               bool use_cust_acc_orders = false,
               int use_lvl_cache = 1,
               bool skip_levels = true,
               bool ignaccsl = false,
               bool remove_extra_scc = true);

  SPOT_API twa_graph_ptr
  degeneralize_tba(const const_twa_graph_ptr& a, bool use_z_lvl = true,
                   bool use_cust_acc_orders = false,
                   int use_lvl_cache = 1,
                   bool skip_levels = true,
                   bool ignaccsl = false,
                   bool remove_extra_scc = true);
  /// \@}

  /// \ingroup twa_acc_transform
  /// \brief Partial degeneralization of a TwA
  ///
  /// Given an automaton whose acceptance contains a conjunction of
  /// Inf terms, perform a partial degeneralization to replace this
  /// conjunction by a single Inf term.
  ///
  /// For instance if the input has acceptance
  ///    (Fin(0)&Inf(1)&Inf(3))|Fin(2)
  /// calling partial_degeneralize with \a todegen set to `{1,3}`
  /// will build an equivalent automaton with acceptance
  ///    (Fin(0)&Inf(2))|Fin(1)
  ///
  /// where Inf(2) tracks the acceptance of the original
  /// Inf(1)&Inf(3), and Fin(1) tracks the acceptance of the original
  /// Fin(2).
  ///
  /// Cases where the sets listed in \a todegen also occur outside
  /// of the Inf-conjunction are also supported.  Subformulas that
  /// are disjunctions of Fin(.) terms (e.g., Fin(1)|Fin(2)) will
  /// be degeneralized as well.
  ///
  /// If this functions is called with a value of \a todegen that does
  /// not match a conjunction of Inf(.), or a disjunction of Fin(.),
  /// an std::runtime_error exception is thrown.
  ///
  /// The version of the function that has no \a todegen argument will
  /// perform all possible partial degeneralizations, and may return
  /// the input automaton unmodified if no partial degeneralization is
  /// possible.
  ///
  /// The "original-state" and "degen-levels" named properties are
  /// updated as for degeneralize() and degeneralize_tba().
  /// @{
  SPOT_API twa_graph_ptr
  partial_degeneralize(const const_twa_graph_ptr& a,
                       acc_cond::mark_t todegen);
  SPOT_API twa_graph_ptr
  partial_degeneralize(twa_graph_ptr a);
  /// @}

  /// \brief Is the automaton partially degeneralizable?
  ///
  /// Return a mark `M={m₁, m₂, ..., mₙ}` such that either
  /// `Inf(m₁)&Inf(m₂)&...&Inf(mₙ)` or `Fin(m₁)|Fin(m₂)|...|Fin(mₙ)`
  /// appears in the acceptance condition of \a aut.
  ///
  /// If multiple such marks exist the smallest such mark is returned.
  /// (This is important in case of overlapping options.  E.g., in the
  /// formula `Inf(0)&Inf(1)&Inf(3) | (Inf(0)&Inf(1))&Fin(2)` we have
  /// two possible degeneralizations options `{0,1,3}`, and `{0,1}`.
  /// Degeneralizing for `{0,1,3}` and then `{0,1}` could enlarge the
  /// automaton by a factor 6, while degeneralizing by `{0,1}` and
  /// then some `{x,y}` may enlarge the automaton only by a factor 4.)
  ///
  /// Return an empty mark otherwise if the automaton is not partially
  /// degeneralizable.
  ///
  /// The optional arguments \a allow_inf and \a allow_fin, can be set
  /// to false to disallow one type of match.
  SPOT_API acc_cond::mark_t
  is_partially_degeneralizable(const const_twa_graph_ptr& aut,
                               bool allow_inf = true,
                               bool allow_fin = true,
                               std::vector<acc_cond::mark_t> forbid = {});

  /// \ingroup twa_algorithms
  /// \brief Propagate marks around the automaton
  ///
  /// For each state of the automaton, marks that are common
  /// to all input transitions will be pushed on the outgoing
  /// transitions, and marks that are common to all outgoing
  /// transitions will be pulled to the input transitions.
  /// This considers only transitions that are not self-loops
  /// and that belong to some SCC.  If an scc_info has already
  /// been built, pass it as \a si to avoid building it again.
  ///
  /// Two variants of the algorithm are provided.  One modifies
  /// the automaton in place; the second returns a vector of marks
  /// indexed by transition numbers.
  ///
  /// @{
  SPOT_API std::vector<acc_cond::mark_t>
  propagate_marks_vector(const const_twa_graph_ptr& aut,
                         const scc_info* si = nullptr);

  SPOT_API void
  propagate_marks_here(twa_graph_ptr& aut,
                       const scc_info* si = nullptr);
  /// @}
}
