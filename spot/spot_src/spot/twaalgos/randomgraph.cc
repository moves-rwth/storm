// -*- coding: utf-8 -*-
// Copyright (C) 2008-2010, 2012-2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005, 2007 Laboratoire d'Informatique de
// Paris 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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
#include <spot/twaalgos/randomgraph.hh>
#include <spot/twa/twagraph.hh>
#include <spot/misc/random.hh>
#include <spot/misc/bddlt.hh>
#include <sstream>
#include <list>
#include <set>
#include <iterator>
#include <vector>

namespace spot
{

  namespace
  {
    static unsigned
    random_deterministic_labels_rec(std::vector<bdd>& labels, int *props,
                                    int props_n, bdd current, unsigned n)
    {
      if (n > 1 && props_n >= 1)
        {
          bdd ap = bdd_ithvar(*props);
          ++props;
          --props_n;

          // There are m labels generated from "current & ap"
          // and n - m labels generated from "current & !ap"
          unsigned m = rrand(1, n - 1);
          if (2 * m < n)
            {
              m = n - m;
              ap = !ap;
            }

          unsigned res = random_deterministic_labels_rec(labels, props,
                                                         props_n,
                                                         current & ap, m);
          res += random_deterministic_labels_rec(labels, props, props_n,
                                                 current & !ap, n - res);
          return res;
        }
      else
        {
          labels.emplace_back(current);
          return 1;
        }
    }

    static std::vector<bdd>
    random_deterministic_labels(int *props, int props_n, unsigned n)
    {
      std::vector<bdd> bddvec;
      random_deterministic_labels_rec(bddvec, props, props_n, bddtrue, n);
      return bddvec;
    }

    static acc_cond::mark_t
    random_acc(unsigned n_accs, float a)
    {
      acc_cond::mark_t m = {};
      for (unsigned i = 0U; i < n_accs; ++i)
        if (drand() < a)
          m.set(i);
      return m;
    }

    static acc_cond::mark_t
    random_acc1(unsigned n_accs)
    {
      auto u = static_cast<unsigned>(mrand(static_cast<int>(n_accs)));
      return acc_cond::mark_t({u});
    }

    bdd
    random_labels(int* props, int props_n, float t)
    {
      int val = 0;
      int size = 0;
      bdd p = bddtrue;
      while (props_n)
        {
          if (size == 8 * sizeof(int))
            {
              p &= bdd_ibuildcube(val, size, props);
              props += size;
              val = 0;
              size = 0;
            }
          val <<= 1;
          val |= (drand() < t);
          ++size;
          --props_n;
        }
      if (size > 0)
        p &= bdd_ibuildcube(val, size, props);

      return p;
    }
  }

  twa_graph_ptr
  random_graph(int n, float d,
               const atomic_prop_set* ap, const bdd_dict_ptr& dict,
               unsigned n_accs, float a, float t,
               bool deterministic, bool state_acc, bool colored)
  {
    if (n <= 0)
      throw std::invalid_argument("random_graph() requires n>0 states");
    auto res = make_twa_graph(dict);
    if (deterministic)
      res->prop_universal(true);
    if (state_acc)
      res->prop_state_acc(true);

    int props_n = ap->size();
    int* props = new int[props_n];

    int pi = 0;
    for (auto i: *ap)
      props[pi++] = res->register_ap(i);

    res->set_generalized_buchi(n_accs);

    // Using std::unordered_set instead of std::set for these sets is 3
    // times slower (tested on a 50000 nodes example).
    typedef std::set<int> node_set;
    node_set nodes_to_process;
    node_set unreachable_nodes;

    res->new_states(n);

    std::vector<unsigned> state_randomizer(n);
    state_randomizer[0] = 0;
    nodes_to_process.insert(0);

    for (int i = 1; i < n; ++i)
      {
        state_randomizer[i] = i;
        unreachable_nodes.insert(i);
      }

    // We want to connect each node to a number of successors between
    // 1 and n.  If the probability to connect to each successor is d,
    // the number of connected successors follows a binomial distribution.
    barand<nrand> bin(n - 1, d);

    while (!nodes_to_process.empty())
      {
        auto src = *nodes_to_process.begin();
        nodes_to_process.erase(nodes_to_process.begin());

        // Choose a random number of successors (at least one), using
        // a binomial distribution.
        unsigned nsucc = 1 + bin.rand();

        bool saw_unreachable = false;

        // Create NSUCC random labels.
        std::vector<bdd> labels;
        if (deterministic)
          {
            labels = random_deterministic_labels(props, props_n, nsucc);

            // if nsucc > 2^props_n, we cannot produce nsucc deterministic
            // edges so we set it to labels.size()
            nsucc = labels.size();
          }
        else
          for (unsigned i = 0; i < nsucc; ++i)
            labels.emplace_back(random_labels(props, props_n, t));

        int possibilities = n;
        unsigned dst;
        acc_cond::mark_t m = {};
        if (state_acc)
          m = colored ? random_acc1(n_accs) : random_acc(n_accs, a);

        for (auto& l: labels)
          {
            if (!state_acc)
              m = colored ? random_acc1(n_accs) : random_acc(n_accs, a);

            // No connection to unreachable successors so far.  This
            // is our last chance, so force it now.
            if (--nsucc == 0
                && !unreachable_nodes.empty()
                && !saw_unreachable)
              {
                // Pick a random unreachable node.
                int index = mrand(unreachable_nodes.size());
                node_set::const_iterator i = unreachable_nodes.begin();
                std::advance(i, index);

                // Link it from src.
                res->new_edge(src, *i, l, m);
                nodes_to_process.insert(*i);
                unreachable_nodes.erase(*i);
                break;
              }

            // Pick the index of a random node.
            int index = mrand(possibilities--);

            // Permute it with state_randomizer[possibilities], so
            // we cannot pick it again.
            dst = state_randomizer[index];
            state_randomizer[index] = state_randomizer[possibilities];
            state_randomizer[possibilities] = dst;

            res->new_edge(src, dst, l, m);
            auto j = unreachable_nodes.find(dst);
            if (j != unreachable_nodes.end())
              {
                nodes_to_process.insert(dst);
                unreachable_nodes.erase(j);
                saw_unreachable = true;
              }
          }

        // The node must have at least one successor.
        assert(res->get_graph().state_storage(src).succ);
      }
    // All nodes must be reachable.
    assert(unreachable_nodes.empty());
    delete[] props;
    return res;
  }
}
