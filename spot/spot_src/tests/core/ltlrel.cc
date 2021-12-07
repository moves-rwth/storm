// -*- coding: utf-8 -*-
// Copyright (C) 2013-2016, 2018-2019 Laboratoire de Recherche et
// Developement de l'Epita (LRDE).
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
#include <spot/tl/relabel.hh>
#include <spot/tl/print.hh>

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

  {
    auto pf1 = spot::parse_infix_psl(argv[1]);
    if (pf1.format_errors(std::cerr))
      return 2;
    auto f1 = pf1.f;

    spot::relabeling_map* m = new spot::relabeling_map;
    auto f2 = spot::relabel_bse(f1, spot::Pnn, m);
    spot::print_psl(std::cout, f2) << '\n';


    typedef std::map<std::string, std::string> map_t;
    map_t sorted_map;
    for (spot::relabeling_map::const_iterator i = m->begin();
         i != m->end(); ++i)
      sorted_map[spot::str_psl(i->first)] =
        spot::str_psl(i->second);
    for (map_t::const_iterator i = sorted_map.begin();
         i != sorted_map.end(); ++i)
      std::cout << "  " << i->first << "   ->   "
                << i->second << '\n';
    delete m;
  }
  assert(spot::fnode::instances_check());
  return 0;
}
