// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2014, 2015, 2016 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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
#include <spot/misc/intvcmp2.hh>

namespace spot
{
  namespace
  {
    // This implements integer compression inspired from "Simple-9".
    //
    // The first bits of an integer tell how the rest of the integer is coded:
    // 00:   30 1-bit values              id=0
    // 01:   10 3-bit values              id=1
    // 10:    6 5-bit values              id=2
    // 1100:  4 7-bit values              id=3
    // 1101:  3 9-bit values (1 bit lost) id=4
    // 1110:  2 14-bit values             id=5
    // 1111:  1 28-bit value              id=6

    template <class Self>
    class stream_compression_base
    {
    public:
      stream_compression_base(size_t size)
        : size_(size)
      {
      }

      void run()
      {
        static const unsigned bits_width[7] = { 1, 3, 5, 7, 9, 14, 28 };
        static const unsigned max_count[8] = { 30, 10, 6, 4, 3, 2, 1, 0 };
        static const unsigned max_allowed[8] = { 1,
                                                 (1U << 3) - 1,
                                                 (1U << 5) - 1,
                                                 (1U << 7) - 1,
                                                 (1U << 9) - 1,
                                                 (1U << 14) - 1,
                                                 (1U << 28) - 1,
                                                 -1U };
        // If we have only X data to compress and they fit with the
        // current bit width, the following table tells us we should
        // use bits_width[count_to_level[X - 1]] to limit the number
        // of trailing zeros we encode.  E.g.  count_to_level[5 - 1]
        // is 2, which mean that 5 values should be encoded with
        // bits_width[2] == 5 bits.
        static const unsigned count_to_level[30] =
          {
            6, // 1
            5, // 2
            4, // 3
            3, // 4
            2, // 5
            2, // 6
            1, // 7
            1, // 8
            1, // 9
            1, // 10
            0, 0, 0, 0, 0, // 11-15
            0, 0, 0, 0, 0, // 16-20
            0, 0, 0, 0, 0, // 21-25
            0, 0, 0, 0, 0, // 26-30
          };

        while (size_ > 0)
          {
            unsigned id = 0;        // Current level in the above two tables.
            unsigned curmax_allowed = max_allowed[id];
            unsigned compressable = 0; // Number of integers ready to pack.
            do
              {
                unsigned int val = self().data_at(compressable);
                ++compressable;
                while (val > curmax_allowed)
                  {
                    curmax_allowed = max_allowed[++id];

                    if (compressable > max_count[id])
                      goto fast_encode;
                  }
                if (compressable >= max_count[id])
                  goto fast_encode;
              }
            while (SPOT_LIKELY(compressable < size_));

            assert(compressable < max_count[id]);

            // Since we have less data than the current "id" allows,
            // try to use more bits so we can encode faster.

            id = count_to_level[compressable - 1];

            if (compressable == max_count[id])
              goto fast_encode;

            // Slow compression for situations where we have
            // compressable < max_count[id].  We can only be in
            // one of the 3 first "id" (1, 3, or 5 bits);
            {
              assert(id <= 2);
              unsigned bits = bits_width[id];
              unsigned finalshifts = (max_count[id] - compressable) * bits;
              size_t pos = 0;
              unsigned output = self().data_at(pos);
              while (--compressable)
                {
                  output <<= bits;
                  output += self().data_at(++pos);
                }
              output <<= finalshifts;
              output += id << 30;
              self().push_data(output);
              return;
            }

          fast_encode:
            switch (id)
              {
              case 0: // 30 1-bit values
                {
                  // This code has been tuned so that the compiler can
                  // efficiently encode it as a series of MOV+LEA
                  // instructions, without shifts.  For instance
                  //
                  //   output <<= 1;
                  //   output += self().data_at(4);
                  //
                  // translates to (assuming %eax points to the input,
                  // and %edx holds the output) the following:
                  //
                  //   mov ecx, [eax+16]
                  //   lea edx, [ecx+edx*2]
                  //
                  // This optimization is the reason why we use 'output +='
                  // instead of the more intuitive 'output |=' everywhere in
                  // this file.

                  unsigned int output = 0x00 << 1; // 00
                  output += self().data_at(0);
                  output <<= 1;
                  output += self().data_at(1);
                  output <<= 1;
                  output += self().data_at(2);
                  output <<= 1;
                  output += self().data_at(3);
                  output <<= 1;
                  output += self().data_at(4);
                  output <<= 1;
                  output += self().data_at(5);
                  output <<= 1;
                  output += self().data_at(6);
                  output <<= 1;
                  output += self().data_at(7);
                  output <<= 1;
                  output += self().data_at(8);
                  output <<= 1;
                  output += self().data_at(9);
                  output <<= 1;
                  output += self().data_at(10);
                  output <<= 1;
                  output += self().data_at(11);
                  output <<= 1;
                  output += self().data_at(12);
                  output <<= 1;
                  output += self().data_at(13);
                  output <<= 1;
                  output += self().data_at(14);
                  output <<= 1;
                  output += self().data_at(15);
                  output <<= 1;
                  output += self().data_at(16);
                  output <<= 1;
                  output += self().data_at(17);
                  output <<= 1;
                  output += self().data_at(18);
                  output <<= 1;
                  output += self().data_at(19);
                  output <<= 1;
                  output += self().data_at(20);
                  output <<= 1;
                  output += self().data_at(21);
                  output <<= 1;
                  output += self().data_at(22);
                  output <<= 1;
                  output += self().data_at(23);
                  output <<= 1;
                  output += self().data_at(24);
                  output <<= 1;
                  output += self().data_at(25);
                  output <<= 1;
                  output += self().data_at(26);
                  output <<= 1;
                  output += self().data_at(27);
                  output <<= 1;
                  output += self().data_at(28);
                  output <<= 1;
                  output += self().data_at(29);
                  self().push_data(output);
                }
                break;
              case 1: // 10 3-bit values
                {
                  // This code has been tuned so that the compiler can
                  // efficiently encode it as a series of MOV+LEA
                  // instructions, without shifts.  For instance
                  //
                  //   output <<= 3;
                  //   output += self().data_at(4);
                  //
                  // translates to (assuming %eax points to the input,
                  // and %edx holds the output) the following:
                  //
                  //   mov ecx, [eax+16]
                  //   lea edx, [ecx+edx*8]

                  unsigned int output = 0x01 << 3; // 01
                  output += self().data_at(0);
                  output <<= 3;
                  output += self().data_at(1);
                  output <<= 3;
                  output += self().data_at(2);
                  output <<= 3;
                  output += self().data_at(3);
                  output <<= 3;
                  output += self().data_at(4);
                  output <<= 3;
                  output += self().data_at(5);
                  output <<= 3;
                  output += self().data_at(6);
                  output <<= 3;
                  output += self().data_at(7);
                  output <<= 3;
                  output += self().data_at(8);
                  output <<= 3;
                  output += self().data_at(9);
                  self().push_data(output);
                }
                break;
              case 2: // 6 5-bit values
                {
                  unsigned int output = 0x02U << 30; // 10
                  output += self().data_at(0) << 25;
                  output += self().data_at(1) << 20;
                  output += self().data_at(2) << 15;
                  output += self().data_at(3) << 10;
                  output += self().data_at(4) << 5;
                  output += self().data_at(5);
                  self().push_data(output);
                }
                break;
              case 3: // 4 7-bit values
                {
                  unsigned int output = 0x0CU << 28; // 1100
                  output += self().data_at(0) << 21;
                  output += self().data_at(1) << 14;
                  output += self().data_at(2) << 7;
                  output += self().data_at(3);
                  self().push_data(output);
                }
                break;
              case 4: // 3 9-bit values
                {
                  unsigned int output = 0x0DU << 28; // 1101x (1 bit lost)
                  output += self().data_at(0) << 18;
                  output += self().data_at(1) << 9;
                  output += self().data_at(2);
                  self().push_data(output);
                }
                break;
              case 5: // 2 14-bit values
                {
                  unsigned int output = 0x0EU << 28; // 1110
                  output += self().data_at(0) << 14;
                  output += self().data_at(1);
                  self().push_data(output);
                }
                break;
              case 6: // one 28-bit value
                {
                  unsigned int output = 0x0FU << 28; // 1111
                  output += self().data_at(0);
                  self().push_data(output);
                }
                break;
              }
              self().forward(max_count[id]);
              size_ -= max_count[id];
          }
      }

    protected:

      size_t size_;

      Self& self()
      {
        return static_cast<Self&>(*this);
      }

      const Self& self() const
      {
        return static_cast<const Self&>(*this);
      }

    };


    class int_array_array_compression final:
      public stream_compression_base<int_array_array_compression>
    {
    public:
      int_array_array_compression(const int* array, size_t n,
                                  int* dest, size_t& dest_n)
        : stream_compression_base<int_array_array_compression>(n),
          array_(array), result_size_(dest_n),
          result_(dest), result_end_(dest + dest_n)
      {
        result_size_ = 0; // this resets dest_n.
      }

      void push_data(unsigned int i)
      {
        assert(result_ < result_end_);
        ++result_size_;
        *result_++ = static_cast<int>(i);
      }

      unsigned int data_at(size_t offset)
      {
        return static_cast<unsigned int>(array_[offset]);
      }

      void forward(size_t offset)
      {
        array_ += offset;
      }

    protected:
      const int* array_;
      size_t& result_size_;
      int* result_;
      int* result_end_;
    };

  } // anonymous


  void
  int_array_array_compress2(const int* array, size_t n,
                            int* dest, size_t& dest_size)
  {
    int_array_array_compression c(array, n, dest, dest_size);
    c.run();
  }



  namespace
  {

    template<class Self>
    class stream_decompression_base
    {
    public:

      void run()
      {
        while (SPOT_LIKELY(self().have_comp_data()))
          {
            unsigned val = self().next_comp_data();

            unsigned id = val >> 28;
            switch (id)
              {
              case 0x00: // 00xx - 30 1-bit values.
              case 0x01:
              case 0x02:
              case 0x03:
                self().write_data_at(0,  !!(val & (1U << 29)));
                self().write_data_at(1,  !!(val & (1U << 28)));
                self().write_data_at(2,  !!(val & (1U << 27)));
                self().write_data_at(3,  !!(val & (1U << 26)));
                self().write_data_at(4,  !!(val & (1U << 25)));
                self().write_data_at(5,  !!(val & (1U << 24)));
                self().write_data_at(6,  !!(val & (1U << 23)));
                self().write_data_at(7,  !!(val & (1U << 22)));
                self().write_data_at(8,  !!(val & (1U << 21)));
                self().write_data_at(9,  !!(val & (1U << 20)));
                self().write_data_at(10, !!(val & (1U << 19)));
                self().write_data_at(11, !!(val & (1U << 18)));
                self().write_data_at(12, !!(val & (1U << 17)));
                self().write_data_at(13, !!(val & (1U << 16)));
                self().write_data_at(14, !!(val & (1U << 15)));
                self().write_data_at(15, !!(val & (1U << 14)));
                self().write_data_at(16, !!(val & (1U << 13)));
                self().write_data_at(17, !!(val & (1U << 12)));
                self().write_data_at(18, !!(val & (1U << 11)));
                self().write_data_at(19, !!(val & (1U << 10)));
                self().write_data_at(20, !!(val & (1U <<  9)));
                self().write_data_at(21, !!(val & (1U <<  8)));
                self().write_data_at(22, !!(val & (1U <<  7)));
                self().write_data_at(23, !!(val & (1U <<  6)));
                self().write_data_at(24, !!(val & (1U <<  5)));
                self().write_data_at(25, !!(val & (1U <<  4)));
                self().write_data_at(26, !!(val & (1U <<  3)));
                self().write_data_at(27, !!(val & (1U <<  2)));
                self().write_data_at(28, !!(val & (1U <<  1)));
                self().write_data_at(29, !!(val & (1U <<  0)));
                self().forward(30);
                break;
              case 0x04: // 01xx - 10 3-bit values.
              case 0x05:
              case 0x06:
              case 0x07:
                self().write_data_at(0, (val >> 27) & 0x07);
                self().write_data_at(1, (val >> 24) & 0x07);
                self().write_data_at(2, (val >> 21) & 0x07);
                self().write_data_at(3, (val >> 18) & 0x07);
                self().write_data_at(4, (val >> 15) & 0x07);
                self().write_data_at(5, (val >> 12) & 0x07);
                self().write_data_at(6, (val >>  9) & 0x07);
                self().write_data_at(7, (val >>  6) & 0x07);
                self().write_data_at(8, (val >>  3) & 0x07);
                self().write_data_at(9, (val >>  0) & 0x07);
                self().forward(10);
                break;
              case 0x08: // 10xx - 6 5-bit values.
              case 0x09:
              case 0x0A:
              case 0x0B:
                self().write_data_at(0, (val >> 25) & 0x1F);
                self().write_data_at(1, (val >> 20) & 0x1F);
                self().write_data_at(2, (val >> 15) & 0x1F);
                self().write_data_at(3, (val >> 10) & 0x1F);
                self().write_data_at(4, (val >>  5) & 0x1F);
                self().write_data_at(5, (val >>  0) & 0x1F);
                self().forward(6);
                break;
              case 0x0C: // 1100 - 4 7-bit values
                self().write_data_at(0, (val >> 21) & 0x7F);
                self().write_data_at(1, (val >> 14) & 0x7F);
                self().write_data_at(2, (val >>  7) & 0x7F);
                self().write_data_at(3, (val >>  0) & 0x7F);
                self().forward(4);
                break;
              case 0x0D: // 1101x - 3 9-bit values.
                self().write_data_at(0, (val >> 18) & 0x1FF);
                self().write_data_at(1, (val >>  9) & 0x1FF);
                self().write_data_at(2, (val >>  0) & 0x1FF);
                self().forward(3);
                break;
              case 0x0E: // 110x - 2 14-bit values.
                self().write_data_at(0, (val >> 14) & 0x3FFF);
                self().write_data_at(1, (val >>  0) & 0x3FFF);
                self().forward(2);
                break;
              case 0x0F: // 1100 - 1 28-bit value.
                self().write_data_at(0, val & 0xFFFFFFF);
                self().forward(1);
                break;
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
    };


    class int_array_array_decompression final:
      public stream_decompression_base<int_array_array_decompression>
    {
    public:
      int_array_array_decompression(const int* array,
                                    size_t array_size,
                                    int* res)
        : array_(array), n_(array_size), pos_(0), result_(res)
      {
      }

      void write_data_at(size_t pos, unsigned int i)
      {
        result_[pos] = i;
      }

      void forward(size_t i)
      {
        result_ += i;
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
      const int* array_;
      size_t n_;
      size_t pos_;
      int* result_;
    };

  }


  void
  int_array_array_decompress2(const int* array, size_t array_size, int* res,
                              size_t)
  {
    int_array_array_decompression c(array, array_size, res);
    c.run();
  }



} // spot
