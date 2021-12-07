// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2018  Laboratoire de Recherche et Developpement de
// l'Epita (LRDE).
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

#include <bddx.h>
#include <spot/misc/hash.hh>
#include <spot/tl/formula.hh>
#include <spot/twa/twagraph.hh>

namespace spot
{
  class acc_mapper_common
  {
  protected:
    bdd_dict_ptr dict_;
    twa_graph_ptr aut_;

    acc_mapper_common(const twa_graph_ptr& aut)
      : dict_(aut->get_dict()), aut_(aut)
    {
    }
  };

  class acc_mapper_string: public acc_mapper_common
  {
    std::unordered_map<std::string, unsigned> map_;

  public:
    acc_mapper_string(const twa_graph_ptr& aut)
      : acc_mapper_common(aut)
    {
    }

    // Declare an acceptance name.
    bool declare(const std::string& name)
    {
      auto i = map_.find(name);
      if (i != map_.end())
        return true;
      auto v = aut_->acc().add_set();
      map_[name] = v;
      return true;
    }

    std::pair<bool, acc_cond::mark_t> lookup(std::string name) const
    {
       auto p = map_.find(name);
       if (p == map_.end())
         return std::make_pair(false, acc_cond::mark_t({}));
       return std::make_pair(true, acc_cond::mark_t({p->second}));
    }
  };


  // The acceptance sets are named using count consecutive integers.
  class acc_mapper_consecutive_int: public acc_mapper_common
  {
  public:
    acc_mapper_consecutive_int(const twa_graph_ptr& aut, unsigned count)
      : acc_mapper_common(aut)
    {
      std::vector<unsigned> vmap(count);
      aut->acc().add_sets(count);
    }

    std::pair<bool, acc_cond::mark_t> lookup(unsigned n)
    {
      if (n < aut_->acc().num_sets())
        return std::make_pair(true, acc_cond::mark_t({n}));
      else
        return std::make_pair(false, acc_cond::mark_t({}));
    }
  };

  // The acceptance sets are named using count integers, but we do not
  // assume the numbers are necessarily consecutive.
  class acc_mapper_int: public acc_mapper_consecutive_int
  {
    unsigned used_;
    std::map<unsigned, acc_cond::mark_t> map_;

  public:
    acc_mapper_int(const twa_graph_ptr& aut, unsigned count)
      : acc_mapper_consecutive_int(aut, count), used_(0)
    {
    }

    std::pair<bool, acc_cond::mark_t> lookup(unsigned n)
    {
       auto p = map_.find(n);
       if (p != map_.end())
         return std::make_pair(true, p->second);
       if (used_ < aut_->acc().num_sets())
         {
           auto res = acc_cond::mark_t({used_++});
           map_[n] = res;
           return std::make_pair(true, res);
         }
       return std::make_pair(false, acc_cond::mark_t({}));
    }
  };
}
