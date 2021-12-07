// -*- coding: utf-8 -*-
// Copyright (C) 2014-2016, 2018-2019 Laboratoire de Recherche et
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

#include "config.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/ltl2taa.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/dot.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " file\n";
  exit(2);
}

int
main(int argc, char** argv)
{
  if (argc != 2)
    syntax(argv[0]);
  std::ifstream input(argv[1]);
  if (!input)
    {
      std::cerr << "failed to open " << argv[1] << '\n';
      return 2;
    }


  auto d = spot::make_bdd_dict();

  std::string s;
  unsigned line = 0;
  while (std::getline(input, s))
    {
      ++line;
      std::cerr << line << ": " << s << '\n';
      if (s.empty() || s[0] == '#') // Skip comments
        continue;

      auto pfpos = spot::parse_infix_psl(s);
      if (pfpos.format_errors(std::cerr))
        return 2;
      auto fpos = pfpos.f;

      auto fneg = spot::formula::Not(fpos);

      {
        auto apos = scc_filter(ltl_to_tgba_fm(fpos, d));
        auto aneg = scc_filter(ltl_to_tgba_fm(fneg, d));
        if (!spot::product(apos, aneg)->is_empty())
          {
            std::cerr << "non-empty intersection between pos and neg (FM)\n";
            exit(2);
          }
      }

      {
        auto apos = scc_filter(ltl_to_tgba_fm(fpos, d, true));
        auto aneg = scc_filter(ltl_to_tgba_fm(fneg, d, true));
        if (!spot::product(apos, aneg)->is_empty())
          {
            std::cerr << "non-empty intersection between pos and neg (FM -x)\n";
            exit(2);
          }
      }

      if (fpos.is_ltl_formula())
        {
          auto apos =
            scc_filter(make_twa_graph(ltl_to_taa(fpos, d),
                                         spot::twa::prop_set::all()));
          auto aneg =
            scc_filter(make_twa_graph(ltl_to_taa(fneg, d),
                                         spot::twa::prop_set::all()));
          if (!spot::product(apos, aneg)->is_empty())
            {
              std::cerr << "non-empty intersection between pos and neg (TAA)\n";
              exit(2);
            }
        }
    }

  assert(spot::fnode::instances_check());
  return 0;
}
