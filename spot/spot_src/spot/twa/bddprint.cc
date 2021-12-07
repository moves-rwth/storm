// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2012, 2014, 2015, 2018, 2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <sstream>
#include <cassert>
#include <ostream>
#include <spot/twa/bddprint.hh>
#include <spot/tl/print.hh>
#include <spot/twa/formula2bdd.hh>
#include <spot/misc/minato.hh>

namespace spot
{
  /// Global dictionary used by print_handler() to lookup variables.
  static bdd_dict* dict;

  /// Global flag to enable Acc[x] output (instead of `x').
  static bool want_acc;

  /// Global flag to enable UTF-8 output.
  static bool utf8;

  static
  std::ostream& print_(std::ostream& o, formula f)
  {
    if (utf8)
      print_utf8_psl(o, f);
    else
      print_psl(o, f);
    return o;
  }

  /// Stream handler used by Buddy to display BDD variables.
  static void
  print_handler(std::ostream& o, int v)
  {
    assert(unsigned(v) < dict->bdd_map.size());
    const bdd_dict::bdd_info& ref = dict->bdd_map[v];
    switch (ref.type)
      {
      case bdd_dict::var:
        print_(o, ref.f);
        break;
      case bdd_dict::acc:
        if (want_acc)
          {
            o << "Acc[";
            print_(o, ref.f) << ']';
          }
        else
          {
            o << '"';
            print_(o, ref.f) << '"';
          }
        break;
      case bdd_dict::anon:
        o << '?' << v;
      }
  }


  static std::ostream* where;
  static void
  print_sat_handler(signed char* varset, int size)
  {
    bool not_first = false;
    for (int v = 0; v < size; ++v)
      {
        if (varset[v] < 0)
          continue;
        if (not_first)
          *where << ' ';
        else
          not_first = true;
        if (varset[v] == 0)
          *where << "! ";
        print_handler(*where, v);
      }
  }

  std::ostream&
  bdd_print_sat(std::ostream& os, const bdd_dict_ptr& d, bdd b)
  {
    dict = d.get();
    where = &os;
    want_acc = false;
    assert(bdd_satone(b) == b);
    bdd_allsat(b, print_sat_handler);
    return os;
  }

  static bool first_done = false;
  static void
  print_accset_handler(signed char* varset, int size)
  {
    for (int v = 0; v < size; ++v)
      if (varset[v] > 0)
        {
          *where << (first_done ? ", " : "{");
          print_handler(*where, v);
          first_done = true;
        }
  }

  std::ostream&
  bdd_print_accset(std::ostream& os, const bdd_dict_ptr& d, bdd b)
  {
    dict = d.get();
    where = &os;
    want_acc = true;
    first_done = false;
    bdd_allsat(b, print_accset_handler);
    if (first_done)
      *where << '}';
    return os;
  }

  std::string
  bdd_format_accset(const bdd_dict_ptr& d, bdd b)
  {
    std::ostringstream os;
    bdd_print_accset(os, d, b);
    return os.str();
  }

  std::string
  bdd_format_sat(const bdd_dict_ptr& d, bdd b)
  {
    std::ostringstream os;
    bdd_print_sat(os, d, b);
    return os.str();
  }

  std::ostream&
  bdd_print_set(std::ostream& os, const bdd_dict_ptr& d, bdd b)
  {
    dict = d.get();
    want_acc = true;
    bdd_strm_hook(print_handler);
    os << bddset << b;
    bdd_strm_hook(nullptr);
    return os;
  }

  std::string
  bdd_format_set(const bdd_dict_ptr& d, bdd b)
  {
    std::ostringstream os;
    bdd_print_set(os, d, b);
    return os.str();
  }

  std::ostream&
  bdd_print_formula(std::ostream& os, const bdd_dict_ptr& d, bdd b)
  {
    return print_(os, bdd_to_formula(b, d));
  }

  std::string
  bdd_format_formula(const bdd_dict_ptr& d, bdd b)
  {
    std::ostringstream os;
    bdd_print_formula(os, d, b);
    return os.str();
  }

  void
  enable_utf8()
  {
    utf8 = true;
  }

  std::ostream&
  bdd_print_isop(std::ostream& os, const bdd_dict_ptr& d, bdd b)
  {
    dict = d.get();
    want_acc = true;
    minato_isop isop(b);

    bdd p;
    while ((p = isop.next()) != bddfalse)
      {
        os << bdd_format_set(d, p);
      }
    return os;
  }

  std::string
  bdd_format_isop(const bdd_dict_ptr& d, bdd b)
  {
    std::ostringstream os;
    bdd_print_isop(os, d, b);
    return os.str();
  }
}
