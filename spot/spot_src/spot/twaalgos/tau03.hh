// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2019 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include <spot/misc/optionmap.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/emptiness.hh>

namespace spot
{
  /// \addtogroup emptiness_check_algorithms
  /// @{

  /// \brief Returns an emptiness checker on the spot::tgba automaton \a a.
  ///
  /// \pre The automaton \a a must have at least one acceptance condition.
  ///
  /// During the visit of \a a, the returned checker stores explicitely all
  /// the traversed states. The implemented algorithm is the following:
  ///
  /** \verbatim
      procedure check ()
      begin
        call dfs_blue(s0);
      end;

      procedure dfs_blue (s)
      begin
        s.color = blue;
        s.acc = emptyset;
        for all t in post(s) do
          if t.color == white then
            call dfs_blue(t);
          end if;
        end for;
        for all t in post(s) do
          let (s, l, a, t) be the edge from s to t;
          if s.acc U a not included in t.acc then
            call dfs_red(t, a U s.acc);
          end if;
        end for;
        if s.acc == all_acc then
          report a cycle;
        end if;
      end;

      procedure dfs_red(s, A)
      begin
        s.acc = s.acc U A;
        for all t in post(s) do
          if t.color != white and A not included in t.acc then
            call dfs_red(t, A);
          end if;
        end for;
      end;
      \endverbatim */
  ///
  /// This algorithm is the one presented in \cite tauriainen.03.tr .
  SPOT_API emptiness_check_ptr
  explicit_tau03_search(const const_twa_ptr& a, option_map o = option_map());

  /// @}
}
