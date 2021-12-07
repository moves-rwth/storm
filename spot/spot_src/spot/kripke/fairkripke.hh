// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2013, 2014, 2015, 2016 Laboratoire de
// Recherche et Developpement de l'Epita
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

#include <spot/twa/twa.hh>
#include <spot/kripke/fwd.hh>

/// \addtogroup kripke Kripke Structures
/// \ingroup twa

namespace spot
{
  /// \ingroup kripke
  /// \brief Iterator code for a Fair Kripke structure.
  ///
  /// This iterator can be used to simplify the writing
  /// of an iterator on a Fair Kripke structure (or lookalike).
  ///
  /// If you inherit from this iterator, you should only
  /// redefine
  ///
  ///   - fair_kripke_succ_iterator::first()
  ///   - fair_kripke_succ_iterator::next()
  ///   - fair_kripke_succ_iterator::done()
  ///   - fair_kripke_succ_iterator::dst()
  ///
  /// This class implements fair_kripke_succ_iterator::cond(),
  /// and fair_kripke_succ_iterator::acc().
  class SPOT_API fair_kripke_succ_iterator : public twa_succ_iterator
  {
  public:
    /// \brief Constructor
    ///
    /// The \a cond and \a acc_cond arguments will be those returned
    /// by fair_kripke_succ_iterator::cond(),
    /// and fair_kripke_succ_iterator::acc().
    fair_kripke_succ_iterator(const bdd& cond, acc_cond::mark_t acc_cond);
    virtual ~fair_kripke_succ_iterator();

    virtual bdd cond() const override;
    virtual acc_cond::mark_t acc() const override;
  protected:
    bdd cond_;
    acc_cond::mark_t acc_cond_;
  };

  /// \ingroup kripke
  /// \brief Interface for a Fair Kripke structure.
  ///
  /// A Kripke structure is a graph in which each node (=state) is
  /// labeled by a conjunction of atomic proposition, and a set of
  /// acceptance conditions.
  ///
  /// Such a structure can be seen as spot::tgba by pushing all labels
  /// to the outgoing transitions.
  ///
  /// A programmer that develops an instance of Fair Kripke structure
  /// needs just provide an implementation for the following methods:
  ///
  ///   - kripke::get_init_state()
  ///   - kripke::succ_iter()
  ///   - kripke::state_condition()
  ///   - kripke::state_acceptance_conditions()
  ///   - kripke::format_state()
  ///
  /// The other methods of the tgba interface are supplied by this
  /// class and need not be defined.
  ///
  /// See also spot::fair_kripke_succ_iterator.
  class SPOT_API fair_kripke: public twa
  {
  public:
    fair_kripke(const bdd_dict_ptr& d)
      : twa(d)
      {
      }

    /// \brief The condition that label the state \a s.
    ///
    /// This should be a conjunction of atomic propositions.
    virtual bdd state_condition(const state* s) const = 0;

    /// \brief The acceptance mark that labels state \a s.
    virtual acc_cond::mark_t
      state_acceptance_mark(const state* s) const = 0;
  };
}
