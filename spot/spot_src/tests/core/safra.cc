// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2016, 2018 Laboratoire de Recherche et
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
#include <cstring>

#include <spot/tl/parse.hh> // spot::parse_infix_psl
#include <spot/tl/formula.hh> // spot::formula
#include <spot/parseaut/public.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/twaalgos/dot.hh> // print_dot
#include <spot/twaalgos/hoa.hh> // print_hoa
#include <spot/twaalgos/determinize.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/simulation.hh>


static void help()
{
  std::cerr <<
    "safra [OPTIONS]\n"
    "\t-f ltl_formula\tinput string is an ltl formulae\n"
    "\t--hoa file.hoa\tinput file has hoa format\n"
    "\t-p\tpretty print states\n"
    "\t-H\toutput hoa format\n"
    "\t-b\treduce result using bisimulation\n"
    "\t--scc_opt\tUse an SCC-based Safra\n"
    "\t--bisim_opt\tUse Simulation info to reduce macro-states size\n"
    "\t--stutter\tStutter-invarience optimisation\n";
  exit(1);
}

int main(int argc, char* argv[])
{
  bool scc_opt = false;
  bool use_bisim = false;
  bool sim = false;
  bool in_hoa = false;
  bool in_ltl = false;
  bool out_dot = true;
  bool out_hoa = false;
  bool pretty_print = false;
  bool complete = false;
  bool use_stutter = false;

  char* input = nullptr;
  if (argc <= 2)
    help();
  for (int i = 1; i < argc; ++i)
    {
      if (!strncmp(argv[i], "--hoa", 5))
        {
          in_hoa = true;
          if (i + 1 >= argc)
            help();
          input = argv[++i];
        }
      else if (!strncmp(argv[i], "-f", 2))
        {
          in_ltl = true;
          if (i + 1 >= argc)
            help();
          input = argv[++i];
        }
      else if (!strncmp(argv[i], "-H", 2))
        {
          out_dot = false;
          out_hoa = true;
        }
      else if (!strncmp(argv[i], "-p", 2))
        pretty_print = true;
      else if (!strncmp(argv[i], "-b", 2))
        sim = true;
      else if (!strncmp(argv[i], "-c", 2))
        complete = true;
      else if (!strncmp(argv[i], "--scc_opt", 9))
        scc_opt = true;
      else if (!strncmp(argv[i], "--bisim_opt", 10))
        use_bisim = true;
      else if (!strncmp(argv[i], "--stutter", 9))
        use_stutter = true;
      else
        {
          std::cerr << "Warning: " << argv[i] << " not used\n";
          return 1;
        }
    }

  if (!input)
    help();

  auto dict = spot::make_bdd_dict();
  spot::twa_graph_ptr res;
  if (in_ltl)
    {
      auto pf = spot::parse_infix_psl(input);
      if (pf.format_errors(std::cerr))
        return 2;
      spot::translator trans(dict);
      trans.set_pref(spot::postprocessor::Deterministic);
      auto tmp = trans.run(pf.f);
      res = spot::tgba_determinize(tmp, pretty_print, scc_opt,
                                   use_bisim, use_stutter);
    }
  else if (in_hoa)
    {
      auto aut = spot::parse_aut(input, dict);
      if (aut->format_errors(std::cerr))
        return 2;
      res = tgba_determinize(aut->aut, pretty_print, scc_opt,
                             use_bisim, use_stutter);
    }
  if (sim)
    res = simulation(res);
  else
    res->merge_edges();
  if (complete)
    spot::complete_here(res);

  if (out_hoa)
    {
      spot::print_hoa(std::cout, res, "t");
      std::cout << std::endl;
    }
  else if (out_dot)
    spot::print_dot(std::cout, res);
  else
    assert(false);
}
