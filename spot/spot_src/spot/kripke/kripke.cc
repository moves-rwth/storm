// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2014, 2016, 2018 Laboratoire de Recherche
// et Developpement de l'Epita
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
#include <spot/kripke/kripke.hh>

namespace spot
{

  kripke_succ_iterator::~kripke_succ_iterator()
  {
  }

  bdd
  kripke_succ_iterator::cond() const
  {
    // Do not assert(!done()) here.  It is OK to call
    // this function on a state without successor.
    return cond_;
  }

  acc_cond::mark_t
  kripke_succ_iterator::acc() const
  {
    // Do not assert(!done()) here.  It is OK to call
    // this function on a state without successor.
    return {};
  }

  kripke::~kripke()
  {
  }

  acc_cond::mark_t
  kripke::state_acceptance_mark(const state*) const
  {
    return {};
  }
}
