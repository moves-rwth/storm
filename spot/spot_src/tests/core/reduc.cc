// -*- coding: utf-8 -*_
// Copyright (C) 2008-2012, 2014-2016, 2018-2019 Laboratoire
// de Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2006, 2007 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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
#include <cassert>
#include <cstdlib>
#include <string>
#include <cstring>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/length.hh>

static void
syntax(char* prog)
{
  std::cerr << prog << " option formula1 (formula2)?\n";
  exit(2);
}

int
main(int argc, char** argv)
{
  bool readfile = false;
  bool hidereduc = false;
  unsigned long sum_before = 0;
  unsigned long sum_after = 0;
  spot::tl_simplifier_options o(false, false, false, false, false);

  if (argc < 3)
    syntax(argv[0]);

  if (!strncmp(argv[1], "-f", 3))
    {
      readfile = true;
      ++argv;
      --argc;
    }

  if (!strncmp(argv[1], "-h", 3))
    {
      hidereduc = true;
      ++argv;
      --argc;
    }

  switch (atoi(argv[1]))
    {
    case 0:
      o.reduce_basics = true;
      break;
    case 1:
      o.synt_impl = true;
      break;
    case 2:
      o.event_univ = true;
      break;
    case 3:
      o.reduce_basics = true;
      o.synt_impl = true;
      o.event_univ = true;
      break;
    case 4:
      o.reduce_basics = true;
      o.synt_impl = true;
      break;
    case 5:
      o.reduce_basics = true;
      o.event_univ = true;
      break;
    case 6:
      o.synt_impl = true;
      o.event_univ = true;
      break;
    case 7:
      o.containment_checks = true;
      break;
    case 8:
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 9:
      o.reduce_basics = true;
      o.synt_impl = true;
      o.event_univ = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 10:
      o.reduce_basics = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 11:
      o.synt_impl = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 12:
      o.reduce_basics = true;
      o.synt_impl = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 13:
      o.event_univ = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 14:
      o.reduce_basics = true;
      o.event_univ = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      break;
    case 15:
      o.reduce_basics = true;
      o.event_univ = true;
      o.containment_checks = true;
      o.containment_checks_stronger = true;
      o.favor_event_univ = true;
      break;
    default:
      return 2;
  }

  int exit_code = 0;

  {
    spot::tl_simplifier* simp = new spot::tl_simplifier(o);
    o.reduce_size_strictly = true;
    spot::tl_simplifier* simp_size = new spot::tl_simplifier(o);

    spot::formula f1 = nullptr;
    spot::formula f2 = nullptr;

    std::ifstream* fin = nullptr;

    if (readfile)
      {
        fin = new std::ifstream(argv[2]);
        if (!*fin)
          {
            std::cerr << "Cannot open " << argv[2] << '\n';
            exit(2);
          }
      }

  next_line:

    if (fin)
      {
        std::string input;
        do
          {
            if (!std::getline(*fin, input))
              goto end;
          }
        while (input == "");

        auto pf1 = spot::parse_infix_psl(input);
        if (pf1.format_errors(std::cerr))
          return 2;
        f1 = pf1.f;
      }
    else
      {
        auto pf1 = spot::parse_infix_psl(argv[2]);
        if (pf1.format_errors(std::cerr))
          return 2;
        f1 = pf1.f;
      }

    if (argc == 4)
      {
        if (readfile)
          {
            std::cerr << "Cannot read from file and check result.\n";
            exit(2);
          }

        auto pf2 = spot::parse_infix_psl(argv[3]);
        if (pf2.format_errors(std::cerr))
          return 2;
        f2 = pf2.f;
      }

    {
      spot::formula ftmp1;

      ftmp1 = f1;
      f1 = simp_size->negative_normal_form(f1, false);

      int length_f1_before = spot::length(f1);
      std::string f1s_before = spot::str_psl(f1);
      std::string f1l;

      spot::formula input_f = f1;
      f1 = simp_size->simplify(input_f);
      if (!simp_size->are_equivalent(input_f, f1))
        {
          std::cerr << "Incorrect reduction from `" << f1s_before
                    << "' to `";
          print_psl(std::cerr, f1) << "'.\n";
          exit_code = 3;
        }
      else
        {
          spot::formula maybe_larger = simp->simplify(input_f);
          f1l = spot::str_psl(maybe_larger);
          if (!simp->are_equivalent(input_f, maybe_larger))
            {
              std::cerr << "Incorrect reduction (reduce_size_strictly=0) from `"
                        << f1s_before << "' to `" << f1l << "'.\n";
              exit_code = 3;
            }
        }

      int length_f1_after = spot::length(f1);
      std::string f1s_after = spot::str_psl(f1);

      std::string f2s = "";
      if (f2)
        {
          ftmp1 = f2;
          f2 = simp_size->negative_normal_form(f2, false);
          f2s = spot::str_psl(f2);
        }

      sum_before += length_f1_before;
      sum_after += length_f1_after;

      // If -h is set, we want to print only formulae that have become larger.
      if (!f2 && (!hidereduc || (length_f1_after > length_f1_before)))
        {
          std::cout << length_f1_before << ' ' << length_f1_after
                    << " '" << f1s_before << "' reduce to '"
                    << f1s_after << '\'';
          if (f1l != "" && f1l != f1s_after)
            std::cout << " or (w/o rss) to '" << f1l << '\'';
          std::cout << '\n';
        }

      if (f2)
        {
          if (f1 != f2)
            {
              if (length_f1_after < length_f1_before)
                std::cout << f1s_before << " ** " << f2s << " ** " << f1s_after
                          << " KOREDUC " << std::endl;
              else
                std::cout << f1s_before << " ** " << f2s << " ** " << f1s_after
                          << " KOIDEM " << std::endl;
              exit_code = 1;
            }
          else
            {
              if (f1s_before != f1s_after)
                std::cout << f1s_before << " ** " << f2s << " ** " << f1s_after
                          << " OKREDUC " << std::endl;
              else
                std::cout << f1s_before << " ** " << f2s << " ** " << f1s_after
                          << " OKIDEM" << std::endl;
              exit_code = 0;
            }
        }
      else
        {
          if (length_f1_after > length_f1_before)
            exit_code = 1;
        }

      if (fin)
        goto next_line;
    }
  end:

    delete simp_size;
    delete simp;

    if (fin)
      {
        float before = sum_before;
        float after = sum_after;
        std::cout << "gain: "
                  << (1 - (after / before)) * 100 << '%' << std::endl;
        delete fin;
      }
  }

  assert(spot::fnode::instances_check());
  return exit_code;
}
