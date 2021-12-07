// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2015 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de
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

#include <string>
#include <iosfwd>
#include <spot/twa/bdddict.hh>
#include <bddx.h>

namespace spot
{

  /// \brief Print a BDD as a list of literals.
  ///
  /// This assumes that \a b is a conjunction of literals.
  /// \param os The output stream.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  SPOT_API std::ostream&
  bdd_print_sat(std::ostream& os, const bdd_dict_ptr& dict, bdd b);

  /// \brief Format a BDD as a list of literals.
  ///
  /// This assumes that \a b is a conjunction of literals.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::string
  bdd_format_sat(const bdd_dict_ptr& dict, bdd b);

  /// \brief Print a BDD as a set of acceptance conditions.
  ///
  /// This is used when saving a TGBA.
  /// \param os The output stream.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::ostream&
  bdd_print_accset(std::ostream& os, const bdd_dict_ptr& dict, bdd b);

  /// \brief Format a BDD as a set of acceptance conditions.
  ///
  /// This is used when saving a TGBA.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::string
  bdd_format_accset(const bdd_dict_ptr& dict, bdd b);

  /// \brief Print a BDD as a set.
  /// \param os The output stream.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  SPOT_API std::ostream&
  bdd_print_set(std::ostream& os, const bdd_dict_ptr& dict, bdd b);

  /// \brief Format a BDD as a set.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::string
  bdd_format_set(const bdd_dict_ptr& dict, bdd b);

  /// \brief Print a BDD as a formula.
  /// \param os The output stream.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  SPOT_API std::ostream&
  bdd_print_formula(std::ostream& os, const bdd_dict_ptr& dict, bdd b);

  /// \brief Format a BDD as a formula.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::string
  bdd_format_formula(const bdd_dict_ptr& dict, bdd b);

  /// \brief Enable UTF-8 output for bdd printers.
  SPOT_API void enable_utf8();


  /// \brief Format a BDD as an irredundant sum of product.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  /// \return The BDD formated as a string.
  SPOT_API std::string
  bdd_format_isop(const bdd_dict_ptr& dict, bdd b);


  /// \brief Print a BDD as an irredundant sum of product.
  /// \param os The output stream.
  /// \param dict The dictionary to use, to lookup variables.
  /// \param b The BDD to print.
  SPOT_API std::ostream&
  bdd_print_isop(std::ostream& os, const bdd_dict_ptr& dict, bdd b);
}
