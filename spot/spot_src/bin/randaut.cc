// -*- coding: utf-8 -*-
// Copyright (C) 2012-2016, 2018-2019 Laboratoire de Recherche et
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

#include <iostream>
#include <fstream>
#include <argp.h>
#include <cstdlib>
#include <sstream>
#include <iterator>
#include "error.h"
#include "argmatch.h"

#include "common_setup.hh"
#include "common_range.hh"
#include "common_cout.hh"
#include "common_aoutput.hh"
#include "common_conv.hh"

#include <spot/misc/timer.hh>
#include <spot/misc/random.hh>

#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/randomgraph.hh>
#include <spot/twaalgos/canonicalize.hh>


const char argp_program_doc[] = "\
Generate random connected automata.\n\n\
The automata are built over the atomic propositions named by PROPS...\n\
or, if N is a nonnegative number, using N arbitrary names.\n\
If the edge density is set to D, and the number of states to Q, the degree\n\
of each state follows a normal distribution with mean 1+(Q-1)D and\n\
variance (Q-1)D(1-D).  In particular, for D=0 all states have a single\n\
successor, while for D=1 all states are interconnected.\v\
Examples:\n\
\n\
This builds a random neverclaim with 4 states and labeled using the two\n\
atomic propositions \"a\" and \"b\":\n\
  % randaut --spin -Q4 a b\n\
\n\
This builds three random, complete, and deterministic TGBA with 5 to 10\n\
states, 1 to 3 acceptance sets, and three atomic propositions:\n\
  % randaut -n3 -D -H -Q5..10 -A1..3 3\n\
\n\
Build 3 random, complete, and deterministic Rabin automata\n\
with 2 to 3 acceptance pairs, state-based acceptance, 8 states, \n\
a high density of edges, and 3 to 4 atomic propositions:\n\
  % randaut -n3 -D -H -Q8 -e.8 -S -A 'Rabin 2..3' 3..4\n\
";

enum {
  OPT_SEED = 1,
  OPT_COLORED,
};

static const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Generation:", 1 },
    { "acceptance", 'A', "ACCEPTANCE", 0,
      "specify the acceptance type of the automaton", 0 },
    { "acc-probability", 'a', "FLOAT", 0,
      "probability that an edge belongs to one acceptance set (0.2)", 0 },
    { "automata", 'n', "INT", 0, "number of automata to output (1)\n"\
      "use a negative value for unbounded generation", 0 },
    { "ba", 'B', nullptr, 0,
      "build a Buchi automaton (implies --acceptance=Buchi --state-acc)", 0 },
    { "colored", OPT_COLORED, nullptr, 0,
      "build an automaton in which each edge (or state if combined with "
      "-S) belong to a single acceptance set", 0 },
    { "density", 'e', "FLOAT", 0, "density of the edges (0.2)", 0 },
    { "deterministic", 'D', nullptr, 0,
      "build a complete, deterministic automaton ", 0 },
    { "unique", 'u', nullptr, 0,
      "do not output the same automaton twice (same in the sense that they "\
      "are isomorphic)", 0 },
    { "seed", OPT_SEED, "INT", 0,
      "seed for the random number generator (0)", 0 },
    { "states", 'Q', "RANGE", 0, "number of states to output (10)", 0 },
    { "state-based-acceptance", 'S', nullptr, 0,
      "used state-based acceptance", 0 },
    { "sbacc", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    RANGE_DOC,
    { nullptr, 0, nullptr, 0, "ACCEPTANCE may be either a RANGE (in which case "
      "generalized Büchi is assumed), or an arbitrary acceptance formula "
      "such as 'Fin(0)|Inf(1)&Fin(2)' in the same syntax as in the HOA "
      "format, or one of the following patterns:\n"
      "  none\n"
      "  all\n"
      "  Buchi\n"
      "  co-Buchi\n"
      "  generalized-Buchi RANGE\n"
      "  generalized-co-Buchi RANGE\n"
      "  Rabin RANGE\n"
      "  Streett RANGE\n"
      "  generalized-Rabin INT RANGE RANGE ... RANGE\n"
      "  parity (min|max|rand) (odd|even|rand) RANGE\n"
      "  random RANGE\n"
      "  random RANGE PROBABILITY\n"
      "The random acceptance condition uses each set only once, "
      "unless a probability (to reuse the set again every time it is used) "
      "is given.", 2 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Miscellaneous options:", -1 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };


static const struct argp_child children[] =
  {
    { &aoutput_argp, 0, nullptr, 3 },
    { &aoutput_o_format_argp, 0, nullptr, 4 },
    { &misc_argp, 0, nullptr, 0 },
    { nullptr, 0, nullptr, 0 }
  };

// We want all these variables to be destroyed when we exit main, to
// make sure it happens before all other global variables (like the
// atomic propositions maps) are destroyed.  Otherwise we risk
// accessing deleted stuff.
static struct opt_t
{
  spot::atomic_prop_set aprops;
}* opt;

static const char* opt_acceptance = nullptr;
typedef spot::twa_graph::graph_t::edge_storage_t tr_t;
typedef std::set<std::vector<tr_t>> unique_aut_t;
static range ap_count_given = {-1, -2}; // Must be two different negative val
static int opt_seed = 0;
static const char* opt_seed_str = "0";
static int opt_automata = 1;
static range opt_states = { 10, 10 };
static float opt_density = 0.2;
static range opt_acc_sets = { -1, 0 };
static float opt_acc_prob = 0.2;
static bool opt_deterministic = false;
static bool opt_state_acc = false;
static bool opt_colored = false;
static bool ba_wanted = false;
static bool generic_wanted = false;
static bool gba_wanted = false;
static std::unique_ptr<unique_aut_t> opt_uniq = nullptr;

static void
ba_options()
{
  opt_acc_sets = { 1, 1 };
  opt_state_acc = true;
}

// Range should have the form 12..34 or 12:34, maybe with spaces.  The
// characters between '.' and ':' include all digits plus '/', but the
// parser will later choke on '/' if it is used, so let's not worry
// here.
static bool
looks_like_a_range(const char* str)
{
  while (*str == ' ' || (*str >= '.' && *str <= ':'))
    ++str;
  return !*str;
}

static int
parse_opt(int key, char* arg, struct argp_state* as)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case '8':
      spot::enable_utf8();
      break;
    case 'a':
      opt_acc_prob = to_probability(arg, "-a/--acc-probability");
      break;
    case 'A':
      if (looks_like_a_range(arg))
        {
          opt_acc_sets = parse_range(arg);
          if (opt_acc_sets.min > opt_acc_sets.max)
            std::swap(opt_acc_sets.min, opt_acc_sets.max);
          if (opt_acc_sets.min < 0)
            error(2, 0, "number of acceptance sets should be positive");
          gba_wanted = true;
        }
      else
        {
          opt_acceptance = arg;
          generic_wanted = true;
        }
      break;
    case 'B':
      ba_options();
      ba_wanted = true;
      break;
    case 'e':
      opt_density = to_probability(arg, "-e/--density");
      break;
    case 'D':
      opt_deterministic = true;
      break;
    case 'n':
      opt_automata = to_int(arg, "-n/--automata");
      break;
    case 'Q':
      opt_states = parse_range(arg);
      if (opt_states.min > opt_states.max)
        std::swap(opt_states.min, opt_states.max);
      if (opt_states.min == 0)
        error(1, 0, "cannot build an automaton with 0 states");
      break;
    case 'S':
      opt_state_acc = true;
      break;
    case 'u':
      opt_uniq =
        std::unique_ptr<unique_aut_t>(new std::set<std::vector<tr_t>>());
      break;
    case OPT_COLORED:
      opt_colored = true;
      break;
    case OPT_SEED:
      opt_seed = to_int(arg, "--seed");
      opt_seed_str = arg;
      break;
    case ARGP_KEY_ARG:
      // If this is the unique non-option argument, it can
      // be a number of atomic propositions to build.
      //
      // argp reorganizes argv[] so that options always come before
      // non-options.  So if as->argc == as->next we know this is the
      // last non-option argument, and if aprops.empty() we know this
      // is the also the first one.
      if (opt->aprops.empty()
          && as->argc == as->next && looks_like_a_range(arg))
        {
          ap_count_given = parse_range(arg);
          // Create the set once if the count is fixed.
          if (ap_count_given.min == ap_count_given.max)
            opt->aprops = spot::create_atomic_prop_set(ap_count_given.min);
          break;
        }
      opt->aprops.insert(spot::formula::ap(arg));
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


int
main(int argc, char** argv)
{
  return protected_main(argv, [&] {
      strcpy(F_doc, "seed number");
      strcpy(L_doc, "automaton number");

      const argp ap = { options, parse_opt, "N|PROP...", argp_program_doc,
                        children, nullptr, nullptr };

      // This will ensure that all objects stored in this struct are
      // destroyed before global variables.
      opt_t o;
      opt = &o;

      if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
        exit(err);

      // running 'randaut 0' is one way to generate automata using no
      // atomic propositions so do not complain in that case.
      if (opt->aprops.empty() && ap_count_given.max < 0)
        error(2, 0,
              "No atomic proposition supplied?   Run '%s --help' for usage.",
              program_name);

      if (generic_wanted && automaton_format == Spin)
        error(2, 0,
              "--spin implies --ba so should not be used with --acceptance");
      if (generic_wanted && ba_wanted)
        error(2, 0, "--acceptance and --ba may not be used together");

      if (automaton_format == Spin && opt_acc_sets.max > 1)
        error(2, 0, "--spin is incompatible with --acceptance=%d..%d",
              opt_acc_sets.min, opt_acc_sets.max);
      if (ba_wanted && opt_acc_sets.min != 1 && opt_acc_sets.max != 1)
        error(2, 0, "--ba is incompatible with --acceptance=%d..%d",
              opt_acc_sets.min, opt_acc_sets.max);
      if (ba_wanted && generic_wanted)
        error(2, 0,
              "--ba is incompatible with --acceptance=%s", opt_acceptance);

      if (automaton_format == Spin)
        ba_options();

      if (opt_colored && opt_acc_sets.min == -1 && !generic_wanted)
        error(2, 0, "--colored requires at least one acceptance set; "
              "use --acceptance");
      if (opt_colored && opt_acc_sets.min == 0)
        error(2, 0, "--colored requires at least one acceptance set; "
              "fix the range of --acceptance");

      if (opt_acc_sets.min == -1)
        opt_acc_sets.min = 0;

      spot::srand(opt_seed);
      auto d = spot::make_bdd_dict();

      automaton_printer printer;

      constexpr unsigned max_trials = 10000;
      unsigned trials = max_trials;

      int automaton_num = 0;

      for (;;)
        {
          spot::process_timer timer;
          timer.start();

          if (ap_count_given.max > 0
              && ap_count_given.min != ap_count_given.max)
            {
              int c = spot::rrand(ap_count_given.min, ap_count_given.max);
              opt->aprops = spot::create_atomic_prop_set(c);
            }

          int size = opt_states.min;
          if (size != opt_states.max)
            size = spot::rrand(size, opt_states.max);

          int accs = opt_acc_sets.min;
          if (accs != opt_acc_sets.max)
            accs = spot::rrand(accs, opt_acc_sets.max);

          spot::acc_cond::acc_code code;
          if (opt_acceptance)
            {
              code = spot::acc_cond::acc_code(opt_acceptance);
              accs = code.used_sets().max_set();
              if (opt_colored && accs == 0)
                error(2, 0, "--colored requires at least one acceptance set; "
                      "fix the range of --acceptance");
            }

          auto aut =
            spot::random_graph(size, opt_density, &opt->aprops, d,
                               accs, opt_acc_prob, 0.5,
                               opt_deterministic, opt_state_acc,
                               opt_colored);

          if (opt_acceptance)
            aut->set_acceptance(accs, code);

          if (opt_uniq)
            {
              auto tmp = spot::canonicalize
                (make_twa_graph(aut, spot::twa::prop_set::all()));
              std::vector<tr_t> trans(tmp->edge_vector().begin() + 1,
                                      tmp->edge_vector().end());
              if (!opt_uniq->emplace(trans).second)
                {
                  --trials;
                  if (trials == 0)
                    error(2, 0, "failed to generate a new unique automaton"
                          " after %d trials", max_trials);
                  continue;
                }
              trials = max_trials;
            }

          timer.stop();

          printer.print(aut, timer, nullptr,
                        opt_seed_str, automaton_num, nullptr);

          ++automaton_num;
          if (opt_automata > 0 && automaton_num >= opt_automata)
            break;
        }
      return 0;
    });
}
