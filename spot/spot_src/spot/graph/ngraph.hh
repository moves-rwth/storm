// -*- coding: utf-8 -*-
// Copyright (C) 2014, 2015, 2016 Laboratoire de Recherche et Développement
// de l'Epita.
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

#include <unordered_map>
#include <vector>
#include <spot/graph/graph.hh>

namespace spot
{
  template <typename Graph,
            typename State_Name,
            typename Name_Hash = std::hash<State_Name>,
            typename Name_Equal = std::equal_to<State_Name>>
  class SPOT_API named_graph
  {
  protected:
    Graph& g_;
  public:

    typedef typename Graph::state state;
    typedef typename Graph::edge edge;
    typedef State_Name name;

    typedef std::unordered_map<name, state,
                               Name_Hash, Name_Equal> name_to_state_t;
    name_to_state_t name_to_state;
    typedef std::vector<name> state_to_name_t;
    state_to_name_t state_to_name;

    named_graph(Graph& g)
      : g_(g)
    {
    }

    Graph& graph()
    {
      return g_;
    }

    Graph& graph() const
    {
      return g_;
    }

    template <typename... Args>
    state new_state(name n, Args&&... args)
    {
      auto p = name_to_state.emplace(n, 0U);
      if (p.second)
        {
          unsigned s = g_.new_state(std::forward<Args>(args)...);
          p.first->second = s;
          if (state_to_name.size() < s + 1)
            state_to_name.resize(s + 1);
          state_to_name[s] = n;
          return s;
        }
      return p.first->second;
    }

    /// \brief Give an alternate name to a state.
    /// \return true iff the newname state was already existing
    /// (in this case the existing newname state will be merged
    /// with state s: the newname will be unreachable and without
    /// successors.)
    bool alias_state(state s, name newname)
    {
      auto p = name_to_state.emplace(newname, s);
      if (!p.second)
        {
          // The state already exists.  Change its number.
          auto old = p.first->second;
          p.first->second = s;
          // Add the successor of OLD to those of S.
          auto& trans = g_.edge_vector();
          auto& states = g_.states();
          trans[states[s].succ_tail].next_succ = states[old].succ;
          states[s].succ_tail = states[old].succ_tail;
          states[old].succ = 0;
          states[old].succ_tail = 0;
          // Remove all references to old in edges:
          unsigned tend = trans.size();
          for (unsigned t = 1; t < tend; ++t)
            {
              if (trans[t].src == old)
                trans[t].src = s;
              if (trans[t].dst == old)
                trans[t].dst = s;
            }
        }
      return !p.second;
    }

    state get_state(name n) const
    {
      return name_to_state.at(n);
    }

    name get_name(state s) const
    {
      return state_to_name.at(s);
    }

    bool has_state(name n) const
    {
      return name_to_state.find(n) != name_to_state.end();
    }

    const state_to_name_t& names() const
    {
      return state_to_name;
    }

    template <typename... Args>
    edge
    new_edge(name src, name dst, Args&&... args)
    {
      return g_.new_edge(get_state(src), get_state(dst),
                         std::forward<Args>(args)...);
    }

    template <typename I, typename... Args>
    edge
    new_univ_edge(name src, I dst_begin, I dst_end, Args&&... args)
    {
      std::vector<unsigned> d;
      d.reserve(std::distance(dst_begin, dst_end));
      while (dst_begin != dst_end)
        d.emplace_back(get_state(*dst_begin++));
      return g_.new_univ_edge(get_state(src), d.begin(), d.end(),
                              std::forward<Args>(args)...);
    }

    template <typename... Args>
    edge
    new_univ_edge(name src,
                  const std::initializer_list<State_Name>& dsts, Args&&... args)
    {
      return new_univ_edge(src, dsts.begin(), dsts.end(),
                           std::forward<Args>(args)...);
    }
  };
}
