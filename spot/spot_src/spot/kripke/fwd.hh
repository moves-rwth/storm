// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2016 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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

#include <memory>

namespace spot
{
  class fair_kripke;
  typedef std::shared_ptr<fair_kripke> fair_kripke_ptr;
  typedef std::shared_ptr<const fair_kripke> const_fair_kripke_ptr;

  class kripke;
  typedef std::shared_ptr<kripke> kripke_ptr;
  typedef std::shared_ptr<const kripke> const_kripke_ptr;

  class kripke_explicit;
  typedef std::shared_ptr<const kripke_explicit> const_kripke_explicit_ptr;
  typedef std::shared_ptr<kripke_explicit> kripke_explicit_ptr;
}
