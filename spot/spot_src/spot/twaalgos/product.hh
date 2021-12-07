// -*- coding: utf-8 -*-
// Copyright (C) 2014-2015, 2018-2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#include <spot/misc/common.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/powerset.hh>
#include <vector>
#include <utility>

namespace spot
{
  /// \brief Automata constructed by product() contain a property
  /// named "product-states" with this type
  typedef std::vector<std::pair<unsigned, unsigned>> product_states;

  /// \ingroup twa_algorithms
  /// \brief Intersect two automata using a synchronous product
  ///
  /// The resulting automaton will accept the intersection of both
  /// languages and have an acceptance condition that is the
  /// conjunction of the acceptance conditions of the two input
  /// automata.  In case one of the left or right automaton is weak,
  /// the acceptance condition of the result is made simpler: it
  /// usually is the acceptance condition of the other argument,
  /// therefore avoiding the need to introduce new accepting sets.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  ///
  /// If an \a aborter is given, the function will return nullptr
  /// whenever the resulting product would be too large.
  SPOT_API
  twa_graph_ptr product(const const_twa_graph_ptr& left,
                        const const_twa_graph_ptr& right,
                        const output_aborter* aborter = nullptr);

  /// \ingroup twa_algorithms
  /// \brief Intersect two automata using a synchronous product
  ///
  /// This variant allows changing the initial state of both automata
  /// in case you want to start the product at a different place.
  ///
  /// The resulting automaton will accept the intersection of the
  /// languages recognized by each input automaton (with its initial
  /// state changed) and have an acceptance condition that is the
  /// conjunction of the acceptance conditions of the two input
  /// automata.  In case one of the left or right automaton is weak,
  /// the acceptance condition of the result is made simpler: it
  /// usually is the acceptance condition of the other argument,
  /// therefore avoiding the need to introduce new accepting sets.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  ///
  /// If an \a aborter is given, the function will return nullptr
  /// whenever the resulting product would be too large.
  SPOT_API
  twa_graph_ptr product(const const_twa_graph_ptr& left,
                        const const_twa_graph_ptr& right,
                        unsigned left_state,
                        unsigned right_state,
                        const output_aborter* aborter = nullptr);

  /// \ingroup twa_algorithms
  /// \brief Sum two automata using a synchronous product
  ///
  /// The resulting automaton will accept the union of both
  /// languages and have an acceptance condition that is the
  /// disjunction of the acceptance conditions of the two input
  /// automata.  In case one of the left or right automaton is weak,
  /// the acceptance condition of the result is made simpler: it
  /// usually is the acceptance condition of the other argument,
  /// therefore avoiding the need to introduce new accepting sets.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  SPOT_API
  twa_graph_ptr product_or(const const_twa_graph_ptr& left,
                           const const_twa_graph_ptr& right);

  /// \ingroup twa_algorithms
  /// \brief Sum two automata using a synchronous product
  ///
  /// This variant allows changing the initial state of both automata
  /// in case you want to start the product at a different place.
  ///
  /// The resulting automaton will accept the sum of the languages
  /// recognized by each input automaton (with its initial state
  /// changed) and have an acceptance condition that is the
  /// disjunction of the acceptance conditions of the two input
  /// automata. In case one of the left or right automaton is weak,
  /// the acceptance condition of the result is made simpler: it
  /// usually is the acceptance condition of the other argument,
  /// therefore avoiding the need to introduce new accepting sets.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  SPOT_API
  twa_graph_ptr product_or(const const_twa_graph_ptr& left,
                           const const_twa_graph_ptr& right,
                           unsigned left_state,
                           unsigned right_state);

  /// \ingroup twa_algorithms
  /// \brief XOR two deterministic automata using a synchronous product
  ///
  /// The two operands must be deterministic.
  ///
  /// The resulting automaton will accept the symmetric difference of
  /// both languages and have an acceptance condition that is the xor
  /// of the acceptance conditions of the two input automata.  In case
  /// both operands are weak, the acceptance condition of the result
  /// is made simpler.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  SPOT_API
  twa_graph_ptr product_xor(const const_twa_graph_ptr& left,
                            const const_twa_graph_ptr& right);

  /// \ingroup twa_algorithms
  /// \brief XNOR two automata using a synchronous product
  ///
  /// The two operands must be deterministic.
  ///
  /// The resulting automaton will accept words that are either in
  /// both input languages, or not in both languages. (The XNOR gate
  /// it the logical complement of XOR.  XNOR is also known as logical
  /// equivalence.)  The output will have an acceptance condition that
  /// is the XNOR of the acceptance conditions of the two input
  /// automata.  In case both the operands are weak, the acceptance
  /// condition of the result is made simpler.
  ///
  /// The algorithm also defines a named property called
  /// "product-states" with type spot::product_states.  This stores
  /// the pair of original state numbers associated to each state of
  /// the product.
  SPOT_API
  twa_graph_ptr product_xnor(const const_twa_graph_ptr& left,
                             const const_twa_graph_ptr& right);

  /// \ingroup twa_algorithms
  /// \brief Build the product of an automaton with a suspendable
  /// automaton.
  ///
  /// The language of this product is the intersection of the
  /// languages of both input automata.
  ///
  /// This function *assumes* that \a right_susp is a suspendable
  /// automaton, i.e., its language L satisfies L = Σ*.L.
  /// Therefore the product between the two automata need only be done
  /// with the accepting SCCs of left.
  ///
  /// If \a left is a weak automaton, the acceptance condition of the
  /// output will be that of \a right_susp.  Otherwise the acceptance
  /// condition is the conjunction of both acceptances.
  SPOT_API
  twa_graph_ptr product_susp(const const_twa_graph_ptr& left,
                             const const_twa_graph_ptr& right_susp);

  /// \ingroup twa_algorithms
  /// \brief Build the "or" product of an automaton with a suspendable
  /// automaton.
  ///
  /// The language of this product is the union of the languages of
  /// both input automata.
  ///
  /// This function *assumes* that \a right_susp is a suspendable
  /// automaton, i.e., its language L satisfies L = Σ*.L.
  /// Therefore, after left has been completed (this will be done by
  /// product_or_susp) the product between the two automata need only
  /// be done with the SCCs of left that contains some rejecting cycles.
  ///
  /// The current implementation is currently suboptimal as instead of
  /// looking for SCC with rejecting cycles, it simply loop for
  /// non-trivial SCC, (or in the case of weak automata, with
  /// non-trivial and rejecting SCCs).
  SPOT_API
  twa_graph_ptr product_or_susp(const const_twa_graph_ptr& left,
                                const const_twa_graph_ptr& right_susp);
}
