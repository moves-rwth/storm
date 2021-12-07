// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2009, 2012, 2015, 2016, 2018 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/tl/dot.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " [-d] filename\n";
  exit(2);
}

int
main(int argc, char** argv)
{
  int exit_code = 0;

  if (argc < 2)
    syntax(argv[0]);

  bool debug = false;
  int filename_index = 1;

  if (!strcmp(argv[1], "-d"))
    {
      debug = true;
      if (argc < 3)
        syntax(argv[0]);
      filename_index = 2;
    }

  std::ifstream fin(argv[filename_index]);
  if (!fin)
    {
      std::cerr << "Cannot open " << argv[filename_index] << '\n';
      exit(2);
    }

  std::string input;
  while (std::getline(fin, input))
    {
      {
        spot::environment& env(spot::default_environment::instance());

        auto f = [&]()
          {
            auto pf = spot::parse_infix_psl(input, env, debug);
            // We want the errors on std::cout for the test suite.
            exit_code = pf.format_errors(std::cout);
            return pf.f;
          }();

        if (f)
          {
#ifdef DOTTY
            spot::print_dot_psl(std::cout, f);
#else
            f.dump(std::cout) << '\n';
#endif
          }
        else
          {
            exit_code = 1;
          }

      }
      assert(spot::fnode::instances_check());
    }
  return exit_code;
}
