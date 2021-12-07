// -*- coding: utf-8 -*-
// Copyright (C) 2018 Laboratoire de Recherche et Développement
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

#include <array>
#include <spot/misc/hashfunc.hh>
#include <spot/misc/common.hh>

namespace spot
{
#ifndef SWIG
  namespace internal
  {
    [[noreturn]] SPOT_API void report_bit_shift_too_big();
    [[noreturn]] SPOT_API void report_bit_out_of_bounds();
  }
#endif

  template<size_t N>
  class SPOT_API bitset
  {
    using word_t = unsigned;
    // the number of bits must hold on an unsigned
    static_assert(8*N*sizeof(word_t) < -1U, "too many bits in bitset");

    std::array<word_t, N> data;

    /// a special constructor for -1 (cf. mone below)
    struct minus_one_tag {};
    explicit bitset(minus_one_tag)
    {
      for (auto& v : data)
        v = -1U;
    }

    constexpr explicit bitset(word_t s)
      : data{{s}}
    {
      SPOT_ASSERT(s == 0U || s == 1U);
    }

  public:
    // constructor
    bitset() = default;
    ~bitset() = default;

    /// the 0
    static constexpr bitset zero() { return bitset{0U}; }
    /// the 1
    static constexpr bitset one() { return bitset{1U}; }
    /// the -1 (all bits are set to 1)
    static bitset mone() { return bitset(minus_one_tag{}); }

    explicit operator bool() const
    {
      for (const auto& v : data)
        if (v)
          return true;
      return false;
    }

    size_t hash() const
    {
      return fnv_hash(data.begin(), data.end());
    }

    bool operator==(const bitset& other) const
    {
      // TODO use std::algorithms instead?
      for (unsigned i = 0; i != N; ++i)
        if (data[i] != other.data[i])
          return false;
      return true;
    }

    bool operator!=(const bitset& other) const
    {
      return !this->operator==(other);
    }

    bool operator<(const bitset& other) const
    {
      for (unsigned i = 0; i != N; ++i)
        if (data[i] < other.data[i])
          return true;
        else if (data[i] > other.data[i])
          return false;
      return false;
    }

    bool operator<=(const bitset& other) const
    {
      for (unsigned i = 0; i != N; ++i)
        if (data[i] < other.data[i])
          return true;
        else if (data[i] > other.data[i])
          return false;
      return true;
    }

    bool operator>(const bitset& other) const
    {
      return other.operator<(*this);
    }

    bool operator>=(const bitset& other) const
    {
      return other.operator<=(*this);
    }

    void set(unsigned s)
    {
#if SPOT_DEBUG || defined(SWIGPYTHON)
      if (SPOT_UNLIKELY(s >= 8 * N * sizeof(word_t)))
        internal::report_bit_out_of_bounds();
#else
      SPOT_ASSUME(s < 8 * N * sizeof(word_t));
#endif
      data[s / (8*sizeof(word_t))] |= 1U << (s % (8*sizeof(word_t)));
    }

    void clear(unsigned s)
    {
#if SPOT_DEBUG || defined(SWIGPYTHON)
      if (SPOT_UNLIKELY(s >= 8 * N * sizeof(word_t)))
        internal::report_bit_out_of_bounds();
#else
      SPOT_ASSUME(s < 8 * N * sizeof(word_t));
#endif
      data[s / (8*sizeof(word_t))] &= ~(1U << (s % (8*sizeof(word_t))));
    }

    bitset operator<<(unsigned s) const
    {
      bitset r = *this;
      r <<= s;
      return r;
    }
    bitset operator>>(unsigned s) const
    {
      bitset r = *this;
      r >>= s;
      return r;
    }

    bitset& operator<<=(unsigned s)
    {
#if SPOT_DEBUG || defined(SWIGPYTHON)
      if (SPOT_UNLIKELY(s >= 8 * N * sizeof(word_t)))
        internal::report_bit_shift_too_big();
#else
      SPOT_ASSUME(s < 8 * N * sizeof(word_t));
#endif

      // Skip the rest of this function in the most common case of
      // N==1.  g++ 6 can optimize away all the loops if N==1, but
      // clang++4 cannot and needs help.
      if (N == 1)
       {
         data[0] <<= s;
         return *this;
       }

      if (s == 0)
        return *this;
      const unsigned wshift = s / (8 * sizeof(word_t));
      const unsigned offset = s % (8 * sizeof(word_t));

      if (offset == 0)
        {
          for (unsigned i = N - 1; i >= wshift; --i)
            data[i] = data[i - wshift];
        }
      else
        {
          const unsigned sub_offset = 8 * sizeof(word_t) - offset;
          for (unsigned i = N - 1; i > wshift; --i)
            data[i] = ((data[i - wshift] << offset) |
                       (data[i - wshift - 1] >> sub_offset));
          data[wshift] = data[0] << offset;
        }
      std::fill(data.begin(), data.begin() + wshift, word_t(0));
      return *this;
    }

    bitset& operator>>=(unsigned s)
    {
#if SPOT_DEBUG || defined(SWIGPYTHON)
      if (SPOT_UNLIKELY(s >= 8 * N * sizeof(word_t)))
        internal::report_bit_shift_too_big();
#else
      SPOT_ASSUME(s < 8 * N * sizeof(word_t));
#endif
      // Skip the rest of this function in the most common case of
      // N==1.  g++ 6 can optimize away all the loops if N==1, but
      // clang++4 cannot and needs help.
      if (N == 1)
       {
         data[0] >>= s;
         return *this;
       }

      if (s == 0)
        return *this;
      const unsigned wshift = s / (8 * sizeof(word_t));
      const unsigned offset = s % (8 * sizeof(word_t));
      const unsigned limit = N - wshift - 1;

      if (offset == 0)
        {
          for (unsigned i = 0; i <= limit; ++i)
            data[i] = data[i + wshift];
        }
      else
        {
          const unsigned sub_offset = 8 * sizeof(word_t) - offset;
          for (unsigned i = 0; i < limit; ++i)
            data[i] = ((data[i + wshift] >> offset) |
                       (data[i + wshift + 1] << sub_offset));
          data[limit] = data[N - 1] >> offset;
        }
      std::fill(data.begin() + limit + 1, data.end(), word_t(0));
      return *this;
    }

    bitset operator~() const
    {
      bitset r = *this;
      for (auto& v : r.data)
        v = ~v;
      return r;
    }

    bitset operator&(const bitset& other) const
    {
      bitset r = *this;
      r &= other;
      return r;
    }

    bitset operator|(const bitset& other) const
    {
      bitset r = *this;
      r |= other;
      return r;
    }

    bitset operator^(const bitset& other) const
    {
      bitset r = *this;
      r ^= other;
      return r;
    }

    bitset& operator&=(const bitset& other)
    {
      for (unsigned i = 0; i != N; ++i)
        data[i] &= other.data[i];
      return *this;
    }
    bitset& operator|=(const bitset& other)
    {
      for (unsigned i = 0; i != N; ++i)
        data[i] |= other.data[i];
      return *this;
    }
    bitset& operator^=(const bitset& other)
    {
      for (unsigned i = 0; i != N; ++i)
        data[i] ^= other.data[i];
      return *this;
    }

    bitset operator-(word_t s) const
    {
      bitset r = *this;
      r -= s;
      return r;
    }
    bitset& operator-=(word_t s)
    {
      for (auto& v : data)
        {
          if (s == 0)
            break;

          if (v >= s)
            {
              v -= s;
              s = 0;
            }
          else
            {
              v -= s;
              s = 1;
            }
        }
      return *this;
    }

    bitset operator-() const
    {
      bitset res = *this;
      unsigned carry = 0;
      for (auto& v : res.data)
        {
          v += carry;
          if (v < carry)
            carry = 2;
          else
            carry = 1;
          v = -v;
        }
      return res;
    }

    unsigned count() const
    {
      unsigned c = 0U;
      for (auto v : data)
        {
#ifdef __GNUC__
          c += __builtin_popcount(v);
#else
          while (v)
            {
              ++c;
              v &= v - 1;
            }
#endif
        }
      return c;
    }

    unsigned highest() const
    {
      unsigned res = (N-1)*8*sizeof(word_t);
      unsigned i = N;
      while (i--)
        {
          auto v = data[i];
          if (v == 0)
            {
              res -= 8*sizeof(word_t);
              continue;
            }
#ifdef __GNUC__
          res += 8*sizeof(word_t) - __builtin_clz(v);
#else
          while (v)
            {
              ++res;
              v >>= 1;
            }
#endif
          return res-1;
        }
      return 0;
    }

    unsigned lowest() const
    {
      unsigned res = 0U;
      for (auto v: data)
        {
          if (v == 0)
            {
              res += 8*sizeof(v);
              continue;
            }
#ifdef __GNUC__
          res += __builtin_ctz(v);
#else
          while ((v & 1) == 0)
            {
              ++res;
              v >>= 1;
            }
#endif
          return res;
        }
      return 0;
    }
  };

}

namespace std
{
  template<size_t N>
  struct hash<spot::bitset<N>>
  {
    size_t operator()(const spot::bitset<N>& b) const
    {
      return b.hash();
    }
  };
}
