// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2015 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <spot/misc/bareword.hh>
#include <ctype.h>
#include <spot/misc/escape.hh>

namespace spot
{
  bool
  is_bare_word(const char* str)
  {
    // Bare words cannot be empty and should start with a letter.
    if (!*str
        || !(isalpha(*str) || *str == '_' || *str == '.'))
      return false;
    // The remaining of the word must be alphanumeric.
    while (*++str)
      if (!(isalnum(*str) || *str == '_' || *str == '.'))
        return false;
    return true;
  }

  std::string
  quote_unless_bare_word(const std::string& str)
  {
    if (is_bare_word(str.c_str()))
      return str;
    else
      return "\"" + escape_str(str) + "\"";
  }

  // This is for Spin 5.  Spin 6 has a relaxed parser that can
  // accept any parenthesized block as an atomic propoistion.
  bool is_spin_ap(const char* str)
  {
    if (!str || !islower(*str))
      return false;
    while (*++str)
      if (!(isalnum(*str) || *str == '_'))
        return false;
    return true;
  }

}
