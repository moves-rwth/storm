// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE)
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <cstddef>
#include <type_traits>

namespace spot
{
  /// \defgroup hash_funcs Hashing functions
  /// \ingroup misc_tools

  /// \ingroup hash_funcs
  /// @{

  /// \brief Thomas Wang's 32 bit hash function.
  ///
  /// Hash an integer amongst the integers.
  /// http://web.archive.org/web/2011/concentric.net/~Ttwang/tech/inthash.htm
  inline size_t
  wang32_hash(size_t key)
  {
    // We assume that size_t has at least 32bits.
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
  }

  /// \brief Knuth's Multiplicative hash function.
  ///
  /// This function is suitable for hashing values whose
  /// high order bits do not vary much (ex. addresses of
  /// memory objects).  Prefer spot::wang32_hash() otherwise.
  /// http://web.archive.org/web/2011/concentric.net/~Ttwang/tech/addrhash.htm
  inline size_t
  knuth32_hash(size_t key)
  {
    // 2654435761 is the golden ratio of 2^32.  The right shift of 3
    // bits assumes that all objects are aligned on a 8 byte boundary.
    return (key >> 3) * 2654435761U;
  }


  /// Struct for Fowler-Noll-Vo parameters
  template<class T, class Enable = void>
  struct fnv
  {};

  /// Fowler-Noll-Vo hash parameters for 32 bits
  template<class T>
  struct fnv<T, typename std::enable_if<sizeof(T) == 4>::type>
  {
    static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
                  "Fowler-Noll-Vo hash requires an unsigned integral type");
    static constexpr T init = 2166136261UL;
    static constexpr T prime = 16777619UL;
  };

  /// Fowler-Noll-Vo hash parameters for 64 bits
  template<class T>
  struct fnv<T, typename std::enable_if<sizeof(T) == 8>::type>
  {
    static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
                  "Fowler-Noll-Vo hash requires an unsigned integral type");
    static constexpr T init = 14695981039346656037ULL;
    static constexpr T prime = 1099511628211ULL;
  };

  /// \brief Fowler-Noll-Vo hash function
  ///
  /// This function is a non-cryptographic fast hash function.
  /// The magic constants depend on the size of a size_t.
  template<class It>
  size_t
  fnv_hash(It begin, It end)
  {
    size_t res = fnv<size_t>::init;
    for (; begin != end; ++begin)
      {
        res ^= *begin;
        res *= fnv<size_t>::prime;
      }
    return res;
  }

  /// @}
}
