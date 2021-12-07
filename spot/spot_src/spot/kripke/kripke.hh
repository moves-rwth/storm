// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2010, 2013, 2014, 2016 Laboratoire de Recherche
// et Developpement de l'Epita
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

#include <spot/kripke/fairkripke.hh>

namespace spot
{

  /// \ingroup kripke
  /// \brief Iterator code for Kripke structure
  ///
  /// This iterator can be used to simplify the writing
  /// of an iterator on a Kripke structure (or lookalike).
  ///
  /// If you inherit from this iterator, you should only
  /// redefine
  ///
  ///   - kripke_succ_iterator::first()
  ///   - kripke_succ_iterator::next()
  ///   - kripke_succ_iterator::done()
  ///   - kripke_succ_iterator::dst()
  ///
  /// This class implements kripke_succ_iterator::cond(),
  /// and kripke_succ_iterator::acc().
  class SPOT_API kripke_succ_iterator : public twa_succ_iterator
  {
  public:
    /// \brief Constructor
    ///
    /// The \a cond argument will be the one returned
    /// by kripke_succ_iterator::cond().
    kripke_succ_iterator(const bdd& cond)
      : cond_(cond)
    {
    }

    void recycle(const bdd& cond)
    {
      cond_ = cond;
    }

    virtual ~kripke_succ_iterator();

    virtual bdd cond() const override;
    virtual acc_cond::mark_t acc() const override;
  protected:
    bdd cond_;
  };

  /// \ingroup kripke
  /// \brief Interface for a Kripke structure
  ///
  /// A Kripke structure is a graph in which each node (=state) is
  /// labeled by a conjunction of atomic proposition.
  ///
  /// Such a structure can be seen as spot::tgba without
  /// any acceptance condition.
  ///
  /// A programmer that develops an instance of Kripke structure needs
  /// just provide an implementation for the following methods:
  ///
  ///   - kripke::get_init_state()
  ///   - kripke::succ_iter()
  ///   - kripke::state_condition()
  ///   - kripke::format_state()
  ///
  /// The other methods of the tgba interface (like those dealing with
  /// acceptance conditions) are supplied by this kripke class and
  /// need not be defined.
  ///
  /// See also spot::kripke_succ_iterator.
  class SPOT_API kripke: public fair_kripke
  {
  public:
    kripke(const bdd_dict_ptr& d)
      : fair_kripke(d)
      {
      }

    virtual ~kripke();

    virtual
      acc_cond::mark_t state_acceptance_mark(const state*) const override;
  };

  typedef std::shared_ptr<kripke> kripke_ptr;
  typedef std::shared_ptr<const kripke> const_kripke_ptr;
}
