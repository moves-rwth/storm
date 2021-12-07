// -*- coding: utf-8 -*-
// Copyright (C) 2015-2020 Laboratoire de Recherche et Développement
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
#include <sstream>
#include <fstream>
#include <sys/wait.h>

#include "error.h"
#include "argmatch.h"

#include "common_setup.hh"
#include "common_cout.hh"
#include "common_conv.hh"
#include "common_finput.hh"
#include "common_aoutput.hh"
#include "common_output.hh"
#include "common_post.hh"
#include "common_trans.hh"
#include "common_hoaread.hh"

#include <spot/tl/relabel.hh>
#include <spot/tl/print.hh>
#include <spot/misc/bareword.hh>
#include <spot/misc/timer.hh>
#include <spot/twaalgos/lbtt.hh>
#include <spot/twaalgos/relabel.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/parseaut/public.hh>

const char argp_program_doc[] ="\
Run LTL/PSL formulas through another program, performing conversion\n\
of input and output as required.";

enum {
  OPT_ERRORS = 256,
  OPT_FAIL_ON_TIMEOUT,
  OPT_GREATEST,
  OPT_NEGATE,
  OPT_SMALLEST,
};

static const argp_option options[] =
  {
    { "negate", OPT_NEGATE, nullptr, 0, "negate each formula", 1 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Error handling:", 4 },
    { "errors", OPT_ERRORS, "abort|warn|ignore", 0,
      "how to deal with tools returning with non-zero exit codes or "
      "automata that ltldo cannot parse (default: abort)", 0 },
    { "fail-on-timeout", OPT_FAIL_ON_TIMEOUT, nullptr, 0,
      "consider timeouts as errors", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output selection:", 5 },
    { "smallest", OPT_SMALLEST, "FORMAT", OPTION_ARG_OPTIONAL,
      "for each formula select the smallest automaton given by all "
      "translators, using FORMAT for ordering (default is %s,%e)", 0 },
    { "greatest", OPT_GREATEST, "FORMAT", OPTION_ARG_OPTIONAL,
      "for each formula select the greatest automaton given by all "
      "translators, using FORMAT for ordering (default is %s,%e)", 0 },
    { "max-count", 'n', "NUM", 0, "output at most NUM automata", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };


static const argp_option more_o_format[] =
  {
    { "%#", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "serial number of the formula translated", 0 },
    { "%T", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "tool used for translation", 0 },
    { "%f", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "formula translated", 0 },
    { "%<", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line before the formula if it "
      "comes from a column extracted from a CSV file", 4 },
    { "%>", 0, nullptr, OPTION_DOC | OPTION_NO_USAGE,
      "the part of the line after the formula if it "
      "comes from a column extracted from a CSV file", 4 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

// This is not very elegant, but we need to add the above %-escape
// sequences to those of aoutput_o_fromat_argp for the --help output.
// So far I've failed to instruct argp to merge those two lists into a
// single block.
static const struct argp*
build_percent_list()
{
  const argp_option* iter = aoutput_o_format_argp.options;
  unsigned count = 0;
  while (iter->name || iter->doc)
    {
      ++count;
      ++iter;
    }

  unsigned s = count * sizeof(argp_option);
  argp_option* d =
    static_cast<argp_option*>(malloc(sizeof(more_o_format) + s));
  memcpy(d, aoutput_o_format_argp.options, s);
  memcpy(d + count, more_o_format, sizeof(more_o_format));

  static const struct argp more_o_format_argp =
    { d, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  return &more_o_format_argp;
}

enum errors_type { errors_abort, errors_warn, errors_ignore };
static errors_type errors_opt;
static bool fail_on_timeout = false;
static int best_type = 0;       // -1 smallest, 1 greatest, 0 no selection
static const char* best_format = "%s,%e";
static int opt_max_count = -1;
static bool negate = false;

static char const *const errors_args[] =
{
  "stop", "abort",
  "warn", "print",
  "ignore", "silent", nullptr
};

static errors_type const errors_types[] =
{
  errors_abort, errors_abort,
  errors_warn, errors_warn,
  errors_ignore, errors_ignore
};

ARGMATCH_VERIFY(errors_args, errors_types);

const struct argp_child children[] =
  {
    { &hoaread_argp, 0, "Parsing of automata:", 3 },
    { &finput_argp, 0, nullptr, 0 },
    { &trans_argp, 0, nullptr, 3 },
    { &aoutput_argp, 0, nullptr, 6 },
    { build_percent_list(), 0, nullptr, 7 },
    { &misc_argp, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  switch (key)
    {
    case OPT_ERRORS:
      errors_opt = XARGMATCH("--errors", arg, errors_args, errors_types);
      break;
    case OPT_FAIL_ON_TIMEOUT:
      fail_on_timeout = true;
      break;
    case OPT_GREATEST:
      best_type = 1;
      if (arg)
        best_format = arg;
      break;
    case OPT_NEGATE:
      negate = true;
      break;
    case OPT_SMALLEST:
      best_type = -1;
      if (arg)
        best_format = arg;
      break;
    case 'n':
      opt_max_count = to_pos_int(arg, "-n/--max-count");
      break;
    case ARGP_KEY_ARG:
      if (arg[0] == '-' && !arg[1])
        jobs.emplace_back(arg, true);
      else
        tools_push_trans(arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

namespace
{
  class xtranslator_runner final: public translator_runner
  {
  public:
    xtranslator_runner(spot::bdd_dict_ptr dict)
      : translator_runner(dict, true)
    {
    }

    spot::twa_graph_ptr
    translate(unsigned int translator_num, bool& problem,
              spot::process_timer& timer)
    {
      output.reset(translator_num);

      std::ostringstream command;
      format(command, tools[translator_num].cmd);

      std::string cmd = command.str();
      //std::cerr << "Running [" << l << translator_num << "]: "
      // << cmd << std::endl;
      timer.start();
      int es = exec_with_timeout(cmd.c_str());
      timer.stop();

      spot::twa_graph_ptr res = nullptr;
      problem = false;
      if (timed_out)
        {
          // A timeout is considered benign, unless --fail-on-timeout.
          if (fail_on_timeout)
            problem = true;
          else
            ++timeout_count;
          std::cerr << program_name
                    << ": timeout during execution of command \""
                    << cmd << "\"\n";
        }
      else if (WIFSIGNALED(es))
        {
          if (errors_opt != errors_ignore)
            {
              problem = true;
              es = WTERMSIG(es);
              std::cerr << program_name << ": execution of command \"" << cmd
                        << "\" terminated by signal " << es << ".\n";
            }
        }
      else if (WIFEXITED(es) && WEXITSTATUS(es) != 0)
        {
          if (errors_opt != errors_ignore)
            {
              problem = true;
              es = WEXITSTATUS(es);
              std::cerr << program_name << ": execution of command \"" << cmd
                        << "\" returned exit code " << es << ".\n";
            }
        }
      else if (output.val())
        {
          auto aut = spot::parse_aut(output.val()->name(), dict,
                                     spot::default_environment::instance(),
                                     opt_parse);
          if (!aut->errors.empty() && errors_opt != errors_ignore)
            {
              problem = true;
              std::cerr << program_name << ": failed to parse the automaton "
                "produced by \"" << cmd << "\".\n";
              aut->format_errors(std::cerr);
            }
          else if (aut->aborted && errors_opt != errors_ignore)
            {
              problem = true;
              std::cerr << program_name << ": command \"" << cmd
                        << "\" aborted its output.\n";
            }
          else
            {
              res = aut->aut;
            }
        }
      // Note that res can stay empty if no automaton was output.

      if (problem && errors_opt == errors_ignore)
        {
          problem = false;
          res = nullptr;
        }
      output.cleanup();
      return res;
    }
  };


  class processor final: public job_processor
  {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    xtranslator_runner runner;
    automaton_printer printer;
    hoa_stat_printer best_printer;
    std::ostringstream best_stream;
    spot::postprocessor& post;
    spot::printable_value<std::string> cmdname;
    spot::printable_value<unsigned> roundval;
    spot::printable_value<std::string> inputf;

  public:
    processor(spot::postprocessor& post)
      : runner(dict), best_printer(best_stream, best_format), post(post)
    {
      printer.add_stat('T', &cmdname);
      printer.add_stat('#', &roundval);
      printer.add_stat('f', &inputf);
      best_printer.declare('T', &cmdname);
      best_printer.declare('#', &roundval);
      best_printer.declare('f', &inputf);
    }

    ~processor()
    {
    }

    int
    process_string(const std::string& input,
                   const char* filename,
                   int linenum) override
    {
      spot::parsed_formula pf = parse_formula(input);

      if (!pf.f || !pf.errors.empty())
        {
          if (filename)
            error_at_line(0, 0, filename, linenum, "parse error:");
          pf.format_errors(std::cerr);
          return 1;
        }

      if (negate)
        {
          pf.f = spot::formula::Not(pf.f);
          std::ostringstream os;
          stream_formula(os, pf.f, filename, std::to_string(linenum).c_str());
          inputf = os.str();
        }
      else
        {
          inputf = input;
        }
      process_formula(pf.f, filename, linenum);
      return 0;
    }

    void output_aut(const spot::twa_graph_ptr& aut,
                    spot::process_timer& ptimer,
                    spot::formula f,
                    const char* filename, int loc,
                    const char* csv_prefix, const char* csv_suffix)
    {
      static long int output_count = 0;
      ++output_count;
      printer.print(aut, ptimer, f, filename, loc, nullptr,
                    csv_prefix, csv_suffix);
      if (opt_max_count >= 0 && output_count >= opt_max_count)
        abort_run = true;
    }

    int
    process_formula(spot::formula f,
                    const char* filename = nullptr, int linenum = 0) override
    {
      std::unique_ptr<spot::relabeling_map> relmap;

      // If atomic propositions are incompatible with one of the
      // output, relabel the formula.
      if (opt_relabel
          || (!f.has_lbt_atomic_props() &&
              (runner.has('l') || runner.has('L') || runner.has('T')))
          || (!f.has_spin_atomic_props() &&
              (runner.has('s') || runner.has('S'))))
        {
          relmap.reset(new spot::relabeling_map);
          f = spot::relabel(f, spot::Pnn, relmap.get());
        }

      static unsigned round = 1;
      runner.round_formula(f, round);

      unsigned ts = tools.size();
      spot::twa_graph_ptr best_aut = nullptr;
      std::string best_stats;
      std::string best_cmdname;
      spot::process_timer best_timer;

      roundval = round;
      for (unsigned t = 0; t < ts; ++t)
        {
          bool problem;
          spot::process_timer timer;
          auto aut = runner.translate(t, problem, timer);
          if (problem)
            {
              // An error message already occurred about the problem,
              // but this additional one will print filename &
              // linenum, and possibly exit.
              std::string sf = spot::str_psl(f);
              error_at_line(errors_opt == errors_abort ? 2 : 0, 0,
                            filename, linenum,
                            "failed to run `%s' on `%s'",
                            tools[t].name, sf.c_str());
            }
          if (aut)
            {
              if (relmap)
                relabel_here(aut, relmap.get());

              cmdname = tools[t].name;
              aut = post.run(aut, f);
              if (best_type)
                {
                  best_printer.print(nullptr, aut, f, filename, linenum, timer,
                                     prefix, suffix);
                  std::string aut_stats = best_stream.str();
                  if (!best_aut ||
                      (strverscmp(best_stats.c_str(), aut_stats.c_str())
                       * best_type) < 0)
                    {
                      best_aut = aut;
                      best_stats = aut_stats;
                      best_cmdname = tools[t].name;
                      best_timer = timer;
                    }
                  best_stream.str("");
                }
              else
                {
                  output_aut(aut, timer, f, filename, linenum,
                             prefix, suffix);
                  if (abort_run)
                    break;
                }
            }
        }
      if (best_aut)
        {
          cmdname = best_cmdname;
          output_aut(best_aut, best_timer, f, filename, linenum,
                     prefix, suffix);
        }

      spot::cleanup_tmpfiles();
      ++round;
      return 0;
    }
  };
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      const argp ap = { options, parse_opt, "[COMMANDFMT...]",
                        argp_program_doc, children, nullptr, nullptr };

      // Disable post-processing as much as possible by default.
      level = spot::postprocessor::Low;
      pref = spot::postprocessor::Any;
      type = spot::postprocessor::Generic;
      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      check_no_formula();

      if (tools.empty())
        error(2, 0, "No translator to run?  Run '%s --help' for usage.",
              program_name);

      setup_sig_handler();

      // Usually we print the formula as it was given to us, but
      // if --negate is used we have to reformat it.  We don't know
      // was the input format unless --lbt-input was given, and in
      // that case we keep it for output.
      output_format = lbt_input ? lbt_output : spot_output;

      spot::postprocessor post;
      post.set_pref(pref | comp | sbacc | colored);
      post.set_type(type);
      post.set_level(level);

      processor p(post);
      if (p.run())
        return 2;
      return 0;
    });
}
