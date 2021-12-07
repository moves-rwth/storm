// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#pragma once

#include <spot/misc/common.hh>
#include <cassert>
#include <cmath>
#include <vector>

namespace spot
{
  /// \defgroup random Random functions
  /// \ingroup misc_tools

  /// \ingroup random
  /// @{

  /// \brief Reset the seed of the pseudo-random number generator.
  ///
  /// \see drand, mrand, rrand
  SPOT_API void srand(unsigned int seed);

  /// \brief Compute a pseudo-random integer value between \a min and
  /// \a max included.
  ///
  /// \see drand, mrand, srand
  SPOT_API int rrand(int min, int max);

  /// \brief Compute a pseudo-random integer value between 0 and
  /// \a max-1 included.
  ///
  /// \see drand, rrand, srand
  SPOT_API int mrand(int max);

  /// \brief Compute a pseudo-random double value
  /// between 0.0 and 1.0 (1.0 excluded).
  ///
  /// \see mrand, rrand, srand
  SPOT_API double drand();

  /// \brief Compute a pseudo-random double value
  /// following a standard normal distribution.  (Odeh & Evans)
  ///
  /// This uses a polynomial approximation of the inverse cumulated
  /// density function from Odeh & Evans, Journal of Applied
  /// Statistics, 1974, vol 23, pp 96-97.
  SPOT_API double nrand();

  /// \brief Compute pseudo-random integer value between 0
  /// and \a n included, following a binomial distribution
  /// with probability \a p.
  ///
  /// \a gen must be a random function computing a pseudo-random
  /// double value following a standard normal distribution.
  /// Use nrand() or bmrand().
  ///
  /// Usually approximating a binomial distribution using a normal
  /// distribution and is accurate only if <code>n*p</code> and
  /// <code>n*(1-p)</code> are greater than 5.
  template<double (*gen)()>
  class barand
  {
  public:
    barand(int n, double p)
      : n_(n), m_(n * p), s_(sqrt(n * p * (1 - p)))
    {
    }

    int
    rand() const
    {
      for (;;)
        {
          int x = round(gen() * s_ + m_);
          if (x < 0)
            continue;
          if (x <= n_)
            return x;
        }
      SPOT_UNREACHABLE();
      return 0;
    }
  protected:
    const int n_;
    const double m_;
    const double s_;
  };

  /// \brief Shuffle the container using mrand function above.
  /// This allows to get rid off shuffle or random_shuffle that use
  /// uniform_distribution and RandomIterator that are not portables.
  template<class iterator_type>
  SPOT_API void mrandom_shuffle(iterator_type&& first, iterator_type&& last)
  {
    auto d = std::distance(first, last);
    if (d > 1)
      {
        for (--last; first < last; ++first, --d)
          {
            auto i = mrand(d);
            std::swap(*first, *(first + i));
          }
      }
  }
  /// @}
}
