// -*- coding: utf-8 -*-
// Copyright (C) 2012-2019 Laboratoire de Recherche et Développement
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

#include <argp.h>
#include "error.h"

#include "common_setup.hh"
#include "common_r.hh"
#include "common_cout.hh"
#include "common_finput.hh"
#include "common_output.hh"
#include "common_aoutput.hh"
#include "common_post.hh"

#include <spot/tl/formula.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/misc/optionmap.hh>
#include <spot/misc/timer.hh>

static const char argp_program_doc[] ="\
Translate linear-time formulas (LTL/PSL) into various types of automata.\n\n\
By default it will apply all available optimizations to output \
the smallest Transition-based Generalized Büchi Automata, \
output in the HOA format.\n\
If multiple formulas are supplied, several automata will be output.";

enum { OPT_NEGATE = 256 };

static const argp_option options[] =
  {
    /**************************************************/
    { "%f", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the formula, in Spot's syntax", 4 },
    { "%<", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line before the formula if it "
      "comes from a column extracted from a CSV file", 4 },
    { "%>", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line after the formula if it "
      "comes from a column extracted from a CSV file", 4 },
    /**************************************************/
    { "negate", OPT_NEGATE, nullptr, 0, "negate each formula", 1 },
    { "unambiguous", 'U', nullptr, 0, "output unambiguous automata", 2 },
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { "extra-options", 'x', "OPTS", 0,
      "fine-tuning options (see spot-x (7))", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &finput_argp, 0, nullptr, 0 },
    { &aoutput_argp, 0, nullptr, 0 },
    { &aoutput_o_format_argp, 0, nullptr, 0 },
    { &post_argp, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

static bool negate = false;
static spot::option_map extra_options;
static spot::postprocessor::output_pref unambig = 0;

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case 'U':
      unambig = spot::postprocessor::Unambiguous;
      break;
    case 'x':
      {
        const char* opt = extra_options.parse_options(arg);
        if (opt)
          error(2, 0, "failed to parse --options near '%s'", opt);
      }
      break;
    case OPT_NEGATE:
      negate = true;
      break;
    case ARGP_KEY_ARG:
      // FIXME: use stat() to distinguish filename from string?
      if (*arg == '-' && !arg[1])
        jobs.emplace_back(arg, true);
      else
        jobs.emplace_back(arg, false);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


namespace
{
  class trans_processor final: public job_processor
  {
  public:
    spot::translator& trans;
    automaton_printer printer;

    trans_processor(spot::translator& trans)
      : trans(trans), printer(ltl_input)
    {
    }

    int
    process_formula(spot::formula f,
                    const char* filename = nullptr, int linenum = 0) override
    {
      // This should not happen, because the parser we use can only
      // read PSL/LTL formula, but since our formula type can
      // represent more than PSL formula, let's make this
      // future-proof.
      if (!f.is_psl_formula())
        {
          std::string s = spot::str_psl(f);
          error_at_line(2, 0, filename, linenum,
                        "formula '%s' is not an LTL or PSL formula",
                        s.c_str());
        }

      if (negate)
        f = spot::formula::Not(f);

      spot::process_timer timer;
      timer.start();
      auto aut = trans.run(&f);
      timer.stop();

      printer.print(aut, timer, f, filename, linenum, nullptr,
                    prefix, suffix);
      // If we keep simplification caches around, atomic propositions
      // will still be defined, and one translation may influence the
      // variable order of the next one.
      trans.clear_caches();
      return 0;
    }
  };
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      // By default we name automata using the formula.
      opt_name = "%f";

      const argp ap = { options, parse_opt, "[FORMULA...]",
                        argp_program_doc, children, nullptr, nullptr };

      simplification_level = 3;

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      check_no_formula();

      spot::translator trans(&extra_options);
      trans.set_type(type);
      trans.set_pref(pref | comp | sbacc | unambig | colored);
      trans.set_level(level);

      trans_processor processor(trans);
      if (processor.run())
        return 2;

      // Diagnose unused -x options
      extra_options.report_unused_options();
      return 0;
    });
}
