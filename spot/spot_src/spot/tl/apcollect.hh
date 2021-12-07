// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2015 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/tl/formula.hh>
#include <set>
#include <bddx.h>
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \addtogroup tl_misc
  /// @{

  /// Set of atomic propositions.
  typedef std::set<formula> atomic_prop_set;

  /// \brief construct an atomic_prop_set with n propositions
  SPOT_API
  atomic_prop_set create_atomic_prop_set(unsigned n);

  /// \brief Return the set of atomic propositions occurring in a formula.
  ///
  /// \param f the formula to inspect
  /// \param s an existing set to fill with atomic_propositions discovered,
  ///        or 0 if the set should be allocated by the function.
  /// \return A pointer to the supplied set, \c s, augmented with
  ///        atomic propositions occurring in \c f; or a newly allocated
  ///        set containing all these atomic propositions if \c s is 0.
  SPOT_API atomic_prop_set*
  atomic_prop_collect(formula f, atomic_prop_set* s = nullptr);

  /// \brief Return the set of atomic propositions occurring in a
  /// formula, as a BDD.
  ///
  /// \param f the formula to inspect
  /// \param a that automaton that should register the BDD variables used.
  /// \return A conjunction the atomic propositions.
  SPOT_API bdd
  atomic_prop_collect_as_bdd(formula f, const twa_ptr& a);

  /// @}
}
