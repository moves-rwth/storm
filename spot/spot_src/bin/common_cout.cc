// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2016 Laboratoire de Recherche et Développement de
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
#include "common_cout.hh"
#include <iostream>
#include <chrono>
#include "error.h"

namespace
{
  static std::chrono::steady_clock::time_point last_flush;

  static void do_check_cout()
  {
    // Make sure we abort if we can't write to std::cout anymore
    // (like disk full or broken pipe with SIGPIPE ignored).
    if (!std::cout)
      error(2, 0, "error writing to standard output");
  }
}

void check_cout()
{
  // If we haven't flushed explicitly for more than 20ms, do it now.
  // Otherwise we would have to wait for the buffer to fill up, and
  // this could take a long time.
  auto now = std::chrono::steady_clock::now();
  auto ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(now - last_flush);
  if (ms.count() >= 20)
    {
      last_flush = now;
      std::cout.flush();
    }

  do_check_cout();
}

void flush_cout()
{
  last_flush = std::chrono::steady_clock::now();
  std::cout.flush();
  do_check_cout();
}
