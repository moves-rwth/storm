// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014, 2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
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

#include <spot/ta/ta.hh>
#include <spot/ta/tgta.hh>
#include <spot/ta/tgtaexplicit.hh>

namespace spot
{
  /// \addtogroup ta_reduction
  /// @{


  /// \brief Construct a simplified TA by merging bisimilar states.
  ///
  /// A TA automaton can be simplified by merging bisimilar states:
  /// Two states are bisimilar if the automaton can accept the
  ///    same executions starting for either of these states. This can be
  /// achieved using any algorithm based on partition refinement
  ///
  /// \param ta_ the TA automaton to convert into a simplified TA
  SPOT_API ta_explicit_ptr
  minimize_ta(const const_ta_ptr& ta_);



  /// \brief Construct a simplified TGTA by merging bisimilar states.
  ///
  /// A TGTA automaton can be simplified by merging bisimilar states:
  /// Two states are bisimilar if the automaton can accept the
  /// same executions starting for either of these states. This can be
  /// achieved using same algorithm used to simplify a TA taking into account
  /// the acceptance conditions of the outgoing transitions.
  ///
  /// \param tgta_ the TGTA automaton to convert into a simplified TGTA
  SPOT_API tgta_explicit_ptr
  minimize_tgta(const const_tgta_explicit_ptr& tgta_);

/// @}
}
