// -*- coding: utf-8 -*-
// Copyright (C) 2014-2018 Laboratoire de Recherche et Développement de
// l'Epita.
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
#include <spot/graph/ngraph.hh>
#include <spot/twa/twa.hh>

template <typename SL, typename TL>
void
dot_state(std::ostream& out, const spot::digraph<SL, TL>& g, unsigned n)
{
  out << " [label=\"" << g.state_data(n) << "\"]\n";
}

template <typename TL>
void
dot_state(std::ostream& out, const spot::digraph<void, TL>&, unsigned)
{
  out << '\n';
}

template <typename SL, typename TL>
void
dot_state(std::ostream& out, const spot::digraph<SL, TL>& g, unsigned n,
          std::string name)
{
  out << " [label=\"" << name << "\\n" << g.state_data(n) << "\"]\n";
}

template <typename TL>
void
dot_state(std::ostream& out, const spot::digraph<void, TL>&, unsigned,
          std::string name)
{
  out << " [label=\"" << name << "\"]\n";
}


template <typename SL, typename TL, typename TR>
void
dot_trans(std::ostream& out, const spot::digraph<SL, TL>&, TR& tr)
{
  out << " [label=\"" << tr.data() << "\"]\n";
}

template <typename SL, typename TR>
void
dot_trans(std::ostream& out, const spot::digraph<SL, void>&, TR&)
{
  out << '\n';
}


template <typename SL, typename TL>
void
dot(std::ostream& out, const spot::digraph<SL, TL>& g)
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

template <typename G1, typename G2, typename G3, typename G4>
void
dot(std::ostream& out, const spot::named_graph<G1, G2, G3, G4>& g)
{
  out << "digraph {\n";
  auto& gg = g.graph();
  unsigned c = gg.num_states();
  for (unsigned s = 0; s < c; ++s)
    {
      out << ' ' << s;
      dot_state(out, gg, s, g.get_name(s));
      for (auto& t: gg.out(s))
        {
          out << ' ' << s << " -> " << t.dst;
          dot_trans(out, gg, t);
        }
    }
  out << "}\n";
}


static bool
g1(const spot::digraph<void, void>& g, unsigned s, int e)
{
  int f = 0;
  for (auto& t: g.out(s))
    {
      (void) t;
      ++f;
    }
  return f == e;
}

static bool f1()
{
  spot::digraph<void, void> g(3);
  spot::named_graph<spot::digraph<void, void>, std::string> gg(g);

  auto s1 = gg.new_state("s1");
  auto s2 = gg.new_state("s2");
  auto s3 = gg.new_state("s3");
  gg.new_edge("s1", "s2");
  gg.new_edge("s1", "s3");
  gg.new_edge("s2", "s3");
  gg.new_edge("s3", "s1");
  gg.new_edge("s3", "s2");
  gg.new_edge("s3", "s3");

  dot(std::cout, gg);

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


static bool f2()
{
  spot::digraph<int, void> g(3);
  spot::named_graph<spot::digraph<int, void>, std::string> gg(g);

  auto s1 = gg.new_state("s1", 1);
  gg.new_state("s2", 2);
  gg.new_state("s3", 3);
  gg.new_edge("s1", "s2");
  gg.new_edge("s1", "s3");
  gg.new_edge("s2", "s3");
  gg.new_edge("s3", "s2");

  dot(std::cout, gg);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += g.state_data(t.dst);
    }
  return f == 5;
}

static bool f3()
{
  spot::digraph<void, int> g(3);
  spot::named_graph<spot::digraph<void, int>, std::string> gg(g);

  auto s1 = gg.new_state("s1");
  gg.new_state("s2");
  gg.new_state("s3");
  gg.new_edge("s1", "s2", 1);
  gg.new_edge("s1", "s3", 2);
  gg.new_edge("s2", "s3", 3);
  gg.new_edge("s3", "s2", 4);

  dot(std::cout, gg);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.label;
    }
  return f == 3 && g.states().size() == 3;
}

static bool f4()
{
  spot::digraph<int, int> g(3);
  spot::named_graph<spot::digraph<int, int>, std::string> gg(g);

  auto s1 = gg.new_state("s1", 2);
  gg.new_state("s2", 3);
  gg.new_state("s3", 4);
  gg.new_edge("s1", "s2", 1);
  gg.new_edge("s1", "s3", 2);
  gg.new_edge("s2", "s3", 3);
  gg.new_edge("s3", "s2", 4);

  dot(std::cout, gg);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.label * g.state_data(t.dst);
    }
  return f == 11;
}

static bool f5()
{
  typedef spot::digraph<void, std::pair<int, float>> graph_t;
  graph_t g(3);
  spot::named_graph<graph_t, std::string> gg(g);

  auto s1 = gg.new_state("s1");
  gg.new_state("s2");
  gg.new_state("s3");
  gg.new_edge("s1", "s2", std::make_pair(1, 1.2f));
  gg.new_edge("s1", "s3", std::make_pair(2, 1.3f));
  gg.new_edge("s2", "s3", std::make_pair(3, 1.4f));
  gg.new_edge("s3", "s2", std::make_pair(4, 1.5f));

  int f = 0;
  float h = 0;
  for (auto& t: g.out(s1))
    {
      f += std::get<0>(t);
      h += std::get<1>(t);
    }
  return f == 3 && (h > 2.49 && h < 2.51);
}

static bool f6()
{
  typedef spot::digraph<void, std::pair<int, float>> graph_t;
  graph_t g(3);
  spot::named_graph<graph_t, std::string> gg(g);

  auto s1 = gg.new_state("s1");
  gg.new_state("s2");
  gg.new_state("s3");
  gg.new_edge("s1", "s2", 1, 1.2f);
  gg.new_edge("s1", "s3", 2, 1.3f);
  gg.new_edge("s2", "s3", 3, 1.4f);
  gg.new_edge("s3", "s2", 4, 1.5f);

  int f = 0;
  float h = 0;
  for (auto& t: g.out(s1))
    {
      f += t.first;
      h += t.second;
    }
  return f == 3 && (h > 2.49 && h < 2.51) && g.is_existential();
}

static bool f7()
{
  typedef spot::digraph<int, int> graph_t;
  graph_t g(3);
  spot::named_graph<graph_t, std::string> gg(g);

  auto s1 = gg.new_state("s1", 2);
  gg.new_state("s2", 3);
  gg.new_state("s3", 4);
  gg.new_univ_edge("s1", {"s2", "s3"}, 1);
  // Standard edges can be used as well
  gg.new_edge("s1", "s3", 2);
  gg.new_univ_edge("s2", {"s3"}, 3);
  gg.new_univ_edge("s3", {"s2"}, 4);

  int f = 0;
  for (auto& t: g.out(s1))
    for (unsigned tt: g.univ_dests(t.dst))
      f += t.label * g.state_data(tt);

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

static bool f8()
{
  typedef spot::digraph<int_pair, int_pair> graph_t;
  graph_t g(3);
  spot::named_graph<graph_t, std::string> gg(g);
  auto s1 = gg.new_state("s1", 2, 4);
  gg.new_state("s2", 3, 6);
  gg.new_state("s3", 4, 8);
  gg.new_edge("s1", "s2", 1, 3);
  gg.new_edge("s1", "s3", 2, 5);
  gg.new_edge("s2", "s3", 3, 7);
  gg.new_edge("s3", "s2", 4, 9);

  dot(std::cout, gg);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.one * g.state_data(t.dst).one;
      f += t.two * g.state_data(t.dst).two;
    }
  return f == 69;
}

struct my_state: public spot::state
{
public:
  virtual ~my_state() noexcept
  {
  }

  my_state() noexcept
  {
  }

  my_state(const my_state&) noexcept
  {
  }

  my_state& operator=(const my_state&) noexcept
  {
    return *this;
  }

  int compare(const spot::state* other) const override
  {
    auto o = spot::down_cast<const my_state*>(other);

    // Do not simply return "other - this", it might not fit in an int.
    if (o < this)
      return -1;
    if (o > this)
      return 1;
    return 0;
  }

  size_t hash() const override
  {
    return
      reinterpret_cast<const char*>(this) - static_cast<const char*>(nullptr);
  }

  my_state* clone() const override
  {
    return const_cast<my_state*>(this);
  }

  void destroy() const override
  {
  }

  friend std::ostream& operator<<(std::ostream& os, const my_state&)
  {
    return os;
  }
};

static bool f9()
{
  typedef spot::digraph<my_state, int_pair> graph_t;
  graph_t g(3);
  spot::named_graph<graph_t, std::string> gg(g);
  auto s1 = gg.new_state("s1");
  gg.new_state("s2");
  auto s3 = gg.new_state("s3");
  gg.alias_state(s3, "s3b");

  gg.new_edge("s1", "s2", 1, 3);
  gg.new_edge("s1", "s3", 2, 5);
  gg.new_edge("s2", "s3b", 3, 7);
  gg.new_edge("s3", "s2", 4, 9);

  dot(std::cout, gg);

  int f = 0;
  for (auto& t: g.out(s1))
    {
      f += t.one + t.two;
    }


  return (f == 11) &&
    g.state_data(s1).compare(&g.state_data(gg.get_state("s1"))) == 0 &&
    g.state_data(s1).compare(&g.state_data(gg.get_state("s2"))) != 0;
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
  bool a9 = f9();
  std::cout << a1 << ' '
            << a2 << ' '
            << a3 << ' '
            << a4 << ' '
            << a5 << ' '
            << a6 << ' '
            << a7 << ' '
            << a8 << ' '
            << a9 << '\n';
  return !(a1 && a2 && a3 && a4 && a5 && a6 && a7 && a8 && a9);
}
