// -*- coding: utf-8 -*-
// Copyright (C) 2009-2012, 2014-2016, 2018, 2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2006, 2007 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <spot/tl/contain.hh>
#include <spot/tl/simplify.hh>
#include <spot/tl/formula.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/are_isomorphic.hh>
#include <spot/misc/hash.hh>
#include <spot/priv/robin_hood.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>

namespace spot
{
  struct language_containment_checker::record_
  {
    const_twa_graph_ptr translation;
    typedef robin_hood::unordered_flat_map<const record_*, bool> incomp_map;
    incomp_map incompatible;

    record_(const_twa_graph_ptr&& trans) noexcept
      : translation(std::move(trans))
    {
    }
  };

  struct language_containment_checker::trans_map_ :
    robin_hood::unordered_node_map<formula,
                                   language_containment_checker::record_>
  {
  };

  language_containment_checker::language_containment_checker
  (bdd_dict_ptr dict, bool exprop, bool symb_merge,
   bool branching_postponement, bool fair_loop_approx)
    : dict_(dict), exprop_(exprop), symb_merge_(symb_merge),
      branching_postponement_(branching_postponement),
      fair_loop_approx_(fair_loop_approx),
      translated_(new trans_map_)
  {
  }

  language_containment_checker::~language_containment_checker()
  {
    clear();
    delete translated_;
  }

  void
  language_containment_checker::clear()
  {
    translated_->clear();
  }

  bool
  language_containment_checker::incompatible_(record_* l, record_* g)
  {
    record_::incomp_map::const_iterator i = l->incompatible.find(g);
    if (i != l->incompatible.end())
      return i->second;

    bool res = product(l->translation, g->translation)->is_empty();
    // bool res = !l->translation->intersects(g->translation);
    l->incompatible[g] = res;
    g->incompatible[l] = res;
    return res;
  }

  static void trim_common_Xs(formula& l, formula& r)
  {
    while (l.is(op::X) && r.is(op::X))
      {
        l = l[0];
        r = r[0];
      }
  }

  // Check whether L(l) is a subset of L(!g).
  bool
  language_containment_checker::contained_neg(formula l,
                                              formula g)
  {
    if (l == g)
      return false;
    trim_common_Xs(l, g);
    record_* rl = register_formula_(l);
    record_* rg = register_formula_(g);
    return incompatible_(rl, rg);
  }


  // Check whether L(l) is a subset of L(g).
  bool
  language_containment_checker::contained(formula l,
                                          formula g)
  {
    if (l == g)
      return true;
    return contained_neg(l, formula::Not(g));
  }


  // Check whether L(!l) is a subset of L(g).
  bool
  language_containment_checker::neg_contained(formula l,
                                              formula g)
  {
    if (l == g)
      return false;
    formula nl = formula::Not(l);
    return contained_neg(nl, formula::Not(g));
  }

  // Check whether L(l) = L(g).
  bool
  language_containment_checker::equal(formula l, formula g)
  {
    trim_common_Xs(l, g);
    if (l == g)
      return true;
    record_* rl = register_formula_(l);
    record_* rg = register_formula_(g);
    if (isomorphism_checker::are_isomorphic(rl->translation, rg->translation))
      return true;
    return contained(l, g) && contained(g, l);
  }

  language_containment_checker::record_*
  language_containment_checker::register_formula_(formula f)
  {
    auto i = translated_->find(f);
    if (i != translated_->end())
      return &i->second;

    auto e = ltl_to_tgba_fm(f, dict_, exprop_, symb_merge_,
                            branching_postponement_, fair_loop_approx_);
    return &translated_->emplace(f, std::move(e)).first->second;
  }
}
