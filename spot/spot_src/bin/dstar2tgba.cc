// -*- coding: utf-8 -*-
// Copyright (C) 2013-2019 Laboratoire de Recherche et Développement
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
#include <memory>
#include <sys/stat.h>
#include <unistd.h>

#include <argp.h>
#include "error.h"

#include "common_setup.hh"
#include "common_finput.hh"
#include "common_cout.hh"
#include "common_aoutput.hh"
#include "common_post.hh"
#include "common_file.hh"
#include "common_hoaread.hh"

#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/lbtt.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/neverclaim.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twa/bddprint.hh>
#include <spot/misc/optionmap.hh>
#include <spot/misc/timer.hh>
#include <spot/parseaut/public.hh>
#include <spot/twaalgos/sccinfo.hh>

static const char argp_program_doc[] ="\
Convert automata with any acceptance condition into variants of \
Büchi automata.\n\nThis reads automata into any supported format \
(HOA, LBTT, ltl2dstar, never claim) and outputs a \
Transition-based Generalized Büchi Automata in GraphViz's format by default.  \
Each supplied file may contain multiple automata.";

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Input:", 1 },
    { "file", 'F', "FILENAME", 0,
      "process the automaton in FILENAME", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { "extra-options", 'x', "OPTS", 0,
      "fine-tuning options (see spot-x (7))", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

static const struct argp_child children[] =
  {
    { &hoaread_argp, 0, nullptr, 0 },
    { &aoutput_argp, 0, nullptr, 0 },
    { &aoutput_io_format_argp, 0, nullptr, 4 },
    { &post_argp, 0, nullptr, 0 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

static spot::option_map extra_options;

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case 'F':
      jobs.emplace_back(arg, true);
      break;
    case 'x':
      {
        const char* opt = extra_options.parse_options(arg);
        if (opt)
          error(2, 0, "failed to parse --options near '%s'", opt);
      }
      break;
    case ARGP_KEY_ARG:
      jobs.emplace_back(arg, true);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


namespace
{
  class dstar_processor final: public hoa_processor
  {
  public:
    spot::postprocessor& post;
    automaton_printer printer;

    dstar_processor(spot::postprocessor& post)
      : hoa_processor(spot::make_bdd_dict()), post(post), printer(aut_input)
    {
    }

    int
    process_automaton(const spot::const_parsed_aut_ptr& haut) override
    {
      spot::process_timer timer;
      timer.start();
      auto aut = post.run(haut->aut, nullptr);
      timer.stop();
      printer.print(aut, timer, nullptr, haut->filename.c_str(), -1, haut);
      flush_cout();
      return 0;
    }
  };
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, "[FILENAME[/COL]...]",
                        argp_program_doc, children, nullptr, nullptr };

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      check_no_automaton();

      spot::postprocessor post(&extra_options);
      post.set_pref(pref | comp | sbacc | colored);
      post.set_type(type);
      post.set_level(level);

      dstar_processor processor(post);
      if (processor.run())
        return 2;

      // Diagnose unused -x options
      extra_options.report_unused_options();
      return 0;
    });
}
