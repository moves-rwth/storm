// -*- coding: utf-8 -*-
// Copyright (C) 2008-2012, 2014-2016, 2018-2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/tl/unabbrev.hh>
#include <spot/tl/nenoform.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/print.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " [-E] file\n";
  exit(2);
}

int
main(int argc, char** argv)
{
  bool check_first = true;

  if (argc > 1 && !strcmp(argv[1], "-E"))
    {
      check_first = false;
      argv[1] = argv[0];
      ++argv;
      --argc;
    }
  if (argc != 2)
    syntax(argv[0]);
  std::ifstream input(argv[1]);
  if (!input)
    {
      std::cerr << "failed to open " << argv[1] << '\n';
      return 2;
    }

  std::string s;
  unsigned line = 0;
  while (std::getline(input, s))
    {
      ++line;
      std::cerr << line << ": " << s << '\n';
      if (s[0] == '#')                // Skip comments
        continue;
      std::vector<std::string> formulas;
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
            formulas.push_back(form);
          }
      }

      unsigned size = formulas.size();
      if (size == 0)                // Skip empty lines
        continue;

      if (size == 1)
        {
          std::cerr << "Not enough formulas on line " << line << '\n';
          return 2;
        }

      auto pf2 = spot::parse_infix_psl(formulas[size - 1]);

      if (pf2.format_errors(std::cerr))
        return 2;
      auto f2 = pf2.f;

      for (unsigned n = 0; n < size - 1; ++n)
        {

          auto pf1 = spot::parse_infix_psl(formulas[n]);

          if (check_first && pf1.format_errors(std::cerr))
            return 2;
          auto f1 = pf1.f;

          int exit_code = 0;

          {
#if defined UNABBREV || defined NENOFORM
            spot::formula tmp;
#endif
#ifdef UNABBREV
            tmp = f1;
            f1 = spot::unabbreviate(f1, UNABBREV);
            f1.dump(std::cout) << std::endl;
#endif
#ifdef NENOFORM
            tmp = f1;
            f1 = spot::negative_normal_form(f1);
            f1.dump(std::cout) << std::endl;
#endif
#ifdef REDUC
            spot::tl_simplifier_options opt(true, true, true,
                                                  false, false);
#  ifdef EVENT_UNIV
            opt.favor_event_univ = true;
#  endif
            spot::tl_simplifier simp(opt);
            {
              spot::formula tmp;
              tmp = f1;
              f1 = simp.simplify(f1);

              if (!simp.are_equivalent(f1, tmp))
                {
                  std::cerr
                    << "Source and simplified formulae are not equivalent!\n";
                  spot::print_psl(std::cerr << "Simplified: ", f1) << '\n';
                  exit_code = 1;
                }
            }
            f1.dump(std::cout) << std::endl;
#endif
#ifdef REDUC_TAU
            spot::tl_simplifier_options opt(false, false, false,
                                                  true, false);
            spot::tl_simplifier simp(opt);
            {
              spot::formula tmp;
              tmp = f1;
              f1 = simp.simplify(f1);

              if (!simp.are_equivalent(f1, tmp))
                {
                  std::cerr
                    << "Source and simplified formulae are not equivalent!\n";
                  spot::print_psl(std::cerr << "Simplified: ", f1) << '\n';
                  exit_code = 1;
                }
            }
            f1.dump(std::cout) << std::endl;
#endif
#ifdef REDUC_TAUSTR
            spot::tl_simplifier_options opt(false, false, false,
                                                  true, true);
            spot::tl_simplifier simp(opt);
            {
              spot::formula tmp;
              tmp = f1;
              f1 = simp.simplify(f1);

              if (!simp.are_equivalent(f1, tmp))
                {
                  std::cerr
                    << "Source and simplified formulae are not equivalent!\n";
                  spot::print_psl(std::cerr << "Simplified: ", f1) << '\n';
                  exit_code = 1;
                }
            }
            f1.dump(std::cout) << std::endl;
#endif

            exit_code |= f1 != f2;

#if (!defined(REDUC) && !defined(REDUC_TAU) && !defined(REDUC_TAUSTR))
            spot::tl_simplifier simp;
#endif

            if (!simp.are_equivalent(f1, f2))
              {
#if (!defined(REDUC) && !defined(REDUC_TAU) && !defined(REDUC_TAUSTR))
                std::cerr
                  << "Source and destination formulae are not equivalent!\n";
#else
                std::cerr
                  << "Simpl. and destination formulae are not equivalent!\n";
#endif
                exit_code = 1;
              }

#if NEGATE
            exit_code ^= 1;
#endif
            if (exit_code)
              return exit_code;
          }
        }
    }

  assert(spot::fnode::instances_check());
  return 0;
}
