// -*- coding: utf-8 -*-
// Copyright (C) 2011-2015, 2017 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <spot/misc/random.hh>
#include <random>

namespace spot
{
  static std::mt19937 gen;

  void
  srand(unsigned int seed)
  {
    gen.seed(seed);
  }

  double
  drand()
  {
    return  gen() / (1.0 + gen.max());
  }

  int
  mrand(int max)
  {
    return static_cast<int>(max * drand());
  }

  int
  rrand(int min, int max)
  {
    return min + static_cast<int>((max - min + 1) * drand());
  }

  double
  nrand()
  {
    const double r = drand();

    const double lim = 1.e-20;
    if (r < lim)
      return -1./lim;
    if (r > 1.0 - lim)
      return 1./lim;

    double t;
    if (r < 0.5)
      t = sqrt(-2.0 * log(r));
    else
      t = sqrt(-2.0 * log(1.0 - r));

    const double p0 = 0.322232431088;
    const double p1 = 1.0;
    const double p2 = 0.342242088547;
    const double p3 = 0.204231210245e-1;
    const double p4 = 0.453642210148e-4;
    const double q0 = 0.099348462606;
    const double q1 = 0.588581570495;
    const double q2 = 0.531103462366;
    const double q3 = 0.103537752850;
    const double q4 = 0.385607006340e-2;
    const double p = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
    const double q = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));

    if (r < 0.5)
      return (p / q) - t;
    else
      return t - (p / q);
  }
}
