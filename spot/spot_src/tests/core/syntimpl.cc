// -*- coding: utf-8 -*-
// Copyright (C) 2008-2012, 2014-2016, 2018-2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6
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
#include <cassert>
#include <cstdlib>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/nenoform.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " formula1 formula2?\n";
  exit(2);
}

int
main(int argc, char** argv)
{
  if (argc < 4)
    syntax(argv[0]);

  int opt = atoi(argv[1]);
  int exit_return = 0;

  {
    auto ftmp1 = spot::parse_infix_psl(argv[2]);

    if (ftmp1.format_errors(std::cerr))
      return 2;

    auto ftmp2 = spot::parse_infix_psl(argv[3]);

    if (ftmp2.format_errors(std::cerr))
      return 2;

    spot::formula f1 = spot::negative_normal_form(ftmp1.f);
    spot::formula f2 = spot::negative_normal_form(ftmp2.f);

    std::string f1s = spot::str_psl(f1);
    std::string f2s = spot::str_psl(f2);

    spot::tl_simplifier* c = new spot::tl_simplifier;

    switch (opt)
      {
      case 0:
        std::cout << "Test f1 < f2" << std::endl;
        if (c->syntactic_implication(f1, f2))
          {
            std::cout << f1s << " < " << f2s << '\n';
            exit_return = 1;
          }
        break;

      case 1:
        std::cout << "Test !f1 < f2" << std::endl;
        if (c->syntactic_implication_neg(f1, f2, false))
          {
            std::cout << "!(" << f1s << ") < " << f2s << '\n';
            exit_return = 1;
          }
        break;

      case 2:
        std::cout << "Test f1 < !f2" << std::endl;
        if (c->syntactic_implication_neg(f1, f2, true))
          {
            std::cout << f1s << " < !(" << f2s << ")\n";
            exit_return = 1;
          }
        break;
      default:
        break;
      }

    f1.dump(std::cout) << '\n';
    f2.dump(std::cout) << '\n';

    delete c;
  }
  assert(spot::fnode::instances_check());
  return exit_return;
}
