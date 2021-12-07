// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2014, 2015, 2018 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE)
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
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/hoa.hh>

using namespace spot;

int main(int argc, char** argv)
{
  (void) argc;
  assert(argc == 2);
  int return_value = 0;

  spot::automaton_parser_options opts;
  opts.want_kripke = true;
  spot::automaton_stream_parser parser(argv[1], opts);

  while (auto paut = parser.parse(make_bdd_dict(),
                                  default_environment::instance()))
    {
      if (paut->format_errors(std::cerr))
        {
          return_value = 1;
          if (paut->ks)
            continue;
        }
      if (!paut->ks)
        break;
      print_hoa(std::cout, paut->ks);
    }
  return return_value;
}
