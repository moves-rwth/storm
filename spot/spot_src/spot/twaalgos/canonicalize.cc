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
#include <spot/twaalgos/canonicalize.hh>
#include <spot/twa/twagraph.hh>
#include <set>
#include <vector>

namespace
{
  typedef std::pair<spot::twa_graph::graph_t::edge_data_t, unsigned>
    edge_sig_t;

  struct signature_t
  {
    std::vector<edge_sig_t> ingoing;
    std::vector<edge_sig_t> outgoing;
    unsigned classnum;

    bool
    operator<(const signature_t& o) const
    {
      return ingoing != o.ingoing ? ingoing < o.ingoing :
        outgoing != o.outgoing ? outgoing < o.outgoing :
        classnum < o.classnum;
    }
  };

  typedef std::map<signature_t, std::vector<unsigned>> sig2states_t;

  static sig2states_t
  sig_to_states(spot::twa_graph_ptr aut, std::vector<unsigned>& state2class)
  {
    std::vector<signature_t> signature(aut->num_states(), signature_t());

    for (auto& t : aut->edges())
      {
        signature[t.dst].ingoing.emplace_back(t.data(), state2class[t.src]);
        signature[t.src].outgoing.emplace_back(t.data(), state2class[t.dst]);
      }

    sig2states_t sig2states;
    for (unsigned s = 0; s < aut->num_states(); ++s)
      {
        std::sort(signature[s].ingoing.begin(), signature[s].ingoing.end());
        std::sort(signature[s].outgoing.begin(), signature[s].outgoing.end());
        signature[s].classnum = state2class[s];
        sig2states[signature[s]].emplace_back(s);
      }

    return sig2states;
  }
}

namespace spot
{
  twa_graph_ptr
  canonicalize(twa_graph_ptr aut)
  {
    if (!aut->is_existential())
      throw std::runtime_error
        ("canonicalize does not yet support alternation");
    std::vector<unsigned> state2class(aut->num_states(), 0);
    state2class[aut->get_init_state_number()] = 1;
    size_t distinct_classes = 2;
    sig2states_t sig2states = sig_to_states(aut, state2class);

    while (sig2states.size() != distinct_classes &&
           sig2states.size() != aut->num_states())
      {
        distinct_classes = sig2states.size();

        unsigned classnum = 0;
        for (auto& s: sig2states)
          {
            for (auto& state: s.second)
              state2class[state] = classnum;
            ++classnum;
          }

        sig2states = sig_to_states(aut, state2class);
      }

    unsigned classnum = 0;
    for (auto& s: sig2states)
      for (auto& state: s.second)
        state2class[state] = classnum++;

    auto& g = aut->get_graph();
    g.rename_states_(state2class);
    aut->set_init_state(state2class[aut->get_init_state_number()]);
    g.sort_edges_();
    g.chain_edges_();
    return aut;
  }
}
