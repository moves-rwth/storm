// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2018 Laboratoire de Recherche et Développement de
// l'Epita.
// Copyright (C) 2004, 2006  Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include "spot/priv/freelist.hh"
#include <cassert>
#include <iostream>

namespace spot
{

  free_list::~free_list()
  {
  }

  int
  free_list::register_n(int n)
  {
    // Browse the free list until we find N consecutive variables.  We
    // try not to fragment the list my allocating the variables in the
    // smallest free range we find.
    free_list_type::iterator best = fl.end();
    free_list_type::iterator cur;
    for (cur = fl.begin(); cur != fl.end(); ++cur)
      {
        if (cur->second < n)
          continue;
        if (n == cur->second)
          {
            best = cur;
            break;
          }
        if (best == fl.end()
            || cur->second < best->second)
          best = cur;
      }

    // We have found enough free variables.
    if (best != fl.end())
      {
        int result = best->first;
        remove(best, result, n);
        return result;
      }

    // We haven't found enough adjacent free variables;
    // ask for some more.
    return extend(n);
  }

  void
  free_list::insert(int base, int n)
  {
    free_list_type::iterator cur;
    int end = base + n;
    for (cur = fl.begin(); cur != fl.end(); ++cur)
      {
        int cend = cur->first + cur->second;
        // cur              [...]
        // to insert  [...]
        // -----------------------
        // result     [...] [...]
        // (Insert a new range, unconnected.)
        if (cur->first > end)
          {
            break;
          }
        // cur        [...]
        // to insert        [...]
        // -----------------------
        // result unknown : we should look at the rest of the freelist.
        else if (base > cend)
          {
            continue;
          }
        //  cur          [....[       [......[
        //  to insert      [....[       [..[
        //  ----------------------------------
        //  result       [......[     [......[
        else if (cur->first <= base)
          {
            if (cend >= end)
              // second case : nothing to do
              return;
            // cur->second is set below.
          }
        //  cur           [....[       [..[
        //  to insert   [....[       [.......[
        //  ----------------------------------
        //  result      [......[     [.......[
        else
          {
            cur->first = base;
            // cur->second is set below.
          }

        // We get here in one of these three situations:
        //
        //  cur          [....[      [....[    [..[
        //  to insert      [....[  [....[    [.......[
        //  -------------------------------------------
        //  result       [......[  [......[  [.......[
        //
        // cur->first is already set, be cur->second has yet to be.
        end = std::max(cend, end);
        cur->second = end - cur->first;
        // Since we have extended the current range, maybe the next
        // items on the list should be merged.
        free_list_type::iterator next = cur;
        ++next;
        while (next != fl.end() && next->first <= end)
          {
            end = std::max(next->first + next->second, end);
            cur->second = end - cur->first;
            free_list_type::iterator next2 = next++;
            fl.erase(next2);
          }
        return;
      }

    // We reach this place either because a new unconnected range
    // should be inserted in the middle of FL, or at the end.
    fl.insert(cur, pos_lenght_pair(base, n));
  }

  void
  free_list::remove(int base, int n)
  {
    free_list_type::iterator cur = fl.begin();
    int end = base + n;
    while (cur != fl.end() && cur->first < end)
      {
        int cend = cur->first + cur->second;
        // Remove may invalidate the current iterator, so advance it first.
        free_list_type::iterator old = cur++;
        if (cend >= base)
          {
            int newbase = std::max(base, old->first);
            int q = std::min(cend, end) - newbase;
            remove(old, newbase, q);
          }
      }
  }

  void
  free_list::remove(free_list_type::iterator i, int base, int n)
  {
    if (base == i->first)
      {
        // Removing at the beginning of the range
        i->second -= n;
        assert(i->second >= 0);
        // Erase the range if it's now empty.
        if (i->second == 0)
          fl.erase(i);
        else
          i->first += n;
      }
    else if (base + n == i->first + i->second)
      {
        // Removing at the end of the range
        i->second -= n;
        assert(i->second > 0); // cannot be empty because base != i->first
      }
    else
      {
        // Removing in the middle of a range.
        int b1 = i->first;
        int n1 = base - i->first;
        int n2 = i->first + i->second - base - n;
        assert(n1 > 0);
        assert(n2 > 0);
        *i = pos_lenght_pair(base + n, n2);
        fl.insert(i, pos_lenght_pair(b1, n1));
      }
  }

  void
  free_list::release_n(int base, int n)
  {
    insert(base, n);
  }

  std::ostream&
  free_list::dump_free_list(std::ostream& os) const
  {
    free_list_type::const_iterator i;
    for (i = fl.begin(); i != fl.end(); ++i)
      os << "  (" << i->first << ", " << i->second << ')';
    return os;
  }

  int
  free_list::free_count() const
  {
    int res = 0;
    free_list_type::const_iterator i;
    for (i = fl.begin(); i != fl.end(); ++i)
      res += i->second;
    return res;
  }

}
