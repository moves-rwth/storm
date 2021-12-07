// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015, 2017 Laboratoire de Recherche et
// Développement de l'EPITA.
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
#include <spot/parseaut/parseaut.hh>
#include <spot/misc/location.hh>

# define YY_DECL \
  int hoayylex(hoayy::parser::semantic_type *yylval, \
               spot::location *yylloc, \
               void* yyscanner, \
               spot::parse_aut_error_list& error_list)
YY_DECL;

namespace spot
{
  void hoayyreset(void* scanner);
  int hoayyopen(const std::string& name, void** scanner);
  int hoayyopen(int fd, void** scanner);
  int hoayystring(const char* data, void** scanner);
  void hoayyclose(void* scanner);

  // This exception is thrown by the lexer when it reads "--ABORT--".
  struct hoa_abort
  {
    spot::location pos;
  };
}
