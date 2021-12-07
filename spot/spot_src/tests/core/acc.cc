// -*- coding: utf-8 -*-x
// Copyright (C) 2014, 2015, 2017, 2018, 2019 Laboratoire de Recherche et
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
#include <iterator>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <spot/twa/acc.hh>

static void check(spot::acc_cond& ac, spot::acc_cond::mark_t m)
{
  std::cout << '#' << m.count() << ": " << m;
  if (ac.accepting(m))
    std::cout << " accepting";
  std::cout << '\n';
}

static void print(const std::vector<std::vector<int>>& res)
{
  for (auto& v: res)
    {
      std::cout << '{';
      const char* comma = "";
      for (int s: v)
        {
          std::cout << comma;
          if (s < 0)
            std::cout << '!' << (-s - 1);
          else
            std::cout << s;
          comma = ", ";
        }
      std::cout << "}\n";
    }
}

static void expect(const std::exception& e, const char* prefix)
{
  if (std::strncmp(e.what(), "Too many acceptance sets used.",
                   strlen(prefix)))
    {
      std::cerr << "exception: " << e.what() << '\n';
      std::cerr << "expected:  " << prefix << '\n';
      abort();
    }
}

int main()
{
  spot::acc_cond ac(4);
  ac.set_generalized_buchi();
  std::cout << ac.get_acceptance() << '\n';

  auto m1 = spot::acc_cond::mark_t({0, 2});
  auto m2 = spot::acc_cond::mark_t({0, 3});
  auto m3 = spot::acc_cond::mark_t({2, 1});
  auto m4 =
    spot::acc_cond::mark_t({0, spot::acc_cond::mark_t::max_accsets() - 2});
  if (!((m4.min_set() == 1) &&
        (m4.max_set() == spot::acc_cond::mark_t::max_accsets() - 1)))
    return 1;

  spot::acc_cond::mark_t m0 = {};
  std::cout << m0.max_set() << ' ' << m0.min_set() << '\n';
  std::cout << m3.max_set() << ' ' << m3.min_set() << '\n';

  check(ac, m1);
  check(ac, m2);
  check(ac, m3);
  check(ac, m1 | m2);
  check(ac, m2 & m1);
  check(ac, m1 | m2 | m3);

  ac.add_set();
  ac.set_generalized_buchi();

  check(ac, m1);
  check(ac, m2);
  check(ac, m3);
  check(ac, m1 | m2);
  check(ac, m2 & m1);
  check(ac, m1 | m2 | m3);

  check(ac, m2 & m3);
  check(ac, ac.comp(m2 & m3));

  spot::acc_cond ac2(ac.num_sets());
  ac2.set_generalized_buchi();
  check(ac2, m3);

  spot::acc_cond ac3(ac.num_sets() + ac2.num_sets());
  ac3.set_generalized_buchi();
  std::cout << ac.num_sets() << " + "
            << ac2.num_sets() << " = " << ac3.num_sets() << '\n';
  auto m5 = m2 | (m3 << ac.num_sets());

  std::cout << m5.max_set() << ' ' << m5.min_set() << '\n';

  check(ac3, m5);
  auto m6 = ac.comp(m2 & m3) | (m3 << ac.num_sets());
  check(ac3, m6);
  auto m7 = ac.comp(m2 & m3) | (ac.all_sets() << ac.num_sets());
  check(ac3, m7);

  const char* comma = "";
  for (auto i: m7.sets())
    {
      std::cout << comma << i;
      comma = ",";
    };
  std::cout << '\n';

  spot::acc_cond ac4;
  ac4.set_generalized_buchi();
  check(ac4, ac4.all_sets());
  check(ac4, ac4.comp(ac4.all_sets()));

  check(ac, (m1 | m2).remove_some(2));

  std::vector<spot::acc_cond::mark_t> s = { m1, m2, m3 };
  check(ac, ac.useless(s.begin(), s.end()));
  s.push_back(ac.mark(4));
  auto u = ac.useless(s.begin(), s.end());
  check(ac, u);
  std::cout << "stripping\n";
  for (auto& v: s)
    {
      check(ac, v);
      check(ac, v.strip(u));
    }


  auto code1 = ac.inf({0, 1, 3});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code1 |= ac.fin({2});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code1 |= ac.fin({0});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code1 |= ac.fin({});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code1 &= ac.inf({});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  auto code2 = code1;
  code1 &= ac.fin({0, 1});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code1 &= ac.fin({});
  std::cout << code1.size() << ' ' << code1 << ' ' << code1.is_dnf() << '\n';
  code2 |= ac.fin({0, 1});
  std::cout << code2.size() << ' ' << code2 << ' ' << code2.is_dnf() << '\n';
  auto code3 = ac.inf({0, 1});
  code3 &= ac.fin({2, 3});
  std::cout << code3.size() << ' ' << code3 << ' ' << code3.is_dnf() << '\n';

  // code3 == (Fin(2)|Fin(3)) & (Inf(0)&Inf(1))
  // {0}
  // {1}
  // {2, 3}
  std::cout << code3 << ' ' << "{0} true\n";
  spot::acc_cond::mark_t m = {};
  m.set(0);
  print(code3.missing(m, true));
  std::cout << code3 << ' ' << "{0} false\n";
  print(code3.missing(m, false));

  std::cout << spot::acc_cond::acc_code("t") << '\n';
  std::cout << spot::acc_cond::acc_code("f") << '\n';
  std::cout << spot::acc_cond::acc_code("Fin(2)") << '\n';
  std::cout << spot::acc_cond::acc_code("Inf(2)") << '\n';
  std::cout << spot::acc_cond::acc_code("Fin(2) | Inf(2)") << '\n';
  std::cout << spot::acc_cond::acc_code("Inf(2) & Fin(2)") << '\n';
  auto c1 = spot::acc_cond::acc_code("Fin(0)|Inf(1)&Fin(2)|Fin(3)");
  auto c2 = spot::acc_cond::acc_code
    ("(  Fin  (  0 ))  | (Inf   (   1) &  Fin(2 ))| Fin (3)   ");
  std::cout << c1 << '\n';
  std::cout << c2 << '\n';
  assert(c1 == c2);

  try
    {
      spot::acc_cond a{spot::acc_cond::mark_t::max_accsets() + 1};
    }
  catch (const std::runtime_error& e)
    {
      expect(e, "Too many acceptance sets used.");
    }

#if SPOT_DEBUG
  // Those error message are disabled in non-debugging code as
  // shifting mark_t is usually done in the innermost loop of
  // algorithms.  However, they are still done in Python, and we
  // test them in python/except.py
  try
    {
      spot::acc_cond::mark_t m{0};
      m <<= spot::acc_cond::mark_t::max_accsets() + 1;
    }
  catch (const std::runtime_error& e)
    {
      expect(e, "Too many acceptance sets used.");
    }

  try
    {
      spot::acc_cond::mark_t m{0};
      m >>= spot::acc_cond::mark_t::max_accsets() + 1;
    }
  catch (const std::runtime_error& e)
    {
      expect(e, "Too many acceptance sets used.");
    }
#endif
  try
    {
      spot::acc_cond::mark_t m{spot::acc_cond::mark_t::max_accsets()};
    }
  catch (const std::runtime_error& e)
    {
      expect(e, "Too many acceptance sets used.");
    }

  auto cond1 =  spot::acc_cond::acc_code(
    "(Inf(0) & Inf(5)) | Inf(5) | Inf(0)");
  std::cout << cond1.unit_propagation() << '\n';
  auto cond2 =  spot::acc_cond::acc_code("Fin(1) | Inf(0) | Inf(0)");
  std::cout << cond2.unit_propagation() << '\n';
  auto cond3 =  spot::acc_cond::acc_code("Inf(0) & Inf(2) | Fin(2)");
  std::cout << cond3.unit_propagation() << '\n';

  return 0;
}
