// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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

#include "common_sys.hh"
#include <spot/twa/twagraph.hh>

int to_int(const char* s, const char* where);
int to_pos_int(const char* s, const char* where);
unsigned to_unsigned (const char *s, const char* where);
float to_float(const char* s, const char* where);
float to_probability(const char* s, const char* where);

// Parse the comma or space seperate string of numbers.
std::vector<long> to_longs(const char* s);
