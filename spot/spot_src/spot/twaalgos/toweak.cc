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
#include <spot/misc/bddlt.hh>
#include <spot/misc/minato.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/toweak.hh>

#include <queue>
#include <functional>

namespace spot
{
  namespace
  {
    struct rc_state
    {
      unsigned id;
      unsigned rank;
      acc_cond::mark_t mark;

      rc_state(unsigned state_id, unsigned state_rank,
               acc_cond::mark_t m = acc_cond::mark_t({}))
       : id(state_id), rank(state_rank), mark(m)
      {
      }
    };

    struct rc_state_hash
    {
      size_t
      operator()(const rc_state& s) const noexcept
      {
        using std::hash;
        return ((hash<unsigned>()(s.id)
                 ^ (hash<unsigned>()(s.rank)))
                 ^ (hash<acc_cond::mark_t>()(s.mark)));
      }
    };

    struct rc_state_equal
    {
      size_t
      operator()(const rc_state& left, const rc_state& right) const
      {
        return left.id == right.id
               && left.rank == right.rank
               && left.mark == right.mark;
      }
    };

    class to_weak
    {
      private:
        const const_twa_graph_ptr aut_;
        const unsigned numsets_;
        std::unordered_map<rc_state,
                           unsigned,
                           rc_state_hash,
                           rc_state_equal> state_map_;
        std::vector<bdd> state_to_var_;
        std::unordered_map<bdd, unsigned, spot::bdd_hash> var_to_state_;
        bdd all_states_;
        twa_graph_ptr res_;
        std::queue<rc_state> todo_;
        bool less_;

        unsigned new_state(unsigned st, unsigned rank, acc_cond::mark_t mark)
        {
          rc_state s(st, rank, mark);
          auto p = state_map_.emplace(s, 0);
          if (p.second)
            {
              p.first->second = res_->new_state();
              todo_.emplace(s);
              int v = aut_->get_dict()->register_anonymous_variables(1, this);
              bdd var = bdd_ithvar(v);
              all_states_ &= var;
              state_to_var_.push_back(var);
              var_to_state_.emplace(var, p.first->second);
            }
          return p.first->second;
        };

        bdd transition_function(rc_state st)
        {
          unsigned id = st.id;
          unsigned rank = st.rank;

          bdd res = bddfalse;

          bool rank_odd = rank % 2;
          for (auto& e : aut_->out(id))
            {
              // If we are on odd level and the edge is marked with the set we
              // don't want to see, skip. (delete transition).
              if (rank_odd && (e.acc & st.mark))
                continue;

              bdd dest = bddtrue;
              for (unsigned d : aut_->univ_dests(e.dst))
                {
                  bdd levels = bddfalse;
                  int curr = static_cast<int>(rank);
                  // We must always be able to go to the previous even rank
                  int lower = less_ ? ((curr - 1) & ~1) : 0;
                  for (int i = curr, start_set = st.mark.min_set() - 1;
                       i >= lower; --i, start_set = 0)
                    {
                      if (i % 2)
                        for (unsigned m = start_set; m < numsets_; ++m)
                          levels |= state_to_var_[new_state(d, i, {m})];
                      else
                        levels |= state_to_var_[new_state(d, i, {})];
                    }
                  dest &= levels;
                }
              res |= (dest & e.cond);
            }
          return res;
        }

      public:
        to_weak(const const_twa_graph_ptr& aut, bool less)
          : aut_(aut),
            numsets_(aut_->num_sets()),
            all_states_(bddtrue),
            res_(make_twa_graph(aut_->get_dict())),
            less_(less)
        {
          res_->copy_ap_of(aut_);
          res_->set_buchi();
          res_->prop_weak(true);
        }

        ~to_weak()
        {
          aut_->get_dict()->unregister_all_my_variables(this);
        }

        twa_graph_ptr run()
        {
          std::vector<unsigned> states;
          for (unsigned d: aut_->univ_dests(aut_->get_init_state_number()))
            states.push_back(new_state(d, aut_->num_states() * 2, {}));

          res_->set_univ_init_state(states.begin(), states.end());

          while (!todo_.empty())
            {
              rc_state st = todo_.front();

              acc_cond::mark_t mark = {};
              if (st.rank % 2)
                mark = {0};

              bdd delta = transition_function(st);
              bdd ap = bdd_exist(bdd_support(delta), all_states_);
              bdd letters = bdd_exist(delta, all_states_);

              while (letters != bddfalse)
                {
                  bdd oneletter = bdd_satoneset(letters, ap, bddtrue);
                  letters -= oneletter;

                  minato_isop isop(delta & oneletter);
                  bdd cube;

                  while ((cube = isop.next()) != bddfalse)
                    {
                      bdd cond = bdd_exist(cube, all_states_);
                      bdd dest = bdd_existcomp(cube, all_states_);

                      states.clear();
                      while (dest != bddtrue)
                        {
                          assert(bdd_low(dest) == bddfalse);
                          bdd v = bdd_ithvar(bdd_var(dest));
                          auto it = var_to_state_.find(v);
                          assert(it != var_to_state_.end());
                          states.push_back(it->second);
                          dest = bdd_high(dest);
                        }
                      res_->new_univ_edge(new_state(st.id, st.rank, st.mark),
                                          states.begin(), states.end(),
                                          cond, mark);
                    }
                }
              todo_.pop();
            }
          res_->merge_edges();
          return res_;
        }
    };
  }
  twa_graph_ptr to_weak_alternating(const_twa_graph_ptr& aut, bool less)
  {
    if (is_weak_automaton(aut))
      return make_twa_graph(aut, twa::prop_set::all());
    /* The current implementation of is_inherently_weak does not support
       alternating automata. In case the input automaton is inherently weak,
       it can be easily transformed to weak without the need to call to_weak
    */
    if (aut->acc().is_generalized_buchi())
      return dualize(to_weak(dualize(aut), less).run());
    else if (aut->acc().is_generalized_co_buchi())
      return to_weak(aut, less).run();

    throw std::runtime_error("to_weak_alternating does only support gen. Büchi"
                             " and gen. co-Büchi automata.");
  }
}
