// -*- coding: utf-8 -*-
// Copyright (C) 2017-2020 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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
#include <cstdlib>
#include <cstdio>
#include <argp.h>
#include <unistd.h>
#include <cmath>
#include <sys/wait.h>
#include <iomanip>
#include "error.h"
#include "argmatch.h"

#include "common_setup.hh"
#include "common_hoaread.hh"
#include "common_finput.hh"
#include "common_color.hh"
#include "common_trans.hh"
#include "common_cout.hh"
#include "common_aoutput.hh"
#include "common_post.hh"

#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/misc/escape.hh>
#include <spot/misc/timer.hh>

const char argp_program_doc[] ="\
Call several tools that process automata and cross-compare their output \
to detect bugs, or to gather statistics.  The list of automata to use \
should be supplied on standard input, or using the -F option.\v\
Exit status:\n\
  0  everything went fine (without --fail-on-timeout, timeouts are OK)\n\
  1  some tools failed to output something we understand, or failed\n\
     sanity checks (statistics were output nonetheless)\n\
  2  autcross aborted on error\n\
";

enum {
  OPT_BOGUS = 256,
  OPT_CSV,
  OPT_HIGH,
  OPT_FAIL_ON_TIMEOUT,
  OPT_IGNORE_EXEC_FAIL,
  OPT_LANG,
  OPT_LOW,
  OPT_MEDIUM,
  OPT_NOCHECKS,
  OPT_OMIT,
  OPT_STOP_ERR,
  OPT_VERBOSE,
};

static const argp_option options[] =
  {
    { nullptr, 0, nullptr, 0, "Input:", 1 },
    { "file", 'F', "FILENAME", 0,
      "process automata from FILENAME", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "autcross behavior:", 5 },
    { "stop-on-error", OPT_STOP_ERR, nullptr, 0,
      "stop on first execution error or failure to pass"
      " sanity checks (timeouts are OK)", 0 },
    { "ignore-execution-failures", OPT_IGNORE_EXEC_FAIL, nullptr, 0,
      "ignore automata from translators that return with a non-zero exit code,"
      " but do not flag this as an error", 0 },
    { "fail-on-timeout", OPT_FAIL_ON_TIMEOUT, nullptr, 0,
      "consider timeouts as errors", 0 },
    { "language-preserved", OPT_LANG, nullptr, 0,
      "expect that each tool preserves the input language", 0 },
    { "no-checks", OPT_NOCHECKS, nullptr, 0,
      "do not perform any sanity checks", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Statistics output:", 7 },
    { "csv", OPT_CSV, "[>>]FILENAME", OPTION_ARG_OPTIONAL,
      "output statistics as CSV in FILENAME or on standard output "
      "(if '>>' is used to request append mode, the header line is "
      "not output)", 0 },
    { "omit-missing", OPT_OMIT, nullptr, 0,
      "do not output statistics for timeouts or failed translations", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification level (for complementation):",
      21 },
    { "low", OPT_LOW, nullptr, 0, "minimal optimizations (fast)", 0 },
    { "medium", OPT_MEDIUM, nullptr, 0, "moderate optimizations", 0 },
    { "high", OPT_HIGH, nullptr, 0,
      "all available optimizations (slow, default)", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output options:", -15 },
    { "quiet", 'q', nullptr, 0,
      "suppress all normal output in absence of errors", 0 },
    { "save-bogus", OPT_BOGUS, "[>>]FILENAME", 0,
      "save automata for which problems were detected in FILENAME", 0 },
    { "verbose", OPT_VERBOSE, nullptr, 0,
      "print what is being done, for debugging", 0 },
    { nullptr, 0, nullptr, 0,
      "If an output FILENAME is prefixed with '>>', it is open "
      "in append mode instead of being truncated.", -14 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &autproc_argp, 0, nullptr, 0 },
    { &hoaread_argp, 0, "Parsing of automata:", 4 },
    { &misc_argp, 0, nullptr, -1 },
    { &color_argp, 0, nullptr, 0 },
    { nullptr, 0, nullptr, 0 }
  };

static bool verbose = false;
static bool quiet = false;
static bool ignore_exec_fail = false;
static unsigned ignored_exec_fail = 0;
static bool fail_on_timeout = false;
static bool stop_on_error = false;
static bool no_checks = false;
static bool opt_language_preserved = false;
static bool opt_omit = false;
static const char* csv_output = nullptr;
static unsigned round_num = 0;
static const char* bogus_output_filename = nullptr;
static output_file* bogus_output = nullptr;

static int
parse_opt(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  switch (key)
    {
    case 'F':
      jobs.emplace_back(arg, true);
      break;
    case 'q':
      quiet = true;
      verbose = false;
      break;
    case OPT_BOGUS:
      {
        bogus_output = new output_file(arg);
        bogus_output_filename = arg;
        break;
      }
    case OPT_CSV:
      csv_output = arg ? arg : "-";
      break;
    case OPT_FAIL_ON_TIMEOUT:
      fail_on_timeout = true;
      break;
    case OPT_HIGH:
      level = spot::postprocessor::High;
      level_set = true;
      break;
    case OPT_IGNORE_EXEC_FAIL:
      ignore_exec_fail = true;
      break;
    case OPT_LANG:
      opt_language_preserved = true;
      break;
    case OPT_LOW:
      level = spot::postprocessor::Low;
      level_set = true;
      break;
    case OPT_MEDIUM:
      level = spot::postprocessor::Medium;
      level_set = true;
      break;
    case OPT_NOCHECKS:
      no_checks = true;
      break;
    case OPT_OMIT:
      opt_omit = true;
      break;
    case OPT_STOP_ERR:
      stop_on_error = true;
      break;
    case OPT_VERBOSE:
      verbose = true;
      quiet = false;
      break;
    case ARGP_KEY_ARG:
      if (arg[0] == '-' && !arg[1])
        jobs.emplace_back(arg, true);
      else
        tools_push_autproc(arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

static bool global_error_flag = false;
// static unsigned oom_count = 0U;

static std::ostream&
global_error()
{
  global_error_flag = true;
  std::cerr << bright_red;
  return std::cerr;
}

static void
end_error()
{
  std::cerr << reset_color;
}

static std::ostream&
example()
{
  std::cerr << bold_std;
  return std::cerr;
}


struct aut_statistics
{
  unsigned ap;
  unsigned states;
  unsigned edges;
  unsigned transitions;
  unsigned acc_sets;
  unsigned scc;
  unsigned nondetstates;
  bool nondeterministic;
  bool alternating;

  aut_statistics()
  {
  }

  void set(const spot::const_twa_graph_ptr& aut)
  {
    if (!csv_output)
      // Do not waste time.
      return;
    ap = aut->ap().size();
    spot::twa_sub_statistics s = sub_stats_reachable(aut);
    states = s.states;
    edges = s.edges;
    transitions = s.transitions;
    spot::scc_info m(aut);
    acc_sets = aut->num_sets();
    unsigned c = m.scc_count();
    scc = c;
    nondetstates = spot::count_nondet_states(aut);
    nondeterministic = nondetstates != 0;
    alternating = !aut->is_existential();
  }

  void to_csv(std::ostream& os) const
  {
    os << ap << ','
       << states << ','
       << edges << ','
       << transitions << ','
       << acc_sets << ','
       << scc << ','
       << nondetstates << ','
       << nondeterministic << ','
       << alternating;
  }

  void empty(std::ostream& os) const
  {
    os << ",,,,,,,,";
  }

  static void fields(std::ostream& os, const char* prefix)
  {
    os << '"'
       << prefix << "ap\",\""
       << prefix << "states\",\""
       << prefix << "edges\",\""
       << prefix << "transitions\",\""
       << prefix << "acc_sets\",\""
       << prefix << "scc\",\""
       << prefix << "nondetstates\",\""
       << prefix << "nondeterministic\",\""
       << prefix << "alternating\"";
  }
};

struct in_statistics
{
  std::string input_source;
  std::string input_name;
  aut_statistics input;

  static void fields(std::ostream& os)
  {
    os << "\"input.source\",\"input.name\",";
    aut_statistics::fields(os, "input.");
  }

  void to_csv(std::ostream& os) const
  {
    spot::escape_rfc4180(os << '"', input_source) << "\",";
    if (!input_name.empty())
      spot::escape_rfc4180(os << '"', input_name) << "\",";
    else
      os << ',';
    input.to_csv(os);
  }
};

struct out_statistics
{

  // If OK is false, output statistics are not available.
  bool ok;
  const char* status_str;
  int status_code;
  double time;
  aut_statistics output;

  out_statistics()
    : ok(false),
      status_str(nullptr),
      status_code(0),
      time(0)
  {
  }

  static void fields(std::ostream& os)
  {
    os << "\"exit_status\",\"exit_code\",\"time\",";
    aut_statistics::fields(os, "output.");
  }

  void to_csv(std::ostream& os) const
  {
    os << '"' << status_str << "\"," << status_code << ','
       << time << ',';
    if (ok)
      output.to_csv(os);
    else
      output.empty(os);
  }
};

std::vector<in_statistics> input_statistics;
typedef std::vector<out_statistics> vector_tool_statistics;
std::vector<vector_tool_statistics> output_statistics;

namespace
{
  class autcross_runner final: public autproc_runner
  {
    spot::bdd_dict_ptr dict;
  public:
    autcross_runner(spot::bdd_dict_ptr dict)
      : dict(dict)
    {
    }

    spot::twa_graph_ptr
    run_tool(unsigned int tool_num, char l, bool& problem,
             out_statistics& stats)
    {
      output.reset(tool_num);

      std::ostringstream command;
      format(command, tools[tool_num].cmd);

      std::string cmd = command.str();
      auto disp_cmd = [&]() {
                        std::cerr << "Running [" << l << tool_num
                                  << "]: " << cmd << '\n';
                      };
      if (!quiet)
        disp_cmd();
      spot::process_timer timer;
      timer.start();
      int es = exec_with_timeout(cmd.c_str());
      timer.stop();
      const char* status_str = nullptr;

      spot::twa_graph_ptr res = nullptr;
      if (timed_out)
        {
          if (fail_on_timeout)
            {
              if (quiet)
                disp_cmd();
              global_error() << "error: timeout during execution of command\n";
              end_error();
            }
          else
            {
              if (!quiet)
                std::cerr << "warning: timeout during execution of command\n";
              ++timeout_count;
            }
          status_str = "timeout";
          problem = fail_on_timeout;
          es = -1;
        }
      else if (WIFSIGNALED(es))
        {
          if (quiet)
            disp_cmd();
          status_str = "signal";
          problem = true;
          es = WTERMSIG(es);
          global_error() << "error: execution terminated by signal "
                         << es << ".\n";
          end_error();
        }
      else if (WIFEXITED(es) && WEXITSTATUS(es) != 0)
        {
          es = WEXITSTATUS(es);
          status_str = "exit code";
          if (!ignore_exec_fail)
            {
              if (quiet)
                disp_cmd();
              problem = true;
              global_error() << "error: execution returned exit code "
                             << es << ".\n";
              end_error();
            }
          else
            {
              problem = false;
              if (!quiet)
                std::cerr << "warning: execution returned exit code "
                          << es << ".\n";
              ++ignored_exec_fail;
            }
        }
      else
        {
          status_str = "ok";
          problem = false;
          es = 0;

          auto aut = spot::parse_aut(output.val()->name(), dict,
                                     spot::default_environment::instance(),
                                     opt_parse);
          if (!aut->errors.empty())
            {
              if (quiet)
                disp_cmd();
              status_str = "parse error";
              problem = true;
              es = -1;
              std::ostream& err = global_error();
              err << "error: failed to parse the produced automaton.\n";
              aut->format_errors(err);
              end_error();
              res = nullptr;
            }
          else if (aut->aborted)
            {
              if (quiet)
                disp_cmd();
              status_str = "aborted";
              problem = true;
              es = -1;
              global_error()  << "error: aborted HOA file.\n";
              end_error();
              res = nullptr;
            }
          else
            {
              res = aut->aut;
            }
        }
      output.cleanup();

      stats.status_str = status_str;
      stats.status_code = es;
      stats.time = timer.walltime();
      if (res)
        {
          stats.ok = true;
          stats.output.set(res);
        }
      return res;
    }
  };

  static std::string
  autname(unsigned i, bool complemented = false)
  {
    std::ostringstream str;
    if (complemented)
      str << "Comp(";
    if (i < tools.size())
      str << 'A' << i;
    else
      str << "input";
    if (complemented)
      str << ')';
    return str.str();
  }

  static bool
  check_empty_prod(const spot::const_twa_graph_ptr& aut_i,
                   const spot::const_twa_graph_ptr& aut_j,
                   size_t i, size_t j)
  {
    if (aut_i->num_sets() + aut_j->num_sets() >
        spot::acc_cond::mark_t::max_accsets())
      {
        if (!quiet)
          std::cerr << "info: building " << autname(i)
                    << '*' << autname(j, true)
                    << " requires more acceptance sets than supported\n";
        return false;
      }

    if (verbose)
      std::cerr << "info: check_empty "
                << autname(i) << '*' << autname(j, true) << '\n';

    auto w = aut_i->intersecting_word(aut_j);
    if (w)
      {
        std::ostream& err = global_error();
        err << "error: " << autname(i) << '*' << autname(j, true)
            << (" is nonempty; both automata accept the infinite word:\n"
                "       ");
        example() << *w << '\n';
        end_error();
      }
    return !!w;
  }

  class autcross_processor final: public hoa_processor
  {
    autcross_runner runner;
  public:
    autcross_processor()
      : hoa_processor(spot::make_bdd_dict(), true), runner(dict_)
    {
    }

    int
    process_automaton(const spot::const_parsed_aut_ptr& haut) override
    {
      auto printsize = [](const spot::const_twa_graph_ptr& aut,
                          bool props)
        {
          std::cerr << '(' << aut->num_states() << " st.,"
          << aut->num_edges() << " ed.,"
          << aut->num_sets() << " sets)";
          if (props)
            {
              if (!aut->is_existential())
                std::cerr << " univ-edges";
              if (is_deterministic(aut))
                std::cerr << " deterministic";
              if (is_complete(aut))
                std::cerr << " complete";
              std::cerr << '\n';
            }
        };

      spot::twa_graph_ptr input = haut->aut;
      runner.round_automaton(input, round_num);

      std::string source = [&]()
        {
          std::ostringstream src;
          src << haut->filename << ':' << haut->loc;
          return src.str();
        }();

      input_statistics.push_back(in_statistics());

      input_statistics[round_num].input_source = std::move(source);
      if (auto name = input->get_named_prop<std::string>("automaton-name"))
        input_statistics[round_num].input_name = *name;

      auto disp_src = [&]() {
                        std::cerr << bold
                                  << input_statistics[round_num].input_source
                                  << reset_color;
                        if (!input_statistics[round_num].input_name.empty())
                          std::cerr << '\t'
                                    << input_statistics[round_num].input_name;
                        std::cerr << '\n';
                      };
      if (!quiet)
        disp_src();
      input_statistics[round_num].input.set(input);

      int problems = 0;
      size_t m = tools.size();
      size_t mi = m + opt_language_preserved;
      std::vector<spot::twa_graph_ptr> pos(mi);
      std::vector<spot::twa_graph_ptr> neg(mi);
      vector_tool_statistics stats(m);

      if (opt_language_preserved)
        pos[mi - 1] = input;

      if (verbose)
        {
          std::cerr << "info: input\t";
          printsize(input, true);
        }

      for (size_t n = 0; n < m; ++n)
        {
          bool prob;
          pos[n] = runner.run_tool(n, 'A', prob, stats[n]);
          problems += prob;
        }
      spot::cleanup_tmpfiles();
      output_statistics.push_back(std::move(stats));

      if (verbose)
        {
          std::cerr << "info: collected automata:\n";
          for (unsigned i = 0; i < m; ++i)
            if (pos[i])
              {
                std::cerr << "info:   A" << i << '\t';
                printsize(pos[i], true);
              }
        }

      if (!no_checks)
        {
          if (!quiet)
            std::cerr
              << "Performing sanity checks and gathering statistics...\n";
          {
            bool print_first = true;
            for (unsigned i = 0; i < mi; ++i)
              {
                if (!pos[i])
                  continue;
                cleanup_acceptance_here(pos[i]);
                if (!pos[i]->is_existential())
                  {
                    if (verbose)
                      {
                        if (print_first)
                          {
                            std::cerr <<
                              "info: getting rid of universal edges...\n";
                            print_first = false;
                          }
                        std::cerr << "info:   "
                                  << std::setw(8) << autname(i) << '\t';
                        printsize(pos[i], false);
                        std::cerr << " -> ";
                      }
                    pos[i] = remove_alternation(pos[i]);
                    if (verbose)
                      {
                        printsize(pos[i], false);
                        std::cerr << '\n';
                      }
                  }
              }
          }

          {
            bool print_first = verbose;
            for (unsigned i = 0; i < mi; ++i)
              {
                if (pos[i])
                  {
                    if (print_first)
                      {
                        std::cerr << "info: complementing automata...\n";
                        print_first = false;
                      }
                    neg[i] = spot::complement(pos[i]);
                    if (verbose)
                      {
                        std::cerr << "info:   "
                                  << std::setw(8) << autname(i) << '\t';
                        printsize(pos[i], false);
                        std::cerr << " -> ";
                        printsize(neg[i], false);
                        std::cerr << '\t' << autname(i, true) << '\n';
                      }
                    cleanup_acceptance_here(pos[i]);
                  }
              };
          }

          // Just make a circular implication check
          // A0 <= A1, A1 <= A2, ..., AN <= A0
          unsigned ok = 0;
          for (size_t i = 0; i < mi; ++i)
            if (pos[i])
              {
                size_t j = ((i + 1) % mi);
                if (i != j && neg[j])
                  {
                    int res = check_empty_prod(pos[i], neg[j], i, j);
                    problems += res;
                    ok += !res;
                  }
              }
          // If the circular check failed, do the rest of all
          // mi(mi-1)/2 checks, as this will help diagnose the issue.
          if (ok != mi)
            for (size_t i = 0; i < mi; ++i)
              if (pos[i])
                {
                  size_t k = ((i + 1) % mi);
                  for (size_t j = 0; j < mi; ++j)
                  if (i != j && j != k && neg[j])
                    problems += check_empty_prod(pos[i], neg[j], i, j);
                }
        }
      else
        {
          if (!quiet)
            std::cerr << "Gathering statistics...\n";
        }


      if (problems && bogus_output)
        print_hoa(bogus_output->ostream(), input) << std::endl;

      if (problems && quiet)
        {
          std::cerr << "input automaton was ";
          disp_src();
        }
      if (!quiet || problems)
        std::cerr << '\n';

      ++round_num;
      // Shall we stop processing now?
      abort_run = global_error_flag && stop_on_error;
      return problems;
    }
  };
}

// Output an RFC4180-compatible CSV file.
static void
print_stats_csv(const char* filename)
{
  if (verbose)
    std::cerr << "info: writing CSV to " << filename << '\n';

  output_file outf(filename);
  std::ostream& out = outf.ostream();

  unsigned ntools = tools.size();
  assert(round_num == output_statistics.size());
  assert(round_num == input_statistics.size());

  if (!outf.append())
    {
      // Do not output the header line if we append to a file.
      // (Even if that file was empty initially.)
      in_statistics::fields(out);
      out << ",\"tool\",";
      out_statistics::fields(out);
      out << '\n';
    }
  for (unsigned r = 0; r < round_num; ++r)
    for (unsigned t = 0; t < ntools; ++t)
      if (!opt_omit || output_statistics[r][t].ok)
        {
          input_statistics[r].to_csv(out);
          out << ",\"";
          spot::escape_rfc4180(out, tools[t].name);
          out << "\",";
          output_statistics[r][t].to_csv(out);
          out << '\n';
        }
  outf.close(filename);
}

int
main(int argc, char** argv)
{
  return protected_main(argv, [&]() -> unsigned {
      const argp ap = { options, parse_opt, "[COMMANDFMT...]",
                        argp_program_doc, children, nullptr, nullptr };

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      auto s = tools.size();
      if (s == 0)
        error(2, 0, "No tool to run?  Run '%s --help' for usage.",
              program_name);

      check_no_automaton();

      if (s == 1 && !opt_language_preserved && !no_checks)
        error(2, 0, "Since --language-preserved is not used, you need "
              "at least two tools to compare.");


      setup_color();
      setup_sig_handler();

      autcross_processor p;
      p.run();

      if (round_num == 0)
        {
          error(2, 0, "no automaton to translate");
        }
      else if (!quiet)
        {
          if (global_error_flag)
            {
              std::ostream& err = global_error();
              if (bogus_output)
                err << ("error: some error was detected during the above "
                        "runs.\n       Check file ")
                    << bogus_output_filename
                    << " for problematic automata.\n";
              else
                err << ("error: some error was detected during the above "
                        "runs,\n       please search for 'error:' messages"
                        " in the above trace.\n");
              end_error();
            }
          else if (timeout_count == 0 && ignored_exec_fail == 0)
            {
              std::cerr << "No problem detected.\n";
            }
          else
            {
              std::cerr << "No major problem detected.\n";
            }

          unsigned additional_errors = 0U;
          additional_errors += timeout_count > 0;
          additional_errors += ignored_exec_fail > 0;
          if (additional_errors)
            {
              std::cerr << (global_error_flag ? "Additionally, " : "However, ");
              if (timeout_count)
                {
                  if (additional_errors > 1)
                    std::cerr << "\n  - ";
                  if (timeout_count == 1)
                    std::cerr << "1 timeout occurred";
                  else
                    std::cerr << timeout_count << " timeouts occurred";
                }

              if (ignored_exec_fail)
                {
                  if (additional_errors > 1)
                    std::cerr << "\n  - ";
                  if (ignored_exec_fail == 1)
                    std::cerr << "1 non-zero exit status was ignored";
                  else
                    std::cerr << ignored_exec_fail
                              << " non-zero exit statuses were ignored";
                }
              if (additional_errors == 1)
                std::cerr << '.';
              std::cerr << '\n';
            }
        }

      if (csv_output)
        print_stats_csv(csv_output);

      return global_error_flag;
    });
}
