// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2015, 2016 Laboratoire de
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

#include <spot/tl/formula.hh>
#include <spot/misc/hash.hh>

namespace spot
{
  class SPOT_API mark_tools final
  {
  public:
    /// \ingroup tl_rewriting
    /// \brief Mark operators NegClosure and EConcat.
    ///
    /// \param f The formula to rewrite.
    formula mark_concat_ops(formula f);

    formula simplify_mark(formula f);

  private:
    typedef std::unordered_map<formula, formula> f2f_map;
    f2f_map simpmark_;
    f2f_map markops_;
  };
}
