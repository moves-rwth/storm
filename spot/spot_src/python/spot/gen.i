// -*- coding: utf-8 -*-
// Copyright (C) 2017-2019 Laboratoire de Recherche et Développement
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

%module(package="spot", director="1") gen

%include "std_string.i"
%include "exception.i"
%include "std_shared_ptr.i"

%shared_ptr(spot::twa_graph)
%shared_ptr(spot::bdd_dict)

%{
#include <spot/gen/automata.hh>
#include <spot/gen/formulas.hh>
using namespace spot;
%}

%import(module="spot.impl") <spot/misc/common.hh>
%import(module="spot.impl") <spot/tl/formula.hh>
%import(module="spot.impl") <spot/twa/fwd.hh>
%import(module="spot.impl") <spot/twa/bdddict.hh>

%exception {
  try {
    $action
  }
  catch (const std::runtime_error& e)
  {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%include <spot/gen/automata.hh>
%include <spot/gen/formulas.hh>

%pythoncode %{
def ltl_patterns(*args):
  """
  Generate LTL patterns.

  Each argument should specify a pattern with a
  range for its parameter(s).

  For single-parameter patterns, arguments of
  ltl_patterns() should be have one of these three forms:
    - (id, n)
    - (id, min, max)
    - id
  In the first case, the pattern id=n is generated.  In the second
  case, all pattern id=n for min<=n<=max are generated.  The
  third case is a shorthand for (id, 1, 10), except when
  id denotes one of the hard-coded list of LTL formulas (like,
  DAC_PATTERNS, EH_PATTERNS, etc.) where all formulas from that
  list are output.

  For two-parameter patterns, arguments of
  ltl_patterns() should be have one of these four forms:
    - (id, n1)
    - (id, n1, n2)
    - (id, min1, max1, min2, max2)
    - id
  In the first case, n2 is assumed to be equal to n1.  In
  the third case, all combination of n1 and n2 such that
  min1<=n1<=max1 and min2<=n2<=max2 are generated.  The
  last case is a shorthand for (id, 1, 3, 1, 3).
  """
  for spec in args:
    min2 = -1
    max2 = -1
    if type(spec) is int:
      pat = spec
      min = 1
      argc = ltl_pattern_argc(spec)
      if argc == 1:
        max = ltl_pattern_max(spec) or 10
      else:
        min2 = 1
        max = max2 = 3
    else:
      argc = ltl_pattern_argc(spec[0])
      ls = len(spec)
      if argc == 1:
        if ls == 2:
          pat, min, max = spec[0], spec[1], spec[1]
        elif ls == 3:
          pat, min, max = spec
        else:
          raise RuntimeError("invalid pattern specification " + str(spec))
      else:
        if ls == 2:
          pat, min, max, min2, max2 = \
            spec[0], spec[1], spec[1], spec[1], spec[1]
        elif ls == 3:
          pat, min, max, min2, max2 = \
            spec[0], spec[1], spec[1], spec[2], spec[2]
        elif ls == 5:
          pat, min, max, min2, max2 = spec
        else:
          raise RuntimeError("invalid pattern specification " + str(spec))
    for n in range(min, max + 1):
      for m in range(min2, max2 + 1):
        yield ltl_pattern(pat, n, m)


# Override aut_pattern now(), because %feature("shadow") does not
# seem to work correctly.  See https://github.com/swig/swig/issues/980
def aut_pattern(pattern: 'spot::gen::aut_pattern_id', n: 'int',
                dict: 'spot::bdd_dict_ptr' = None) -> "spot::twa_graph_ptr":
  return _gen.aut_pattern(pattern, n, dict or spot._bdd_dict)


def aut_patterns(*args):
  """
  Generate automata patterns.

  The arguments should be have one of these three forms:
    - (id, n)
    - (id, min, max)
    - id
  In the first case, the pattern id=n is generated.  In the second
  case, all pattern id=n for min<=n<=max are generated.  The
  third case is a shorthand for (id, 1, 10).
  """
  for spec in args:
    if type(spec) is int:
      pat = spec
      min = 1
      max = 10
    else:
      ls = len(spec)
      if ls == 2:
        pat, min, max = spec[0], spec[1], spec[1]
      elif ls == 3:
        pat, min, max = spec
      else:
        raise RuntimeError("invalid pattern specification")
    for n in range(min, max + 1):
      yield aut_pattern(pat, n)
%}
