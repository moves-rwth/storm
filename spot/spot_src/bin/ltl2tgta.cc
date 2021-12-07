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
#include <fstream>

#include <argp.h>
#include <unistd.h>
#include "error.h"

#include "common_setup.hh"
#include "common_r.hh"
#include "common_cout.hh"
#include "common_finput.hh"
#include "common_post.hh"

#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/tl/simplify.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twa/bddprint.hh>

#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/dot.hh>
#include <spot/taalgos/minimize.hh>
#include <spot/misc/optionmap.hh>

const char argp_program_doc[] ="\
Translate linear-time formulas (LTL/PSL) into Testing Automata.\n\n\
By default it outputs a transition-based generalized Testing Automaton \
the smallest Transition-based Generalized Büchi Automata, \
in GraphViz's format.  The input formula is assumed to be \
stuttering-insensitive.";

enum {
  OPT_GTA = 1,
  OPT_INIT,
  OPT_SPLV,
  OPT_SPNO,
  OPT_TA,
  OPT_TGTA,
};

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Automaton type:", 1 },
    { "tgta", OPT_TGTA, nullptr, 0,
      "Transition-based Generalized Testing Automaton (default)", 0 },
    { "ta", OPT_TA, nullptr, 0, "Testing Automaton", 0 },
    { "gta", OPT_GTA, nullptr, 0, "Generalized Testing Automaton", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Options for TA and GTA creation:", 3 },
    { "single-pass-lv", OPT_SPLV, nullptr, 0,
      "add an artificial livelock state to obtain a single-pass (G)TA", 0 },
    { "single-pass", OPT_SPNO, nullptr, 0,
      "create a single-pass (G)TA without artificial livelock state", 0 },
    { "multiple-init", OPT_INIT, nullptr, 0,
      "do not create the fake initial state", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output options:", 4 },
    { "utf8", '8', nullptr, 0, "enable UTF-8 characters in output", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { "extra-options", 'x', "OPTS", 0,
      "fine-tuning options (see spot-x (7))", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &finput_argp, 0, nullptr, 1 },
    { &post_argp_nooutput, 0, nullptr, 20 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

enum ta_types { TGTA, GTA, TA };
ta_types ta_type = TGTA;

bool utf8 = false;
const char* stats = "";
spot::option_map extra_options;
bool opt_with_artificial_initial_state = true;
bool opt_single_pass_emptiness_check = false;
bool opt_with_artificial_livelock = false;

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case '8':
      spot::enable_utf8();
      break;
    case 'x':
      {
        const char* opt = extra_options.parse_options(arg);
        if (opt)
          error(2, 0, "failed to parse --options near '%s'", opt);
      }
      break;
    case OPT_TGTA:
      ta_type = TGTA;
      type = spot::postprocessor::TGBA;
      break;
    case OPT_GTA:
      ta_type = GTA;
      type = spot::postprocessor::TGBA;
      break;
    case OPT_TA:
      ta_type = TA;
      type = spot::postprocessor::BA;
      break;
    case OPT_INIT:
      opt_with_artificial_initial_state = false;
      break;
    case OPT_SPLV:
      opt_with_artificial_livelock = true;
      break;
    case OPT_SPNO:
      opt_single_pass_emptiness_check = true;
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

    trans_processor(spot::translator& trans)
      : trans(trans)
    {
    }

    int
    process_formula(spot::formula f,
                    const char* filename = nullptr, int linenum = 0) override
    {
      auto aut = trans.run(&f);

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

      bdd ap_set = atomic_prop_collect_as_bdd(f, aut);

      if (ta_type != TGTA)
        {
          auto testing_automaton =
            tgba_to_ta(aut, ap_set, type == spot::postprocessor::BA,
                       opt_with_artificial_initial_state,
                       opt_single_pass_emptiness_check,
                       opt_with_artificial_livelock);
          if (level != spot::postprocessor::Low)
            testing_automaton = spot::minimize_ta(testing_automaton);
          spot::print_dot(std::cout, testing_automaton);
        }
      else
        {
          auto tgta = tgba_to_tgta(aut, ap_set);
          if (level != spot::postprocessor::Low)
            tgta = spot::minimize_tgta(tgta);
          spot::print_dot(std::cout, tgta->get_ta());
        }
      // If we keep simplification caches around, atomic propositions
      // will still be defined, and one translation may influence the
      // variable order of the next one.
      trans.clear_caches();
      flush_cout();
      return 0;
    }
  };
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, "[FORMULA...]",
                        argp_program_doc, children, nullptr, nullptr };

      simplification_level = 3;

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      check_no_formula();

      spot::translator trans(&extra_options);
      trans.set_pref(pref | comp | sbacc);
      trans.set_type(type);
      trans.set_level(level);

      trans_processor processor(trans);
      if (processor.run())
        return 2;
      // Diagnose unused -x options
      extra_options.report_unused_options();
      return 0;
    });
}
