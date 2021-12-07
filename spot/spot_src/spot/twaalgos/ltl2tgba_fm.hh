// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2017, 2019 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005, 2006 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <spot/twa/twagraph.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/simplify.hh>

namespace spot
{
  /// \ingroup twa_ltl
  /// \brief Build a spot::twa_graph_ptr from an LTL or PSL formula.
  ///
  /// This originally derived from an algorithm by Couvreur
  /// \cite couvreur.99.fm , but it has been improved in many
  /// ways \cite duret.14.ijccbs .
  ///
  /// \param f The formula to translate into an automaton.
  ///
  /// \param dict The spot::bdd_dict the constructed automata should use.
  ///
  /// \param exprop When set, the algorithm will consider all properties
  /// combinations possible on each state, in an attempt to reduce
  /// the non-determinism.  The automaton will have the same size as
  /// without this option, but because the transitions will be more
  /// deterministic, the product automaton will be smaller (or, at worse,
  /// equal).
  ///
  /// \param symb_merge When false, states with the same symbolic
  /// representation (these are equivalent formulae) will not be
  /// merged.
  ///
  /// \param branching_postponement When set, several transitions leaving
  /// from the same state with the same label (i.e., condition + acceptance
  /// conditions) will be merged.  \cite sebastiani.03.charme
  ///
  /// \param fair_loop_approx When set, a really simple characterization of
  /// unstable state is used to suppress all acceptance conditions from
  /// incoming transitions.
  ///
  /// \param unobs When non-zero, the atomic propositions in the LTL formula
  /// are interpreted as events that exclude each other.  The events in the
  /// formula are observable events, and \c unobs can be filled with
  /// additional unobservable events.
  ///
  /// \param simplifier If this parameter is set, the LTL formulae
  /// representing each state of the automaton will be simplified
  /// before computing the successor.  \a simpl should be configured
  /// for the type of reduction you want, see spot::tl_simplifier.
  /// This idea is taken from \cite thirioux.02.fmics .
  ///
  /// \param unambiguous When true, unambigous TGBA will be produced
  /// using the trick described in \cite benedikt.13.tacas .
  ///
  /// \return A spot::twa_graph that recognizes the language of \a f.
  SPOT_API twa_graph_ptr
  ltl_to_tgba_fm(formula f, const bdd_dict_ptr& dict,
                 bool exprop = false, bool symb_merge = true,
                 bool branching_postponement = false,
                 bool fair_loop_approx = false,
                 const atomic_prop_set* unobs = nullptr,
                 tl_simplifier* simplifier = nullptr,
                 bool unambiguous = false);
}
