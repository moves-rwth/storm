// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017 Laboratoire
// de Recherche et Développement de l'Epita (LRDE).
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
#include <spot/misc/location.hh>
#include <spot/tl/defaultenv.hh>
#include <string>
#include <list>
#include <utility>
#include <iosfwd>

namespace spot
{
  /// \addtogroup tl_io
  /// @{

#ifndef SWIG
  /// \brief A parse diagnostic with its location.
  typedef std::pair<location, std::string> one_parse_error;
  /// \brief A list of parser diagnostics, as filled by parse.
  typedef std::list<one_parse_error> parse_error_list;
#else
  // Turn parse_error_list into an opaque type for Swig.
  struct parse_error_list {};
#endif

  /// \brief The result of a formula parser.
  struct SPOT_API parsed_formula final
  {
    /// \brief The parsed formula.
    ///
    /// This could be formula(nullptr) in case of a serious parse error.
    formula f = nullptr;

    /// The input text, before parsing.
    std::string input;

    /// \brief Syntax errors that occurred during parsing.
    ///
    /// Note that the parser does not print any diagnostic.
    /// Deciding how to output those errors is up to you.
    ///
    /// \see format_errors
    parse_error_list errors;

    parsed_formula(const std::string& str = "")
      : input(str)
    {
    }

    /// \brief Format diagnostics.
    ///
    /// \param os Where diagnostics should be output.
    /// \return \c true iff any diagnostic was output.
    bool format_errors(std::ostream& os);

    /// \brief Format shifted diagnostics.
    ///
    /// If the user input was something like "formula = a U b;" but you
    /// only passed "a U b" to the parser, it might be convenient to
    /// display the diagnostic in the context of "formula = a U b;".
    ///
    /// So pass the real input as \a input, and specify the number
    /// of character to skip before the actual text passed to the
    /// parser starts.
    ///
    /// This procedure assumes that the text passed to the parser
    /// appears as-is in the input string.  If you had to un-escape it
    /// in any way, the error locations will be wrong.
    ///
    /// \param os Where diagnostics should be output.
    /// \param input the real input string
    /// \param shift how many characters to add to the error locations
    /// \return \c true iff any diagnostic was output.
    bool format_errors(std::ostream& os,
                       const std::string& input,
                       unsigned shift);
  };


  /// \brief Build a formula from an LTL string.
  /// \param ltl_string The string to parse.
  /// \param env The environment into which parsing should take place.
  /// \param debug When true, causes the parser to trace its execution.
  /// \param lenient When true, parenthesized blocks that cannot be
  ///                parsed as subformulas will be considered as
  ///                atomic propositions.
  /// \return A formula built from \a ltl_string, or
  ///         formula(nullptr) if the input was unparsable.
  ///
  ///
  /// Note that the parser usually tries to recover from errors.  The
  /// field parsed_formula::f in the returned object can be a non-zero
  /// value even if it encountered error during the parsing of \a
  /// ltl_string.  If you want to make sure \a ltl_string was parsed
  /// succesfully, check \a parsed_formula::errors for emptiness.
  ///
  /// \warning This function is not reentrant.
  SPOT_API
  parsed_formula parse_infix_psl(const std::string& ltl_string,
                                 environment& env =
                                 default_environment::instance(),
                                 bool debug = false,
                                 bool lenient = false);

  /// \brief Build a Boolean formula from a string.
  /// \param ltl_string The string to parse.
  /// \param env The environment into which parsing should take place.
  /// \param debug When true, causes the parser to trace its execution.
  /// \param lenient When true, parenthesized blocks that cannot be
  ///                parsed as subformulas will be considered as
  ///                atomic propositions.
  /// \return A parsed_formula
  ///
  /// Note that the parser usually tries to recover from errors.  The
  /// field parsed_formula::f in the returned object can be a non-zero
  /// value even if it encountered error during the parsing of \a
  /// ltl_string.  If you want to make sure \a ltl_string was parsed
  /// succesfully, check \a parsed_formula::errors for emptiness.
  ///
  /// \warning This function is not reentrant.
  SPOT_API
  parsed_formula parse_infix_boolean(const std::string& ltl_string,
                                     environment& env =
                                     default_environment::instance(),
                                     bool debug = false,
                                     bool lenient = false);

  /// \brief Build a formula from an LTL string in LBT's format.
  /// \param ltl_string The string to parse.
  /// \param env The environment into which parsing should take place.
  /// \param debug When true, causes the parser to trace its execution.
  /// \return A formula built from \a ltl_string, or
  ///         formula(nullptr) if the input was unparsable.
  ///
  /// Note that the parser usually tries to recover from errors.  The
  /// field parsed_formula::f in the returned object can be a non-zero
  /// value even if it encountered error during the parsing of \a
  /// ltl_string.  If you want to make sure \a ltl_string was parsed
  /// succesfully, check \a parsed_formula::errors for emptiness.
  ///
  /// The LBT syntax, also used by the lbtt and scheck tools, is
  /// extended to support W, and M operators (as done in lbtt), and
  /// double-quoted atomic propositions that do not start with 'p'.
  ///
  /// \warning This function is not reentrant.
  SPOT_API
  parsed_formula parse_prefix_ltl(const std::string& ltl_string,
                                  environment& env =
                                  default_environment::instance(),
                                  bool debug = false);

  /// \brief A simple wrapper to parse_infix_psl() and parse_prefix_ltl().
  ///
  /// This is mostly meant for interactive use.  It first tries
  /// parse_infix_psl(); if this fails it tries parse_prefix_ltl();
  /// and if both fails it returns the errors of the first call to
  /// parse_infix_psl() as a parse_error exception.
  SPOT_API formula
  parse_formula(const std::string& ltl_string,
                environment& env = default_environment::instance());

  /// \brief Build a formula from a string representing a SERE.
  /// \param sere_string The string to parse.
  /// \param env The environment into which parsing should take place.
  /// \param debug When true, causes the parser to trace its execution.
  /// \param lenient When true, parenthesized blocks that cannot be
  ///                parsed as subformulas will be considered as
  ///                atomic propositions.
  /// \return A formula built from \a sere_string, or
  ///         formula(0) if the input was unparsable.
  ///
  /// Note that the parser usually tries to recover from errors.  The
  /// field parsed_formula::f in the returned object can be a non-zero
  /// value even if it encountered error during the parsing of \a
  /// ltl_string.  If you want to make sure \a ltl_string was parsed
  /// succesfully, check \a parsed_formula::errors for emptiness.
  ///
  /// \warning This function is not reentrant.
  SPOT_API
  parsed_formula parse_infix_sere(const std::string& sere_string,
                                  environment& env =
                                  default_environment::instance(),
                                  bool debug = false,
                                  bool lenient = false);

  /// \brief Fix location of diagnostics assuming the input is utf8.
  ///
  /// The different parser functions return a parse_error_list that
  /// contain locations specified at the byte level.  Although these
  /// parser recognize some utf8 characters they only work byte by
  /// byte and will report positions by counting byte.
  ///
  /// This function fixes the positions returned by the parser to
  /// look correct when the string is interpreted as a utf8-encoded
  /// string.
  ///
  /// It is invalid to call this function on a string that is not
  /// valid utf8.
  ///
  /// You should NOT call this function before calling
  /// spot::parsed_formula::format_errors() because it is already
  /// called inside if needed.  You may need this function only if you
  /// want to write your own error reporting code.
  ///
  /// \param input_string The string that were parsed.
  /// \param error_list The error list filled by spot::parse
  ///        or spot::parse_sere while parsing \a input_string.
  SPOT_API
  void
  fix_utf8_locations(const std::string& input_string,
                     parse_error_list& error_list);

  /// @}
}
