// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2013-2015, 2019 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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
#include <spot/twa/taatgba.hh>

namespace spot
{
  /// \ingroup twa_ltl
  /// \brief Build a spot::taa* from an LTL formula.
  ///
  /// This is based on \cite tauriainen.06.tr .
  ///
  /// \param f The formula to translate into an automaton.
  /// \param dict The spot::bdd_dict the constructed automata should use.
  /// \param refined_rules If this parameter is set, refined rules are used.
  /// \return A spot::taa that recognizes the language of \a f.
  SPOT_API taa_tgba_formula_ptr
  ltl_to_taa(formula f, const bdd_dict_ptr& dict,
             bool refined_rules = false);
}
