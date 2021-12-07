// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2013-2015, 2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/misc/common.hh>
#include <bddx.h>
#include <stack>

namespace spot
{
  /// \ingroup misc_tools
  /// \brief Generate an irredundant sum-of-products (ISOP) form of a
  /// BDD function.
  ///
  /// This algorithm implements a derecursived version the Minato-Morreale
  /// algorithm. \cite minato.92.sasimi
  class SPOT_API minato_isop
  {
  public:
    /// \brief Conctructor.
    /// \arg input The BDD function to translate in ISOP.
    minato_isop(bdd input);
    /// \brief Conctructor.
    /// \arg input The BDD function to translate in ISOP.
    /// \arg vars  The set of BDD variables to factorize in \a input.
    minato_isop(bdd input, bdd vars);
    /// \brief Conctructor.
    ///
    /// This version allow some flexibility in computing the ISOP.
    /// the result must be within \a input_min and \a input_max.
    /// \arg input_min The minimum BDD function to translate in ISOP.
    /// \arg input_max The maximum BDD function to translate in ISOP.
    minato_isop(bdd input_min, bdd input_max, bool);
    /// \brief Compute the next sum term of the ISOP form.
    /// Return \c bddfalse when all terms have been output.
    bdd next();

  private:
    /// Internal variables for minato_isop.
    struct local_vars
    {
      // If you are following the paper, f_min and f_max correspond
      // to the pair of BDD functions used to encode the ternary function f
      // (see Section 3.4).
      // Also note that f0, f0', and f0'' all share the same _max function.
      // Likewise for f1, f1', and f1''.
      bdd f_min, f_max;
      // Because we need a non-recursive version of the algorithm,
      // we had to split it in four steps (each step is separated
      // from the other by a call to ISOP in the original algorithm).
      enum { FirstStep, SecondStep, ThirdStep, FourthStep } step;
      // The list of variables to factorize.  This is an addition to
      // the original algorithm.
      bdd vars;
      bdd v1;
      bdd f0_min, f0_max;
      bdd f1_min, f1_max;
      bdd g0, g1;
      local_vars(bdd f_min, bdd f_max, bdd vars) noexcept
        : f_min(f_min), f_max(f_max), step(FirstStep), vars(vars) {}
    };
    std::stack<local_vars> todo_;
    std::stack<bdd> cube_;
    bdd ret_;
  };
}
