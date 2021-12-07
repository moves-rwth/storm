// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2019 Laboratoire de Recherche et Développement
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

#include "common_hoaread.hh"
#include "common_setup.hh"
#include "argmatch.h"
#include "error.h"

enum
  {
    OPT_TRUST_HOA = 1,
  };

static const argp_option options[] =
{
  { "trust-hoa", OPT_TRUST_HOA, "BOOL", 0,
    "If false, properties listed in HOA files are ignored, "
    "unless they can be easily verified.  If true (the default) "
    "any supported property is trusted.", 1 },
  { nullptr, 0, nullptr, 0, nullptr, 0 }
};

spot::automaton_parser_options opt_parse;

spot::twa_graph_ptr
read_automaton(const char* filename, spot::bdd_dict_ptr& dict)
{
  auto p = spot::parse_aut(filename, dict,
                           spot::default_environment::instance(),
                           opt_parse);
  if (p->format_errors(std::cerr))
    error(2, 0, "failed to read automaton from %s", filename);
  if (p->aborted)
    error(2, 0, "failed to read automaton from %s (--ABORT-- read)", filename);
  return std::move(p->aut);
}

static bool parse_bool(const char* opt, const char* arg)
{
  enum bool_type { bool_false, bool_true };
  static char const *const bool_args[] =
    {
      "false", "no", "0",
      "true", "yes", "1",
      nullptr
    };
  static bool_type const bool_types[] =
    {
      bool_false, bool_false, bool_false,
      bool_true, bool_true, bool_true,
    };
  ARGMATCH_VERIFY(bool_args, bool_types);
  bool_type bt = XARGMATCH(opt, arg, bool_args, bool_types);
  return bt == bool_true;
}

static int
parse_opt_hoaread(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case OPT_TRUST_HOA:
      opt_parse.trust_hoa = parse_bool("--trust-hoa", arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


const struct argp hoaread_argp = { options, parse_opt_hoaread,
                                   nullptr, nullptr, nullptr,
                                   nullptr, nullptr };
