// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2015, 2016-2017 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE)
// Copyright (C) 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/misc/common.hh>
#include <string>
#include <map>
#include <iosfwd>
#include <set>

namespace spot
{
  /// \ingroup misc_tools
  /// \brief Manage a map of options.
  ///
  /// Each option is defined by a string and is associated to an integer value.
  class SPOT_API option_map
  {
  public:
    /// \brief Add the parsed options to the map.
    ///
    /// \a options are separated by a space, comma, semicolon or tabulation and
    /// can be optionally followed by an integer value (preceded by an equal
    /// sign). If not specified, the default value is 1.
    ///
    /// The following three lines are equivalent.
    /** \verbatim
        optA !optB optC=4194304
        optA=1, optB=0, optC=4096K
        optC = 4M; optA !optB
        \endverbatim */
    ///
    /// \return A non-null pointer to the option for which an expected integer
    /// value cannot be parsed.
    const char* parse_options(const char* options);

    /// \brief Get the value of \a option.
    ///
    /// \return The value associated to \a option if it exists,
    /// \a def otherwise.
    /// \see operator[]()
    int get(const char* option, int def = 0) const;

    /// \brief Get the value of \a option.
    ///
    /// \return The value associated to \a option if it exists,
    /// \a def otherwise.
    /// \see operator[]()
    std::string get_str(const char* option, std::string def = {}) const;

    /// \brief Get the value of \a option.
    ///
    /// \return The value associated to \a option if it exists, 0 otherwise.
    /// \see get()
    int operator[](const char* option) const;

    /// \brief Set the value of \a option to \a val.
    ///
    /// \return The previous value associated to \a option if declared,
    /// or \a def otherwise.
    int set(const char* option, int val, int def = 0);

    /// \brief Set the value of a string \a option to \a val.
    ///
    /// \return The previous value associated to \a option if declared,
    /// or \a def otherwise.
    std::string set_str(const char* option,
                        std::string val, std::string def = {});

    /// \brief Raise a runtime_error if some options have not been used.
    void report_unused_options() const;

    /// Acquire all the settings of \a o.
    void set(const option_map& o);

    /// \brief Get a reference to the current value of \a option.
    int& operator[](const char* option);

    /// \brief Print the option_map \a m.
    friend SPOT_API std::ostream&
      operator<<(std::ostream& os, const option_map& m);

  private:
    std::map<std::string, int> options_;
    std::map<std::string, std::string> options_str_;
    // Unused values.  Initially they will store all options, and they
    // will be erased as they are used.  The resulting set can be used
    // for diagnosing errors.
    mutable std::set<std::string> unused_;

    void set_(const std::string&, int val);
    void set_str_(const std::string&, const std::string& val);
  };
}
