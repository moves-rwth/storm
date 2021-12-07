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

#include "common_post.hh"
#include "common_r.hh"
#include "common_aoutput.hh"
#include "common_setup.hh"
#include "error.h"
#include "argmatch.h"

spot::postprocessor::output_type type = spot::postprocessor::TGBA;
spot::postprocessor::output_pref pref = spot::postprocessor::Small;
spot::postprocessor::output_pref comp = spot::postprocessor::Any;
spot::postprocessor::output_pref sbacc = spot::postprocessor::Any;
spot::postprocessor::output_pref colored = spot::postprocessor::Any;
spot::postprocessor::optimization_level level = spot::postprocessor::High;

bool level_set = false;
bool pref_set = false;

enum {
  OPT_COBUCHI = 256,
  OPT_HIGH,
  OPT_LOW,
  OPT_MEDIUM,
  OPT_SMALL,
  OPT_TGBA,
};

static constexpr const argp_option options[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output automaton type:", 2 },
    { "generic", 'G', nullptr, 0,
      "any acceptance condition is allowed", 0 },
    { "tgba", OPT_TGBA, nullptr, 0,
      "Transition-based Generalized Büchi Automaton (default)", 0 },
    { "ba", 'B', nullptr, 0,
      "Büchi Automaton (implies -S)", 0 },
    { "monitor", 'M', nullptr, 0, "Monitor (accepts all finite prefixes "
      "of the given property)", 0 },
    { "complete", 'C', nullptr, 0, "output a complete automaton", 0 },
    { "state-based-acceptance", 'S', nullptr, 0,
      "define the acceptance using states", 0 },
    { "sbacc", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "parity", 'P',
      "any|min|max|odd|even|min odd|min even|max odd|max even",
      OPTION_ARG_OPTIONAL,
      "automaton with parity acceptance", 0, },
    { "colored-parity", 'p',
      "any|min|max|odd|even|min odd|min even|max odd|max even",
      OPTION_ARG_OPTIONAL,
      "colored automaton with parity acceptance", 0, },
    { "cobuchi", OPT_COBUCHI, nullptr, 0,
      "automaton with co-Büchi acceptance (will recognize "
      "a superset of the input language if not co-Büchi "
      "realizable)", 0 },
    { "coBuchi", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification goal:", 20 },
    { "small", OPT_SMALL, nullptr, 0, "prefer small automata (default)", 0 },
    { "deterministic", 'D', nullptr, 0, "prefer deterministic automata "
      "(combine with --generic to be sure to obtain a deterministic "
      "automaton)", 0 },
    { "any", 'a', nullptr, 0, "no preference, do not bother making it small "
      "or deterministic", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification level:", 21 },
    { "low", OPT_LOW, nullptr, 0, "minimal optimizations (fast)", 0 },
    { "medium", OPT_MEDIUM, nullptr, 0, "moderate optimizations", 0 },
    { "high", OPT_HIGH, nullptr, 0,
      "all available optimizations (slow, default)", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

static constexpr const argp_option options_nooutput[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification goal:", 20 },
    { "small", OPT_SMALL, nullptr, 0, "prefer small automata (default)", 0 },
    { "deterministic", 'D', nullptr, 0, "prefer deterministic automata", 0 },
    { "any", 'a', nullptr, 0, "no preference, do not bother making it small "
      "or deterministic", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification level:", 21 },
    { "low", OPT_LOW, nullptr, 0, "minimal optimizations (fast)", 0 },
    { "medium", OPT_MEDIUM, nullptr, 0, "moderate optimizations", 0 },
    { "high", OPT_HIGH, nullptr, 0,
      "all available optimizations (slow, default)", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

static const argp_option options_disabled[] =
  {
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Output automaton type:", 2 },
    { "generic", 'G', nullptr, 0,
      "any acceptance is allowed (default)", 0 },
    { "tgba", OPT_TGBA, nullptr, 0,
      "Transition-based Generalized Büchi Automaton", 0 },
    { "ba", 'B', nullptr, 0,
      "Büchi Automaton (with state-based acceptance)", 0 },
    { "monitor", 'M', nullptr, 0, "Monitor (accepts all finite prefixes "
      "of the given property)", 0 },
    { "complete", 'C', nullptr, 0, "output a complete automaton", 0 },
    { "state-based-acceptance", 'S', nullptr, 0,
      "define the acceptance using states", 0 },
    { "sbacc", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "parity", 'P',
      "any|min|max|odd|even|min odd|min even|max odd|max even",
      OPTION_ARG_OPTIONAL,
      "automaton with parity acceptance", 0, },
    { "colored-parity", 'p',
      "any|min|max|odd|even|min odd|min even|max odd|max even",
      OPTION_ARG_OPTIONAL,
      "colored automaton with parity acceptance", 0, },
    { "cobuchi", OPT_COBUCHI, nullptr, 0,
      "automaton with co-Büchi acceptance (will recognize "
      "a superset of the input language if not co-Büchi "
      "realizable)", 0 },
    { "coBuchi", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification goal:", 20 },
    { "small", OPT_SMALL, nullptr, 0, "prefer small automata", 0 },
    { "deterministic", 'D', nullptr, 0, "prefer deterministic automata "
      "(combine with --generic to be sure to obtain a deterministic "
      "automaton)", 0 },
    { "any", 'a', nullptr, 0, "no preference, do not bother making it small "
      "or deterministic", 0 },
    /**************************************************/
    { nullptr, 0, nullptr, 0, "Simplification level:", 21 },
    { "low", OPT_LOW, nullptr, 0, "minimal optimizations (fast)", 0 },
    { "medium", OPT_MEDIUM, nullptr, 0, "moderate optimizations", 0 },
    { "high", OPT_HIGH, nullptr, 0,
      "all available optimizations (slow)", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

static int
parse_opt_post(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case 'a':
      pref = spot::postprocessor::Any;
      pref_set = true;
      break;
    case 'B':
      type = spot::postprocessor::BA;
      colored = spot::postprocessor::Any;
      sbacc = spot::postprocessor::SBAcc;
      break;
    case 'C':
      comp = spot::postprocessor::Complete;
      break;
    case 'D':
      pref = spot::postprocessor::Deterministic;
      pref_set = true;
      break;
    case 'G':
      type = spot::postprocessor::Generic;
      colored = spot::postprocessor::Any;
      break;
    case 'M':
      type = spot::postprocessor::Monitor;
      colored = spot::postprocessor::Any;
      break;
    case 'P':
    case 'p':
      {
        static char const *const parity_args[] =
          {
            "any", "min", "max", "odd", "even",
            "min odd", "odd min",
            "min even", "even min",
            "max odd", "odd max",
            "max even", "even max",
            nullptr
          };
        static spot::postprocessor::output_type const parity_types[] =
          {
            spot::postprocessor::Parity,
            spot::postprocessor::ParityMin,
            spot::postprocessor::ParityMax,
            spot::postprocessor::ParityOdd,
            spot::postprocessor::ParityEven,
            spot::postprocessor::ParityMinOdd,
            spot::postprocessor::ParityMinOdd,
            spot::postprocessor::ParityMinEven,
            spot::postprocessor::ParityMinEven,
            spot::postprocessor::ParityMaxOdd,
            spot::postprocessor::ParityMaxOdd,
            spot::postprocessor::ParityMaxEven,
            spot::postprocessor::ParityMaxEven,
          };
        ARGMATCH_VERIFY(parity_args, parity_types);
        if (arg)
          type = XARGMATCH(key == 'P' ? "--parity" : "--colored-parity",
                           arg, parity_args, parity_types);
        else
          type = spot::postprocessor::Parity;
        if (key == 'p')
          colored = spot::postprocessor::Colored;
      }
      break;
    case 'S':
      sbacc = spot::postprocessor::SBAcc;
      break;
    case OPT_COBUCHI:
      type = spot::postprocessor::CoBuchi;
      break;
    case OPT_HIGH:
      level = spot::postprocessor::High;
      simplification_level = 3;
      level_set = true;
      break;
    case OPT_LOW:
      level = spot::postprocessor::Low;
      simplification_level = 1;
      level_set = true;
      break;
    case OPT_MEDIUM:
      level = spot::postprocessor::Medium;
      simplification_level = 2;
      level_set = true;
      break;
    case OPT_SMALL:
      pref = spot::postprocessor::Small;
      pref_set = true;
      break;
    case OPT_TGBA:
      if (automaton_format == Spin)
        error(2, 0, "--spin and --tgba are incompatible");
      type = spot::postprocessor::TGBA;
      colored = spot::postprocessor::Any;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

const struct argp post_argp = { options, parse_opt_post,
                                nullptr, nullptr, nullptr, nullptr, nullptr };
const struct argp post_argp_disabled = { options_disabled, parse_opt_post,
                                         nullptr, nullptr, nullptr,
                                         nullptr, nullptr };
const struct argp post_argp_nooutput = { options_nooutput, parse_opt_post,
                                         nullptr, nullptr, nullptr,
                                         nullptr, nullptr };
