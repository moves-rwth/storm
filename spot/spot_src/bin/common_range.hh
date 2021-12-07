// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014, 2015, 2016 Laboratoire de Recherche et
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

#pragma once

#define RANGE_DOC                                                       \
    { nullptr, 0, nullptr, 0,                                           \
      "RANGE may have one of the following forms: 'INT', "              \
      "'INT..INT', or '..INT'.\nIn the latter case, the missing number " \
      "is assumed to be 1.", 0 }

#define RANGE_DOC_FULL                                          \
    { nullptr, 0, nullptr, 0,                                   \
      "RANGE may have one of the following forms: 'INT', "      \
      "'INT..INT', '..INT', or 'INT..'", 0 }

#define WORD_DOC                                                        \
    { nullptr, 0, nullptr, 0,                                           \
      "WORD is lasso-shaped and written as 'BF;BF;...;BF;cycle{BF;...;BF}' " \
      "where BF are arbitrary Boolean formulas.  The 'cycle{...}' part is " \
      "mandatory, but the prefix can be omitted.", 0 }

struct range
{
  int min;
  int max;

  bool contains(int val)
  {
    return val >= min && val <= max;
  }
};

// INT, INT..INT, ..INT, or INT..
//
// The missing_left and missing_right argument gives the default bound
// values.  Additionally, if missing_right == 0, then the INT.. form
// is disallowed.
range parse_range(const char* str,
                  int missing_left = 1, int missing_right = 0);
