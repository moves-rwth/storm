// -*- coding: utf-8 -*-
// Copyright (C) 2007, 2011, 2014, 2015, 2017, 2018 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2006, 2007 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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

#include "config.h"
#include <bddx.h>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include "spot/priv/bddalloc.hh"

namespace spot
{

  bool bdd_allocator::initialized = false;

  static void show_bdd_stats()
  {
    bddStat s;
    bdd_stats(&s);
    std::cerr << "spot: BDD stats: produced=" << s.produced
              << " nodenum=" << s.nodenum
              << " freenodes=" << s.freenodes
              << " (" << (s.freenodes * 100 / s.nodenum)
              << "%) minfreenodes=" << s.minfreenodes
              << "% varnum=" << s.varnum
              << " cachesize=" << s.cachesize
              << " hashsize=" << s.hashsize
              << " gbcnum=" << s.gbcnum
              << '\n';
  }

  static void resize_handler(int oldsize, int newsize)
  {
    std::cerr << "spot: BDD resize "
              << oldsize << " -> " << newsize << '\n';
  }

  static void gbc_handler(int pre, bddGbcStat *s)
  {
    if (!pre)
      std::cerr << "spot: BDD GC #" << s->num
                << " in " << ((float)s->time)/CLOCKS_PER_SEC << "s / "
                << ((float)s->sumtime)/CLOCKS_PER_SEC << "s total\n";
    show_bdd_stats();
  }

  bdd_allocator::bdd_allocator()
  {
    initialize();
    lvarnum = bdd_varnum();
    fl.emplace_front(0, lvarnum);
  }

  void
  bdd_allocator::initialize()
  {
    if (initialized)
      return;
    initialized = true;
    // Buddy might have been initialized by a third-party library.
    if (bdd_isrunning())
      return;
    // The values passed to bdd_init should depends on the problem
    // the library is solving.  It would be nice to allow users
    // to tune this.  By the meantime, we take the typical values
    // for large examples advocated by the BuDDy manual.
    bdd_init(1 << 19, 2);
    bdd_setcacheratio(40);
    bdd_setvarnum(2);
    // When the node table is full, add 2**19 nodes; this requires 10MB.
    bdd_setmaxincrease(1 << 19);
    // Disable the default GC handler.  (Note that this will only be
    // done if Buddy is initialized by Spot.  Otherwise we prefer not
    // to overwrite a handler that might have been set by the user.)
    if (getenv("SPOT_BDD_TRACE"))
      {
        bdd_gbc_hook(gbc_handler);
        bdd_resize_hook(resize_handler);
        std::cerr << "spot: BDD package initialized\n";
        show_bdd_stats();
      }
    else
      {
        bdd_gbc_hook(nullptr);
      }
  }

  void
  bdd_allocator::extvarnum(int more)
  {
    int varnum = bdd_varnum();
    // If varnum has been extended from another allocator (or
    // externally), use the new variables.
    if (lvarnum < varnum)
      {
        more -= varnum - lvarnum;
        lvarnum = varnum;
      }
    // If we still need more variable, do allocate them.
    if (more > 0)
      {
        bdd_extvarnum(more);
        varnum += more;
        lvarnum = varnum;
      }
  }

  int
  bdd_allocator::allocate_variables(int n)
  {
    return register_n(n);
  }

  void
  bdd_allocator::release_variables(int base, int n)
  {
    release_n(base, n);
  }

  int
  bdd_allocator::extend(int n)
  {
    // If we already have some free variable at the end
    // of the variable space, allocate just the difference.
    if (!fl.empty() && fl.back().first + fl.back().second == lvarnum)
      {
        int res = fl.back().first;
        int endvar = fl.back().second;
        assert(n > endvar);
        extvarnum(n - endvar);
        fl.pop_back();
        return res;
      }
    else
      {
        // Otherwise, allocate as much variables as we need.
        int res = lvarnum;
        extvarnum(n);
        return res;
      }
  }
}
