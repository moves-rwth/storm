// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2016, 2019 Laboratoire de Recherche et
// Developpement de l'Epita (LRDE)
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

#include <spot/kripke/kripke.hh>
#include <spot/tl/apcollect.hh>

namespace spot
{
  struct spins_interface;

  class SPOT_API ltsmin_model final
  {
  public:
    ~ltsmin_model();

    // \brief Load an ltsmin model, either from divine or promela.
    //
    // The filename given can be either a *.pm/*.pml/*.prom promela
    // source or a *.spins dynamic library compiled with "spins file".
    // If a promela source is supplied, this function will call spins to
    // update the *.spins library only if it is not newer.
    //
    // Similarly the divine models can be specified as *.dve source or
    // *.dve or *.dve2C libraries.
    //
    static ltsmin_model load(const std::string& file);

    // \brief Generate a Kripke structure on-the-fly
    //
    // The dead parameter is used to control the behavior of the model
    // on dead states (i.e. the final states of finite sequences).  If
    // DEAD is formula::ff(), it means we are not interested in finite
    // sequences of the system, and dead state will have no successor.
    // If DEAD is formula::tt(), we want to check finite sequences as
    // well as infinite sequences, but do not need to distinguish
    // them.  In that case dead state will have a loop labeled by
    // true.  If DEAD is any atomic proposition (formula::ap("...")),
    // this is the name a property that should be true when looping on
    // a dead state, and false otherwise.
    //
    // This function returns nullptr on error.
    //
    // \a to_observe the list of atomic propositions that should be observed
    //               in the model
    // \a dict the BDD dictionary to use
    // \a dead an atomic proposition or constant to use for looping on
    //         dead states
    // \a compress whether to compress the states.  Use 0 to disable, 1
    // to enable compression, 2 to enable a faster compression that only
    // work if all variables are smaller than 2^28.
    kripke_ptr kripke(const atomic_prop_set* to_observe,
                      bdd_dict_ptr dict,
                      formula dead = formula::tt(),
                      int compress = 0) const;

    /// Number of variables in a state
    int state_size() const;
    /// Name of each variable
    const char* state_variable_name(int var) const;
    /// Type of each variable
    int state_variable_type(int var) const;
    /// Number of different types
    int type_count() const;
    /// Name of each type
    const char* type_name(int type) const;
    /// Count of enumerated values for a type
    int type_value_count(int type);
    /// Name of each enumerated value for a type
    const char* type_value_name(int type, int val);

  private:
    ltsmin_model(std::shared_ptr<const spins_interface> iface) : iface(iface)
      {
      }
    std::shared_ptr<const spins_interface> iface;
  };
}
