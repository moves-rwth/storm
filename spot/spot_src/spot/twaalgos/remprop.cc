// -*- coding: utf-8 -*-
// Copyright (C) 2015-2019 Laboratoire de Recherche et Développement
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

#include "config.h"
#include <spot/twaalgos/remprop.hh>
#include <spot/twaalgos/mask.hh>
#include <spot/misc/casts.hh>
#include <ctype.h>
#include <sstream>

namespace spot
{
  namespace
  {
    static
    void unexpected_char(const char* arg, const char* pos)
    {
      std::ostringstream out;
      out << "unexpected ";
      if (isprint(*pos))
        out << '\'' << *pos << '\'';
      else
        out << "character";
      out << " at position " << pos - arg << " in '";
      out << arg << '\'';
      throw std::invalid_argument(out.str());
    }
  }


  void remove_ap::add_ap(const char* arg)
  {
    auto start = arg;
    while (*start)
      {
        while (*start == ' ' || *start == '\t')
          ++start;
        if (!*start)
          break;
        if (*start == ',' || *start == '=')
          unexpected_char(arg, start);
        formula the_ap = nullptr;

        if (*start == '"')
          {
            auto end = ++start;
            while (*end && *end != '"')
              {
                if (*end == '\\')
                  ++end;
                ++end;
              }
            if (!*end)
              {
                std::string s = "missing closing '\"' in ";
                s += arg;
                throw std::invalid_argument(s);
              }
            std::string ap(start, end - start);
            the_ap = formula::ap(ap);
            do
              ++end;
            while (*end == ' ' || *end == '\t');
            start = end;
          }
        else
          {
            auto end = start;
            while (*end && *end != ',' && *end != '=')
              ++end;
            auto rend = end;
            while (rend > start && (rend[-1] == ' ' || rend[-1] == '\t'))
              --rend;
            std::string ap(start, rend - start);
            the_ap = formula::ap(ap);
            start = end;
          }
        if (*start)
          {
            if (!(*start == ',' || *start == '='))
              unexpected_char(arg, start);
            if (*start == '=')
              {
                do
                  ++start;
                while (*start == ' ' || *start == '\t');
                if (*start == '0')
                  props_neg.insert(the_ap);
                else if (*start == '1')
                  props_pos.insert(the_ap);
                else
                  unexpected_char(arg, start);
                the_ap = nullptr;
                do
                  ++start;
                while (*start == ' ' || *start == '\t');
              }
            if (*start)
              {
                if (*start != ',')
                  unexpected_char(arg, start);
                ++start;
              }
          }
        if (the_ap)
          props_exist.insert(the_ap);
      }
  }

  twa_graph_ptr remove_ap::strip(const_twa_graph_ptr aut) const
  {
    bdd restrict_ = bddtrue;
    bdd exist = bddtrue;
    auto d = aut->get_dict();

    twa_graph_ptr res = make_twa_graph(d);
    res->copy_ap_of(aut);
    res->prop_copy(aut, { true, true, false, false, false, false });
    res->copy_acceptance_of(aut);

    for (auto ap: props_exist)
      {
        int v = d->has_registered_proposition(ap, aut);
        if (v >= 0)
          {
            exist &= bdd_ithvar(v);
            res->unregister_ap(v);
          }
      }
    for (auto ap: props_pos)
      {
        int v = d->has_registered_proposition(ap, aut);
        if (v >= 0)
          {
            restrict_ &= bdd_ithvar(v);
            res->unregister_ap(v);
          }
      }
    for (auto ap: props_neg)
      {
        int v = d->has_registered_proposition(ap, aut);
        if (v >= 0)
          {
            restrict_ &= bdd_nithvar(v);
            res->unregister_ap(v);
          }
      }

    if (res->prop_terminal().is_false())
      // Non-terminal automata could become terminal.
      res->prop_terminal(trival::maybe());

    transform_accessible(aut, res, [&](unsigned, bdd& cond,
                                       acc_cond::mark_t&, unsigned)
                         {
                           cond = bdd_restrict(bdd_exist(cond, exist),
                                               restrict_);
                         });
    return res;
  }
}
