// -*- coding: utf-8 -*-
// Copyright (C) 2017-2018 Laboratoire de Recherche et Développement
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

#include "config.h"
#include <spot/twaalgos/split.hh>
#include <spot/misc/minato.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/misc/bddlt.hh>

namespace spot
{
  twa_graph_ptr
  split_2step(const const_twa_graph_ptr& aut, bdd input_bdd)
  {
    auto split = make_twa_graph(aut->get_dict());
    split->copy_ap_of(aut);
    split->copy_acceptance_of(aut);
    split->new_states(aut->num_states());
    split->set_init_state(aut->get_init_state_number());

    // a sort of hash-map
    std::map<size_t, std::set<unsigned>> env_hash;

    struct trans_t
    {
      unsigned dst;
      bdd cond;
      acc_cond::mark_t acc;

      size_t hash() const
      {
        return bdd_hash()(cond)
          ^ wang32_hash(dst) ^ std::hash<acc_cond::mark_t>()(acc);
      }
    };

    std::vector<trans_t> dests;
    for (unsigned src = 0; src < aut->num_states(); ++src)
      {
        bdd support = bddtrue;
        for (const auto& e : aut->out(src))
          support &= bdd_support(e.cond);
        support = bdd_existcomp(support, input_bdd);

        bdd all_letters = bddtrue;
        while (all_letters != bddfalse)
          {
            bdd one_letter = bdd_satoneset(all_letters, support, bddtrue);
            all_letters -= one_letter;

            dests.clear();
            for (const auto& e : aut->out(src))
              {
                bdd cond = bdd_exist(e.cond & one_letter, input_bdd);
                if (cond != bddfalse)
                  dests.emplace_back(trans_t{e.dst, cond, e.acc});
              }

            bool to_add = true;
            size_t h = fnv<size_t>::init;
            for (const auto& t: dests)
              {
                h ^= t.hash();
                h *= fnv<size_t>::prime;
              }

            for (unsigned i: env_hash[h])
              {
                auto out = split->out(i);
                if (std::equal(out.begin(), out.end(),
                               dests.begin(), dests.end(),
                               [](const twa_graph::edge_storage_t& x,
                                  const trans_t& y)
                               {
                                 return   x.dst == y.dst
                                      &&  x.cond.id() == y.cond.id()
                                      &&  x.acc == y.acc;
                               }))
                  {
                    to_add = false;
                    split->new_edge(src, i, one_letter);
                    break;
                  }
              }

            if (to_add)
              {
                unsigned d = split->new_state();
                split->new_edge(src, d, one_letter);
                env_hash[h].insert(d);
                for (const auto& t: dests)
                  split->new_edge(d, t.dst, t.cond, t.acc);
              }
          }
      }

    split->merge_edges();

    split->prop_universal(spot::trival::maybe());
    return split;
  }

  twa_graph_ptr unsplit_2step(const const_twa_graph_ptr& aut)
  {
    twa_graph_ptr out = make_twa_graph(aut->get_dict());
    out->copy_acceptance_of(aut);
    out->copy_ap_of(aut);
    out->new_states(aut->num_states());
    out->set_init_state(aut->get_init_state_number());

    std::vector<bool> seen(aut->num_states(), false);
    std::deque<unsigned> todo;
    todo.push_back(aut->get_init_state_number());
    seen[aut->get_init_state_number()] = true;
    while (!todo.empty())
      {
        unsigned cur = todo.front();
        todo.pop_front();
        seen[cur] = true;

        for (const auto& i : aut->out(cur))
          for (const auto& o : aut->out(i.dst))
            {
              out->new_edge(cur, o.dst, i.cond & o.cond, i.acc | o.acc);
              if (!seen[o.dst])
                todo.push_back(o.dst);
            }
      }
    return out;
  }

  twa_graph_ptr split_edges(const const_twa_graph_ptr& aut)
  {
    twa_graph_ptr out = make_twa_graph(aut->get_dict());
    out->copy_acceptance_of(aut);
    out->copy_ap_of(aut);
    out->prop_copy(aut, twa::prop_set::all());
    out->new_states(aut->num_states());
    out->set_init_state(aut->get_init_state_number());

    internal::univ_dest_mapper<twa_graph::graph_t> uniq(out->get_graph());

    bdd all = aut->ap_vars();
    for (auto& e: aut->edges())
      {
        bdd cond = e.cond;
        if (cond == bddfalse)
          continue;
        unsigned dst = e.dst;
        if (aut->is_univ_dest(dst))
          {
            auto d = aut->univ_dests(dst);
            dst = uniq.new_univ_dests(d.begin(), d.end());
          }
        while (cond != bddfalse)
          {
            bdd cube = bdd_satoneset(cond, all, bddfalse);
            cond -= cube;
            out->new_edge(e.src, dst, cube, e.acc);
          }
      }
    return out;
  }
}
