// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2014, 2015, 2018 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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
#include <iostream>
#include <spot/misc/intvcmp2.hh>
#include <cstring>


static int
check_aa(int* data, int size, unsigned expected = 0)
{
  int* comp = new int[size * 2];
  size_t csize = size * 2;
  spot::int_array_array_compress2(data, size, comp, csize);

  std::cout << "AC[" << csize << "] ";
  for (size_t i = 0; i < csize; ++i)
    std::cout << comp[i] << ' ';
  std::cout << std::endl;

  int* decomp = new int[size + 30];
  spot::int_array_array_decompress2(comp, csize, decomp, size);

  std::cout << "AD[" << size << "] ";
  for (int i = 0; i < size; ++i)
    std::cout << decomp[i] << ' ';
  std::cout << std::endl;

  int res = memcmp(data, decomp, size * sizeof(int));

  if (res)
    {
      std::cout << "*** cmp error *** " << res << std::endl;
      std::cout << "AE[" << size << "] ";
      for (int i = 0; i < size; ++i)
        std::cout << data[i] << ' ';
      std::cout << std::endl;
    }

  if (expected && (csize * sizeof(int) != expected))
    {
      std::cout << "*** size error *** (expected "
                << expected << " bytes, got " << csize * sizeof(int)
                << " bytes)" << std::endl;
      res = 1;
    }

  std::cout << std::endl;

  delete[] comp;
  delete[] decomp;
  return !!res;
}

static int
check(int* comp, int size, unsigned expected = 0)
{
  return
    //check_vv(comp, size, expected) +
    //check_av(comp, size, expected) +
    check_aa(comp, size, expected);
}

int main()
{
  int errors = 0;

  int comp1[] = { 1, 0, 0, 0, 0, 0, 3, 3, 4, 0, 0, 0 };
  errors += check(comp1, sizeof(comp1) / sizeof(*comp1));

  int comp2[] = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 1 };
  errors += check(comp2, sizeof(comp2) / sizeof(*comp2));

  int comp3[] = { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1,
                  0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0,
                  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
                  0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
                  0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1,
                  0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0 };
  errors += check(comp3, sizeof(comp3) / sizeof(*comp3));

  int comp4[] = { 1, 2, 1, 2, 1, 2, 2, 0 }; // 32 bits
  errors += check(comp4, sizeof(comp4) / sizeof(*comp4), 4);

  int comp5[] = { 1, 2, 1, 2, 1, 2, 2, 0, 1, 2, 1, 2, 1, 2, 2, 0 }; // 64 bits
  errors += check(comp5, sizeof(comp5) / sizeof(*comp5), 8);

  int comp6[] = { 1, 2, 1, 2, 1, 2, 2, 0, 1, 2, 1, 2, 1, 2, 2, 0,
                  1, 2, 1, 2, 1, 2, 2, 0, 1, 2, 1, 2, 1, 2, 2, 0 }; // 128 bits
  errors += check(comp6, sizeof(comp6) / sizeof(*comp6), 16);

  int comp7[] = { 4, 8, 10, 3, 49, 50, 0, 20, 13 };
  errors += check(comp7, sizeof(comp7) / sizeof(*comp7));

  int comp8[] = { 4959, 6754, 8133, 10985, 11121, 14413, 17335, 20754,
                  21317, 30008, 30381, 33494, 34935, 41210, 41417 };
  errors += check(comp8, sizeof(comp8) / sizeof(*comp8));

  int comp9[] = { 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  errors += check(comp9, sizeof(comp9) / sizeof(*comp9));

  int comp10[] = { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0,
                   0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,
                   9, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 10,
                   0, 0, 0, 0, 0, 0, 0, 0, 9 };
  errors += check(comp10, sizeof(comp10) / sizeof(*comp10));

  int comp11[] = { 2, 254, 254, 0, 1, 4, 0, 0, 0, 1, 0, 1, 253, 0 };
  errors += check(comp11, sizeof(comp11) / sizeof(*comp11));

  return errors;
}
