// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2015, 2016 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/tl/environment.hh>
#include <spot/tl/formula.hh>

namespace spot
{
  /// \ingroup tl_environment
  /// \brief A laxist environment.
  ///
  /// This environment recognizes all atomic propositions.
  ///
  /// This is a singleton.  Use default_environment::instance()
  /// to obtain the instance.
  class SPOT_API default_environment final: public environment
  {
  public:
    virtual ~default_environment();
    virtual formula require(const std::string& prop_str) override;
    virtual const std::string& name() const override;

    /// Get the sole instance of spot::default_environment.
    static default_environment& instance();
  protected:
    default_environment();
  };
}
