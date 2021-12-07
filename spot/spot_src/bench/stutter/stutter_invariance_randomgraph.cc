// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015, 2017 Laboratoire de Recherche et
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

#include <spot/misc/timer.hh>
#include <spot/tl/apcollect.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/randomgraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/stutter.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twa/bdddict.hh>
#include <spot/misc/random.hh>
#include <cstdio>
#include <cstring>
#include <vector>

constexpr unsigned algo_max = 8;

int
main(int argc, char** argv)
{
  if (argc != 3)
    {
      std::cerr << "usage: " << argv[0] << " density n_ap\n";
      exit(2);
    }
  float d = strtof(argv[1], nullptr);
  if (d < 0.0 || d > 1.0)
    {
      std::cerr << "density should be between 0 and 1";
      exit(2);
    }
  unsigned props_n = strtoul(argv[2], nullptr, 10);


  spot::bdd_dict_ptr dict = spot::make_bdd_dict();
  constexpr unsigned n = 10;

  // random ap set
  auto ap = spot::create_atomic_prop_set(props_n);
  // ap set as bdd
  bdd apdict = bddtrue;
  for (auto& i: ap)
    apdict &= bdd_ithvar(dict->register_proposition(i, &ap));

  std::vector<unsigned char> disable_algo(algo_max);
  unsigned algo_count = algo_max;
  unsigned seed = 0;

  spot::stat_printer stats(std::cout, ",%s,%t,%e,%S,%n");

  for (unsigned states_n = 1; states_n <= 50; ++states_n)
    {
      // generate n random automata
      for (unsigned i = 0; i < n; ++i)
	{
	  spot::twa_graph_ptr a;
	  do
	    {
	      spot::srand(++seed);
	      a = spot::random_graph(states_n, d, &ap, dict, 2, 0.1, 0.5, true);
	    }
	  while (a->is_empty());
	  auto na = spot::remove_fin(spot::dualize(a));

	  std::cout << d << ',' << props_n << ',' << seed;
	  stats.print(a);
	  stats.print(na);

	  bool prev = true;
	  for (int algo = 1; algo <= 8; ++algo)
	    {
	      std::cout << ',';
	      if (disable_algo[algo - 1])
		continue;

	      auto dup_a = spot::make_twa_graph(a, spot::twa::prop_set::all());
	      auto dup_na = spot::make_twa_graph(na,
						 spot::twa::prop_set::all());

	      spot::stopwatch sw;
	      sw.start();
	      bool res = spot::is_stutter_invariant(std::move(dup_a),
						    std::move(dup_na),
						    algo);
	      auto time = sw.stop();
	      std::cout << time;
	      if (algo > 1 && res != prev)
		{
		  std::cerr << "\nerror: algorithms " << algo - 1
			    << " (" << prev << ") and " << algo << " ("
			    << res << ") disagree on seed "
			    << seed << "\n";
		  exit(2);
		}
	      if (time >= 30.0)
		{
		  disable_algo[algo - 1] = 1;
		  --algo_count;
		}
	      prev = res;
	    }
	  std::cout << ',' << prev << '\n';;
	  if (algo_count == 0)
	    break;
	}
      if (algo_count == 0)
	break;
    }
  dict->unregister_all_my_variables(&ap);
  return 0;
}
