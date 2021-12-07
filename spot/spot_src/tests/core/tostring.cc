// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2009, 2012, 2015, 2016, 2018 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <iostream>
#include <cassert>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>

const char* inputs[] =
  {
    "a",
    "1",
    "0",
    "a => b",
    "G a ",
    "a U b",
    "a & b",
    "a & b & c",
    "b & a & b",
    "b & a & a",
    "a & b & (c |(f U g)| e)",
    "b & a & a & (c | e |(f U g)| e | c) & b",
    "a <=> b",
    "a & b & (c |(f U g)| e)",
    "b & a & a & (c | e |(g U g)| e | c) & b",
    "F\"F1\"&G\"G\"&X\"X\"",
    "GFfalse",
    "GFtrue",
    "p=0Uq=1Ut=1",
    "F\"FALSE\"",
    "G\"TruE\"",
    "FFALSE",
    "GTruE",
    "p=0UFXp=1",
    "GF\"\\GF\"",
    "GF\"foo bar\"",
    "FFG__GFF",
    "X\"U\"",
    "X\"W\"",
    "X\"M\"",
    "X\"R\"",
    "{a;b;{c && d*};**}|=>G{a*:b*}",
    "GF!{{a || c} && b}",
    "GF!{{a || c*} && b}<>->{{!a}*}",
    "GF{{a || c*} & b[*]}[]->{d}",
    "{a[*2..3]}",
    "{a[*0..1]}",
    "{a[*0..]}",
    "{a[*..]}",
    "{a[*1..]}",
    "{a[+]}",
    "{[+]}",
    "{a[*2..3][*4..5]}",
    "{a**}<>->1",
  };


int main()
{
  for (const char* input: inputs)
  {
    {
      auto pf1 = spot::parse_infix_psl(input);
      if (pf1.format_errors(std::cerr))
        return 2;
      auto f1 = pf1.f;

      // The string generated from an abstract tree should be parsable
      // again.

      std::string f1s = spot::str_psl(f1);
      std::cout << f1s << '\n';

      auto pf2 = spot::parse_infix_psl(f1s);
      if (pf2.format_errors(std::cerr))
        return 2;
      auto f2 = pf2.f;

      // This second abstract tree should be equal to the first.

      if (f1 != f2)
        return 1;

      // It should also map to the same string.

      std::string f2s = spot::str_psl(f2);

      if (f2s != f1s)
        {
          std::cerr << f1s << " != " << f2s << '\n';
          return 1;
        }
    }
    assert(spot::fnode::instances_check());
  }
  return 0;
}
