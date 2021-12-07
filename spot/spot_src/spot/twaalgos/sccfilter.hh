// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2012, 2013, 2014, 2015, 2018 Laboratoire de
// Recherche et Developpement de l'Epita (LRDE).
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

#include <spot/misc/common.hh>
#include <bddx.h>
#include <spot/twa/fwd.hh>

namespace spot
{
  class scc_info;


  /// \brief Prune unaccepting SCCs and remove superfluous acceptance
  /// conditions.
  ///
  /// This function will explore the SCCs of the automaton and remove
  /// dead SCCs (i.e. SCC that are not accepting, and those with no
  /// exit path leading to an accepting SCC).
  ///
  /// Additionally, for Generalized Büchi acceptance, this will try to
  /// remove useless acceptance conditions.  This operation may
  /// diminish the number of acceptance condition of the automaton
  /// (for instance when two acceptance conditions are always used
  /// together we only keep one) but it will never remove all
  /// acceptance conditions, even if it would be OK to have zero.
  ///
  /// Acceptance conditions on transitions going to rejecting SCCs are
  /// all removed.  Acceptance conditions going to an accepting SCC
  /// and coming from another SCC are only removed if \a
  /// remove_all_useless is set.  The default value of \a
  /// remove_all_useless is \c false because some algorithms (like the
  /// degeneralization) will work better if transitions going to an
  /// accepting SCC are accepting.
  ///
  /// If the input is inherently weak, the output will be a weak
  /// automaton with state-based acceptance.  The acceptance condition
  /// is set to Büchi unless the input was co-Büchi or t (in which
  /// case we keep this acceptance).
  ///
  /// If \a given_sm is supplied, the function will use its result
  /// without computing a map of its own.
  ///
  /// \warning Calling scc_filter on a TωA that is not inherently weak
  /// and has the SBA property (i.e., transitions leaving accepting
  /// states are all marked as accepting) may destroy this property.
  /// Use scc_filter_states() instead.
  SPOT_API twa_graph_ptr
  scc_filter(const const_twa_graph_ptr& aut, bool remove_all_useless = false,
             scc_info* given_si = nullptr);

  /// \brief Prune unaccepting SCCs.
  ///
  /// This is an abridged version of scc_filter(), that preserves
  /// state-based acceptance.  I.e., if the input TωA has the SBA
  /// property, (i.e., transitions leaving accepting states are all
  /// marked as accepting), then the output TωA will also have that
  /// property.
  SPOT_API twa_graph_ptr
  scc_filter_states(const const_twa_graph_ptr& aut,
                    bool remove_all_useless = false,
                    scc_info* given_si = nullptr);

  /// \brief Prune unaccepting SCCs, superfluous acceptance
  /// sets, and suspension variables.
  ///
  /// In addition to removing useless states, and acceptance sets,
  /// this remove all ignoredvars occurring in conditions, and all
  /// suspvars in conditions leadings to non-accepting SCC (as well
  /// as the conditions between two SCCs if early_susp is false).
  ///
  /// This is used by compsusp(), and is probably useless for any
  /// other use.
  SPOT_API twa_graph_ptr
  scc_filter_susp(const const_twa_graph_ptr& aut, bool remove_all_useless,
                  bdd suspvars, bdd ignoredvars, bool early_susp,
                  scc_info* given_si = nullptr);
}
