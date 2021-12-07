// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2018, 2019 Laboratoire de Recherche et
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

#include "common_sys.hh"
#include "progname.h"
#include <functional>

void setup(char** progname);

extern const struct argp misc_argp;
extern const struct argp misc_argp_hidden;


// Call setup(progname) then Run mainfun() and handle exceptions.
int protected_main(char** progname, std::function<int()> mainfun);

// Diagnose exceptions.
[[noreturn]] void handle_any_exception();
#define BEGIN_EXCEPTION_PROTECT try { (void)0;
#define END_EXCEPTION_PROTECT } catch (...) { handle_any_exception(); }
