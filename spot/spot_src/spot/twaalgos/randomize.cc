// -*- coding: utf-8 -*-
// Copyright (C) 2014-2018 Laboratoire de Recherche et Développement
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

#include "config.h"
#include <algorithm>
#include <numeric>
#include <random>
#include <spot/twaalgos/randomize.hh>
#include <spot/misc/random.hh>

namespace spot
{
  void
  randomize(twa_graph_ptr& aut, bool randomize_states,
            bool randomize_edges)
  {
    if (!aut->is_existential())
      throw std::runtime_error
        ("randomize() does not yet support alternation");

    if (!randomize_states && !randomize_edges)
      return;
    auto& g = aut->get_graph();
    if (randomize_states)
      {
        unsigned n = g.num_states();
        std::vector<unsigned> nums(n);
        std::iota(nums.begin(), nums.end(), 0);
        mrandom_shuffle(nums.begin(), nums.end());
        g.rename_states_(nums);
        aut->set_init_state(nums[aut->get_init_state_number()]);

        if (auto sn =
            aut->get_named_prop<std::vector<std::string>>("state-names"))
          {
            unsigned sns = sn->size(); // Might be != n.
            auto nn = new std::vector<std::string>(n);
            for (unsigned i = 0; i < sns && i < n; ++i)
              (*nn)[nums[i]] = (*sn)[i];
            aut->set_named_prop("state-names", nn);
          }
        if (auto hs = aut->get_named_prop<std::map<unsigned, unsigned>>
            ("highlight-states"))
          {
            std::map<unsigned, unsigned> hs2;
            for (auto p: *hs)
              hs2[nums[p.first]] = p.second;
            std::swap(*hs, hs2);
          }
      }
    if (randomize_edges)
      {
        g.remove_dead_edges_();
        auto& v = g.edge_vector();
        mrandom_shuffle(v.begin() + 1, v.end());
      }
    aut->set_named_prop("highlight-edges", nullptr);
    typedef twa_graph::graph_t::edge_storage_t tr_t;
    g.sort_edges_([](const tr_t& lhs, const tr_t& rhs)
                        { return lhs.src < rhs.src; });
    g.chain_edges_();
  }
}
