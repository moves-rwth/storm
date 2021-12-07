// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2013-2016, 2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
// Copyright (C) 2005 Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "config.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <spot/misc/optionmap.hh>

namespace spot
{
  const char*
  option_map::parse_options(const char* options)
  {
    while (*options)
      {
        // Skip leading separators.
        while (*options && strchr(" \t\n,;", *options))
          ++options;

        // `!foo' is a shorthand for `foo=0'.
        const char* negated = nullptr;
        if (*options == '!')
          {
            // Skip spaces.
            while (*options && strchr(" \t\n", *options))
              ++options;
            negated = options++;
          }

        if (!*options)
          {
            if (negated)
              return negated;
            else
              break;
          }

        const char* name_start = options;

        // Find the end of the name.
        while (*options && !strchr(", \t\n;=", *options))
          ++options;

        std::string name(name_start, options);

        // Skip spaces.
        while (*options && strchr(" \t\n", *options))
          ++options;

        if (*options != '=')
          {
            set_(name, !negated);
          }
        else if (negated)
          {
            return negated;
          }
        else
          {
            ++options;
            // Skip spaces.
            while (*options && strchr(" \t\n", *options))
              ++options;
            if (!*options)
              return name_start;

            if (*options == '\'' || *options == '"')
              {
                auto sep = *options;
                auto start = options + 1;
                do
                  ++options;
                while (*options && *options != sep);
                if (*options != sep)
                  return start - 1;
                set_str_(name, std::string(start, options));
                if (*options)
                  ++options;
              }
            else
              {
                char* val_end;
                int val = strtol(options, &val_end, 10);
                if (val_end == options)
                  return name_start;

                if (*val_end == 'K')
                  {
                    val *= 1024;
                    ++val_end;
                  }
                else if (*val_end == 'M')
                  {
                    val *= 1024 * 1024;
                    ++val_end;
                  }
                else if (*val_end && !strchr(" \t\n,;", *val_end))
                  {
                    return options;
                  }
                options = val_end;
                set_(name, val);
              }
          }
      }
    return nullptr;
  }

  int
  option_map::get(const char* option, int def) const
  {
    unused_.erase(option);
    auto it = options_.find(option);
    return (it == options_.end()) ? def : it->second;
  }

  std::string
  option_map::get_str(const char* option, std::string def) const
  {
    unused_.erase(option);
    auto it = options_str_.find(option);
    return (it == options_str_.end()) ? def : it->second;
  }

  void option_map::set_(const std::string& option, int val)
  {
    options_[option] = val;
    unused_.insert(option);
  }

  void option_map::set_str_(const std::string& option, const std::string& val)
  {
    options_str_[option] = val;
    unused_.insert(option);
  }

  int
  option_map::set(const char* option, int val, int def)
  {
    int old = get(option, def);
    set_(option, val);
    return old;
  }

  std::string
  option_map::set_str(const char* option, std::string val, std::string def)
  {
    std::string old = get_str(option, def);
    set_str_(option, val);
    return old;
  }

  void
  option_map::set(const option_map& o)
  {
    options_.insert(o.options_.begin(), o.options_.end());
    options_str_.insert(o.options_str_.begin(), o.options_str_.end());
    unused_.insert(o.unused_.begin(), o.unused_.end());
  }

  int
  option_map::operator[](const char* option) const
  {
    return get(option);
  }

  int&
  option_map::operator[](const char* option)
  {
    unused_.erase(option);
    return options_[option];
  }

  std::ostream&
  operator<<(std::ostream& os, const option_map& m)
  {
    for (auto p: m.options_)
      os << '"' << p.first << "\" = " << p.second << '\n';
    for (auto p: m.options_str_)
      os << '"' << p.first << "\" = \"" << p.second << "\"\n";
    return os;
  }

  void option_map::report_unused_options() const
  {
    auto s = unused_.size();
    if (s == 0U)
      return;
    std::ostringstream os;
    if (s == 1U)
      {
        os << "option '" << *unused_.begin()
           << "' was not used (possible typo?)";
      }
    else
      {
        os << "the following options where not used (possible typos?):";
        for (auto opt: unused_)
          os << "\n\t- '" << opt << '\'';
      }
    throw std::runtime_error(os.str());
  }
}
