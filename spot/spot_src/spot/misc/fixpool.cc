// -*- coding: utf-8 -*-
// Copyright (C) 2017-2018, 2020 Laboratoire de Recherche et
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

#include "config.h"
#include <spot/misc/fixpool.hh>

#include <climits>
#include <cstddef>

namespace spot
{

  namespace
  {
// use gcc and clang built-in functions
// both compilers use the same function names, and define __GNUC__
#if __GNUC__
    template<class T>
    struct _clz;

    template<>
    struct _clz<unsigned>
    {
      unsigned
      operator()(unsigned n) const noexcept
      {
        return __builtin_clz(n);
      }
    };

    template<>
    struct _clz<unsigned long>
    {
      unsigned long
      operator()(unsigned long n) const noexcept
      {
        return __builtin_clzl(n);
      }
    };

    template<>
    struct _clz<unsigned long long>
    {
      unsigned long long
      operator()(unsigned long long n) const noexcept
      {
        return __builtin_clzll(n);
      }
    };

    static
    size_t
    clz(size_t n)
    {
      return _clz<size_t>()(n);
    }

#else
    size_t
    clz(size_t n)
    {
      size_t res = CHAR_BIT*sizeof(size_t);
      while (n)
        {
          --res;
          n >>= 1;
        }
      return res;
    }
#endif
  }

  fixed_size_pool::fixed_size_pool(size_t size)
    : size_(
          [](size_t size)
          {
            // to properly store chunks and freelist, we need size to be at
            // least the size of a block_
            if (size < sizeof(block_))
                size = sizeof(block_);
            // powers of 2 are a good alignment
            if (!(size & (size-1)))
              return size;
            // small numbers are best aligned to the next power of 2
            else if (size < alignof(std::max_align_t))
              return size_t{1} << (CHAR_BIT*sizeof(size_t) - clz(size));
            else
              {
                size_t mask = alignof(std::max_align_t)-1;
                return (size + mask) & ~mask;
              }
          }(size)),
      freelist_(nullptr),
      chunklist_(nullptr)
  {
    new_chunk_();
  }

  void fixed_size_pool::new_chunk_()
  {
    const size_t requested = (size_ > 128 ? size_ : 128) * 8192 - 64;
    chunk_* c = reinterpret_cast<chunk_*>(::operator new(requested));
    c->prev = chunklist_;
    chunklist_ = c;

    free_start_ = c->data_ + size_;
    free_end_ = c->data_ + requested;
  }
}
