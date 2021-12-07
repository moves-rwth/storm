// -*- coding: utf-8 -*-
// Copyright (C) 2010-2016, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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


//#define TRACE

#include "config.h"
#ifdef TRACE
#  define trace std::cerr
#else
#  define trace while (0) std::cerr
#endif

#include <set>
#include <list>
#include <sstream>
#include <spot/taalgos/minimize.hh>
#include <spot/misc/hash.hh>
#include <spot/misc/bddlt.hh>
#include <spot/ta/tgtaexplicit.hh>
#include <spot/taalgos/statessetbuilder.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twa/bddprint.hh>

namespace spot
{
  typedef state_set hash_set;
  typedef state_map<unsigned> hash_map;
  typedef std::list<hash_set*> partition_t;

  namespace
  {
    static std::ostream&
    dump_hash_set(const hash_set* hs,
                  const const_ta_ptr& aut,
                  std::ostream& out)
    {
      out << '{';
      const char* sep = "";
      for (hash_set::const_iterator i = hs->begin(); i != hs->end(); ++i)
        {
          out << sep << aut->format_state(*i);
          sep = ", ";
        }
      out << '}';
      return out;
    }

     static std::string
     format_hash_set(const hash_set* hs, const const_ta_ptr& aut)
     {
       std::ostringstream s;
       dump_hash_set(hs, aut, s);
       return s.str();
     }

     // From the base automaton and the list of sets, build the minimal
     // automaton
     static void
     build_result(const const_ta_ptr& a, std::list<hash_set*>& sets,
                  twa_graph_ptr result_tgba, const ta_explicit_ptr& result)
     {
       // For each set, create a state in the tgbaulting automaton.
       // For a state s, state_num[s] is the number of the state in the minimal
       // automaton.
       hash_map state_num;
       std::list<hash_set*>::iterator sit;
       unsigned num = 0;
       for (sit = sets.begin(); sit != sets.end(); ++sit)
         {
           hash_set::iterator hit;
           hash_set* h = *sit;
           for (hit = h->begin(); hit != h->end(); ++hit)
             state_num[*hit] = num;
           result_tgba->new_state();
           ++num;
         }

       // For each transition in the initial automaton, add the corresponding
       // transition in ta.

       for (sit = sets.begin(); sit != sets.end(); ++sit)
         {
           hash_set::iterator hit;
           hash_set* h = *sit;
           hit = h->begin();
           const state* src = *hit;
           unsigned src_num = state_num[src];

           bdd tgba_condition = bddtrue;
           bool is_initial_state = a->is_initial_state(src);
           if (!a->get_artificial_initial_state() && is_initial_state)
             tgba_condition = a->get_state_condition(src);
           bool is_accepting_state = a->is_accepting_state(src);
           bool is_livelock_accepting_state =
             a->is_livelock_accepting_state(src);

           state_ta_explicit* new_src =
             new state_ta_explicit(result_tgba->state_from_number(src_num),
                                   tgba_condition, is_initial_state,
                                   is_accepting_state,
                                   is_livelock_accepting_state);

           state_ta_explicit* ta_src = result->add_state(new_src);

           if (ta_src != new_src)
             {
               delete new_src;
             }
           else if (a->get_artificial_initial_state())
             {
               if (a->get_artificial_initial_state() == src)
                 result->set_artificial_initial_state(new_src);
             }
           else if (is_initial_state)
             {
               result->add_to_initial_states_set(new_src);
             }

           ta_succ_iterator* succit = a->succ_iter(src);

           for (succit->first(); !succit->done(); succit->next())
             {
               const state* dst = succit->dst();
               hash_map::const_iterator i = state_num.find(dst);

               if (i == state_num.end()) // Ignore useless destinations.
                 continue;

               bdd tgba_condition = bddtrue;
               is_initial_state = a->is_initial_state(dst);
               if (!a->get_artificial_initial_state() && is_initial_state)
                 tgba_condition = a->get_state_condition(dst);
               bool is_accepting_state = a->is_accepting_state(dst);
               bool is_livelock_accepting_state =
                 a->is_livelock_accepting_state(dst);

               state_ta_explicit* new_dst =
                 new state_ta_explicit
                 (result_tgba->state_from_number(i->second),
                  tgba_condition, is_initial_state,
                  is_accepting_state,
                  is_livelock_accepting_state);

               state_ta_explicit* ta_dst = result->add_state(new_dst);

               if (ta_dst != new_dst)
                 {
                   delete new_dst;
                 }
               else if (a->get_artificial_initial_state())
                 {
                   if (a->get_artificial_initial_state() == dst)
                     result->set_artificial_initial_state(new_dst);
                 }

               else if (is_initial_state)
                 result->add_to_initial_states_set(new_dst);

               result->create_transition
                 (ta_src, succit->cond(),
                  succit->acc(),
                  ta_dst);
             }
           delete succit;
         }
     }

    static partition_t
    build_partition(const const_ta_ptr& ta_)
    {
      unsigned num_sets = ta_->acc().num_sets();
      std::map<acc_cond::mark_t, bdd> m2b;
      int acc_vars = ta_->get_dict()->register_anonymous_variables(num_sets,
                                                                   &m2b);
      auto mark_to_bdd = [&](acc_cond::mark_t m) -> bdd
        {
          auto i = m2b.find(m);
          if (i != m2b.end())
            return i->second;

          bdd res = bddtrue;
          for (unsigned n = 0; n < num_sets; ++n)
            if (m.has(n))
              res &= bdd_ithvar(acc_vars + n);
            else
              res &= bdd_nithvar(acc_vars + n);
          m2b.emplace_hint(i, m, res);
          return res;
        };

      partition_t cur_run;
      partition_t next_run;

      // The list of equivalent states.
      partition_t done;

      std::set<const state*> states_set = get_states_set(ta_);

      hash_set* I = new hash_set;

      // livelock acceptance states
      hash_set* G = new hash_set;

      // Buchi acceptance states
      hash_set* F = new hash_set;

      // Buchi and livelock acceptance states
      hash_set* G_F = new hash_set;

      // the other states (non initial and not in G, F and G_F)
      hash_set* S = new hash_set;

      std::set<const state*>::iterator it;

      auto artificial_initial_state = ta_->get_artificial_initial_state();

      for (it = states_set.begin(); it != states_set.end(); ++it)
        {
          const state* s = *it;
          if (s == artificial_initial_state)
            I->insert(s);
          else if (!artificial_initial_state && ta_->is_initial_state(s))
            I->insert(s);
          else if (ta_->is_livelock_accepting_state(s)
                   && ta_->is_accepting_state(s))
            G_F->insert(s);
          else if (ta_->is_accepting_state(s))
            F->insert(s);
          else if (ta_->is_livelock_accepting_state(s))
            G->insert(s);
          else
            S->insert(s);
        }

      hash_map state_set_map;

      // Size of ta_
      unsigned size = states_set.size() + 6;
      // Use bdd variables to number sets.  set_num is the first variable
      // available.
      unsigned set_num =
        ta_->get_dict()->register_anonymous_variables(size, &m2b);

      std::set<int> free_var;
      for (unsigned i = set_num; i < set_num + size; ++i)
        free_var.insert(i);
      std::map<int, int> used_var;

      for (hash_set::const_iterator i = I->begin(); i != I->end(); ++i)
        {
          hash_set* cI = new hash_set;
          cI->insert(*i);
          done.emplace_back(cI);

          used_var[set_num] = 1;
          free_var.erase(set_num);
          state_set_map[*i] = set_num;
          ++set_num;

        }
      delete I;

      if (!G->empty())
        {
          unsigned s = G->size();
          unsigned num = set_num;
          ++set_num;
          used_var[num] = s;
          free_var.erase(num);
          if (s > 1)
            cur_run.emplace_back(G);
          else
            done.emplace_back(G);
          for (hash_set::const_iterator i = G->begin(); i != G->end(); ++i)
            state_set_map[*i] = num;

        }
      else
        {
          delete G;
        }

      if (!F->empty())
        {
          unsigned s = F->size();
          unsigned num = set_num;
          ++set_num;
          used_var[num] = s;
          free_var.erase(num);
          if (s > 1)
            cur_run.emplace_back(F);
          else
            done.emplace_back(F);
          for (hash_set::const_iterator i = F->begin(); i != F->end(); ++i)
            state_set_map[*i] = num;
        }
      else
        {
          delete F;
        }

      if (!G_F->empty())
        {
          unsigned s = G_F->size();
          unsigned num = set_num;
          ++set_num;
          used_var[num] = s;
          free_var.erase(num);
          if (s > 1)
            cur_run.emplace_back(G_F);
          else
            done.emplace_back(G_F);
          for (hash_set::const_iterator i = G_F->begin(); i != G_F->end(); ++i)
            state_set_map[*i] = num;
        }
      else
        {
          delete G_F;
        }

      if (!S->empty())
        {
          unsigned s = S->size();
          unsigned num = set_num;
          ++set_num;
          used_var[num] = s;
          free_var.erase(num);
          if (s > 1)
            cur_run.emplace_back(S);
          else
            done.emplace_back(S);
          for (hash_set::const_iterator i = S->begin(); i != S->end(); ++i)
            state_set_map[*i] = num;
        }
      else
        {
          delete S;
        }


      // A bdd_states_map is a list of formulae (in a BDD form)
      // associated with a destination set of states.
      typedef std::map<bdd, hash_set*, bdd_less_than> bdd_states_map;

      bool did_split = true;
      unsigned num = set_num;
      ++set_num;
      used_var[num] = 1;
      free_var.erase(num);
      bdd bdd_false_acceptance_condition = bdd_ithvar(num);

      while (did_split)
        {
          did_split = false;
          while (!cur_run.empty())
            {
              // Get a set to process.
              hash_set* cur = cur_run.front();
              cur_run.pop_front();

              trace
                << "processing " << format_hash_set(cur, ta_) << std::endl;

              hash_set::iterator hi;
              bdd_states_map bdd_map;
              for (hi = cur->begin(); hi != cur->end(); ++hi)
                {
                  const state* src = *hi;
                  bdd f = bddfalse;
                  ta_succ_iterator* si = ta_->succ_iter(src);
                  trace << "+src: " << src << std::endl;
                  for (si->first(); !si->done(); si->next())
                    {
                      const state* dst = si->dst();
                      hash_map::const_iterator i = state_set_map.find(dst);

                      assert(i != state_set_map.end());
                      auto curacc =
                        mark_to_bdd(si->acc());
                      f |= (bdd_ithvar(i->second)
                            & si->cond() & curacc);
                      trace
                        << "+f: " << bdd_format_accset(ta_->get_dict(), f)
                        << "\n      -bdd_ithvar(i->second): "
                        << bdd_format_accset(ta_->get_dict(),
                                             bdd_ithvar(i->second))
                        << "\n      -si->cond(): "
                        << bdd_format_accset(ta_->get_dict(),
                                             si->cond())
                        << "\n      -current_acceptance_conditions: "
                        << si->acc()
                      << std::endl;
                    }
                  delete si;

                  // Have we already seen this formula ?
                  bdd_states_map::iterator bsi = bdd_map.find(f);
                  if (bsi == bdd_map.end())
                    {
                      // No, create a new set.
                      hash_set* new_set = new hash_set;
                      new_set->insert(src);
                      bdd_map[f] = new_set;
                    }
                  else
                    {
                      // Yes, add the current state to the set.
                      bsi->second->insert(src);
                    }
                }

              bdd_states_map::iterator bsi = bdd_map.begin();
              if (bdd_map.size() == 1)
                {
                  // The set was not split.
                  trace
                    << "set " << format_hash_set(bsi->second, ta_)
                    << " was not split" << std::endl;
                  next_run.emplace_back(bsi->second);
                }
              else
                {
                  did_split = true;
                  for (; bsi != bdd_map.end(); ++bsi)
                    {
                      hash_set* set = bsi->second;
                      // Free the number associated to these states.
                      unsigned num = state_set_map[*set->begin()];
                      assert(used_var.find(num) != used_var.end());
                      unsigned left = (used_var[num] -= set->size());
                      // Make sure LEFT does not become negative
                      // (hence bigger than SIZE when read as unsigned)
                      assert(left < size);
                      if (left == 0)
                        {
                          used_var.erase(num);
                          free_var.insert(num);
                        }
                      // Pick a free number
                      assert(!free_var.empty());
                      num = *free_var.begin();
                      free_var.erase(free_var.begin());
                      used_var[num] = set->size();
                      for (hash_set::iterator hit = set->begin();
                           hit != set->end(); ++hit)
                        state_set_map[*hit] = num;
                      // Trivial sets can't be splitted any further.
                      if (set->size() == 1)
                        {
                          trace
                            << "set " << format_hash_set(set, ta_)
                            << " is minimal" << std::endl;
                          done.emplace_back(set);
                        }
                      else
                        {
                          trace
                            << "set " << format_hash_set(set, ta_)
                            << " should be processed further" << std::endl;
                          next_run.emplace_back(set);
                        }
                    }
                }
              delete cur;
            }
          if (did_split)
            trace
              << "splitting did occur during this pass." << std::endl;

          std::swap(cur_run, next_run);
        }

      done.splice(done.end(), cur_run);

#ifdef TRACE
      trace << "Final partition: ";
      for (partition_t::const_iterator i = done.begin(); i != done.end(); ++i)
        trace << format_hash_set(*i, ta_) << ' ';
      trace << std::endl;
#endif

      ta_->get_dict()->unregister_all_my_variables(&m2b);
      return done;
    }
  }

  ta_explicit_ptr
  minimize_ta(const const_ta_ptr& ta_)
  {

    auto tgba = make_twa_graph(ta_->get_dict());
    auto res = make_ta_explicit(tgba, ta_->acc().num_sets(), nullptr);

    partition_t partition = build_partition(ta_);

    // Build the ta automata result.
    build_result(ta_, partition, tgba, res);

    // Free all the allocated memory.
    std::list<hash_set*>::iterator itdone;
    for (itdone = partition.begin(); itdone != partition.end(); ++itdone)
      delete *itdone;

    return res;
  }

  tgta_explicit_ptr
  minimize_tgta(const const_tgta_explicit_ptr& tgta_)
  {

    auto tgba = make_twa_graph(tgta_->get_dict());
    auto res = make_tgta_explicit(tgba, tgta_->acc().num_sets(), nullptr);

    auto ta = tgta_->get_ta();

    partition_t partition = build_partition(ta);

    // Build the minimal tgta automaton.
    build_result(ta, partition, tgba, res->get_ta());

    // Free all the allocated memory.
    std::list<hash_set*>::iterator itdone;
    for (itdone = partition.begin(); itdone != partition.end(); ++itdone)
      delete *itdone;

    return res;
  }

}
