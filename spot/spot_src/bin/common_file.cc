// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2016 Laboratoire de Recherche et Développement de
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

#include "common_file.hh"
#include <error.h>
#include <iostream>


output_file::output_file(const char* name)
{
  std::ios_base::openmode mode = std::ios_base::trunc;
  if (name[0] == '>' && name[1] == '>')
    {
      mode = std::ios_base::app;
      append_ = true;
      name += 2;
    }
  if (name[0] == '-' && name[1] == 0)
    {
      os_ = &std::cout;
      return;
    }
  of_ = new std::ofstream(name, mode);
  if (!*of_)
    error(2, errno, "cannot open '%s'", name);
  os_ = of_;
}


void output_file::close(const std::string& name)
{
  // We close of_, not os_, so that we never close std::cout.
  if (os_)
    os_->flush();
  if (of_)
    of_->close();
  if (os_ && !*os_)
    error(2, 0, "error writing to %s",
          (name == "-") ? "standard output" : name.c_str());
}
