// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015, 2016, 2017, 2018, 2019 Laboratoire de Recherche et
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


#include "common_sys.hh"
#include <argp.h>
#include "error.h"

#include "common_setup.hh"
#include "common_finput.hh"
#include "common_output.hh"
#include "common_conv.hh"
#include "common_cout.hh"

#include <spot/tl/mutation.hh>

enum {
  OPT_AP2CONST = 1,
  OPT_SIMPLIFY_BOUNDS,
  OPT_REMOVE_MULTOP_OPERANDS,
  OPT_REMOVE_OPS,
  OPT_SPLIT_OPS,
  OPT_REWRITE_OPS,
  OPT_REMOVE_ONE_AP,
  OPT_SORT,
};

static unsigned mutation_nb = 1;
static unsigned max_output = -1U;

static unsigned opt_all = spot::Mut_All;
static unsigned mut_opts = 0;
static bool opt_sort = false;

static const char * argp_program_doc =
  "List formulas that are similar to but simpler than a given formula.";

static const argp_option options[] = {
  { nullptr, 0, nullptr, 0,
    "Mutation rules (all enabled unless those options are used):", 15},
  { "ap-to-const", OPT_AP2CONST, nullptr, 0,
    "atomic propositions are replaced with true/false", 15 },
  { "remove-one-ap", OPT_REMOVE_ONE_AP, nullptr, 0,
    "all occurrences of an atomic proposition are replaced with another " \
    "atomic proposition used in the formula", 15 },
  { "remove-multop-operands", OPT_REMOVE_MULTOP_OPERANDS, nullptr, 0,
    "remove one operand from multops", 15 },
  { "remove-ops", OPT_REMOVE_OPS, nullptr, 0,
    "replace unary/binary operators with one of their operands", 15 },
  { "split-ops", OPT_SPLIT_OPS, nullptr, 0,
    "when an operator can be expressed as a conjunction/disjunction using "
    "simpler operators, each term of the conjunction/disjunction is a "
    "mutation. e.g. a <-> b can be written as ((a & b) | (!a & !b)) or as "
    "((a -> b) & (b -> a)) so those four terms can be a mutation of a <-> b",
    0 },
  { "rewrite-ops", OPT_REWRITE_OPS, nullptr, 0,
    "rewrite operators that have a semantically simpler form: a U b becomes "
    "a W b, etc.", 0 },
  { "simplify-bounds", OPT_SIMPLIFY_BOUNDS, nullptr, 0,
    "on a bounded unary operator, decrement one of the bounds, or set min to "
    "0 or max to unbounded", 15 },
    /**************************************************/
  { nullptr, 0, nullptr, 0, "Output options:", -20 },
  { "max-count", 'n', "NUM", 0, "maximum number of mutations to output", 0 },
  { "mutations", 'm', "NUM", 0, "number of mutations to apply to the "
    "formulae (default: 1)", 0 },
  { "sort", OPT_SORT, nullptr, 0, "sort the result by formula size", 0 },
  { nullptr, 0, nullptr, 0, "The FORMAT string passed to --format may use "
    "the following interpreted sequences:", -19 },
  { "%f", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "the formula (in the selected syntax)", 0 },
  { "%F", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "the name of the input file", 0 },
  { "%L", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "the original line number in the input file", 0 },
  { "%<", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "the part of the line before the formula if it "
    "comes from a column extracted from a CSV file", 0 },
  { "%>", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "the part of the line after the formula if it "
    "comes from a column extracted from a CSV file", 0 },
  { "%%", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
    "a single %", 0 },
  COMMON_LTL_OUTPUT_SPECS,
    /**************************************************/
  { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
  { nullptr, 0, nullptr, 0, nullptr, 0 }
};

static const argp_child children[] = {
  { &finput_argp, 0, nullptr, 10 },
  { &output_argp, 0, nullptr, 0 },
  { &misc_argp, 0, nullptr, 0 },
  { nullptr, 0, nullptr, 0 }
};

namespace
{
  class mutate_processor final: public job_processor
  {
  public:
    int
    process_formula(spot::formula f, const char* filename = nullptr,
                    int linenum = 0) override
    {
      auto mutations =
        spot::mutate(f, mut_opts, max_output, mutation_nb, opt_sort);
      for (auto g: mutations)
        output_formula_checked(g, nullptr, filename, linenum, prefix, suffix);
      return 0;
    }
  };
}

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  switch (key)
    {
    case 'm':
      mutation_nb = to_unsigned(arg, "-m/--mutations");
      break;
    case 'n':
      max_output = to_int(arg, "-n/--max-count");
      break;
    case ARGP_KEY_ARG:
      // FIXME: use stat() to distinguish filename from string?
      jobs.emplace_back(arg, true);
      break;
    case OPT_AP2CONST:
      opt_all = 0;
      mut_opts |= spot::Mut_Ap2Const;
      break;
    case OPT_REMOVE_ONE_AP:
      opt_all = 0;
      mut_opts |= spot::Mut_Remove_One_Ap;
      break;
    case OPT_REMOVE_MULTOP_OPERANDS:
      opt_all = 0;
      mut_opts |= spot::Mut_Remove_Multop_Operands;
      break;
    case OPT_REMOVE_OPS:
      opt_all = 0;
      mut_opts |= spot::Mut_Remove_Ops;
      break;
    case OPT_SPLIT_OPS:
      opt_all = 0;
      mut_opts |= spot::Mut_Split_Ops;
      break;
    case OPT_REWRITE_OPS:
      opt_all = 0;
      mut_opts |= spot::Mut_Rewrite_Ops;
      break;
    case OPT_SIMPLIFY_BOUNDS:
      opt_all = 0;
      mut_opts |= spot::Mut_Simplify_Bounds;
      break;
    case OPT_SORT:
      opt_sort = true;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

int
main(int argc, char* argv[])
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, "[FILENAME[/COL]...]",
                        argp_program_doc, children, nullptr, nullptr };

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      mut_opts |= opt_all;

      check_no_formula();

      mutate_processor processor;
      if (processor.run())
        return 2;
      flush_cout();
      return 0;
    });
}
