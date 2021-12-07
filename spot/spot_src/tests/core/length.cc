// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2015-2016, 2018-2019 Laboratoire de Recherche
// et Developement de l'Epita (LRDE).
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
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/tl/length.hh>

static void
syntax(char *prog)
{
  std::cerr << prog << " formula\n";
  exit(2);
}

int
main(int argc, char **argv)
{
  if (argc < 2 || argc > 3)
    syntax(argv[0]);

  bool boolone = false;
  if (!strcmp(argv[1], "-b"))
    {
      boolone = true;
      ++argv;
    }

  {
    auto pf1 = spot::parse_infix_psl(argv[1]);
    if (pf1.format_errors(std::cerr))
      return 2;
    auto f1 = pf1.f;

    if (boolone)
      std::cout << spot::length_boolone(f1) << std::endl;
    else
      std::cout << spot::length(f1) << std::endl;
  }

  assert(spot::fnode::instances_check());
  return 0;
}
