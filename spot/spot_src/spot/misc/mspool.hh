// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2015, 2016, 2018 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE)
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

#include <spot/misc/common.hh>
#include <unordered_map>

#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
#include <valgrind/memcheck.h>
#endif

namespace spot
{

  /// A multiple-size memory pool implementation.
  class multiple_size_pool
  {
    static const size_t alignment_ = 2 * sizeof(size_t) - 1;
  public:
    /// Create a pool.
    multiple_size_pool()
      : free_start_(nullptr), free_end_(nullptr), chunklist_(nullptr)
    {
    }

    /// Free any memory allocated by this pool.
    ~multiple_size_pool()
    {
      while (chunklist_)
        {
          chunk_* prev = chunklist_->prev;
          free(chunklist_);
          chunklist_ = prev;
        }
    }

    size_t fixsize(size_t size) const
    {
      if (size < sizeof(block_))
        size = sizeof(block_);

      return (size + alignment_ - 1) & ~(alignment_ - 1);
    }

    /// Allocate \a size bytes of memory.
    void*
    allocate(size_t size)
    {
      size = fixsize(size);

      block_*& f = freelist_[size];
      // If we have free blocks available, return the first one.
      if (f)
        {
          block_* first = f;
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
          VALGRIND_MALLOCLIKE_BLOCK(f, size, 0, false);
          // field f->next is initialized: prevents valgrind from complaining
          // about jumps depending on uninitialized memory
          VALGRIND_MAKE_MEM_DEFINED(f, sizeof(block_*));
#endif
          f = f->next;
          return first;
        }

      // Else, create a block out of the last chunk of allocated
      // memory.

      // If all the last chunk has been used, allocate one more.
      if (free_start_ + size > free_end_)
        {
          const size_t requested = (size > 128 ? size : 128) * 8192 - 64;
          chunk_* c = reinterpret_cast<chunk_*>(malloc(requested));
          if (!c)
            throw std::bad_alloc();
          c->prev = chunklist_;
          chunklist_ = c;

          free_start_ = c->data_ + size;
          free_end_ = c->data_ + requested;
        }

      void* res = free_start_;
      free_start_ += size;
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
      VALGRIND_MALLOCLIKE_BLOCK(res, size, 0, false);
#endif
      return res;
    }

    /// \brief Recycle \a size bytes of memory.
    ///
    /// Despite the name, the memory is not really deallocated in the
    /// "delete" sense: it is still owned by the pool and will be
    /// reused by allocate as soon as possible.  The memory is only
    /// freed when the pool is destroyed.
    ///
    /// The size argument should be the same as the one passed to
    /// allocate().
    void
    deallocate (const void* ptr, size_t size)
    {
      SPOT_ASSERT(ptr);
      size = fixsize(size);
      block_* b = reinterpret_cast<block_*>(const_cast<void*>(ptr));
      block_*& f = freelist_[size];
      b->next = f;
      f = b;
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
      VALGRIND_FREELIKE_BLOCK(ptr, 0);
#endif
    }

  private:
    struct block_ { block_* next; };
    std::unordered_map<size_t, block_*> freelist_;
    char* free_start_;
    char* free_end_;
    // chunk = several agglomerated blocks
    union chunk_ { chunk_* prev; char data_[1]; }* chunklist_;
  };
}
