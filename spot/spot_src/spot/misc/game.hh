// -*- coding: utf-8 -*-
// Copyright (C) 2017-2019 Laboratoire de Recherche et Développement
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

#include <algorithm>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include <bddx.h>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/parity.hh>

namespace spot
{

class SPOT_API parity_game
{
private:
  const const_twa_graph_ptr arena_;
  const std::vector<bool> owner_;

public:
  /// \a parity_game provides an interface to manipulate a colorized parity
  /// automaton as a parity game, including methods to solve the game.
  /// The input automaton (arena) should be colorized and have a max-odd parity
  /// acceptance condition.
  ///
  /// \param arena the underlying parity automaton
  /// \param owner a vector of Booleans indicating the owner of each state:
  ///   true stands for Player 1, false stands for Player 0.
  parity_game(const twa_graph_ptr& arena, const std::vector<bool>& owner);

  unsigned num_states() const
  {
    return arena_->num_states();
  }

  unsigned get_init_state_number() const
  {
    return arena_->get_init_state_number();
  }

  internal::state_out<const twa_graph::graph_t>
  out(unsigned src) const
  {
    return arena_->out(src);
  }

  internal::state_out<const twa_graph::graph_t>
  out(unsigned src)
  {
    return arena_->out(src);
  }

  bool owner(unsigned src) const
  {
    return owner_[src];
  }

  unsigned max_parity() const
  {
    unsigned max_parity = 0;
      for (const auto& e: arena_->edges())
        max_parity = std::max(max_parity, e.acc.max_set());
    SPOT_ASSERT(max_parity);
    return max_parity - 1;
  }

  /// Print the parity game in PGSolver's format.
  void print(std::ostream& os);

  typedef std::unordered_set<unsigned> region_t;
  // Map state number to index of the transition to take.
  typedef std::unordered_map<unsigned, unsigned> strategy_t;

  /// Compute the winning strategy and winning region of this game for player
  /// 1 using Zielonka's recursive algorithm. \cite zielonka.98.tcs
  void solve(region_t (&w)[2], strategy_t (&s)[2]) const;

private:
  typedef twa_graph::graph_t::edge_storage_t edge_t;

  // Compute (in place) a set of states from which player can force a visit
  // through set, and a strategy to do it.
  // if attr_max is true, states that can force a visit through an edge with
  // max parity are also counted in.
  strategy_t attractor(const region_t& subgame, region_t& set,
                       unsigned max_parity, int odd,
                       bool attr_max = false) const;

  // Compute the winning strategy and winning region for both players.
  void solve_rec(region_t& subgame, unsigned max_parity,
                 region_t (&w)[2], strategy_t (&s)[2]) const;
};

}
