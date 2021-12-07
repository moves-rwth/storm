// -*- coding: utf-8 -*-
// Copyright (C) 2014-2018 Laboratoire de Recherche et
// Développement de l'Epita.
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
#include <spot/graph/graph.hh>

template <typename SL, typename TL>
void
dot_state(std::ostream& out, spot::digraph<SL, TL>& g, unsigned n)
{
  out << " [label=\"" << g.state_data(n) << "\"]\n";
}

template <typename TL>
void
dot_state(std::ostream& out, spot::digraph<void, TL>&, unsigned)
{
  out << '\n';
}

template <typename SL, typename TL, typename TR>
void
dot_trans(std::ostream& out, spot::digraph<SL, TL>&, TR& tr)
{
  out << " [label=\"" << tr.data() << "\"]\n";
}

template <typename SL, typename TR>
void
dot_trans(std::ostream& out, spot::digraph<SL, void>&, TR&)
{
  out << '\n';
}


template <typename SL, typename TL>
void
dot(std::ostream& out, spot::digraph<SL, TL>& g)
{
  out << "digraph {\n";
  unsigned c = g.num_states();
  for (unsigned s = 0; s < c; ++s)
    {
      out << ' ' << s;
      dot_state(out, g, s);
      for (auto& t: g.out(s))
        {
          out << ' ' << s << " -> " << t.dst;
          dot_trans(out, g, t);
        }
    }
  out << "}\n";
}


static bool
g1(const spot::digraph<void, void>& g,
   unsigned s, int e)
{
  int f = 0;
  for (auto& t: g.out(s))
    {
      (void) t;
      ++f;
    }
  return f == e;
}

static bool
f1()
{
  spot::digraph<void, void> g(3);

  auto s1 = g.new_state();
  auto s2 = g.new_state();
  auto s3 = g.new_state();
  g.new_edge(s1, s2);
  g.new_edge(s1, s3);
  g.new_edge(s2, s3);
  g.new_edge(s3, s1);
  g.new_edge(s3, s2);
  g.new_edge(s3, s3);

  dot(std::cout, g);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      (void) t;
      ++f;
    }
  return f == 2
    && g1(g, s3, 3)
    && g1(g, s2, 1)
    && g1(g, s1, 2);
}


static bool
f2()
{
  spot::digraph<int, void> g(3);

  auto s1 = g.new_state(1);
  auto s2 = g.new_state(2);
  auto s3 = g.new_state(3);
  g.new_edge(s1, s2);
  g.new_edge(s1, s3);
  g.new_edge(s2, s3);
  g.new_edge(s3, s2);

  dot(std::cout, g);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += g.state_data(t.dst);
    }
  return f == 5;
}

static bool
f3()
{
  spot::digraph<void, int> g(3);

  auto s1 = g.new_state();
  auto s2 = g.new_state();
  auto s3 = g.new_state();
  g.new_edge(s1, s2, 1);
  g.new_edge(s1, s3, 2);
  g.new_edge(s2, s3, 3);
  g.new_edge(s3, s2, 4);

  dot(std::cout, g);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.label;
    }
  return f == 3 && g.states().size() == 3;
}

static bool
f4()
{
  spot::digraph<int, int> g(3);

  auto s1 = g.new_state(2);
  auto s2 = g.new_state(3);
  auto s3 = g.new_state(4);
  g.new_edge(s1, s2, 1);
  g.new_edge(s1, s3, 2);
  g.new_edge(s2, s3, 3);
  g.new_edge(s3, s2, 4);

  dot(std::cout, g);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.label * g.state_data(t.dst);
    }
  return f == 11;
}

static bool
f5()
{
  spot::digraph<void, std::pair<int, float>> g(3);

  auto s1 = g.new_state();
  auto s2 = g.new_state();
  auto s3 = g.new_state();
  g.new_edge(s1, s2, std::make_pair(1, 1.2f));
  g.new_edge(s1, s3, std::make_pair(2, 1.3f));
  g.new_edge(s2, s3, std::make_pair(3, 1.4f));
  g.new_edge(s3, s2, std::make_pair(4, 1.5f));

  int f = 0;
  float h = 0;
  for (auto& t: g.out(s1))
    {
      f += std::get<0>(t);
      h += std::get<1>(t);
    }
  return f == 3 && (h > 2.49 && h < 2.51);
}

static bool
f6()
{
  spot::digraph<void, std::pair<int, float>> g(3);

  auto s1 = g.new_state();
  auto s2 = g.new_state();
  auto s3 = g.new_state();
  g.new_edge(s1, s2, 1, 1.2f);
  g.new_edge(s1, s3, 2, 1.3f);
  g.new_edge(s2, s3, 3, 1.4f);
  g.new_edge(s3, s2, 4, 1.5f);

  int f = 0;
  float h = 0;
  for (auto& t: g.out(s1))
    {
      f += t.first;
      h += t.second;
    }
  return f == 3 && (h > 2.49 && h < 2.51) && g.is_existential();
}

static bool
f7()
{
  spot::digraph<int, int> g(3);
  auto s1 = g.new_state(2);
  auto s2 = g.new_state(3);
  auto s3 = g.new_state(4);
  g.new_univ_edge(s1, {s2, s3}, 1);
  g.new_univ_edge(s1, {s3}, 2);
  g.new_univ_edge(s2, {s3}, 3);
  g.new_univ_edge(s3, {s2}, 4);

  int f = 0;
  for (auto& t: g.out(s1))
    for (unsigned tt: g.univ_dests(t))
      f += t.label * g.state_data(tt);

  g.dump_storage(std::cout);

  return f == 15 && !g.is_existential();
}


struct int_pair
{
  int one;
  int two;

  friend std::ostream& operator<<(std::ostream& os, int_pair p)
  {
    os << '(' << p.one << ',' << p.two << ')';
    return os;
  }

#if __GNUC__ <= 4 && __GNUC_MINOR__ <= 6
  int_pair(int one, int two)
    : one(one), two(two)
  {
  }

  int_pair()
  {
  }
#endif
};

static bool
f8()
{
  spot::digraph<int_pair, int_pair> g(3);
  auto s1 = g.new_state(2, 4);
  auto s2 = g.new_state(3, 6);
  auto s3 = g.new_state(4, 8);
  g.new_edge(s1, s2, 1, 3);
  g.new_edge(s1, s3, 2, 5);
  g.new_edge(s2, s3, 3, 7);
  g.new_edge(s3, s2, 4, 9);

  dot(std::cout, g);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.one * g.state_data(t.dst).one;
      f += t.two * g.state_data(t.dst).two;
    }
  return f == 69;
}


int main()
{
  bool a1 = f1();
  bool a2 = f2();
  bool a3 = f3();
  bool a4 = f4();
  bool a5 = f5();
  bool a6 = f6();
  bool a7 = f7();
  bool a8 = f8();
  std::cout << a1 << ' '
            << a2 << ' '
            << a3 << ' '
            << a4 << ' '
            << a5 << ' '
            << a6 << ' '
            << a7 << ' '
            << a8 << '\n';
  return !(a1 && a2 && a3 && a4 && a5 && a6 && a7 && a8);
}
