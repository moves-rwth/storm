// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2015-2018, 2020 Laboratoire de Recherche et
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

#include <spot/misc/common.hh>

#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
#include <valgrind/memcheck.h>
#endif

namespace spot
{
  /// A fixed-size memory pool implementation.
  class SPOT_API fixed_size_pool
  {
  public:
    /// Create a pool allocating objects of \a size bytes.
    fixed_size_pool(size_t size);

    /// Free any memory allocated by this pool.
    ~fixed_size_pool()
    {
      while (chunklist_)
        {
          chunk_* prev = chunklist_->prev;
          ::operator delete(chunklist_);
          chunklist_ = prev;
        }
    }

    /// Allocate \a size bytes of memory.
    void*
    allocate()
    {
      block_* f = freelist_;
      // If we have free blocks available, return the first one.
      if (f)
        {
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
          VALGRIND_MALLOCLIKE_BLOCK(f, size_, 0, false);
          // field f->next is initialized: prevents valgrind from complaining
          // about jumps depending on uninitialized memory
          VALGRIND_MAKE_MEM_DEFINED(f, sizeof(block_*));
#endif
          freelist_ = f->next;
          return f;
        }

      // Else, create a block out of the last chunk of allocated
      // memory.

      // If all the last chunk has been used, allocate one more.
      if (free_start_ + size_ > free_end_)
        new_chunk_();

      void* res = free_start_;
      free_start_ += size_;
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
      VALGRIND_MALLOCLIKE_BLOCK(res, size_, 0, false);
#endif
      return res;
    }

    /// \brief Recycle \a size bytes of memory.
    ///
    /// Despite the name, the memory is not really deallocated in the
    /// "delete" sense: it is still owned by the pool and will be
    /// reused by allocate as soon as possible.  The memory is only
    /// freed when the pool is destroyed.
    void
    deallocate(void* ptr)
    {
      SPOT_ASSERT(ptr);
      block_* b = reinterpret_cast<block_*>(ptr);
      b->next = freelist_;
      freelist_ = b;
#if SPOT_DEBUG && defined(HAVE_VALGRIND_MEMCHECK_H)
      VALGRIND_FREELIKE_BLOCK(ptr, 0);
#endif
    }

  private:
    void new_chunk_();

    const size_t size_;
    struct block_ { block_* next; }* freelist_;
    char* free_start_;
    char* free_end_;
    // chunk = several agglomerated blocks
    union chunk_ { chunk_* prev; char data_[1]; }* chunklist_;
  };

}
