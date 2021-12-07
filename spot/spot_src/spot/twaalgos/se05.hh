// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2019 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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

#include <cstddef>
#include <spot/misc/optionmap.hh>
#include <spot/twa/fwd.hh>
#include <spot/twaalgos/emptiness.hh>

namespace spot
{
  /// \addtogroup emptiness_check_algorithms
  /// @{

  /// \brief Returns an emptiness check on the spot::tgba automaton \a a.
  ///
  /// \pre The automaton \a a must have at most one acceptance condition (i.e.
  /// it is a TBA).
  ///
  /// During the visit of \a a, the returned checker stores explicitely all
  /// the traversed states.
  /// The method \a check() of the checker can be called several times
  /// (until it returns a null pointer) to enumerate all the visited accepting
  /// paths. The implemented algorithm is an optimization of
  /// spot::explicit_magic_search and is the following:
  ///
  /** \verbatim
      procedure check ()
      begin
        call dfs_blue(s0);
      end;

      procedure dfs_blue (s)
      begin
        s.color = cyan;
        for all t in post(s) do
          if t.color == white then
            call dfs_blue(t);
          else if t.color == cyan and
                  (the edge (s,t) is accepting or
                   (it exists a predecessor p of s in st_blue and s != t and
                   the arc between p and s is accepting)) then
            report cycle;
          end if;
          if the edge (s,t) is accepting then
            call dfs_red(t);
          end if;
        end for;
        s.color = blue;
      end;

      procedure dfs_red(s)
      begin
        if s.color == cyan then
          report cycle;
        end if;
        s.color = red;
        for all t in post(s) do
          if t.color == blue then
            call dfs_red(t);
          end if;
        end for;
      end;
      \endverbatim */
  ///
  /// It is an adaptation to TBA of the one presented in
  /// \cite schwoon.04.tr .
  ///
  /// \sa spot::explicit_magic_search
  ///
  SPOT_API emptiness_check_ptr
  explicit_se05_search(const const_twa_ptr& a, option_map o = option_map());

  /// \brief Returns an emptiness checker on the spot::tgba automaton \a a.
  ///
  /// \pre The automaton \a a must have at most one acceptance condition (i.e.
  /// it is a TBA).
  ///
  /// During the visit of \a a, the returned checker does not store explicitely
  /// the traversed states but uses the bit-state hashing technic presented in
  /// \cite holzmann.91.book
  ///
  /// Consequently, the detection of an acceptence cycle is not ensured.
  ///
  /// The size of the heap is limited to \n size bytes.
  ///
  /// The implemented algorithm is the same as the one of
  /// spot::explicit_se05_search.
  ///
  /// \sa spot::explicit_se05_search
  ///
  SPOT_API emptiness_check_ptr
  bit_state_hashing_se05_search(const const_twa_ptr& a, size_t size,
                                option_map o = option_map());


  /// \brief Wrapper for the two se05 implementations.
  ///
  /// This wrapper calls explicit_se05_search() or
  /// bit_state_hashing_se05_search() according to the \c "bsh" option
  /// in the \c option_map.  If \c "bsh" is set and non null, its value
  /// is used as the size of the hash map.
  SPOT_API emptiness_check_ptr
  se05(const const_twa_ptr& a, option_map o);

  /// @}
}
