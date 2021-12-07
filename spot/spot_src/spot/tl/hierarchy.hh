// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018, 2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE)
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
#include <spot/twa/fwd.hh>

namespace spot
{
  /// \ingroup tl_hier
  /// \brief Enum used to change the behavior of is_persistence() or
  /// is_recurrence().
  ///
  /// If PR_Auto, both methods will first check the environment variable
  /// <code>SPOT_PR_CHECK</code> to see if one algorithm or the other is wanted
  /// to be forced. Otherwise, it will make the most appropriate decision.
  ///
  /// If PR_via_Rabin, they will check if the formula is detbuchi_realizable
  /// going through a rabin automaton: TGBA->DPA->DRA->(D?)BA.
  ///
  /// If PR_via_CoBuchi, they will check if the formula is cobuchi_realizable.
  ///
  /// If PR_via_Parity, they will check if the formula is
  /// detbuchi/cobuchi-realizable by calling reduce_parity() on a DPA.
  ///
  /// Note that is_persistence() and is_recurrence() will work on a formula f
  /// or its negation because of the duality of both classes
  /// (see https://spot.lrde.epita.fr/hierarchy.html for details).
  /// For instance if is_recurrence() wants to use PR_via_CoBuchi it will check
  /// if !f is cobuchi_realizable.
  enum class prcheck
  {
    Auto,
    via_CoBuchi,
    via_Rabin,
    via_Parity,
  };

  /// \ingroup tl_hier
  /// \brief Return true if \a f represents a persistence property.
  ///
  /// \param f the formula to check.
  /// \param aut the corresponding automaton (not required).
  /// \param algo the algorithm to use (see enum class prcheck).
  SPOT_API bool
  is_persistence(formula f,
                 twa_graph_ptr aut = nullptr,
                 prcheck algo = prcheck::Auto);

  /// \ingroup tl_hier
  /// \brief Return true if \a f represents a recurrence property.
  ///
  /// Actually, it calls is_persistence() with the negation of \a f.
  ///
  /// \param f the formula to check.
  /// \param aut the corresponding automaton (not required).
  /// \param algo the algorithm to use (see enum class prcheck).
  SPOT_API bool
  is_recurrence(formula f,
                twa_graph_ptr aut = nullptr,
                prcheck algo = prcheck::Auto);

  /// Enum used to change the behavior of is_obligation().
  enum class ocheck
  {
    Auto,
    via_CoBuchi,
    via_Rabin,
    via_WDBA,
  };

  /// \ingroup tl_hier
  /// \brief Return true if \a f has the recurrence property.
  ///
  /// Actually, it calls is_persistence() with the negation of \a f.
  ///
  /// \param f the formula to check.
  /// \param aut the corresponding automaton (not required).
  /// \param algo the algorithm to use.
  ///
  /// \a aut is constructed from f if not supplied.
  ///
  /// If \a algo is ocheck::via_WDBA, aut is converted into a WDBA
  /// which is then checked for equivalence with aut.  If \a algo is
  /// ocheck::via_CoBuchi, we test that both f and !f are co-Büchi
  /// realizable.  If \a algo is ocheck::via_Rabin, we test that both
  /// f and !f are DBA-realizable.
  ///
  /// Auto currently defaults to via_WDBA, unless the
  /// <code>SPOT_O_CHECK</code> environment variable specifies
  /// otherwise.
  SPOT_API bool
  is_obligation(formula f,
                twa_graph_ptr aut = nullptr,
                ocheck algo = ocheck::Auto);

  /// \ingroup tl_hier
  /// \brief Return the class of \a f in the temporal hierarchy of Manna
  /// and Pnueli (PODC'90).
  ///
  /// The class is indicated using a character among:
  /// - 'B' (bottom) safety properties that are also guarantee properties
  /// - 'G' guarantee properties that are not also safety properties
  /// - 'S' safety properties that are not also guarantee properties
  /// - 'O' obligation properties that are not safety or guarantee
  ///       properties
  /// - 'P' persistence properties that are not obligations
  /// - 'R' recurrence properties that are not obligations
  /// - 'T' (top) properties that are not persistence or recurrence
  ///   properties
  SPOT_API char mp_class(formula f);


  /// \ingroup tl_hier
  /// \brief Return the class of \a f in the temporal hierarchy of Manna
  /// and Pnueli (PODC'90).
  ///
  /// The \a opt parameter should be a string specifying options
  /// for expressing the class.  If \a opt is empty, the
  /// result is one character among B, G, S, O, P, R, T, specifying
  /// the most precise class to which the formula belongs.
  /// If \a opt contains 'w', then the string contains all the
  /// characters corresponding to the classes that contain \a f.
  /// If \a opt contains 'v', then the characters are replaced
  /// by the name of each class.  Space and commas are ignored.
  /// Any ']' ends the processing of the options.
  SPOT_API std::string mp_class(formula f, const char* opt);

  /// \ingroup tl_hier
  /// \brief Expand a class in the temporal hierarchy of Manna
  /// and Pnueli (PODC'90).
  ///
  /// \a mpc should be a character among B, G, S, O, P, R, T
  /// specifying a class in the hierarchy.
  ///
  /// The \a opt parameter should be a string specifying options for
  /// expressing the class.  If \a opt is empty, the result is \a mpc.
  /// If \a opt contains 'w', then the string contains all the
  /// characters corresponding to the super-classes of \a mpc.  If \a
  /// opt contains 'v', then the characters are replaced by the name
  /// of each class.  Space and commas are ignored.  Any ']' ends the
  /// processing of the options.
  SPOT_API std::string mp_class(char mpc, const char* opt);


  /// \brief Compute the nesting depth of an operator.
  ///
  /// Return the maximum number of occurrence of \a oper among all
  /// branches of the AST of \a f.
  SPOT_API unsigned nesting_depth(formula f, op oper);

#ifndef SWIG
  /// \brief Compute the nesting depth of a set of operators.
  ///
  /// Return the maximum number of occurrence of any operator between
  /// \a begin and \a end among all branches of the AST of \a f.
  SPOT_API unsigned nesting_depth(formula f, const op* begin, const op* end);
#endif

  /// \brief Compute the nesting depth of a set of operators.
  ///
  /// Return the maximum number of occurrence of any operator listed
  /// \a opers, among all branches of the AST of \a f.
  ///
  /// Operators to count should be supplied in \a opers as a string of
  /// letters among 'X', 'F', 'G', 'U', 'R', 'M', 'W', '&', '|', '!',
  /// 'i' (implication), 'e' (equivalence).
  ///
  /// Add letter '~' to force \a into negative normal form before
  /// processing it.
  ///
  /// The string should be terminated by '\0' or ']'.
  SPOT_API unsigned nesting_depth(formula f, const char* opers);


  /// \brief Check whether a formula represents a liveness property.
  ///
  /// A formula represents a liveness property if any finite prefix
  /// can be extended into a word accepted by the formula.
  ///
  /// The test is done by conversion to automaton.  If you already
  /// have an automaton, use spot::is_liveness_automaton() instead.
  SPOT_API bool is_liveness(formula f);
}
