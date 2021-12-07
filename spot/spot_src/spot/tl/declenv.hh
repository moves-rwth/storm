// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2012, 2013, 2014, 2015, 2016 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <string>
#include <map>
#include <spot/tl/formula.hh>

namespace spot
{
  /// \ingroup tl_environment
  /// \brief A declarative environment.
  ///
  /// This environment recognizes all atomic propositions
  /// that have been previously declared.  It will reject other.
  class SPOT_API declarative_environment : public environment
  {
  public:
    declarative_environment();
    ~declarative_environment() = default;

    /// Declare an atomic proposition.  Return false iff the
    /// proposition was already declared.
    bool declare(const std::string& prop_str);

    virtual formula require(const std::string& prop_str) override;

    /// Get the name of the environment.
    virtual const std::string& name() const override;

    typedef std::map<const std::string, formula> prop_map;

    /// Get the map of atomic proposition known to this environment.
    const prop_map& get_prop_map() const;

  private:
    prop_map props_;
  };
}
