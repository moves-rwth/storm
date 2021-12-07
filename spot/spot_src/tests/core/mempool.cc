// -*- coding: utf-8 -*-
// Copyright (C) 2016, 2018 Laboratoire de Recherche et Développement
// de l'Epita.
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

#define SPOT_DEBUG 1

#include <spot/misc/fixpool.hh>
#include <spot/misc/mspool.hh>
#include <spot/priv/allocator.hh>

#include <set>

namespace
{
  struct boxint
  {
    int i;
  };

  class foo
  {
    boxint* b;
  public:
    foo(int i): b(new boxint{i}) {}
    ~foo() { delete b; }

    void incr() { ++b->i; }
  };

  // use a fixpool for allocation
  class bar
  {
    int i;

    static spot::fixed_size_pool& pool()
    {
      static spot::fixed_size_pool p{sizeof(bar)};
      return p;
    }

  public:
    bar(int i): i(i) {}

    static void* operator new(size_t)
    {
      return pool().allocate();
    }
    static void operator delete(void* ptr)
    {
      pool().deallocate(ptr);
    }

    void incr() { ++i; }
  };

  // use a mspool for allocation
  class baz
  {
    int i;

    static spot::multiple_size_pool& pool()
    {
      static spot::multiple_size_pool p{};
      return p;
    }

  public:
    baz(int i): i(i) {}

    static void* operator new(size_t)
    {
      return pool().allocate(sizeof(baz));
    }
    static void operator delete(void* ptr)
    {
      pool().deallocate(ptr, sizeof(baz));
    }

    void incr() { ++i; }
  };


} // anonymous namespace


int main()
{

#ifndef HAVE_VALGRIND_MEMCHECK_H
  return 77;
#endif

  {
    spot::fixed_size_pool p(sizeof(foo));

    foo* a = new (p.allocate()) foo(1);
    a->incr();
    // delete and deallocate, no problem
    a->~foo();
    p.deallocate(a);

    a = new (p.allocate()) foo(2);
    a->incr();
    // delete but do not deallocate: valgrind should find a leak
    a->~foo();

    a = new (p.allocate()) foo(3);
    a->incr();
    // deallocate but do not delete: valgrind should find a leak
    p.deallocate(a);
  }

  {
    spot::multiple_size_pool p;

    foo* a = new (p.allocate(sizeof(foo))) foo(1);
    a->incr();
    // delete and deallocate, no problem
    a->~foo();
    p.deallocate(a, sizeof(foo));

    a = new (p.allocate(sizeof(foo))) foo(2);
    a->incr();
    // delete but do not deallocate: valgrind should find a leak
    a->~foo();

    a = new (p.allocate(sizeof(foo))) foo(3);
    a->incr();
    // deallocate but do not delete: valgrind should find a leak
    p.deallocate(a, sizeof(foo));
  }

  {
    bar* b = new bar(1);
    b->incr();
    // no delete: valgrind should find a leak
  }
  {
    baz* c = new baz(1);
    c->incr();
    // no delete: valgrind should find a leak
  }
  {
    std::set<int, std::less<int>, spot::pool_allocator<int>> s;
    s.insert(1);
    s.insert(2);
    s.insert(1);
    s.erase(1);
    s.insert(3);
    s.insert(4);

    s.clear();

    auto t = s;
    t.insert(5);
    t.insert(6);

    std::swap(s, t);

    s.erase(5);
    s.erase(6);

    if (s != t)
      return 1;
    else
      return 0;
  }


  return 0;
}

