// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2015-2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE)
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

#include <spot/misc/fixpool.hh>

namespace spot
{
  /// An allocator to be used with STL containers.
  /// It uses a spot::fixed_size_pool to handle memory.
  /// It is intended to improve performance and locality of node-based
  /// containers (std::{unordered}{multi}{set,map}).
  /// It is geared towards efficiently allocating memory for one object at a
  /// time (the nodes of the node-based containers). Larger allocations are
  /// served by calling the global memory allocation mechanism (::operator new).
  /// Using it for contiguous containers (such as std::vector or std::deque)
  /// will be less efficient than using the default std::allocator.
  ///
  /// Short reminder on STL concept of Allocator:
  ///   allocate() may throw
  ///   deallocate() must not throw
  ///   equality testing (i.e. == and !=) must not throw
  ///   copying allocator (constructor and assignment) must not throw
  ///   moving allocator (constructor and assignment) must not throw
  ///
  /// WARNING this class is NOT thread-safe: the allocator relies on a static
  ///   fixed_size_pool (which is not thread-safe either).
  template<class T>
  class pool_allocator
  {
    static
    fixed_size_pool&
    pool()
    {
      static fixed_size_pool p = fixed_size_pool(sizeof(T));
      return p;
    }

  public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = size_t;

    constexpr pool_allocator() noexcept
    {}
    template<class U>
    constexpr pool_allocator(const pool_allocator<U>&) noexcept
    {}

    template<class U>
    struct rebind
    {
      using other = pool_allocator<U>;
    };

    pointer
    allocate(size_type n)
    {
      if (SPOT_LIKELY(n == 1))
        return static_cast<pointer>(pool().allocate());
      else
        return static_cast<pointer>(::operator new(n*sizeof(T)));
    }

    void
    deallocate(pointer ptr, size_type n) noexcept
    {
      if (SPOT_LIKELY(n == 1))
        pool().deallocate(static_cast<void*>(ptr));
      else
        ::operator delete(ptr);
    }

    bool
    operator==(const pool_allocator&) const noexcept
    {
      return true;
    }
    bool
    operator!=(const pool_allocator& o) const noexcept
    {
      return !(this->operator==(o));
    }
  };
}
