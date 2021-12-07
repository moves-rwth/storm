// -*- coding: utf-8 -*-
// Copyright (C) 2014-2016, 2018, 2019 Laboratoire de Recherche et
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
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/taalgos/minimize.hh>
#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/dot.hh>
#include <spot/taalgos/stats.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " file\n";
  exit(2);
}

static void
stats(std::string title, const spot::ta_ptr& ta)
{
  auto s = stats_reachable(ta);

  std::cout << std::left << std::setw(20) << title << " | "
            << std::right << std::setw(6) << s.states << " | "
            << std::setw(6) << s.edges << " | "
            << std::setw(6) << s.acceptance_states << '\n';
}

static void
stats(std::string title, const spot::twa_ptr& tg)
{
  auto s = stats_reachable(tg);

  std::cout << std::left << std::setw(20) << title << " | "
            << std::right << std::setw(6) << s.states << " | "
            << std::setw(6) << s.edges << " | "
            << std::setw(6) << "XXX" << '\n';
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
  while (std::getline(input, s))
    {
      std::cout << "in: " << s << '\n';
      if (s.empty() || s[0] == '#') // Skip comments
        continue;

      auto pf = spot::parse_infix_psl(s);

      if (pf.format_errors(std::cerr))
        return 2;
      auto f = pf.f;


      {
        auto a = ltl_to_tgba_fm(f, d);
        bdd ap_set = atomic_prop_collect_as_bdd(f, a);

        // run 0 ../ikwiad -TGTA -ks "$1"
        // run 0 ../ikwiad -TGTA -RT -ks "$1"
        {
          auto t = spot::tgba_to_tgta(a, ap_set);
          stats("-TGTA", t);
          stats("-TGTA -RT", minimize_tgta(t));
        }

        {
          // run 0 ../ikwiad -TA -ks "$1"
          // run 0 ../ikwiad -TA -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    false, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    false, // single_pass (-sp),
                                    false); // artificial_livelock (-lv)
          stats("-TA", t);
          stats("-TA -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -lv -ks "$1"
          // run 0 ../ikwiad -TA -lv -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    false, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    false, // single_pass (-sp),
                                    true); // artificial_livelock (-lv)
          stats("-TA -lv", t);
          stats("-TA -lv -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -sp -ks "$1"
          // run 0 ../ikwiad -TA -sp -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    false, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    true, // single_pass (-sp),
                                    false); // artificial_livelock (-lv)
          stats("-TA -sp", t);
          stats("-TA -sp -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -lv -sp -ks "$1"
          // run 0 ../ikwiad -TA -lv -sp -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    false, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    true, // single_pass (-sp),
                                    true); // artificial_livelock (-lv)
          stats("-TA -lv -sp", t);
          stats("-TA -lv -sp -RT", minimize_ta(t));
        }

        a = spot::degeneralize(a);

        {
          // run 0 ../ikwiad -TA -DS -ks "$1"
          // run 0 ../ikwiad -TA -DS -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    true, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    false, // single_pass (-sp),
                                    false); // artificial_livelock (-lv)
          stats("-TA -DS", t);
          stats("-TA -DS -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -DS -lv -ks "$1"
          // run 0 ../ikwiad -TA -DS -lv -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    true, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    false, // single_pass (-sp),
                                    true); // artificial_livelock (-lv)
          stats("-TA -DS -lv", t);
          stats("-TA -DS -lv -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -DS -sp -ks "$1"
          // run 0 ../ikwiad -TA -DS -sp -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    true, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    true, // single_pass (-sp),
                                    false); // artificial_livelock (-lv)
          stats("-TA -DS -sp", t);
          stats("-TA -DS -sp -RT", minimize_ta(t));
        }

        {
          // run 0 ../ikwiad -TA -DS -lv -sp -ks "$1"
          // run 0 ../ikwiad -TA -DS -lv -sp -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    true, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    true, // single_pass (-sp),
                                    true); // artificial_livelock (-lv)
          stats("-TA -DS -lv -sp", t);
          stats("-TA -DS -lv -sp -RT", minimize_ta(t));
        }
      }
      // Some cases with -x -R3 -DS -in
      {
        auto a = spot::degeneralize(scc_filter(ltl_to_tgba_fm(f, d, true)));
        bdd ap_set = atomic_prop_collect_as_bdd(f, a);

        {
          // run 0 ../ikwiad -x -R3 -DS -TA -in -ks "$1"
          // run 0 ../ikwiad -x -R3 -DS -TA -in -RT -ks "$1"
          auto t = spot::tgba_to_ta(a, ap_set,
                                    true, // degen (-DS)
                                    false, // artificial_initial_state (-in)
                                    false, // single_pass (-sp),
                                    true); // artificial_livelock (-lv)
          stats("-x -TA -DS -in", t);
          stats("-x -TA -DS -in -RT", minimize_ta(t));
        }

      }
    }

  assert(spot::fnode::instances_check());
  return 0;
}
