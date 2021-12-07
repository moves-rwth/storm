// -*- coding: utf-8 -*-
// Copyright (C) 2017-2020 Laboratoire de Recherche et Developpement
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

#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of an automaton, for any acceptance condition.
  ///
  /// Currently only implemented for twa_graph automata, i.e., not automata
  /// constructed on-the-fly.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check(const const_twa_graph_ptr& aut);

  /// \ingroup emptiness_check_algorithms
  /// \brief Accepting run search in an automaton, for any acceptance condition.
  /// \return An accepting run over the automaton, or nullptr if the language is
  /// empty.
  ///
  /// Currently only implemented for twa_graph automata, i.e., not automata
  /// constructed on-the-fly.
  SPOT_API twa_run_ptr
  generic_accepting_run(const const_twa_graph_ptr& aut);

  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of one SCC, for any acceptance condition.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc);

  /// \ingroup emptiness_check_algorithms
  /// \brief Emptiness check of one SCC, for any acceptance condition.
  ///
  /// This version makes it possible to ignore the acceptance
  /// condition of the automaton, and use \a forced_acc.
  ///
  /// \cite baier.19.atva
  SPOT_API bool
  generic_emptiness_check_for_scc(const scc_info& si, unsigned scc,
                                  const acc_cond& forced_acc);


  /// \ingroup emptiness_check_algorithms
  ///
  /// Select the version of the generic-emptiness check to use, this
  /// is mainly for benchmarking purpose.
  ///
  /// We currently have three versions:
  /// - "spot28" is similar to the algorithm described our ATVA'19 paper
  ///   \cite baier.19.atva , however it has an implementation bug
  ///   that cause superfluous recursive calls to be performed (the
  ///   result is still correct.
  /// - "atva19" is similar to the algorithm described our ATVA'19 paper
  ///   \cite baier.19.atva , with the above bug fixed.
  /// - "spot29" improves upon the worst case of atva19.  This is
  ///   the default.
  SPOT_API void
  generic_emptiness_check_select_version(const char* emversion = nullptr);

}
