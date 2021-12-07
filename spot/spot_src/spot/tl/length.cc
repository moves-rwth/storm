// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2012, 2014, 2015, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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
#include <spot/tl/length.hh>
#include <spot/tl/formula.hh>

namespace spot
{
  int
  length(formula f)
  {
    int len = 0;
    f.traverse([&len](const formula& x)
               {
                 auto s = x.size();
                 if (s > 1)
                   len += s - 1;
                 else
                   ++len;
                 return false;
               });
    return len;
  }

  int
  length_boolone(formula f)
  {
    int len = 0;
    f.traverse([&len](const formula& x)
               {
                 if (x.is_boolean())
                   {
                     ++len;
                     return true;
                   }
                 auto s = x.size();
                 if (s > 2)
                   {
                     int b = 0;
                     for (const auto& y: x)
                       if (y.is_boolean())
                         ++b;
                     len += s - b * 2 + 1;
                   }
                 else if (s > 1)
                   {
                     len += s - 1;
                   }
                 else
                   {
                     ++len;
                   }
                 return false;
               });
    return len;
  }

}
