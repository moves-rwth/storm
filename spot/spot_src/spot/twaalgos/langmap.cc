// -*- coding: utf-8 -*-
// Copyright (C) 2016-2018 Laboratoire de Recherche et Développement
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
#include <numeric>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/langmap.hh>
#include <spot/twaalgos/remfin.hh>

namespace spot
{
  std::vector<unsigned>
  language_map(const const_twa_graph_ptr& aut)
  {
    if (!is_deterministic(aut))
      throw std::runtime_error(
          "language_map only works with deterministic automata");

    unsigned n_states = aut->num_states();
    std::vector<unsigned> res(n_states);
    std::iota(std::begin(res), std::end(res), 0);

    std::vector<twa_graph_ptr> alt_init_st_auts(n_states);
    std::vector<twa_graph_ptr> compl_alt_init_st_auts(n_states);

    // Prepare all automata needed.
    for (unsigned i = 0; i < n_states; ++i)
      {
        twa_graph_ptr c = make_twa_graph(aut, twa::prop_set::all());
        assert(c); // for some reason g++ 7.3 thinks this could be null
        c->set_init_state(i);
        alt_init_st_auts[i] = c;
        compl_alt_init_st_auts[i] = remove_fin(dualize(c));
      }

    for (unsigned i = 1; i < n_states; ++i)
      for (unsigned j = 0; j < i; ++j)
        {
          if (res[j] != j)
            continue;

          if (!alt_init_st_auts[i]->intersects(compl_alt_init_st_auts[j])
              && (!compl_alt_init_st_auts[i]->intersects(alt_init_st_auts[j])))
            {
              res[i] = res[j];
              break;
            }
        }

    return res;
  }

  void highlight_languages(twa_graph_ptr& aut)
  {
    std::vector<unsigned> lang = language_map(aut);
    unsigned lang_sz = lang.size();
    std::vector<unsigned> cnt(lang_sz, 0);
    for (unsigned v: lang)
      {
        assert(v < lang_sz);
        ++cnt[v];
      }

    unsigned color = 0;
    auto hs = new std::map<unsigned, unsigned>;
    aut->set_named_prop("highlight-states", hs);

    assert(lang_sz == aut->num_states());

    // Give a unique color number to each state that has not a unique
    // language.  This assumes that lang[i] <= i, as guaranteed by
    // language_map.
    for (unsigned i = 0; i < lang_sz; ++i)
      {
        unsigned v = lang[i];
        if (cnt[v] > 1)
          {
            if (v == i)
              lang[i] = color++;
            else
              assert(v < i);
            (*hs)[i] = lang[v];
          }
      }
  }
}
