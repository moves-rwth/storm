// -*- coding: utf-8 -*-
// Copyright (C) 2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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

#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/nenoform.hh>

namespace spot
{
  /// \ingroup twa_misc
  /// \brief Given a terminal automaton \a f_terminal recognizing
  /// some formula F(φ), modify it to recognize GF(φ).
  ///
  /// If \a state_based is set, the automaton all terminal states are
  /// replaced by a unique accepting state that has the same outgoing
  /// transitions as the initial state, and the initial state is
  /// actually relocated to that accepting state.  The latter point is
  /// not necessary, but it favors shorter accepting cycles.
  ///
  /// If \a state_based is not set, all transition going to terminal
  /// states are made accepting and redirected to the initial state.
  ///
  /// This construction is inspired by a similar construction in the
  /// LICS'18 paper by J. Esparza, J. Křetínský, and S. Sickert.
  SPOT_API twa_graph_ptr
  g_f_terminal_inplace(twa_graph_ptr f_terminal, bool state_based = false);

  /// \ingroup twa_ltl
  /// \brief Convert GF(φ) into a (D)BA if φ is a guarantee property.
  ///
  /// If the formula \a gf has the form GΦ where Φ matches either F(φ)
  /// or F(φ₁)|F(φ₂)|...|F(φₙ), we translate Φ into A_Φ and attempt to
  /// minimize it to a WDBA W_Φ.  If the resulting automaton is
  /// terminal, we then call g_f_terminal_inplace(W_Φ).  If \a
  /// deterministic is not set, we keep the minimized automaton only
  /// if g_f_terminal_inplace(A_Φ) is larger.
  ///
  /// Return nullptr if the input formula is not of the supported
  /// form.
  ///
  /// This construction generalizes a construction in the LICS'18
  /// paper of J. Esparza, J. Křetínský, and S. Sickert.  This version
  /// will work if Φ represent a safety property, even if it is not a
  /// syntactic safety.  When building deterministic transition-based
  /// automata, it will also try to remove useless trivial components
  /// at the beginning of wdba(A_Φ).
  SPOT_API twa_graph_ptr
  gf_guarantee_to_ba_maybe(formula gf, const bdd_dict_ptr& dict,
                           bool deterministic = true, bool state_based = false);

  /// \ingroup twa_ltl
  /// \brief Convert GF(φ) into a (D)BA if φ is a guarantee property.
  ///
  /// This is similar to gf_guarantee_to_ba_maybe() except it raises
  /// an exception of the input formula is not of the supported form.
  SPOT_API twa_graph_ptr
  gf_guarantee_to_ba(formula gf, const bdd_dict_ptr& dict,
                     bool deterministic = true, bool state_based = false);

  /// \ingroup twa_ltl
  /// \brief Convert FG(φ) into a DCA if φ is a safety property.
  ///
  /// This is the dual of gf_guarantee_to_ba_maybe().  See that
  /// function for details.
  ///
  /// Return nullptr if the input formula is not of the supported
  /// form.
  SPOT_API twa_graph_ptr
  fg_safety_to_dca_maybe(formula fg, const bdd_dict_ptr& dict,
                            bool state_based);

  /// \ingroup twa_ltl
  /// \brief Convert FG(φ) into a DCA if φ is a safety property.
  ///
  /// This is similar to fg_safety_to_dba_maybe() except it raises
  /// an exception of the input formula is not of the supported form.
  SPOT_API twa_graph_ptr
  fg_safety_to_dca(formula fg, const bdd_dict_ptr& dict,
                      bool state_based = false);
}
