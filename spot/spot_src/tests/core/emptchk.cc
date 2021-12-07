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
#include <spot/twaalgos/degen.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/emptiness.hh>

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
      std::cout << "========================================================\n";
      std::cout << line << ": " << s << '\n';
      if (s.empty() || s[0] == '#') // Skip comments
        continue;

      std::vector<std::string> tokens;
      {
        std::istringstream ss(s);
        std::string form;
        while (std::getline(ss, form, ','))
          {
            std::string tmp;
            while (form.size() > 0 && form.back() == '\\'
                   && std::getline(ss, tmp, ','))
              {
                form.back() = ',';
                form += tmp;
              }
            tokens.push_back(form);
          }
      }

      if (tokens.size() != 2)
        {
          std::cerr << "Expecting two tokens on input line.\n";
          exit(2);
        }

      int runs = atoi(tokens[0].c_str());

      auto pf = spot::parse_infix_psl(tokens[1]);
      if (pf.format_errors(std::cerr))
        return 2;
      auto f = pf.f;

      auto d = spot::make_bdd_dict();

      // Build many different automata from this formula.
      spot::const_twa_ptr aut[4];
      {
        auto a = spot::ltl_to_taa(f, d);
        aut[0] = a;
        auto all = spot::twa::prop_set::all();
        aut[1] = spot::degeneralize_tba(spot::make_twa_graph(a, all));
      }
      {
        auto a = spot::ltl_to_tgba_fm(f, d);
        aut[2] = a;
        aut[3] = spot::degeneralize(a);
      }

      const char* algos[] = {
        "Cou99", "Cou99(shy)",
        "CVWY90", "CVWY90(bsh=10M)", "CVWY90(repeated)",
        "SE05", "SE05(bsh=10M)", "SE05(repeated)",
        "Tau03_opt", "GV04",
      };

      for (auto& algo: algos)
        {
          const char* err;
          auto i = spot::make_emptiness_check_instantiator(algo, &err);
          if (!i)
            {
              std::cerr << "Failed to parse `" << err <<  "'\n";
              exit(2);
            }

          for (unsigned j = 0; j < sizeof(aut)/sizeof(*aut); ++j)
            {
              auto a = aut[j];
              std::cout << "** Testing aut[" << j << "] using " << algo << '\n';
              unsigned n_acc = a->acc().num_sets();
              unsigned n_max = i->max_sets();
              if (n_max < n_acc)
                {
                  std::cout << "Skipping because automaton has " << n_acc
                            << " acceptance sets, and " << algo
                            << " accepts at most " << n_max << ".\n";
                  continue;
                }
              unsigned n_min = i->min_sets();
              if (n_min > n_acc)
                {
                  std::cout << "Skipping because automaton has " << n_acc
                            << " acceptance sets, and " << algo
                            << " wants at least " << n_min << ".\n";
                  continue;
                }

              auto ec = i->instantiate(a);
              bool search_many = i->options().get("repeated");
              assert(ec);
              int ce_found = 0;
              do
                {
                  if (auto res = ec->check())
                    {
                      ++ce_found;
                      std::cout << ce_found << " counterexample found\n";
                      if (auto run = res->accepting_run())
                        {
                          spot::print_dot(std::cout, run->as_twa());
                        }
                      std::cout << '\n';
                      if (runs == 0)
                        {
                          std::cerr << "ERROR: Expected no counterexample.\n";
                          exit(1);
                        }
                    }
                  else
                    {
                      if (ce_found)
                        std::cout << "No more counterexample found.\n\n";
                      else
                        std::cout << "No counterexample found.\n\n";
                      break;
                    }
                }
              while (search_many);

              // The expected number of runs is only for TAA translations
              if (search_many && runs > ce_found && j < 2)
                {
                  std::cerr << "ERROR: only " << ce_found
                            << " counterexamples founds, expected at least "
                            << runs << '\n';
                  exit(1);
                }
              if (!search_many && ec->safe() && runs && !ce_found)
                {
                  std::cerr << "ERROR: expected a counterexample.\n";
                  exit(1);
                }
            }
        }
    }

  assert(spot::fnode::instances_check());
  return 0;
}
