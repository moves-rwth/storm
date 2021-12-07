// -*- coding: utf-8 -*-
// Copyright (C) 2013-2015, 2018 Laboratoire de Recherche et
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

#include "config.h"
#include <ostream>
#include <spot/parseaut/public.hh>

namespace spot
{
  bool
  parsed_aut::format_errors(std::ostream& os)
  {
    bool printed = false;
    spot::parse_aut_error_list::iterator it;
    for (auto& err : errors)
      {
        if (!filename.empty() && filename != "-")
          os << filename << ':';
        os << err.first << ": ";
        os << err.second << std::endl;
        printed = true;
      }
    return printed;
  }
}
