// -*- coding: utf-8 -*-
// Copyright (C) 2011-2017, 2019, 2020 Laboratoire de Recherche et Developpement
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

#pragma once

#include <spot/tl/formula.hh>
#include <bddx.h>
#include <spot/twa/bdddict.hh>
#include <iosfwd>

namespace spot
{
  class tl_simplifier_options
  {
  public:
    tl_simplifier_options(bool basics = true,
                          bool synt_impl = true,
                          bool event_univ = true,
                          bool containment_checks = false,
                          bool containment_checks_stronger = false,
                          bool nenoform_stop_on_boolean = false,
                          bool reduce_size_strictly = false,
                          bool boolean_to_isop = false,
                          bool favor_event_univ = false,
                          bool keep_top_xor = false)
      : reduce_basics(basics),
        synt_impl(synt_impl),
        event_univ(event_univ),
        containment_checks(containment_checks),
        containment_checks_stronger(containment_checks_stronger),
        nenoform_stop_on_boolean(nenoform_stop_on_boolean),
        reduce_size_strictly(reduce_size_strictly),
        boolean_to_isop(boolean_to_isop),
        favor_event_univ(favor_event_univ),
        keep_top_xor(keep_top_xor)
    {
    }

    tl_simplifier_options(int level) :
      tl_simplifier_options(false, false, false)
    {
      switch (level)
        {
        case 3:
          containment_checks = true;
          containment_checks_stronger = true;
          SPOT_FALLTHROUGH;
        case 2:
          synt_impl = true;
          SPOT_FALLTHROUGH;
        case 1:
          reduce_basics = true;
          event_univ = true;
          SPOT_FALLTHROUGH;
        default:
          break;
        }
    }

    bool reduce_basics;
    bool synt_impl;
    bool event_univ;
    bool containment_checks;
    bool containment_checks_stronger;
    // If true, Boolean subformulae will not be put into
    // negative normal form.
    bool nenoform_stop_on_boolean;
    // If true, some rules that produce slightly larger formulae
    // will be disabled.  Those larger formulae are normally easier
    // to translate, so we recommend to set this to false.
    bool reduce_size_strictly;
    // If true, Boolean subformulae will be rewritten in ISOP form.
    bool boolean_to_isop;
    // Try to isolate subformulae that are eventual and universal.
    bool favor_event_univ;
    // Keep Xor and Equiv at the top of the formula, possibly under
    // &,|, and X operators.  Only rewrite Xor and Equiv under
    // temporal operators.
    bool keep_top_xor;
  };

  // fwd declaration to hide technical details.
  class tl_simplifier_cache;

  /// \ingroup tl_rewriting
  /// \brief Rewrite or simplify \a f in various ways.
  class SPOT_API tl_simplifier
  {
  public:
    tl_simplifier(const bdd_dict_ptr& dict = make_bdd_dict());
    tl_simplifier(const tl_simplifier_options& opt,
                   bdd_dict_ptr dict = make_bdd_dict());
    ~tl_simplifier();

    /// Simplify the formula \a f (using options supplied to the
    /// constructor).
    formula simplify(formula f);

#ifndef SWIG
    /// The simplifier options.
    ///
    /// Those should can still be changed before the first formula is
    /// simplified.
    tl_simplifier_options& options();
#endif

    /// Build the negative normal form of formula \a f.
    /// All negations of the formula are pushed in front of the
    /// atomic propositions.  Operators <=>, =>, xor are all removed
    /// (calling spot::unabbreviate for those is not needed).
    ///
    /// \param f The formula to normalize.
    /// \param negated If \c true, return the negative normal form of
    ///        \c !f
    formula
      negative_normal_form(formula f, bool negated = false);

    /// \brief Syntactic implication.
    ///
    /// Returns whether \a f syntactically implies \a g.
    ///
    /// This is adapted from the rules of Somenzi and
    /// Bloem. \cite somenzi.00.cav
    bool syntactic_implication(formula f, formula g);
    /// \brief Syntactic implication with one negated argument.
    ///
    /// If \a right is true, this method returns whether
    /// \a f implies !\a g.  If \a right is false, this returns
    /// whether !\a f implies \a g.
    bool syntactic_implication_neg(formula f, formula g,
                                   bool right);

    /// \brief check whether two formulae are equivalent.
    ///
    /// This costly check performs up to four translations,
    /// two products, and two emptiness checks.
    bool are_equivalent(formula f, formula g);


    /// \brief Check whether \a f implies \a g.
    ///
    /// This operation is costlier than syntactic_implication()
    /// because it requires two translation, one product and one
    /// emptiness check.
    bool implication(formula f, formula g);

    /// \brief Convert a Boolean formula as a BDD.
    ///
    /// If you plan to use this method, be sure to pass a bdd_dict
    /// to the constructor.
    bdd as_bdd(formula f);

    /// \brief Clear the as_bdd() cache.
    ///
    /// Calling this function is recommended before running other
    /// algorithms that create BDD variables in a more natural
    /// order.  For instance ltl_to_tgba_fm() will usually be more
    /// efficient if the BDD variables for atomic propositions have
    /// not been ordered before hand.
    ///
    /// This also clears the language containment cache.
    void clear_as_bdd_cache();

    /// \brief Clear all caches.
    ///
    /// This empties all the cache used by the simplifier.
    void clear_caches();

    /// Return the bdd_dict used.
    bdd_dict_ptr get_dict() const;

    /// Cached version of spot::star_normal_form().
    formula star_normal_form(formula f);

    /// \brief Rewrite a Boolean formula \a f into as an irredundant
    /// sum of product.
    ///
    /// This uses a cache, so it is OK to call this with identical
    /// arguments.
    formula boolean_to_isop(formula f);

    /// Dump statistics about the caches.
    void print_stats(std::ostream& os) const;

  private:
    tl_simplifier_cache* cache_;
    // Copy disallowed.
    tl_simplifier(const tl_simplifier&) = delete;
    void operator=(const tl_simplifier&) = delete;
  };
}
