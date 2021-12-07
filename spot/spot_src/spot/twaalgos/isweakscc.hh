// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2014, 2017 Laboratoire de Recherche et
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

#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  /// \addtogroup twa_misc
  /// @{

  /// \brief Whether the SCC number \a scc in \a map has a rejecting
  /// cycle.
  SPOT_API bool
  scc_has_rejecting_cycle(scc_info& map, unsigned scc);

  /// \brief Whether the SCC number \a scc in \a map is inherently
  /// weak.
  ///
  /// An SCC is inherently weak if either its cycles are all
  /// accepting, or they are all non-accepting.
  ///
  /// Note the terminal SCCs are also inherently weak with that
  /// definition.
  SPOT_API bool
  is_inherently_weak_scc(scc_info& map, unsigned scc);

  /// \brief Whether the SCC number \a scc in \a map is weak.
  ///
  /// An SCC is weak if its non-accepting, or if all its transition
  /// are fully accepting (i.e., the belong to all acceptance sets).
  ///
  /// Note that terminal SCCs are also weak with that definition.
  SPOT_API bool
  is_weak_scc(scc_info& map, unsigned scc);

  /// \brief Whether the SCC number \a scc in \a map is complete.
  ///
  /// An SCC is complete iff for all states and all label there exists
  /// a transition that stays into this SCC.  For this function,
  /// universal transitions are considered in the SCC if all there
  /// destination are into the SCC.
  SPOT_API bool
  is_complete_scc(scc_info& map, unsigned scc);

  /// \brief Whether the SCC number \a scc in \a map is terminal.
  ///
  /// An SCC is terminal if it is weak, complete, and accepting.
  SPOT_API bool
  is_terminal_scc(scc_info& map, unsigned scc);

  /// @}
}
