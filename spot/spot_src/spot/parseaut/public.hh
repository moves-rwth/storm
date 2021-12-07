// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2015, 2016, 2017 Laboratoire de Recherche et
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

#include <spot/twa/twagraph.hh>
#include <spot/kripke/kripkegraph.hh>
#include <spot/misc/location.hh>
#include <spot/tl/defaultenv.hh>
#include <string>
#include <list>
#include <utility>
#include <iosfwd>
#include <spot/misc/bitvect.hh>

namespace spot
{
  /// \addtogroup twa_io
  /// @{

#ifndef SWIG
  /// \brief A parse diagnostic with its location.
  typedef std::pair<spot::location, std::string> parse_aut_error;
  /// \brief A list of parser diagnostics, as filled by parse.
  typedef std::list<parse_aut_error> parse_aut_error_list;
#else
    // Turn parse_aut_error_list into an opaque type for Swig.
  struct parse_aut_error_list {};
#endif

  enum class parsed_aut_type { HOA, NeverClaim, LBTT, DRA, DSA, Unknown };

  /// \brief Result of the automaton parser
  struct SPOT_API parsed_aut final
  {
    /// \brief The parsed automaton.
    ///
    /// May be null if the parser reached the end of the stream or a
    /// serious error. In the latter case, \c errors is non-empty.
    /// May also be null if the parser is used to parse a Kripke
    /// structure.
    twa_graph_ptr aut;
    /// \brief The parsed kripke structure.
    ///
    /// Used instead of \c aut when the parser is called with option
    /// want_kripke.
    kripke_graph_ptr ks;

    /// Whether an HOA file was termined with <code>--ABORT</code>
    bool aborted = false;
    /// Location of the automaton in the stream.
    spot::location loc;
    /// Format of the automaton.
    parsed_aut_type type = parsed_aut_type::Unknown;
    /// Name of the stream (used for displaying syntax errors)
    const std::string filename;
    /// \brief Syntax errors that occurred during parsing.
    ///
    /// Note that the parser does not print any diagnostic.
    /// Deciding how to output those errors is up to you.
    parse_aut_error_list errors;

    parsed_aut(const std::string& str)
      : filename(str)
    {
    }
    /// \brief Format diagnostics produced by spot::parse_aut.
    /// \param os Where diagnostics should be output.
    /// \return \c true iff any diagnostic was output.
    bool format_errors(std::ostream& os);
  };

  typedef std::shared_ptr<parsed_aut> parsed_aut_ptr;
  typedef std::shared_ptr<const parsed_aut> const_parsed_aut_ptr;

  struct automaton_parser_options final
  {
    bool ignore_abort = false;        ///< Skip aborted automata
    bool debug = false;                ///< Run the parser in debug mode?
    bool trust_hoa = true;        ///< Trust properties in HOA files
    bool raise_errors = false;        ///< Raise errors as exceptions.
    bool want_kripke = false;        ///< Parse as a Kripke structure.
  };

  /// \brief Parse a stream of automata
  ///
  /// This object should be constructed for a given stream (a file, a
  /// file descriptor, or a raw buffer), and then it parse() method
  /// may be called in a loop to parse each automaton in the stream.
  ///
  /// Several input formats are supported, and automatically
  /// recognized: HOA, LBTT, DSTAR, or neverclaim.  We recommend
  /// using the HOA format, because it is the most general.
  ///
  /// The specification of the HOA format can be found at
  ///    http://adl.github.io/hoaf/
  ///
  /// The grammar of neverclaim will not accept every possible
  /// neverclaim output.  It has been tuned to accept the output of
  /// spin -f, ltl2ba, ltl3ba, and modella.  If you know of some other
  /// tool that produce Büchi automata in the form of a neverclaim,
  /// but is not understood by this parser, please report it to
  /// spot@lrde.epita.fr.
  class SPOT_API automaton_stream_parser final
  {
    spot::location last_loc;
    std::string filename_;
    automaton_parser_options opts_;
    void* scanner_;
  public:
    /// \brief Parse from a file.
    ///
    /// \param filename The file to read from.
    /// \param opts Parser options.
    automaton_stream_parser(const std::string& filename,
                            automaton_parser_options opts = {});

    /// \brief Parse from an already opened file descriptor.
    ///
    /// The file descriptor will not be closed.
    ///
    /// \param fd The file descriptor to read from.
    /// \param filename What to display in error messages.
    /// \param opts Parser options.
    automaton_stream_parser(int fd, const std::string& filename,
                            automaton_parser_options opts = {});

    /// \brief Parse from a buffer
    ///
    /// \param data The buffer to read from.
    /// \param filename What to display in error messages.
    /// \param opts Parser options.
    automaton_stream_parser(const char* data,
                            const std::string& filename,
                            automaton_parser_options opts = {});

    ~automaton_stream_parser();

    /// \brief Parse the next automaton in the stream.
    ///
    /// Note that the parser usually tries to recover from errors.  It
    /// can return an automaton even if it encountered an error during
    /// parsing.  If you want to make sure the input was parsed
    /// successfully, make sure \c errors is empty and \c aborted is
    /// false in the result.  (Testing \c aborted is obviously
    /// superfluous if the parser is configured to skip aborted
    /// automata.)
    ///
    /// The \c aut field of the result can be null in two conditions:
    /// some serious error occurred (in this case \c errors is non
    /// empty), or the end of the stream was reached.
    ///
    /// \warning This function is not reentrant.
    parsed_aut_ptr parse(const bdd_dict_ptr& dict,
                         environment& env =
                         default_environment::instance());
  };

  /// \brief Read the first spot::twa_graph from a file.
  /// \param filename The name of the file to parse.
  /// \param dict The BDD dictionary where to use.
  /// \param env The environment of atomic proposition into which parsing
  ///        should take place.
  /// \param opts Additional options to pass to the parser.
  /// \return A pointer to a \c parsed_aut structure.
  ///
  /// This is a wrapper around spot::automaton_stream_parser that returns
  /// the first automaton of the file.  Empty inputs are reported as
  /// syntax errors, so the \c aut field of the result is guaranteed not
  /// to be null if \c errors is empty.  (This is unlike
  /// automaton_stream_parser::parse() where a null \c aut denots the
  /// end of a stream.)
  ///
  /// \warning This function is not reentrant.
  SPOT_API parsed_aut_ptr
  parse_aut(const std::string& filename,
            const bdd_dict_ptr& dict,
            environment& env = default_environment::instance(),
            automaton_parser_options opts = {});
  /// @}
}
