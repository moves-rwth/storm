// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014 Laboratoire de Recherche et Développement
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

#include "common_sys.hh"
#include "error.h"
#include "common_r.hh"

int simplification_level = 0;

void
parse_r(const char* arg)
{
  if (!arg)
    {
      simplification_level = 3;
      return;
    }
  if (arg[1] == 0 && arg[0] >= '0' && arg[0] <= '3')
    {
      simplification_level = arg[0] - '0';
      return;
    }
  error(2, 0, "invalid simplification level '%s'",  arg);
}
