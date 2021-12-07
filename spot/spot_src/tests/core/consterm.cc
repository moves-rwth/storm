// -*- coding: utf-8 -*-
// Copyright (C) 2010-2012, 2015-2016, 2018-2019 Laboratoire de Recherche
// et Dévelopement de l'Epita (LRDE).
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
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <spot/tl/parse.hh>

static void
syntax(char *prog)
{
  std::cerr << prog << " formula\n";
  exit(2);
}

int
main(int argc, char **argv)
{
  if (argc != 2)
    syntax(argv[0]);

  std::ifstream input(argv[1]);
  if (!input)
    {
      std::cerr << "failed to open " << argv[1] << '\n';
      return 2;
    }

  std::string s;
  while (std::getline(input, s))
    {
      if (s[0] == '#')                // Skip comments
        {
          std::cerr << s << '\n';
          continue;
        }
      std::istringstream ss(s);
      std::string form;
      bool expected;
      std::getline(ss, form, ',');
      ss >> expected;

      auto pf1 = spot::parse_infix_sere(form);
      if (pf1.format_errors(std::cerr))
        return 2;

      bool b = pf1.f.accepts_eword();
      std::cout << form << ',' << b << '\n';
      if (b != expected)
        {
          std::cerr << "computed '" << b
                    << "' but expected '" << expected << "'\n";
          return 2;
        }
    }

  assert(spot::fnode::instances_check());
  return 0;
}
