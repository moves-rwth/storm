// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017-2020 Laboratoire de Recherche et
// Développement de l'Epita.
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
  /// @{
  /// \ingroup twa_acc_transform
  /// \brief Remove useless acceptance sets
  ///
  /// Removes from \a aut the acceptance marks that are not used
  /// in its acceptance condition.  Also removes from the acceptance
  /// conditions the terms that corresponds to empty or full sets.
  ///
  /// If \a strip is true (the default), the remaining acceptance set
  /// numbers will be shifted down to reduce the maximal number of
  /// acceptance sets used.
  ///
  /// cleanup_acceptance_here() works in place, cleanup_acceptance()
  /// returns a new automaton that has been simplified.
  SPOT_API twa_graph_ptr
  cleanup_acceptance_here(twa_graph_ptr aut, bool strip = true);

  SPOT_API twa_graph_ptr
  cleanup_acceptance(const_twa_graph_ptr aut, bool strip = true);
  /// @}

  /// @{
  /// \ingroup twa_acc_transform
  /// \brief Simplify an acceptance condition
  ///
  /// Does everything cleanup_acceptance() does, but additionally:
  /// merge identical sets, detect whether two sets i and j are
  /// complementary to apply the following reductions:
  ///   - `Fin(i) & Inf(j) = Fin(i)`
  ///   - `Fin(i) & Fin(j) = f`
  ///   - `Fin(i) & Inf(i) = f`
  ///   - `Fin(i) | Inf(j) = Inf(j)`
  ///   - `Inf(i) | Inf(j) = t`
  ///   - `Fin(i) | Inf(i) = t`
  /// And also merge terms like `Inf(i)|Inf(j)` or `Fin(i)&Fin(j)`
  /// provided at least i or j is used uniquely in the formula.
  /// (for instance if i is unique, `Inf(i)|Inf(j)` is rewritten
  /// as `Inf(i)`, and `i` is added on all transitions where `j` is present
  /// in the automaton.)
  ///
  /// simplify_acceptance_here() works in place, simplify_acceptance()
  /// returns a new automaton that has been simplified.
  SPOT_API twa_graph_ptr
  simplify_acceptance_here(twa_graph_ptr aut);

  SPOT_API twa_graph_ptr
  simplify_acceptance(const_twa_graph_ptr aut);
  /// @}
}
