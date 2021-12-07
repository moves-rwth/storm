// -*- coding: utf-8 -*-
// Copyright (C) 2018, 2019 Laboratoire de Recherche et Développement de
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
#include <spot/twaalgos/contains.hh>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/isdet.hh>

namespace spot
{
  namespace
  {
    static spot::const_twa_graph_ptr
    translate(formula f, const bdd_dict_ptr& dict)
    {
      return ltl_to_tgba_fm(f, dict);
    }
  }

  bool contains(const_twa_graph_ptr left, const_twa_graph_ptr right)
  {
    return !complement(left)->intersects(right);
  }

  bool contains(const_twa_graph_ptr left, formula right)
  {
    return contains(left, translate(right, left->get_dict()));
  }

  bool contains(formula left, const_twa_graph_ptr right)
  {
    return !translate(formula::Not(left), right->get_dict())->intersects(right);
  }

  bool contains(formula left, formula right)
  {
    return contains(left, translate(right, make_bdd_dict()));
  }

  bool are_equivalent(const_twa_graph_ptr left, const_twa_graph_ptr right)
  {
    // Start with a deterministic automaton at right if possible to
    // avoid a determinization (in case the first containment check
    // fails).
    if (!is_deterministic(right))
      std::swap(left, right);
    return contains(left, right) && contains(right, left);
  }

  bool are_equivalent(const_twa_graph_ptr left, formula right)
  {
    // The first containement check does not involve a
    // complementation, the second might.
    return contains(left, right) && contains(right, left);
  }

  bool are_equivalent(formula left, const_twa_graph_ptr right)
  {
    // The first containement check does not involve a
    // complementation, the second might.
    return contains(right, left) && contains(left, right);
  }

  bool are_equivalent(formula left, formula right)
  {
    return contains(right, left) && contains(left, right);
  }
}
