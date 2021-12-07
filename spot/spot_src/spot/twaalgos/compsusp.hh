// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2015, 2018 Laboratoire de Recherche et
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

#include <spot/tl/formula.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \brief Compositional translation algorithm with resetable
  /// suspension.
  ///
  /// Described in "Compositional Approach to Suspension and Other
  /// Improvements to LTL Translation", Tomáš Babiak, Thomas Badie,
  /// Alexandre Duret-Lutz, Mojmír Křetínský, Jan Strejček (SPIN'13).
  ///
  /// If \a no_wdba or \a no_simulation is true, the corresponding
  /// operation is not performed on the skeleton automaton.  If \a
  /// early_susp is true, then composition starts on the transition
  /// that enters the accepting SCC, not just in the SCC itself.  If
  /// \a no_susp_product is true, then the composition is not
  /// performed and the skeleton automaton is returned for debugging.
  /// If \a wdba_smaller is true, then the WDBA-minimization of the
  /// skeleton is used only if it produces a smaller automaton.
  ///
  /// Finally the \a oblig flag is a work in progress and should not
  /// be set to true.
  ///
  /// This interface is subject to change, and clients aiming for
  /// long-term stability should better use the services of the
  /// spot::translator class instead.
  SPOT_API twa_graph_ptr
  compsusp(formula f, const bdd_dict_ptr& dict,
           bool no_wdba = false, bool no_simulation = false,
           bool early_susp = false, bool no_susp_product = false,
           bool wdba_smaller = false, bool oblig = false);
}
