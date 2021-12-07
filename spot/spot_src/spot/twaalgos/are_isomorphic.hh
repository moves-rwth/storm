// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015 Laboratoire de Recherche et
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

#include <spot/twa/twagraph.hh>
#include <vector>

namespace spot
{
  /// Check if two automata are isomorphic.
  class SPOT_API isomorphism_checker final
  {
  public:
    isomorphism_checker(const const_twa_graph_ptr ref);

    /// \ingroup twa_misc
    /// \brief Check whether an automaton is isomorphic to the one passed to
    /// the constructor.
    ///
    /// Two automata are considered isomorphic if there exists a bijection f
    /// between the states of a1 and the states of a2 such that for any pair of
    /// states (s1, s2) of a1, there is a transition from s1 to s2 with
    /// condition c and acceptance set A iff there is a transition with
    /// condition c and acceptance set A between f(s1) and f(s2) in a2.
    /// This can be done simply by checking if
    /// canonicalize(aut1) == canonicalize(aut2), but is_isomorphic can do some
    /// optimizations in some cases.
    bool
    is_isomorphic(const const_twa_graph_ptr aut);


    /// \ingroup twa_misc
    /// \brief Check whether two automata are isomorphic.
    static bool are_isomorphic(const const_twa_graph_ptr ref,
                               const const_twa_graph_ptr aut);

  private:
    bool is_isomorphic_(const const_twa_graph_ptr aut);
    twa_graph_ptr ref_;
    bool ref_deterministic_ = false;
    unsigned nondet_states_ = 0;
  };
}
