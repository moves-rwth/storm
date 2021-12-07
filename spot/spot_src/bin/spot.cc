// -*- coding: utf-8 -*-
// Copyright (C) 2013-2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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

#include "common_sys.hh"
#include <string>
#include <iostream>
#include <cstdlib>
#include <argp.h>
#include "common_setup.hh"

const char argp_program_doc[] ="Command-line tools installed by Spot.";

#define DOC(NAME, TXT) NAME, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, TXT, 0

static const argp_option options[] =
  {
    { nullptr, 0, nullptr, 0, "Tools that output LTL/PSL formulas:", 0 },
    { DOC("randltl", "Generate random LTL or PSL formulas.") },
    { DOC("genltl", "Generate LTL formulas from scalable patterns.") },
    { DOC("ltlfilt", "Filter, convert, and transform LTL or PSL formulas.") },
    { DOC("ltlgrind", "Mutate LTL or PSL formulas to generate similar but "
          "simpler ones.  Use this when looking for shorter formula to "
          "reproduce a bug.") },
    { nullptr, 0, nullptr, 0, "Tools that output automata or circuits:", 0 },
    { DOC("randaut", "Generate random ω-automata.") },
    { DOC("genaut", "Generate ω-automata from scalable patterns.") },
    { DOC("ltl2tgba", "Convert LTL or PSL into variants of Transition-based "
          "Generalized Büchi Automata, and to other types of automata.") },
    { DOC("ltl2tgta", "Convert LTL or PSL into variants of Transition-based "
          "Generalized Testing Automata.") },
    { DOC("autfilt", "Filter, convert, and transform ω-automata.") },
    { DOC("dstar2tgba", "Convert ω-automata into variants of "
          "Transition-based Büchi automata.") },
    { DOC("ltlsynt",
          "Synthesize AIGER circuits from LTL/PSL specifications.") },
    { nullptr, 0, nullptr, 0, "Tools that run other tools:", 0 },
    { DOC("autcross", "Cross-compare tools processing ω-automata,"
          " watch for bugs, and generate statistics.") },
    { DOC("ltlcross", "Cross-compare translators of LTL or PSL formulas "
          "into ω-automata, watch for bugs, and generate statistics.") },
    { DOC("ltldo", "Wrap any tool that inputs LTL or PSL formulas and possibly "
          "outputs ω-automata; provides Spot's I/O interface.") },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &misc_argp_hidden, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

int
main(int argc, char** argv)
{
  setup(argv);

  const argp ap = { options, nullptr, nullptr, argp_program_doc, children,
                    nullptr, nullptr };

  if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
    exit(err);

  std::cerr << "This binary serves no purpose other than generating"
            << " the spot.7 manpage.\n";

  return 1;
}
