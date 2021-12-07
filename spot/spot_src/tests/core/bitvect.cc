// -*- coding: utf-8 -*-
// Copyright (C) 2013-2016, 2018 Laboratoire de Recherche et
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
#include <iostream>
#include <spot/misc/bitvect.hh>

static void ruler()
{
  std::cout << "\n   ";
  for (size_t x = 0; x < 76; ++x)
    if (x % 10 == 0)
      std::cout << x / 10;
    else
      std::cout << '_';
  std::cout << "\n   ";
  for (size_t x = 0; x < 76; ++x)
    std::cout << x % 10;
  std::cout << "\n\n";
}

#define ECHO(name) std::cout << #name": " << *name << '\n'

int main()
{
  ruler();
  spot::bitvect* v = spot::make_bitvect(15);
  ECHO(v);
  v->set(10);
  v->set(7);
  v->set(12);
  ECHO(v);

  ruler();
  spot::bitvect* w = spot::make_bitvect(42);
  w->set(30);
  w->set(41);
  w->set(13);
  w->set(7);
  ECHO(w);
  *w ^= *v;
  ECHO(w);

  ruler();
  spot::bitvect* x = spot::make_bitvect(75);
  x->set(70);
  x->set(60);
  ECHO(x);
  *x |= *w;
  ECHO(x);

  std::cout << "subset? " << w->is_subset_of(*x)
            << ' ' << v->is_subset_of(*w) << '\n';

  delete v;
  delete w;
  delete x;

  ruler();

  spot::bitvect_array* a = spot::make_bitvect_array(60, 10);
  for (size_t y = 0; y < a->size(); ++y)
    for (size_t x = 0; x < 60; ++x)
      {
        if (((x ^ y) & 3) < 2)
          a->at(y).set(x);
      }
  std::cout << *a;

  ruler();

  a->at(6) = a->at(4);
  a->at(8) = a->at(7);
  a->at(6) ^= a->at(8);

  std::cout << *a;

  std::cout << "Comp: "
            << (a->at(0) == a->at(1))
            << (a->at(0) == a->at(2))
            << (a->at(1) != a->at(2))
            << (a->at(0) < a->at(2))
            << (a->at(0) > a->at(2))
            << (a->at(3) < a->at(4))
            << (a->at(5) > a->at(6)) << std::endl;

  delete a;
}
