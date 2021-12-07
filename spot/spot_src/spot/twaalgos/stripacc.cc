// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014-2015, 2017-2018 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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
#include <spot/twaalgos/stripacc.hh>

namespace spot
{
  void strip_acceptance_here(twa_graph_ptr a)
  {
    unsigned n = a->num_states();
    for (unsigned s = 0; s < n; ++s)
      for (auto& t: a->out(s))
        t.acc = {};
    a->set_generalized_buchi(0);
    a->release_named_properties();
    a->prop_weak(true);
  }
}
