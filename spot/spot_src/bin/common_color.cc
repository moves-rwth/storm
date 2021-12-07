// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2019 Laboratoire de Recherche et Développement
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
#include "common_color.hh"
#include "common_setup.hh"

#include <argp.h>
#include <unistd.h>

#include "argmatch.h"

static char const *const color_args[] =
{
  "always", "yes", "force",
  "never", "no", "none",
  "auto", "tty", "if-tty", nullptr
};
static color_type const color_types[] =
{
  color_always, color_always, color_always,
  color_never, color_never, color_never,
  color_if_tty, color_if_tty, color_if_tty
};
ARGMATCH_VERIFY(color_args, color_types);

color_type color_opt = color_if_tty;
const char* bright_red = "\033[1;31m";
const char* bold = "\033[1m";
const char* bold_std = "\033[0;1m";
const char* reset_color = "\033[m";


void setup_color()
{
  if (color_opt == color_if_tty)
    color_opt = isatty(STDERR_FILENO) ? color_always : color_never;

  if (color_opt == color_never)
    {
      bright_red = "";
      bold = "";
      bold_std = "";
      reset_color = "";
    }
}

enum {
  OPT_COLOR = 256,
};

static const argp_option options_color[] =
  {
    { "color", OPT_COLOR, "WHEN", OPTION_ARG_OPTIONAL,
      "colorize output; WHEN can be 'never', 'always' (the default if "
      "--color is used without argument), or "
      "'auto' (the default if --color is not used)", -15 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };


static int
parse_opt_color(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case OPT_COLOR:
      {
        if (arg)
          color_opt = XARGMATCH("--color", arg, color_args, color_types);
        else
          color_opt = color_always;
        break;
      }
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

const struct argp color_argp = { options_color, parse_opt_color,
                                 nullptr, nullptr, nullptr, nullptr, nullptr };
