// -*- coding: utf-8 -*-
// Copyright (C) 2010-2011, 2013-2016, 2018 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "config.h"
#include <spot/twaalgos/gtec/ce.hh>
#include <spot/twaalgos/bfssteps.hh>
#include <spot/misc/hash.hh>

namespace spot
{
  namespace
  {
    class shortest_path final: public bfs_steps
    {
    public:
      shortest_path(const state_set* t,
                    const std::shared_ptr<const couvreur99_check_status>& ecs,
                    couvreur99_check_result* r)
        : bfs_steps(ecs->aut), target(t), ecs(ecs), r(r)
      {
      }

      const state*
      search(const state* start, twa_run::steps& l)
      {
        return this->bfs_steps::search(filter(start), l);
      }

      const state*
      filter(const state* s) override
      {
        r->inc_ars_prefix_states();
        auto i = ecs->h.find(s);
        s->destroy();
        // Ignore unknown states ...
        if (i == ecs->h.end())
          return nullptr;
        // ... as well as dead states.
        if (i->second == -1)
          return nullptr;
        return i->first;
      }

      bool
      match(twa_run::step&, const state* dest) override
      {
        return target->find(dest) != target->end();
      }

    private:
      state_set seen;
      const state_set* target;
      std::shared_ptr<const couvreur99_check_status> ecs;
      couvreur99_check_result* r;
    };
  }

  couvreur99_check_result::couvreur99_check_result
  (const std::shared_ptr<const couvreur99_check_status>& ecs,
   option_map o)
    : emptiness_check_result(ecs->aut, o), ecs_(ecs)
  {
  }

  unsigned
  couvreur99_check_result::acss_states() const
  {
    int scc_root = ecs_->root.top().index;
    unsigned count = 0;
    for (auto i: ecs_->h)
      if (i.second >= scc_root)
        ++count;
    return count;
  }

  twa_run_ptr
  couvreur99_check_result::accepting_run()
  {
    run_ = std::make_shared<twa_run>(ecs_->aut);

    assert(!ecs_->root.empty());

    // Compute an accepting cycle.
    accepting_cycle();

    // Compute the prefix: it's the shortest path from the initial
    // state of the automata to any state of the cycle.

    // Register all states from the cycle as target of the BFS.
    state_set ss;
    for (twa_run::steps::const_iterator i = run_->cycle.begin();
         i != run_->cycle.end(); ++i)
      ss.insert(i->s);
    shortest_path shpath(&ss, ecs_, this);

    const state* prefix_start = ecs_->aut->get_init_state();
    // There are two cases: either the initial state is already on
    // the cycle, or it is not.  If it is, we will have to rotate
    // the cycle so it begins on this position.  Otherwise we will shift
    // the cycle so it begins on the state that follows the prefix.
    // cycle_entry_point is that state.
    const state* cycle_entry_point;
    state_set::const_iterator ps = ss.find(prefix_start);
    if (ps != ss.end())
      {
        // The initial state is on the cycle.
        prefix_start->destroy();
        cycle_entry_point = *ps;
      }
    else
      {
        // This initial state is outside the cycle.  Compute the prefix.
        cycle_entry_point = shpath.search(prefix_start, run_->prefix);
      }

    // Locate cycle_entry_point on the cycle.
    twa_run::steps::iterator cycle_ep_it;
    for (cycle_ep_it = run_->cycle.begin();
         cycle_ep_it != run_->cycle.end()
           && cycle_entry_point->compare(cycle_ep_it->s); ++cycle_ep_it)
      continue;
    assert(cycle_ep_it != run_->cycle.end());

    // Now shift the cycle so it starts on cycle_entry_point.
    run_->cycle.splice(run_->cycle.end(), run_->cycle,
                       run_->cycle.begin(), cycle_ep_it);

    return run_;
  }

  void
  couvreur99_check_result::accepting_cycle()
  {
    acc_cond::mark_t acc_to_traverse =
      ecs_->aut->acc().accepting_sets(ecs_->root.top().condition);
    // Compute an accepting cycle using successive BFS that are
    // restarted from the point reached after we have discovered a
    // transition with a new acceptance conditions.
    //
    // This idea is taken from Product<T>::findWitness in LBTT 1.1.2,
    // which in turn is probably inspired from
    // @Article{          latvala.00.fi,
    //   author        = {Timo Latvala and Keijo Heljanko},
    //   title                = {Coping With Strong Fairness},
    //   journal        = {Fundamenta Informaticae},
    //   year                = {2000},
    //   volume        = {43},
    //   number        = {1--4},
    //   pages                = {1--19},
    //   publisher        = {IOS Press}
    // }
    const state* substart = ecs_->cycle_seed;
    do
      {
        struct scc_bfs final: bfs_steps
        {
          const couvreur99_check_status* ecs;
          couvreur99_check_result* r;
          acc_cond::mark_t& acc_to_traverse;
          int scc_root;

          scc_bfs(const couvreur99_check_status* ecs,
                  couvreur99_check_result* r, acc_cond::mark_t& acc_to_traverse)
            : bfs_steps(ecs->aut), ecs(ecs), r(r),
              acc_to_traverse(acc_to_traverse),
              scc_root(ecs->root.top().index)
          {
          }

          virtual const state*
          filter(const state* s) override
          {
            auto i = ecs->h.find(s);
            s->destroy();
            // Ignore unknown states.
            if (i == ecs->h.end())
              return nullptr;
            // Stay in the final SCC.
            if (i->second < scc_root)
              return nullptr;
            r->inc_ars_cycle_states();
            return i->first;
          }

          virtual bool
          match(twa_run::step& st, const state* s) override
          {
            acc_cond::mark_t less_acc =
              acc_to_traverse - st.acc;
            if (less_acc != acc_to_traverse
                || (!acc_to_traverse
                    && s == ecs->cycle_seed))
              {
                acc_to_traverse = less_acc;
                return true;
              }
            return false;
          }

        } b(ecs_.get(), this, acc_to_traverse);

        substart = b.search(substart, run_->cycle);
        assert(substart);
      }
    while (acc_to_traverse || substart != ecs_->cycle_seed);
  }

  void
  couvreur99_check_result::print_stats(std::ostream& os) const
  {
    ecs_->print_stats(os);
    // FIXME: This is bogusly assuming run_ exists.  (Even if we
    // created it, the user might have deleted it.)
    os << run_->prefix.size() << " states in run_->prefix" << std::endl;
    os << run_->cycle.size() << " states in run_->cycle" << std::endl;
  }

}
