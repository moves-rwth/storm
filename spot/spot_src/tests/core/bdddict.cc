// -*- coding: utf-8 -*-x
// Copyright (C) 2017 Laboratoire de Recherche et
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <signal.h>
#include <iostream>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>

// assert_emptiness() calls abort() which prevents gcov from flushing
// its coverage data.  So we convert SIGABRT as exit(1) whenever
// possible.
#if HAVE_SIGACTION
static void
sig_handler(int)
{
  exit(1);
}

static void
setup_sig_handler()
{
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGABRT, &sa, nullptr);
}
#else
static void
setup_sig_handler()
{
}
#endif

int main()
{
  setup_sig_handler();
  spot::formula f = spot::parse_formula("& & G p0 p1 p2");
  spot::translator trans;
  spot::twa_graph_ptr aut = trans.run(f);
  aut->get_dict()->assert_emptiness(); // Should fail.
  return 0;
}
