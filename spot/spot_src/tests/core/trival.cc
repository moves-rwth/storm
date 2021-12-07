// -*- coding: utf-8 -*-
// Copyright (C) 2016, 2018 Laboratoire de Recherche et Developpement
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

#undef NDEBUG
#include "config.h"
#include <spot/misc/trival.hh>
#include <cassert>

int main()
{
  spot::trival v1;
  spot::trival v2(false);
  spot::trival v3(true);
  spot::trival v4 = spot::trival::maybe();
  assert(v1 != v2);
  assert(v1 != v3);
  assert(v2 != v3);
  assert(v4 != v2);
  assert(v4 != v3);
  assert(v2 == false);
  assert(true == v3);
  assert(v4 == spot::trival::maybe());
  assert((bool)v3);
  assert(!(bool)v2);
  assert(!(bool)!v1);
  assert(!(bool)v1);
  assert(!(bool)!v3);

  for (auto u : {v2, v1, v3})
    for (auto v : {v2, v1, v3})
      std::cout << u << " && " << v << " == " << (u && v) << '\n';
  for (auto u : {v2, v1, v3})
    for (auto v : {v2, v1, v3})
      std::cout << u << " || " << v << " == " << (u || v) << '\n';
}
