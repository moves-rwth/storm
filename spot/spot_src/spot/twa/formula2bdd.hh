// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2015 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/twa/bdddict.hh>

namespace spot
{
  /// \brief Convert a Boolean formula into a BDD.
  ///
  /// Convert the Boolean formula \a f into a BDD, using existing
  /// variables from \a d, and registering new ones as necessary.  \a
  /// for_me, the address of the user of these BDD variables will be
  /// passed to \a d when registering the variables.
  ///
  /// If you only use the BDD representation temporarily, for instance
  /// passing it right away to bdd_to_formula(), you should not forget
  /// to unregister the variables that have been registered for \a
  /// for_me.  See bdd_dict::unregister_all_my_variables().
  /// @{
  SPOT_API bdd
  formula_to_bdd(formula f, const bdd_dict_ptr& d, void* for_me);

  template<typename T>
  SPOT_API bdd
  formula_to_bdd(formula f, const bdd_dict_ptr& d,
                 const std::shared_ptr<T>& for_me)
  {
    return formula_to_bdd(f, d, for_me.get());
  }
  /// @}

  /// \brief Convert a BDD into a formula.
  ///
  /// Format the BDD as an irredundant sum of product (see the
  /// minato_isop class for details) and map the BDD variables back
  /// into their atomic propositions.  This works only for Boolean
  /// formulas, and all the BDD variables used in \a f should have
  /// been registered in \a d.  Although the result has type
  /// formula, it obviously does not use any temporal operator.
  SPOT_API
  formula bdd_to_formula(bdd f, const bdd_dict_ptr d);
}
