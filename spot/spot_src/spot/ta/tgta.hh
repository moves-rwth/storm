// -*- coding: utf-8 -*-
// Copyright (C) 2010, 2011, 2012, 2013, 2014, 2015, 2016 Laboratoire
// de Recherche et Développement de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANta_explicitBILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more deta_explicitils.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <spot/twa/twa.hh>

namespace spot
{

  /// \ingroup ta_essentials
  /// \brief A Transition-based Generalized Testing Automaton (TGTA).
  ///
  /// Transition-based Generalized Testing Automaton (TGTA) is a new
  /// kind of automaton that combines features from both TA and TGBA.
  /// From TA, we take the idea of labeling transitions with
  /// changesets, however we remove the use of livelock-acceptance
  /// (because it may require a two-pass emptiness check), and the
  /// implicit stuttering.  From TGBA, we inherit the use of
  /// transition-based generalized acceptance conditions.  The
  /// resulting Chimera, which we call "Transition-based Generalized
  /// Testing Automaton" (TGTA), accepts only stuttering-insensitive
  /// languages like TA, and inherits advantages from both TA and
  /// TGBA: it has a simple one-pass emptiness-check procedure (the
  /// same as algorithm the one for TGBA), and can benefit from
  /// reductions based on the stuttering of the properties pretty much
  /// like a TA.  Livelock acceptance states, which are no longer
  /// supported are emulated using states with a Büchi accepting
  /// self-loop labeled by empty changeset.
  ///
  /// Browsing such automaton can be achieved using two functions:
  /// \c get_initial_state and \c
  /// succ_iter. The former returns the initial state(s) while the latter lists
  /// the successor states of any state. A second implementation of \c succ_iter
  /// returns only the successors reached through a changeset passed as
  /// a parameter.
  ///
  /// Note that although this is a transition-based automata,
  /// we never represent transitions!  Transition informations are
  /// obtained by querying the iterator over the successors of
  /// a state.

  class SPOT_API tgta : public twa
  {

  protected:
    tgta(const bdd_dict_ptr& d)
      : twa(d)
    {
    }

  public:
    virtual ~tgta()
    {
    }

    /// \brief Get an iterator over the successors of \a state
    /// filtred by the value of the changeset on transitions between the
    /// \a state and his successors
    ///
    /// The iterator has been allocated with \c new.  It is the
    /// responsability of the caller to \c delete it when no
    /// longer needed.
    ///
    virtual twa_succ_iterator*
    succ_iter_by_changeset(const spot::state* s, bdd change_set) const = 0;
  };

  typedef std::shared_ptr<tgta> tgta_ptr;
  typedef std::shared_ptr<const tgta> const_tgta_ptr;
}
