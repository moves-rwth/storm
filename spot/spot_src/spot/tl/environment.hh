// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2012, 2014, 2015 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <string>

namespace spot
{
  /// \ingroup tl_environment
  /// \brief An environment that describes atomic propositions.
  class environment
  {
  public:
    /// \brief Obtain the formula associated to \a prop_str
    ///
    /// Usually \a prop_str, is the name of an atomic proposition,
    /// and spot::require simply returns the associated
    /// spot::formula.
    ///
    /// Note this is not a \c const method.  Some environments will
    /// "create" the atomic proposition when requested.
    ///
    /// \return 0 iff \a prop_str is not part of the environment,
    ///   or the associated spot::formula otherwise.
    virtual formula require(const std::string& prop_str) = 0;

    /// Get the name of the environment.
    virtual const std::string& name() const = 0;

    virtual
    ~environment()
    {
    }
  };
}
