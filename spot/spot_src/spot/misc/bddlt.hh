// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2014, 2017 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <bddx.h>
#include <functional>

namespace spot
{
  /// \ingroup misc_tools
  /// \brief Comparison functor for BDDs.
  ///
  /// This comparison function use BDD ids for efficiency.  An
  /// algorithm depending on this order may return different results
  /// depending on how the BDD library has been used before.
  struct bdd_less_than :
    public std::binary_function<const bdd&, const bdd&, bool>
  {
    bool
    operator()(const bdd& left, const bdd& right) const
    {
      return left.id() < right.id();
    }
  };

  /// \ingroup misc_tools
  /// \brief Comparison functor for BDDs.
  ///
  /// This comparison function actually check for BDD variables, so as
  /// long as the variable order is the same, the output of this
  /// comparison will be stable and independent on previous BDD
  /// operations.
  struct bdd_less_than_stable :
    public std::binary_function<const bdd&, const bdd&, bool>
  {
    bool
    operator()(const bdd& left, const bdd& right) const
    {
      int li = left.id();
      int ri = right.id();
      if (li == ri)
        return false;
      if (li <= 1 || ri <= 1)
        return li < ri;
      {
        int vl = bdd_var(left);
        int vr = bdd_var(right);
        if (vl != vr)
          return vl < vr;
      }
      // We check the high side before low, this way
      //  !a&b comes before a&!b and a&b
      {
        bdd hl = bdd_high(left);
        bdd hr = bdd_high(right);
        if (hl != hr)
          return operator()(hl, hr);
        return operator()(bdd_low(left), bdd_low(right));
      }
    }
  };

  /// \ingroup misc_tools
  /// \brief Hash functor for BDDs.
  struct bdd_hash :
    public std::unary_function<const bdd&, size_t>
  {
    size_t
    operator()(const bdd& b) const noexcept
    {
      return b.id();
    }
  };
}
