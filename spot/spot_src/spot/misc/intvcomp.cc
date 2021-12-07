// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2014, 2016 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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
#include <spot/misc/common.hh>
#include <cstddef>
#include <cassert>
#include <spot/misc/intvcomp.hh>
#include <iostream>

namespace spot
{

  //  Compression scheme
  //  ------------------
  //
  // Assumptions:
  //  - small and positive values are more frequent than negative
  //    and large values.
  //  - 0 is the most frequent value
  //  - repeated values (esp. repeated 0s occur often).
  //
  //  00  encodes "value 0"
  //  010 encodes "value 1"
  //  011 encodes "a value in [2..5]" followed by 2 bits
  //  100 encodes "a value in [6..21]" followed by 4 bits
  //  101 encodes "repeat prev. value [1..8] times" followed by 3 bits count
  //  110 encodes "repeat prev. value [9..40] times" followed by 5 bits count
  //  111 encodes "an int value" followed by 32 bits
  //
  // If 101 or 110 occur at the start, the value to repeat is 0.

  namespace
  {

    template <class Self>
    class stream_compression_base
    {
      static const unsigned int max_bits = sizeof(unsigned int) * 8;

    public:
      stream_compression_base()
        : cur_(0), bits_left_(max_bits)
      {
      }

      void emit(unsigned int val)
      {
        if (val == 0)
          {
            self().push_bits(0x0, 2, 0x3);
          }
        else if (val == 1)
          {
            self().push_bits(0x2, 3, 0x7);
          }
        else if (val >= 2 && val <= 5)
          {
            self().push_bits(0x3, 3, 0x7);
            self().push_bits(val - 2, 2, 0x3);
          }
        else if (val >= 6 && val <= 21)
          {
            self().push_bits(0x4, 3, 0x7);
            self().push_bits(val - 6, 4, 0xf);
          }
        else
          {
            assert(val > 21);
            self().push_bits(0x7, 3, 0x7);
            self().push_bits(val, 32, -1U);
          }
      }

      void run()
      {
        unsigned int last_val = 0;

        while (SPOT_LIKELY(self().have_data()))
          {
            unsigned int val = self().next_data();
            // Repeated value?  Try to find more.
            if (val == last_val)
              {
                unsigned int count = 1;
                while (count < 40 && self().skip_if(val))
                  ++count;

                if ((val == 0 && count < 3) || (val == 1 && count == 1))
                  {
                    // it is more efficient to emit 0 once or twice directly
                    // (e.g., 00 00 vs. 011 11)
                    // for value 1, repetition is worthwhile for count > 1
                    // (e.g., 010 010 vs. 011 00)
                    while (count--)
                      emit(val);
                  }
                else if (count < 9)
                  {
                    self().push_bits(0x5, 3, 0x7);
                    self().push_bits(count - 1, 3, 0x7);
                  }
                else
                  {
                    self().push_bits(0x6, 3, 0x7);
                    self().push_bits(count - 9, 5, 0x1f);
                  }
              }
            else
              {
                emit(val);
                last_val = val;
              }
          }
        flush();
      }

      // This version assumes there is at least n bits free in cur_.
      void
      push_bits_unchecked(unsigned int bits, unsigned int n, unsigned int mask)
      {
        cur_ <<= n;
        cur_ |= (bits & mask);
        if (SPOT_LIKELY(bits_left_ -= n))
          return;

        self().push_data(cur_);
        cur_ = 0;
        bits_left_ = max_bits;
      }

      void
      push_bits(unsigned int bits, unsigned int n, unsigned int mask)
      {
        if (SPOT_LIKELY(n <= bits_left_))
          {
            push_bits_unchecked(bits, n, mask);
            return;
          }

        // bits_left_ < n

        unsigned int right_bit_count = n - bits_left_;
        unsigned int left = bits >> right_bit_count;
        push_bits_unchecked(left, bits_left_, (1U << bits_left_) - 1);
        push_bits_unchecked(bits, right_bit_count, (1U << right_bit_count) - 1);
      }

      void flush()
      {
        if (bits_left_ == max_bits)
          return;
        cur_ <<= bits_left_;
        self().push_data(cur_);
      }

    protected:
      Self& self()
      {
        return static_cast<Self&>(*this);
      }

      const Self& self() const
      {
        return static_cast<const Self&>(*this);
      }

      unsigned int cur_;
      unsigned int bits_left_;
    };

    class int_array_vector_compression final:
      public stream_compression_base<int_array_vector_compression>
    {
    public:
      int_array_vector_compression(const int* array, size_t n)
        : array_(array), n_(n), pos_(0), result_(new std::vector<unsigned int>)
      {
      }

      void push_data(unsigned int i)
      {
        result_->emplace_back(i);
      }

      const std::vector<unsigned int>*
      result() const
      {
        return result_;
      }

      bool have_data() const
      {
        return pos_ < n_;
      }

      unsigned int next_data()
      {
        return static_cast<unsigned int>(array_[pos_++]);
      }

      bool skip_if(unsigned int val)
      {
        if (SPOT_UNLIKELY(!have_data()))
          return false;

        if (static_cast<unsigned int>(array_[pos_]) != val)
          return false;

        ++pos_;
        return true;
      }

    protected:
      const int* array_;
      size_t n_;
      size_t pos_;
      std::vector<unsigned int>* result_;
    };

    class int_vector_vector_compression final:
      public stream_compression_base<int_vector_vector_compression>
    {
    public:
      int_vector_vector_compression(const std::vector<int>& input,
                                    std::vector<unsigned int>& output)
        : input_(input), pos_(input.begin()), end_(input.end()), output_(output)
      {
      }

      void push_data(unsigned int i)
      {
        output_.emplace_back(i);
      }

      bool have_data() const
      {
        return pos_ < end_;
      }

      unsigned int next_data()
      {
        return static_cast<unsigned int>(*pos_++);
      }

      bool skip_if(unsigned int val)
      {
        if (SPOT_UNLIKELY(!have_data()))
          return false;

        if (static_cast<unsigned int>(*pos_) != val)
          return false;

        ++pos_;
        return true;
      }

    protected:
      const std::vector<int>& input_;
      std::vector<int>::const_iterator pos_;
      std::vector<int>::const_iterator end_;
      std::vector<unsigned int>& output_;
    };

    class int_array_array_compression final:
      public stream_compression_base<int_array_array_compression>
    {
    public:
      int_array_array_compression(const int* array, size_t n,
                                  int* dest, size_t& dest_n)
        : array_(array), n_(n), pos_(0),
          result_size_(dest_n), result_(dest), result_end_(dest + dest_n)
      {
        result_size_ = 0; // this resets dest_n.
      }

      void push_data(unsigned int i)
      {
        assert(result_ < result_end_);
        ++result_size_;
        *result_++ = static_cast<int>(i);
      }

      bool have_data() const
      {
        return pos_ < n_;
      }

      unsigned int next_data()
      {
        return static_cast<unsigned int>(array_[pos_++]);
      }

      bool skip_if(unsigned int val)
      {
        if (SPOT_UNLIKELY(!have_data()))
          return false;

        if (static_cast<unsigned int>(array_[pos_]) != val)
          return false;

        ++pos_;
        return true;
      }

    protected:
      const int* array_;
      size_t n_;
      size_t pos_;
      size_t& result_size_;
      int* result_;
      int* result_end_;
    };
  }

  void
  int_vector_vector_compress(const std::vector<int>& input,
                             std::vector<unsigned>& output)
  {
    int_vector_vector_compression c(input, output);
    c.run();
  }

  const std::vector<unsigned int>*
  int_array_vector_compress(const int* array, size_t n)
  {
    int_array_vector_compression c(array, n);
    c.run();
    return c.result();
  }

  void
  int_array_array_compress(const int* array, size_t n,
                            int* dest, size_t& dest_size)
  {
    int_array_array_compression c(array, n, dest, dest_size);
    c.run();
  }

  //////////////////////////////////////////////////////////////////////

  namespace
  {

    template<class Self>
    class stream_decompression_base
    {
      static const unsigned int max_bits = sizeof(unsigned int) * 8;

    public:
      void refill()
      {
        if (SPOT_UNLIKELY(look_bits_ == 0))
          {
            look_bits_ = max_bits;
            look_ = buffer_;

            if (SPOT_LIKELY(self().have_comp_data()))
              buffer_ = self().next_comp_data();

            if (SPOT_LIKELY(buffer_bits_ != max_bits))
              {
                unsigned int fill_size = max_bits - buffer_bits_;
                look_ <<= fill_size;
                look_ |= buffer_ >> buffer_bits_;
              }
          }
        else
          {
            unsigned int fill_size = max_bits - look_bits_;
            if (fill_size > buffer_bits_)
              fill_size = buffer_bits_;

            look_ <<= fill_size;
            buffer_bits_ -= fill_size;
            look_ |= (buffer_ >> buffer_bits_) & ((1U << fill_size) - 1);
            look_bits_ += fill_size;

            if (buffer_bits_ == 0)
              {
                if (SPOT_LIKELY(self().have_comp_data()))
                  buffer_ = self().next_comp_data();

                unsigned int left = max_bits - look_bits_;
                if (left != 0)
                  {
                    look_ <<= left;
                    look_ |= buffer_ >> look_bits_;
                    buffer_bits_ = look_bits_;
                    look_bits_ = max_bits;
                  }
                else
                  {
                    buffer_bits_ = max_bits;
                  }
              }
          }
      }

      unsigned int look_n_bits(unsigned int n)
      {
        if (SPOT_UNLIKELY(look_bits_ < n))
          refill();
        assert(n <= look_bits_);
        return (look_ >> (look_bits_ - n)) & ((1U << n) - 1);
      }

      void skip_n_bits(unsigned int n)
      {
        assert (n <= look_bits_);
        look_bits_ -= n;
      }

      unsigned int get_n_bits(unsigned int n)
      {
        if (SPOT_UNLIKELY(look_bits_ < n))
          refill();
        look_bits_ -= n;
        return (look_ >> look_bits_) & ((1U << n) - 1);
      }

      unsigned int get_32_bits()
      {
        //        std::cerr << "get_32" << std::endl;
        if (SPOT_LIKELY(look_bits_ < 32))
          refill();
        unsigned int val = look_;
        look_bits_ = 0;
        refill();
        return val;
      }

      void run()
      {
        if (SPOT_UNLIKELY(!self().have_comp_data()))
          return;

        look_ = self().next_comp_data();
        look_bits_ = max_bits;
        if (SPOT_LIKELY(self().have_comp_data()))
          {
            buffer_ = self().next_comp_data();
            buffer_bits_ = max_bits;
          }
        else
          {
            buffer_ = 0;
            buffer_bits_ = 0;
          }

        while (SPOT_LIKELY(!self().complete()))
          {
            unsigned int token = look_n_bits(3);
            switch (token)
              {
              case 0x0: // 00[0]
              case 0x1: // 00[1]
                skip_n_bits(2);
                self().push_data(0);
                break;
              case 0x2: // 010
                skip_n_bits(3);
                self().push_data(1);
                break;
              case 0x3: // 011
                skip_n_bits(3);
                self().push_data(2 + get_n_bits(2));
                break;
              case 0x4: // 100
                skip_n_bits(3);
                self().push_data(6 + get_n_bits(4));
                break;
              case 0x5: // 101
                skip_n_bits(3);
                self().repeat(1 + get_n_bits(3));
                break;
              case 0x6: // 110
                skip_n_bits(3);
                self().repeat(9 + get_n_bits(5));
                break;
              case 0x7: // 111
                skip_n_bits(3);
                self().push_data(get_32_bits());
                break;
              default:
                SPOT_UNREACHABLE();
              }
          }
      }


    protected:
      Self& self()
      {
        return static_cast<Self&>(*this);
      }

      const Self& self() const
      {
        return static_cast<const Self&>(*this);
      }

      unsigned int look_;
      unsigned int look_bits_;
      unsigned int buffer_;
      unsigned int buffer_bits_;
    };

    class int_vector_vector_decompression final:
      public stream_decompression_base<int_vector_vector_decompression>
    {
    public:
      int_vector_vector_decompression(const std::vector<unsigned int>& array,
                                      std::vector<int>& res, size_t size)
        : prev_(0), array_(array),
          pos_(array.begin()), end_(array.end()),
          result_(res), size_(size)
      {
        result_.reserve(size);
      }

      bool complete() const
      {
        return size_ == 0;
      }

      void push_data(int i)
      {
        prev_ = i;
        result_.emplace_back(i);
        --size_;
      }

      void repeat(unsigned int i)
      {
        size_ -= i;
        while (i--)
          result_.emplace_back(prev_);
      }

      bool have_comp_data() const
      {
        return pos_ != end_;
      }

      unsigned int next_comp_data()
      {
        return *pos_++;
      }

    protected:
      int prev_;
      const std::vector<unsigned int>& array_;
      std::vector<unsigned int>::const_iterator pos_;
      std::vector<unsigned int>::const_iterator end_;
      std::vector<int>& result_;
      size_t size_;
    };

    class int_vector_array_decompression final:
      public stream_decompression_base<int_vector_array_decompression>
    {
    public:
      int_vector_array_decompression(const std::vector<unsigned int>* array,
                                     int* res,
                                     size_t size)
        : prev_(0), array_(array), n_(array->size()), pos_(0), result_(res),
          size_(size)
      {
      }

      bool complete() const
      {
        return size_ == 0;
      }

      void push_data(int i)
      {
        prev_ = i;
        *result_++ = i;
        --size_;
      }

      void repeat(unsigned int i)
      {
        size_ -= i;
        while (i--)
          *result_++ = prev_;
      }

      bool have_comp_data() const
      {
        return pos_ < n_;
      }

      unsigned int next_comp_data()
      {
        return (*array_)[pos_++];
      }

    protected:
      int prev_;
      const std::vector<unsigned int>* array_;
      size_t n_;
      size_t pos_;
      int* result_;
      size_t size_;
    };

    class int_array_array_decompression final:
      public stream_decompression_base<int_array_array_decompression>
    {
    public:
      int_array_array_decompression(const int* array,
                                    size_t array_size,
                                    int* res,
                                    size_t size)
        : prev_(0), array_(array), n_(array_size), pos_(0), result_(res),
          size_(size)
      {
      }

      bool complete() const
      {
        return size_ == 0;
      }

      void push_data(int i)
      {
        prev_ = i;
        *result_++ = i;
        --size_;
      }

      void repeat(unsigned int i)
      {
        size_ -= i;
        while (i--)
          *result_++ = prev_;
      }

      bool have_comp_data() const
      {
        return pos_ < n_;
      }

      unsigned int next_comp_data()
      {
        return array_[pos_++];
      }

    protected:
      int prev_;
      const int* array_;
      size_t n_;
      size_t pos_;
      int* result_;
      size_t size_;
    };

  }

  void
  int_vector_vector_decompress(const std::vector<unsigned int>& input,
                               std::vector<int>& output, size_t size)
  {
    int_vector_vector_decompression c(input, output, size);
    c.run();
  }

  void
  int_vector_array_decompress(const std::vector<unsigned int>* array, int* res,
                              size_t size)
  {
    int_vector_array_decompression c(array, res, size);
    c.run();
  }

  void
  int_array_array_decompress(const int* array, size_t array_size,
                             int* res, size_t size)
  {
    int_array_array_decompression c(array, array_size, res, size);
    c.run();
  }


}
