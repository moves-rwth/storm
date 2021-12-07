// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2013, 2014, 2015 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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
#include <spot/misc/minato.hh>
#include <cassert>

namespace spot
{

  minato_isop::minato_isop(bdd input)
    : minato_isop(input, bdd_support(input))
  {
  }

  minato_isop::minato_isop(bdd input, bdd vars)
    : ret_(bddfalse)
  {
    // If INPUT has the form a&b&c&(binary function) we want to
    // compute the ISOP of the only binary and prepend a&b&c latter.
    //
    // Calling bdd_satprefix (it returns a&b&c and modify input to
    // point to function) this way is an optimization to the
    // original algorithm, because in many cases we are trying to
    // build ISOPs out of formulae that are already cubes.
    cube_.push(bdd_satprefix(input));
    todo_.emplace(input, input, vars);
  }

  minato_isop::minato_isop(bdd input_min, bdd input_max, bool)
    : ret_(bddfalse)
  {
    if (input_min == input_max)
      {
        cube_.push(bdd_satprefix(input_min));
        input_max = input_min;
      }
    else
      {
        cube_.push(bddtrue);
      }
    bdd common = input_min & input_max;
    todo_.emplace(input_min, input_max, bdd_support(common));
  }

  bdd
  minato_isop::next()
  {
    while (!todo_.empty())
      {
        local_vars& l = todo_.top();
        switch (l.step)
          {
          case local_vars::FirstStep:
          next_var:
            {
              if (l.f_min == bddfalse)
                {
                  ret_ = bddfalse;
                  todo_.pop();
                  continue;
                }
              if (l.vars == bddtrue || l.f_max == bddtrue)
                {
                  ret_ = l.f_max;
                  todo_.pop();
                  return cube_.top() & ret_;
                }
              assert(l.vars != bddfalse);

              // Pick the first variable in VARS that is used by F_MIN
              // or F_MAX.  We know that VARS, F_MIN or F_MAX are not
              // constants (bddtrue or bddfalse) because one of the
              // two above `if' would have matched; so it's ok to call
              // bdd_var().
              int v = bdd_var(l.vars);
              l.vars = bdd_high(l.vars);
              int v_min = bdd_var(l.f_min);
              int v_max = bdd_var(l.f_max);
              if (v < v_min && v < v_max)
                // Do not use a while() for this goto, because we want
                // `continue' to be relative to the outermost while().
                goto next_var;

              l.step = local_vars::SecondStep;

              bdd v0 = bdd_nithvar(v);
              l.v1 = bdd_ithvar(v);

              // All the following should be equivalent to
              //   f0_min = bdd_restrict(f_min, v0);
              //   f0_max = bdd_restrict(f_max, v0);
              //   f1_min = bdd_restrict(f_min, v1);
              //   f1_max = bdd_restrict(f_max, v1);
              // but we try to avoid bdd_restrict when possible.
              if (v == v_min)
                {
                  l.f0_min = bdd_low(l.f_min);
                  l.f1_min = bdd_high(l.f_min);
                }
              else if (v_min < v)
                {
                  l.f0_min = bdd_restrict(l.f_min, v0);
                  l.f1_min = bdd_restrict(l.f_min, l.v1);
                }
              else
                {
                  l.f1_min = l.f0_min = l.f_min;
                }
              if (v == v_max)
                {
                  l.f0_max = bdd_low(l.f_max);
                  l.f1_max = bdd_high(l.f_max);
                }
              else if (v_max < v)
                {
                  l.f0_max = bdd_restrict(l.f_max, v0);
                  l.f1_max = bdd_restrict(l.f_max, l.v1);
                }
              else
                {
                  l.f1_max = l.f0_max = l.f_max;
                }

              cube_.push(cube_.top() & v0);
              todo_.emplace(l.f0_min - l.f1_max, l.f0_max, l.vars);
            }
            continue;

          case local_vars::SecondStep:
            l.step = local_vars::ThirdStep;
            l.g0 = ret_;
            cube_.pop();
            cube_.push(cube_.top() & l.v1);
            todo_.emplace(l.f1_min - l.f0_max, l.f1_max, l.vars);
            continue;

          case local_vars::ThirdStep:
            l.step = local_vars::FourthStep;
            l.g1 = ret_;
            cube_.pop();
            {
              bdd fs_max = l.f0_max & l.f1_max;
              bdd fs_min = fs_max & ((l.f0_min - l.g0) | (l.f1_min - l.g1));
              todo_.emplace(fs_min, fs_max, l.vars);
            }
            continue;

          case local_vars::FourthStep:
            ret_ |= (l.g0 - l.v1) | (l.g1 & l.v1);
            todo_.pop();
            continue;
          }
        SPOT_UNREACHABLE();
      }
    return bddfalse;
  }

}
