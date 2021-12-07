// -*- coding: utf-8 -*-
// Copyright (C) 2014-2017, 2019 Laboratoire de Recherche
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

#include <spot/twa/twagraph.hh>

/// \defgroup stutter_inv Stutter-invariance checks and related functions

namespace spot
{
  /// \ingroup stutter_inv
  /// \brief Close the automaton by allowing letters to be duplicated.
  ///
  /// Any letter that enters a state will spawn a copy of this state
  /// with a self-loop using the same letter.  For more details
  /// about this function, see \cite michaud.15.spin .
  SPOT_API twa_graph_ptr
  sl(const_twa_graph_ptr aut);

  /// @{
  /// \ingroup stutter_inv
  /// \brief Close the automaton by allowing letters to be duplicated.
  ///
  /// For any transition (s,d) labeled by a letter ℓ, we add a state x
  /// and three transitions (s,x), (x,x), (x,d) all labeled by ℓ.
  /// For more details about this function, see \cite michaud.15.spin .
  ///
  /// The inplace version of the function modifies the input
  /// automaton.
  SPOT_API twa_graph_ptr
  sl2_inplace(twa_graph_ptr aut);

  SPOT_API twa_graph_ptr
  sl2(const_twa_graph_ptr aut);
  /// @}

  /// @{
  /// \ingroup stutter_inv
  /// \brief Close the automaton by allowing duplicate letter removal.
  ///
  /// This is done by adding shortcuts into the automaton.  If (x,y) is
  /// a transition labeled by B, and (y,z) is a transition labeled by C,
  /// we add a transition (x,z) labeled by B∧C.
  ///
  /// For more details about this function, see \cite michaud.15.spin .
  ///
  /// The inplace version of the function modifies the input
  /// automaton.
  SPOT_API twa_graph_ptr
  closure_inplace(twa_graph_ptr aut);

  SPOT_API twa_graph_ptr
  closure(const_twa_graph_ptr aut);
  /// @}

  /// \ingroup stutter_inv
  /// \brief Check if a formula is stutter invariant.
  ///
  /// It first calls spot::formula::is_syntactic_stutter_invariant()
  /// to test for the absence of X, but if some X is found, is an
  /// automaton-based check is performed to detect reliably (and
  /// rather efficiently) whether the language is actually
  /// stutter-invariant.
  ///
  /// If you already have an automaton for f, passing it at a second
  /// argument will save some time.  If you also have an automaton for
  /// the negation of f, it is better to use the overload of
  /// is_stutter_invariant() that takes two automata.
  ///
  /// The prop_stutter_invariant() property of \a aut_f is set as a
  /// side-effect.
  ///
  /// For more details about this function, see \cite michaud.15.spin .
  SPOT_API bool
  is_stutter_invariant(formula f, twa_graph_ptr aut_f = nullptr);

  /// \ingroup stutter_inv
  /// \brief Check if an automaton has the stutter invariance
  /// property.
  ///
  /// The automaton-based check requires the complement automaton to
  /// be built.  If you have one, passing it as the second argument
  /// will avoid a costly determinization in case \a aut_f is
  /// non-deterministic.
  ///
  /// The prop_stutter_invariant() property of \a aut_f is set as a
  /// side-effect.
  ///
  /// For more details about this function, see \cite michaud.15.spin .
  SPOT_API bool
  is_stutter_invariant(twa_graph_ptr aut_f,
                       const_twa_graph_ptr aut_nf = nullptr,
                       int algo = 0);

  /// \ingroup stutter_inv
  /// \brief Check whether \a aut is stutter-invariant
  ///
  /// This procedure requires the negation of \a aut_f to
  /// be computed.  This is easily done of \a aut_f is deterministic
  /// or if a formula represented by \a aut_f is known.  Otherwise
  /// \a aut_f will be complemented by determinization, which can
  /// be expansive.   The determinization can be forbidden using
  /// the \a do_not_determinize flag.
  ///
  /// If no complemented automaton could be constructed, the
  /// the result will be returned as trival::maybe().
  ///
  /// If \a find_counterexamples is set and the automaton is found to
  /// be stutter-sensitive, then two named properties named
  /// "accepted-word" and "rejected-word" will be added to the
  /// automaton.  Those sample words will be stutter-equivalent, and
  /// serve as a proof that the property is stutter-sensitive.
  ///
  /// This variant of is_stutter_invariant() is used for the
  /// --check=stutter option of command-line tools.
  SPOT_API trival
  check_stutter_invariance(twa_graph_ptr aut_f,
                           formula f = nullptr,
                           bool do_not_determinize = false,
                           bool find_counterexamples = false);


  ///@{
  /// \ingroup stutter_inv
  /// \brief Determinate the states that are stutter-invariant in \a pos.
  ///
  /// A state is stutter-invariant if the language recognized from
  /// this state is stutter-invariant, or if the state can only be
  /// reached by passing though a stutter-invariant state.
  ///
  /// The algorithm needs to compute the complement of \a pos. You can
  /// avoid that costly operation by either supplying the complement
  /// automaton, or supplying a formula for the (positive) automaton.
  SPOT_API std::vector<bool>
  stutter_invariant_states(const_twa_graph_ptr pos,
                           const_twa_graph_ptr neg = nullptr);

  SPOT_API std::vector<bool>
  stutter_invariant_states(const_twa_graph_ptr pos, formula f_pos);
  ///@}

  ///@{
  /// \ingroup stutter_inv
  /// \brief Highlight the states of \a pos that are stutter-invariant.
  ///
  /// A state is stutter-invariant if the language recognized from
  /// this state is stutter-invariant, or if the state can only be
  /// reached by passing though a stutter-invariant state.
  ///
  /// The algorithm needs to compute the complement of \a pos. You can
  /// avoid that costly operation by either supplying the complement
  /// automaton, or supplying a formula for the (positive) automaton.
  ///
  /// The \a color argument is an index in a predefined set of colors.
  ///
  /// This function simply works by calling
  /// stutter_invariant_states(), and using the resulting vector to
  /// setup the "highlight-states" property of the automaton.
  SPOT_API void
  highlight_stutter_invariant_states(twa_graph_ptr pos,
                                     formula f_pos, unsigned color = 0);
  SPOT_API void
  highlight_stutter_invariant_states(twa_graph_ptr pos,
                                     const_twa_graph_ptr neg = nullptr,
                                     unsigned color = 0);
  ///@}

  ///@{
  /// \ingroup stutter_inv
  /// \brief Determinate the letters with which each state is
  /// stutter-invariant.
  ///
  /// A state q is stutter-invariant for ℓ iff the membership to L(q)
  /// of any word starting with ℓ is unchanged by duplicating any
  /// letter, or removing a duplicate letter.
  ///
  /// The algorithm needs to compute the complement of \a pos. You can
  /// avoid that costly operation by either supplying the complement
  /// automaton, or supplying a formula for the (positive) automaton.
  SPOT_API std::vector<bdd>
  stutter_invariant_letters(const_twa_graph_ptr pos,
                            const_twa_graph_ptr neg = nullptr);

  SPOT_API std::vector<bdd>
  stutter_invariant_letters(const_twa_graph_ptr pos, formula f_pos);
  /// @}


  /// \ingroup stutter_inv
  /// \brief Test if the set of stutter-invariant states is
  /// forward-closed.
  ///
  /// Test if the set of states returned by
  /// spot::stutter_invariant_states() is closed by the successor
  /// relation.  I.e., the successor of an SI-state is an SI-state.
  ///
  /// This function returns -1 is \a sistates is forward closed, or it
  /// will return the number of a state that is not an SI-state but
  /// has a predecessor that is an SI-state.
  ///
  /// The \a sistate vector should be a vector computed for \a aut
  /// using spot::stutter_invariant_states().
  SPOT_API int
  is_stutter_invariant_forward_closed(twa_graph_ptr aut,
                                      const std::vector<bool>& sistates);

  /// \ingroup stutter_inv
  /// \brief Change the automaton so its set of stutter-invariant
  /// state is forward-closed.
  ///
  /// \see spot::is_stutter_invariant_forward_closed()
  ///
  /// The \a sistate vector should be a vector computed for \a aut
  /// using spot::stutter_invariant_states().  The automaton \a aut
  /// will be fixed in place by duplicating problematic states, and an
  /// updated copy of the \a sistates vector will be returned.
  ///
  /// This function will detect the cases where not change to \a aut
  /// is necessary at a cost that is very close to
  /// spot::is_stutter_invariant_forward_closed(), so calling this last
  /// function first is useless.
  SPOT_API std::vector<bool>
  make_stutter_invariant_forward_closed_inplace
  (twa_graph_ptr aut, const std::vector<bool>& sistates);

}
