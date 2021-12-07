// -*- coding: utf-8 -*-
// Copyright (C) 2009-2016, 2018-2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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
#include <spot/twaalgos/powerset.hh>
#include <spot/tl/formula.hh>

namespace spot
{
  /// \addtogroup twa_reduction
  /// @{

  /// \brief Construct a minimal deterministic monitor.
  ///
  /// The automaton will be converted into minimal deterministic
  /// monitor.  All useless SCCs should have been previously removed
  /// (using scc_filter() for instance).  Then the automaton will be
  /// determinized and minimized using the standard DFA construction
  /// as if all states were accepting states.
  ///
  /// For more detail about monitors, see \cite tabakov.10.rv .
  /// (Note: although the above paper uses Spot, this function did not
  /// exist in Spot at that time.)
  ///
  /// \param a the automaton to convert into a minimal deterministic monitor
  /// \pre Dead SCCs should have been removed from \a a before
  ///      calling this function.
  SPOT_API twa_graph_ptr minimize_monitor(const const_twa_graph_ptr& a);

  /// \brief Minimize a Büchi automaton in the WDBA class.
  ///
  /// This takes a TGBA whose language is representable by a Weak
  /// Deterministic Büchi Automaton, and construct a minimal WDBA for
  /// this language.  This essentially chains three algorithms:
  /// determinization, acceptance adjustment (Löding's coloring
  /// algorithm), and minimization (using a Moore-like approach).
  ///
  /// If the input automaton does not represent a WDBA language, the
  /// resulting automaton is still a WDBA, but it will accept a
  /// superset of the original language.  Use the
  /// minimize_obligation() function if you are not sure whether it is
  /// safe to call this function.
  ///
  /// The construction is inspired by the following paper, however we
  /// guarantee that the output language is a subsets of the original
  /// language while they don't. \cite dax.07.atva
  ///
  /// If an \a output_aborter is given, the determinization is aborted
  /// whenever it would produce an automaton that is too large.  In
  /// that case, a nullptr is returned.
  SPOT_API twa_graph_ptr minimize_wdba(const const_twa_graph_ptr& a,
                                       const output_aborter* aborter = nullptr);

  /// \brief Minimize an automaton if it represents an obligation property.
  ///
  /// This function attempts to minimize the automaton \a aut_f using the
  /// algorithm implemented in the minimize_wdba() function, and presented
  /// by \cite dax.07.atva .
  ///
  /// Because it is hard to determine if an automaton corresponds
  /// to an obligation property, you should supply either the formula
  /// \a f expressed by the automaton \a aut_f, or \a aut_neg_f the negation
  /// of the automaton \a aut_neg_f.
  ///
  /// \param aut_f the automaton to minimize
  /// \param f the LTL formula represented by the automaton \a aut_f
  /// \param aut_neg_f an automaton representing the negation of \a aut_f
  /// \param reject_bigger Whether the minimal WDBA should be discarded if
  /// it has more states than the input.
  /// \return a new tgba if the automaton could be minimized, \a aut_f if
  /// the automaton cannot be minimized, 0 if we do not know if the
  /// minimization is correct because neither \a f nor \a aut_neg_f
  /// were supplied.
  ///
  /// The function proceeds as follows.  If the formula \a f or the
  /// automaton \a aut can easily be proved to represent an obligation
  /// formula, then the result of <code>minimize(aut)</code> is
  /// returned.  Otherwise, if \a aut_neg_f was not supplied but \a f
  /// was, \a aut_neg_f is built from the negation of \a f.  Then we
  /// check that <code>product(aut,!minimize(aut_f))</code> and <code>
  /// product(aut_neg_f,minize(aut))</code> are both empty.  If they
  /// are, the the minimization was sound.  (See the paper for full
  /// details.)
  ///
  /// If \a reject_bigger is set, this function will return the input
  /// automaton \a aut_f when the minimized WDBA has more states than
  /// the input automaton.  (More states are possible because of
  /// determinization step during minimize_wdba().)  Note that
  /// checking the size of the minimized WDBA occurs before ensuring
  /// that the minimized WDBA is correct.
  ///
  /// If an \a output_aborter is given, the determinization is aborted
  /// whenever it would produce an automaton that is too large.  In
  /// this case, aut_f is returned unchanged.
  SPOT_API twa_graph_ptr
  minimize_obligation(const const_twa_graph_ptr& aut_f,
                      formula f = nullptr,
                      const_twa_graph_ptr aut_neg_f = nullptr,
                      bool reject_bigger = false,
                      const output_aborter* aborter = nullptr);

  /// \brief Whether calling minimize_obligation is sure to work
  ///
  /// This checks whether \a f is a syntactic obligation, or if \a
  /// aut_f obviously corresponds to an obligation (for instance if
  /// this is a terminal automaton, or if it is both weak and
  /// deterministic).  In this case, calling minimize_obligation()
  /// should not be a waste of time, as it will return a new
  /// automaton.
  ///
  /// If this function returns false, the input property might still
  /// be a pathological obligation.  The only way to know is to call
  /// minimize_obligation(), but as it is less likely, you might
  /// decide to save time.
  SPOT_API
  bool minimize_obligation_garanteed_to_work(const const_twa_graph_ptr& aut_f,
                                             formula f = nullptr);

  /// @}
}
