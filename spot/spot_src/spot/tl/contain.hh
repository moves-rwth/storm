// -*- coding: utf-8 -*-
// Copyright (C) 2011-2016, 2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2006 Laboratoire d'Informatique de Paris 6 (LIP6),
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

#pragma once

#include <spot/tl/formula.hh>
#include <spot/twa/bdddict.hh>

namespace spot
{
  class tl_simplifier_cache;

  /// Check containment between LTL formulae.
  class SPOT_API language_containment_checker
  {
    struct record_;
    struct trans_map_;
  public:
    /// This class uses spot::ltl_to_tgba_fm to translate LTL
    /// formulae.  See that function for the meaning of these options.
    language_containment_checker(bdd_dict_ptr dict = make_bdd_dict(),
                                 bool exprop = false,
                                 bool symb_merge = true,
                                 bool branching_postponement = false,
                                 bool fair_loop_approx = false);

    ~language_containment_checker();

    /// Clear the cache.
    void clear();

    /// Check whether L(l) is a subset of L(g).
    bool contained(formula l, formula g);
    /// Check whether L(!l) is a subset of L(g).
    bool neg_contained(formula l, formula g);
    /// Check whether L(l) is a subset of L(!g).
    bool contained_neg(formula l, formula g);

    /// Check whether L(l) = L(g).
    bool equal(formula l, formula g);

  protected:
    bool incompatible_(record_* l, record_* g);

    record_* register_formula_(formula f);

    /* Translation options */
    bdd_dict_ptr dict_;
    bool exprop_;
    bool symb_merge_;
    bool branching_postponement_;
    bool fair_loop_approx_;
    /* Translation Maps */
    trans_map_* translated_;
    tl_simplifier_cache* c_;
  };
}
