// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2016, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <ostream>
#include <spot/twaalgos/gtec/status.hh>

namespace spot
{
  couvreur99_check_status::couvreur99_check_status(const const_twa_ptr& aut)
    : aut(aut)
  {
  }

  couvreur99_check_status::~couvreur99_check_status()
  {
    auto i = h.begin();
    while (i != h.end())
      {
        // Advance the iterator before deleting the key.
        const state* s = i->first;
        ++i;
        s->destroy();
      }
  }

  void
  couvreur99_check_status::print_stats(std::ostream& os) const
  {
    os << h.size() << " unique states visited" << std::endl;
    os << root.size()
       << " strongly connected components in search stack\n";
  }

  int
  couvreur99_check_status::states() const
  {
    return h.size();
  }
}
