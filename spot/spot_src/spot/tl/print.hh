// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <iosfwd>

namespace spot
{
  /// \addtogroup tl_io
  /// @{

  /// \brief Output a PSL formula as a string which is parsable.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_psl(std::ostream& os, formula f, bool full_parent = false);

  /// \brief Convert a PSL formula into a string which is parsable
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_psl(formula f, bool full_parent = false);

  /// \brief Output a PSL formula as an utf-8 string which is parsable.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_utf8_psl(std::ostream& os, formula f,
                 bool full_parent = false);

  /// \brief Convert a PSL formula into a utf-8 string which is parsable
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_utf8_psl(formula f, bool full_parent = false);

  /// \brief Output a SERE formula as a string which is parsable.
  /// \param f The formula to translate.
  /// \param os The stream where it should be output.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_sere(std::ostream& os, formula f, bool full_parent = false);

  /// \brief Convert a SERE formula into a string which is parsable
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_sere(formula f, bool full_parent = false);

  /// \brief Output a SERE formula as a utf-8 string which is parsable.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_utf8_sere(std::ostream& os, formula f,
                  bool full_parent = false);

  /// \brief Convert a SERE formula into a string which is parsable
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_utf8_sere(formula f, bool full_parent = false);

  /// \brief Output an LTL formula as a string parsable by Spin.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_spin_ltl(std::ostream& os, formula f,
                 bool full_parent = false);

  /// \brief Convert an LTL formula into a string parsable by Spin.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_spin_ltl(formula f, bool full_parent = false);

  /// \brief Output an LTL formula as a string parsable by Wring.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  SPOT_API std::ostream&
  print_wring_ltl(std::ostream& os, formula f);

  /// \brief Convert a formula into a string parsable by Wring
  /// \param f The formula to translate.
  SPOT_API std::string
  str_wring_ltl(formula f);

  /// \brief Output a PSL formula as a LaTeX string.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_latex_psl(std::ostream& os, formula f,
                  bool full_parent = false);

  /// \brief Output a formula as a LaTeX string which is parsable.
  /// unless the formula contains automaton operators (used in ELTL formulae).
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_latex_psl(formula f, bool full_parent = false);

  /// \brief Output a SERE formula as a LaTeX string.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_latex_sere(std::ostream& os, formula f,
                   bool full_parent = false);

  /// \brief Output a SERE formula as a LaTeX string which is parsable.
  /// unless the formula contains automaton operators (used in ELTL formulae).
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_latex_sere(formula f, bool full_parent = false);

  /// \brief Output a PSL formula as a self-contained LaTeX string.
  ///
  /// The result cannot be parsed back.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_sclatex_psl(std::ostream& os, formula f,
                    bool full_parent = false);

  /// \brief Output a PSL formula as a self-contained LaTeX string.
  ///
  /// The result cannot be parsed bacl.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_sclatex_psl(formula f, bool full_parent = false);

  /// \brief Output a SERE formula as a self-contained LaTeX string.
  ///
  /// The result cannot be parsed back.
  /// \param os The stream where it should be output.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::ostream&
  print_sclatex_sere(std::ostream& os, formula f,
                     bool full_parent = false);

  /// \brief Output a SERE formula as a self-contained LaTeX string.
  ///
  /// The result cannot be parsed bacl.
  /// \param f The formula to translate.
  /// \param full_parent Whether or not the string should by fully
  ///                           parenthesized.
  SPOT_API std::string
  str_sclatex_sere(formula f, bool full_parent = false);

  /// \brief Output an LTL formula as a string in LBT's format.
  ///
  /// The formula must be an LTL formula (ELTL and PSL operators
  /// are not supported).  The M and W operator will be output
  /// as-is, because this is accepted by LBTT, however if you
  /// plan to use the output with other tools, you should probably
  /// rewrite these two operators using unabbreviate_wm().
  ///
  /// \param f The formula to translate.
  /// \param os The stream where it should be output.
  SPOT_API std::ostream&
  print_lbt_ltl(std::ostream& os, formula f);

  /// \brief Output an LTL formula as a string in LBT's format.
  ///
  /// The formula must be an LTL formula (ELTL and PSL operators
  /// are not supported).  The M and W operator will be output
  /// as-is, because this is accepted by LBTT, however if you
  /// plan to use the output with other tools, you should probably
  /// rewrite these two operators using unabbreviate_wm().
  ///
  /// \param f The formula to translate.
  SPOT_API std::string
  str_lbt_ltl(formula f);
  /// @}
}
