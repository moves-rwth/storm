// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2015-2018 Laboratoire de Recherche et Développement
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

#include <memory>
#include <type_traits>

// We usually write code like
//   subtypename* i = down_cast<subtypename*>(m);
//   ... use i ...
// When NDEBUG is set, the down_cast is a fast static_cast.
// Otherwise, the down_cast is a dynamic_cast and may return 0
// on error, which is caught by an assert in the down_cast function.
//
// NB: It is valid to use down_cast with non-pointer template argument:
//    subtypename& i = down_cast<subtypename&>(m);
// If an error occurs during the cast, an exception is thrown.
//
// NB: down_cast can also be used on shared_ptr.
namespace spot
{
  template<typename T, typename U>
  inline
  T down_cast(U* u) noexcept
  {
    SPOT_ASSERT(dynamic_cast<T>(u));
    return static_cast<T>(u);
  }

  template<typename T, typename U>
  inline
  T down_cast(const std::shared_ptr<U>& u) noexcept
  {
    SPOT_ASSERT(std::dynamic_pointer_cast<typename T::element_type>(u));
    return std::static_pointer_cast<typename T::element_type>(u);
  }

  template<typename T, typename U>
  inline
  T down_cast(U u)
#ifdef NDEBUG
    noexcept
#endif
  {
#ifdef NDEBUG
    return static_cast<T>(u);
#else
    return dynamic_cast<T>(u);
#endif
  }

} // namespace spot
