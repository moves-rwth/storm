// -*- coding: utf-8 -*-
// Copyright (C) 2015 Laboratoire de Recherche et Développement
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

#include <spot/tl/formula.hh>
#include <unordered_map>

namespace spot
{
  constexpr const char* default_unabbrev_string = "eFGiMW^";

  /// \ingroup tl_rewriting
  /// \brief Clone and rewrite a formula to remove specified operators
  /// logical operators.
  class SPOT_API unabbreviator final
  {
  private:
    // What to rewrite?
    bool re_e_ = false;
    bool re_f_ = false;
    bool re_g_ = false;
    bool re_i_ = false;
    bool re_m_ = false;
    bool re_r_ = false;
    bool re_w_ = false;
    bool re_xor_ = false;
    bool re_some_bool_ = false;        // rewrite xor, i, or e
    bool re_some_f_g_ = false;        // rewrite F or G
    bool re_some_other_ = false;        // rewrite W, M, or R
    // Cache of rewritten subformulas
    std::unordered_map<formula, formula> cache_;
  public:
    /// \brief Constructor
    ///
    /// The set of operators to remove should be passed as a string
    /// which in which each letter denote an operator (using LBT's
    /// convention).
    unabbreviator(const char* opt = default_unabbrev_string);
    formula run(formula in);
  };

  /// \ingroup tl_rewriting
  /// \brief Clone and rewrite a formula to remove specified operators
  /// logical operators.
  ///
  /// The set of operators to remove should be passed as a string
  /// which in which each letter denote an operator (using LBT's
  /// convention).
  SPOT_API formula
  unabbreviate(formula in, const char* opt= default_unabbrev_string);
}
