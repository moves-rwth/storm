// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2016, 2018 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "spot/priv/freelist.hh"
#include <list>
#include <utility>

namespace spot
{
  /// \ingroup misc_tools
  /// \brief Manage ranges of variables.
  class bdd_allocator: private free_list
  {
  public:
    /// Default constructor.
    bdd_allocator();

    bdd_allocator(const bdd_allocator& other)
      : free_list(other), lvarnum(other.lvarnum)
    {
    }

    bdd_allocator& operator=(const bdd_allocator& other)
    {
      free_list::operator=(other);
      lvarnum = other.lvarnum;
      return *this;
    }

    /// Initialize the BDD library.
    static void initialize();
    /// Allocate \a n BDD variables.
    int allocate_variables(int n);
    /// Release \a n BDD variables starting at \a base.
    void release_variables(int base, int n);

    using free_list::dump_free_list;
  protected:
    static bool initialized; ///< Whether the BDD library has been initialized.
    int lvarnum; ///< number of variables in use in this allocator.
  private:
    /// Require more variables.
    void extvarnum(int more);
    virtual int extend(int n) override;
  };
}
