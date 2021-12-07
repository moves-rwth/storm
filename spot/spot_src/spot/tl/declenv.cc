// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2012, 2014, 2015, 2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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

#include "config.h"
#include <spot/tl/declenv.hh>

namespace spot
{
  declarative_environment::declarative_environment()
  {
  }

  bool
  declarative_environment::declare(const std::string& prop_str)
  {
    if (props_.find(prop_str) != props_.end())
      return false;
    props_[prop_str] = formula::ap(prop_str);
    return true;
  }

  formula
  declarative_environment::require(const std::string& prop_str)
  {
    prop_map::iterator i = props_.find(prop_str);
    if (i == props_.end())
      return nullptr;
    return i->second;
  }

  const std::string&
  declarative_environment::name() const
  {
    static std::string name("declarative environment");
    return name;
  }

  const declarative_environment::prop_map&
  declarative_environment::get_prop_map() const
  {
    return props_;
  }
}
