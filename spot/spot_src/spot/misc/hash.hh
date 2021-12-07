// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2011, 2014, 2015, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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

#include <string>
#include <functional>
#include <spot/misc/hashfunc.hh>

#include <unordered_map>
#include <unordered_set>

namespace spot
{

  /// \ingroup hash_funcs
  /// \brief A hash function for pointers.
  template <class T>
  struct ptr_hash :
    public std::unary_function<const T*, size_t>
  {
    // A default constructor is needed if the ptr_hash object is
    // stored in a const member.  This occur with the clang version
    // installed by OS X 10.9.
    ptr_hash()
    {
    }

    size_t operator()(const T* p) const noexcept
    {
      return knuth32_hash(reinterpret_cast<const char*>(p)
                          - static_cast<const char*>(nullptr));
    }
  };

  /// \ingroup hash_funcs
  /// \brief A hash function for strings.
  typedef std::hash<std::string> string_hash;

  /// \ingroup hash_funcs
  /// \brief A hash function that returns identity
  template<typename T>
  struct identity_hash:
    public std::unary_function<const T&, size_t>
  {
    // A default constructor is needed if the identity_hash object is
    // stored in a const member.
    identity_hash()
    {
    }

    size_t operator()(const T& s) const noexcept
    {
      return s;
    }
  };


  struct pair_hash
  {
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &p) const noexcept
    {
      std::hash<T> th;
      std::hash<U> uh;

      return wang32_hash(static_cast<size_t>(th(p.first)) ^
                         static_cast<size_t>(uh(p.second)));
    }
  };
}
