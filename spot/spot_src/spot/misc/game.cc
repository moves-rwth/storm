// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement
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

#include <cmath>
#include <spot/misc/game.hh>

namespace spot
{

parity_game::parity_game(const twa_graph_ptr& arena,
                         const std::vector<bool>& owner)
  : arena_(arena)
  , owner_(owner)
{
  bool max, odd;
  arena_->acc().is_parity(max, odd, true);
  if (!(max && odd))
    throw std::runtime_error("arena must have max-odd acceptance condition");

  for (const auto& e : arena_->edges())
    if (e.acc.max_set() == 0)
      throw std::runtime_error("arena must be colorized");

  assert(owner_.size() == arena_->num_states());
}

void parity_game::print(std::ostream& os)
{
  os << "parity " << num_states() - 1 << ";\n";
  std::vector<bool> seen(num_states(), false);
  std::vector<unsigned> todo({get_init_state_number()});
  while (!todo.empty())
    {
      unsigned src = todo.back();
      todo.pop_back();
      if (seen[src])
        continue;
      seen[src] = true;
      os << src << ' ';
      os << out(src).begin()->acc.max_set() - 1 << ' ';
      os << owner(src) << ' ';
      bool first = true;
      for (auto& e: out(src))
        {
          if (!first)
            os << ',';
          first = false;
          os << e.dst;
          if (!seen[e.dst])
            todo.push_back(e.dst);
        }
      if (src == get_init_state_number())
        os << " \"INIT\"";
      os << ";\n";
    }
}

void parity_game::solve(region_t (&w)[2], strategy_t (&s)[2]) const
{
  region_t states_;
  for (unsigned i = 0; i < num_states(); ++i)
    states_.insert(i);
  unsigned m = max_parity();
  solve_rec(states_, m, w, s);
}

parity_game::strategy_t
parity_game::attractor(const region_t& subgame, region_t& set,
                       unsigned max_parity, int p, bool attr_max) const
{
  strategy_t strategy;
  std::set<unsigned> complement(subgame.begin(), subgame.end());
  for (unsigned s: set)
    complement.erase(s);

  acc_cond::mark_t max_acc({});
  for (unsigned i = 0; i <= max_parity; ++i)
    max_acc.set(i);

  bool once_more;
  do
    {
      once_more = false;
      for (auto it = complement.begin(); it != complement.end();)
        {
          unsigned s = *it;
          unsigned i = 0;

          bool is_owned = owner_[s] == p;
          bool wins = !is_owned;

          for (const auto& e: out(s))
            {
              if ((e.acc & max_acc) && subgame.count(e.dst))
                {
                  if (set.count(e.dst)
                      || (attr_max && e.acc.max_set() - 1 == max_parity))
                    {
                      if (is_owned)
                        {
                          strategy[s] = i;
                          wins = true;
                          break; // no need to check all edges
                        }
                    }
                  else
                    {
                      if (!is_owned)
                        {
                          wins = false;
                          break; // no need to check all edges
                        }
                    }
                }
              ++i;
            }

          if (wins)
            {
              // FIXME C++17 extract/insert could be useful here
              set.emplace(s);
              it = complement.erase(it);
              once_more = true;
            }
          else
            ++it;
        }
    } while (once_more);
  return strategy;
}

void parity_game::solve_rec(region_t& subgame, unsigned max_parity,
                            region_t (&w)[2], strategy_t (&s)[2]) const
{
  assert(w[0].empty());
  assert(w[1].empty());
  assert(s[0].empty());
  assert(s[1].empty());
  // The algorithm works recursively on subgames. To avoid useless copies of
  // the game at each call, subgame and max_parity are used to filter states
  // and transitions.
  if (subgame.empty())
    return;
  int p = max_parity % 2;

  // Recursion on max_parity.
  region_t u;
  auto strat_u = attractor(subgame, u, max_parity, p, true);

  if (max_parity == 0)
    {
      s[p] = std::move(strat_u);
      w[p] = std::move(u);
      // FIXME what about w[!p]?
      return;
    }

  for (unsigned s: u)
    subgame.erase(s);
  region_t w0[2]; // Player's winning region in the first recursive call.
  strategy_t s0[2]; // Player's winning strategy in the first recursive call.
  solve_rec(subgame, max_parity - 1, w0, s0);
  if (w0[0].size() + w0[1].size() != subgame.size())
    throw std::runtime_error("size mismatch");
  //if (w0[p].size() != subgame.size())
  //  for (unsigned s: subgame)
  //    if (w0[p].find(s) == w0[p].end())
  //      w0[!p].insert(s);
  subgame.insert(u.begin(), u.end());

  if (w0[p].size() + u.size() == subgame.size())
    {
      s[p] = std::move(strat_u);
      s[p].insert(s0[p].begin(), s0[p].end());
      w[p].insert(subgame.begin(), subgame.end());
      return;
    }

  // Recursion on game size.
  auto strat_wnp = attractor(subgame, w0[!p], max_parity, !p);

  for (unsigned s: w0[!p])
    subgame.erase(s);

  region_t w1[2]; // Odd's winning region in the second recursive call.
  strategy_t s1[2]; // Odd's winning strategy in the second recursive call.
  solve_rec(subgame, max_parity, w1, s1);
  if (w1[0].size() + w1[1].size() != subgame.size())
    throw std::runtime_error("size mismatch");

  w[p] = std::move(w1[p]);
  s[p] = std::move(s1[p]);

  w[!p] = std::move(w1[!p]);
  w[!p].insert(w0[!p].begin(), w0[!p].end());
  s[!p] = std::move(strat_wnp);
  s[!p].insert(s0[!p].begin(), s0[!p].end());
  s[!p].insert(s1[!p].begin(), s1[!p].end());

  subgame.insert(w0[!p].begin(), w0[!p].end());
}

}
