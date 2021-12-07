// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013, 2014, 2015, 2016, 2018 Laboratoire de recherche
// et développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005, 2006 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
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

#pragma once

//#define NDFSR_TRACE

#include <iostream>
#ifdef NDFSR_TRACE
#define ndfsr_trace std::cerr
#else
#define ndfsr_trace while (0) std::cerr
#endif

#include <cassert>
#include <list>
#include <spot/misc/hash.hh>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <spot/twaalgos/bfssteps.hh>

namespace spot
{
  struct stack_item
  {
    stack_item(const state* n, twa_succ_iterator* i, bdd l, acc_cond::mark_t a)
      noexcept : s(n), it(i), label(l), acc(a) {};
    /// The visited state.
    const state* s;
    /// Design the next successor of \a s which has to be visited.
    twa_succ_iterator* it;
    /// The label of the transition traversed to reach \a s
    /// (false for the first one).
    bdd label;
    /// The acceptance set of the transition traversed to reach \a s
    /// (false for the first one).
    acc_cond::mark_t acc;
  };

  typedef std::list<stack_item> stack_type;

  namespace
  {
    // The acss_statistics is available only when the heap has a
    // size() method (which we indicate using n==1).

    template <typename T, int n>
    struct stats_interface
      : public ars_statistics
    {
    };

    template <typename T>
    struct stats_interface<T, 1>
      : public acss_statistics
    {
      virtual unsigned
      acss_states() const override
      {
        // all visited states are in the state space search
        return static_cast<const T*>(this)->h_.size();
      }
    };

  }


  template <typename ndfs_search, typename heap>
  class ndfs_result final:
    public emptiness_check_result,
    // Conditionally inherit from acss_statistics or ars_statistics.
    public stats_interface<ndfs_result<ndfs_search, heap>, heap::Has_Size>
  {
  public:
    ndfs_result(const std::shared_ptr<ndfs_search>& ms)
      : emptiness_check_result(ms->automaton()), ms_(ms),
        h_(ms->get_heap())
    {
    }

    virtual ~ndfs_result()
    {
    }

    virtual twa_run_ptr accepting_run() override
    {
      const stack_type& stb = ms_->get_st_blue();
      const stack_type& str = ms_->get_st_red();

      SPOT_ASSERT(!stb.empty());

      acc_cond::mark_t covered_acc = {};
      accepting_transitions_list acc_trans;

      const state* start;

      start = stb.front().s->clone();
      if (!str.empty())
        {
          if (a_->num_sets() == 0)
            {
              // take arbitrarily the last transition on the red stack
              stack_type::const_iterator i, j;
              i = j = str.begin(); ++i;
              if (i == str.end())
                i = stb.begin();
              transition t = { i->s->clone(), j->label, j->acc,
                               j->s->clone() };
              SPOT_ASSERT(h_.has_been_visited(t.source));
              SPOT_ASSERT(h_.has_been_visited(t.dest));
              acc_trans.push_back(t);
            }
          else
            {
              // ignore the prefix
              stack_type::const_reverse_iterator i, j;

              i = j = stb.rbegin(); ++j;
              while (i->s->compare(start) != 0)
                ++i, ++j;

              stack_type::const_reverse_iterator end = stb.rend();
              for (; j != end; ++i, ++j)
                {
                  if ((covered_acc & j->acc) != j->acc)
                    {
                      transition t = { i->s->clone(), j->label, j->acc,
                                       j->s->clone() };
                      SPOT_ASSERT(h_.has_been_visited(t.source));
                      SPOT_ASSERT(h_.has_been_visited(t.dest));
                      acc_trans.push_back(t);
                      covered_acc |= j->acc;
                    }
                }

              j = str.rbegin();
              if ((covered_acc & j->acc) != j->acc)
                {
                  transition t = { i->s->clone(), j->label, j->acc,
                                   j->s->clone() };
                  SPOT_ASSERT(h_.has_been_visited(t.source));
                  SPOT_ASSERT(h_.has_been_visited(t.dest));
                  acc_trans.push_back(t);
                  covered_acc |= j->acc;
                }

              i = j; ++j;
              end = str.rend();
              for (; j != end; ++i, ++j)
                {
                  if ((covered_acc & j->acc) != j->acc)
                    {
                      transition t = { i->s->clone(), j->label, j->acc,
                                       j->s->clone() };
                      SPOT_ASSERT(h_.has_been_visited(t.source));
                      SPOT_ASSERT(h_.has_been_visited(t.dest));
                      acc_trans.push_back(t);
                      covered_acc |= j->acc;
                    }
                }
            }
        }

      if (!a_->acc().accepting(covered_acc))
        {
          bool b = dfs(start, acc_trans, covered_acc);
          SPOT_ASSERT(b);
          (void) b;
        }

      start->destroy();

      SPOT_ASSERT(!acc_trans.empty());

      auto run = std::make_shared<twa_run>(automaton());
      // construct run->cycle from acc_trans.
      construct_cycle(run, acc_trans);
      // construct run->prefix (a minimal path from the initial state to any
      // state of run->cycle) and adjust the cycle to the state reached by the
      // prefix.
      construct_prefix(run);

      for (typename accepting_transitions_list::const_iterator i =
             acc_trans.begin(); i != acc_trans.end(); ++i)
        {
          i->source->destroy();
          i->dest->destroy();
        }

      return run;
    }

  private:
    std::shared_ptr<ndfs_search> ms_;
    const heap& h_;
    template <typename T, int n>
    friend struct stats_interface;

    struct transition {
      const state* source;
      bdd label;
      acc_cond::mark_t acc;
      const state* dest;
    };
    typedef std::list<transition> accepting_transitions_list;

    typedef std::unordered_set<const state*,
                               state_ptr_hash, state_ptr_equal> state_set;

    void clean(const const_twa_ptr& a, stack_type& st1,
               state_set& seen, state_set& dead)
    {
      while (!st1.empty())
        {
          a->release_iter(st1.front().it);
          st1.pop_front();
        }
      for (state_set::iterator i = seen.begin(); i != seen.end();)
        {
          const state* s = *i;
          ++i;
          s->destroy();
        }
      for (state_set::iterator i = dead.begin(); i != dead.end();)
        {
          const state* s = *i;
          ++i;
          s->destroy();
        }
    }

    bool dfs(const state* target, accepting_transitions_list& acc_trans,
             acc_cond::mark_t& covered_acc)
    {
      SPOT_ASSERT(h_.has_been_visited(target));
      stack_type st1;

      state_set seen, dead;
      const state* start = target->clone();

      seen.insert(start);
      twa_succ_iterator* i = a_->succ_iter(start);
      i->first();
      st1.emplace_front(start, i, bddfalse, acc_cond::mark_t({}));

      while (!st1.empty())
        {
          stack_item& f = st1.front();
          ndfsr_trace << "DFS1 treats: " << a_->format_state(f.s)
                      << std::endl;
          if (!f.it->done())
            {
              const state *s_prime = f.it->dst();
              ndfsr_trace << "  Visit the successor: "
                          << a_->format_state(s_prime) << std::endl;
              bdd label = f.it->cond();
              auto acc = f.it->acc();
              f.it->next();
              if (h_.has_been_visited(s_prime))
                {
                  if (dead.find(s_prime) != dead.end())
                    {
                      ndfsr_trace << "  it is dead, pop it" << std::endl;
                      s_prime->destroy();
                    }
                  else if (seen.find(s_prime) == seen.end())
                    {
                      this->inc_ars_cycle_states();
                      ndfsr_trace << "  it is not seen, go down" << std::endl;
                      seen.insert(s_prime);
                      twa_succ_iterator* i = a_->succ_iter(s_prime);
                      i->first();
                      st1.emplace_front(s_prime, i, label, acc);
                    }
                  else if ((acc & covered_acc) != acc)
                    {
                      this->inc_ars_cycle_states();
                      ndfsr_trace << "  a propagation is needed, "
                                  << "start a search" << std::endl;
                      if (search(s_prime, target, dead))
                        {
                          transition t = { f.s->clone(), label, acc,
                                           s_prime->clone() };
                          SPOT_ASSERT(h_.has_been_visited(t.source));
                          SPOT_ASSERT(h_.has_been_visited(t.dest));
                          acc_trans.push_back(t);
                          covered_acc |= acc;
                          if (a_->acc().accepting(covered_acc))
                            {
                              clean(a_, st1, seen, dead);
                              s_prime->destroy();
                              return true;
                            }
                        }
                      s_prime->destroy();
                    }
                  else
                    {
                      ndfsr_trace << "  already seen, pop it" << std::endl;
                      s_prime->destroy();
                    }
                }
              else
                {
                  ndfsr_trace << "  not seen during the search, pop it"
                              << std::endl;
                  s_prime->destroy();
                }
            }
          else
            {
              ndfsr_trace << "  all the successors have been visited"
                          << std::endl;
              stack_item f_dest(f);
              a_->release_iter(st1.front().it);
              st1.pop_front();
              if (!st1.empty() && ((f_dest.acc & covered_acc) != f_dest.acc))
                {
                  ndfsr_trace << "  a propagation is needed, start a search"
                              << std::endl;
                  if (search(f_dest.s, target, dead))
                    {
                      transition t = { st1.front().s->clone(),
                                       f_dest.label, f_dest.acc,
                                       f_dest.s->clone() };
                      SPOT_ASSERT(h_.has_been_visited(t.source));
                      SPOT_ASSERT(h_.has_been_visited(t.dest));
                      acc_trans.push_back(t);
                      covered_acc |= f_dest.acc;
                      if (a_->acc().accepting(covered_acc))
                        {
                          clean(a_, st1, seen, dead);
                          return true;
                        }
                    }
                }
              else
                {
                  ndfsr_trace << "  no propagation needed, pop it"
                              << std::endl;
                }
            }
        }

      clean(a_, st1, seen, dead);
      return false;
    }

    class test_path: public bfs_steps
    {
    public:
      test_path(ars_statistics* ars,
                const const_twa_ptr& a, const state* t,
                const state_set& d, const heap& h)
        : bfs_steps(a), ars(ars), target(t), dead(d), h(h)
      {
      }

      ~test_path()
      {
        state_set::const_iterator i = seen.begin();
        while (i != seen.end())
          {
            const state* ptr = *i;
            ++i;
            ptr->destroy();
          }
      }

      const state* search(const state* start, twa_run::steps& l)
      {
        const state* s = filter(start);
        if (s)
          return this->bfs_steps::search(s, l);
        else
          return nullptr;
      }

      const state* filter(const state* s) override
      {
        if (!h.has_been_visited(s)
            || seen.find(s) != seen.end()
            || dead.find(s) != dead.end())
          {
            s->destroy();
            return nullptr;
          }
        ars->inc_ars_cycle_states();
        seen.insert(s);
        return s;
      }

      void
      finalize(const std::map<const state*,
                              twa_run::step, state_ptr_less_than>&,
               const twa_run::step&, const state*, twa_run::steps&) override
      {
      }

      const state_set& get_seen() const
      {
        return seen;
      }

      bool match(twa_run::step&, const state* dest) override
      {
        return target->compare(dest) == 0;
      }

    private:
      ars_statistics* ars;
      state_set seen;
      const state* target;
      const state_set& dead;
      const heap& h;
    };

    bool search(const state* start, const state* target, state_set& dead)
    {
      twa_run::steps path;
      if (start->compare(target) == 0)
        return true;

      test_path s(this, a_, target, dead, h_);
      const state* res = s.search(start->clone(), path);
      if (res)
        {
          SPOT_ASSERT(res->compare(target) == 0);
          return true;
        }
      else
        {
          state_set::const_iterator it;
          for (it = s.get_seen().begin(); it != s.get_seen().end(); ++it)
            dead.insert((*it)->clone());
          return false;
        }
    }

    typedef std::unordered_multimap<const state*, transition,
                                    state_ptr_hash,
                                    state_ptr_equal> m_source_trans;

    template<bool cycle>
    class min_path: public bfs_steps
    {
    public:
      min_path(ars_statistics* ars,
               const const_twa_ptr& a,
               const m_source_trans& target, const heap& h)
        : bfs_steps(a), ars(ars), target(target), h(h)
      {
      }

      ~min_path()
      {
        state_set::const_iterator i = seen.begin();
        while (i != seen.end())
          {
            const state* ptr = *i;
            ++i;
            ptr->destroy();
          }
      }

      const state* search(const state* start, twa_run::steps& l)
      {
        const state* s = filter(start);
        if (s)
          return this->bfs_steps::search(s, l);
        else
          return nullptr;
      }

      const state* filter(const state* s) override
      {
        ndfsr_trace << "filter: " << a_->format_state(s);
        if (!h.has_been_visited(s) || seen.find(s) != seen.end())
          {
            if (!h.has_been_visited(s))
              ndfsr_trace << " not visited" << std::endl;
            else
              ndfsr_trace << " already seen" << std::endl;
            s->destroy();
            return nullptr;
          }
        ndfsr_trace << " OK" << std::endl;
        if (cycle)
          ars->inc_ars_cycle_states();
        else
          ars->inc_ars_prefix_states();
        seen.insert(s);
        return s;
      }

      bool match(twa_run::step&, const state* dest) override
      {
        ndfsr_trace << "match: " << a_->format_state(dest)
                    << std::endl;
        return target.find(dest) != target.end();
      }

    private:
      ars_statistics* ars;
      state_set seen;
      const m_source_trans& target;
      const heap& h;
    };

    void construct_cycle(twa_run_ptr run,
                         const accepting_transitions_list& acc_trans)
    {
      SPOT_ASSERT(!acc_trans.empty());
      transition current = acc_trans.front();
      // insert the first accepting transition in the cycle
      ndfsr_trace << "the initial accepting transition is from "
                  << a_->format_state(current.source) << " to "
                  << a_->format_state(current.dest) << std::endl;
      const state* begin = current.source;

      m_source_trans target;
      typename accepting_transitions_list::const_iterator i =
        acc_trans.begin();
      ndfsr_trace << "targets are the source states: ";
      for (++i; i != acc_trans.end(); ++i)
        {
          if (i->source->compare(begin) == 0 &&
              i->source->compare(i->dest) == 0)
            {
              ndfsr_trace << "(self loop " << a_->format_state(i->source)
                          << " -> " << a_->format_state(i->dest)
                          << " ignored) ";
              twa_run::step st = { i->source->clone(), i->label, i->acc };
              run->cycle.push_back(st);
            }
          else
            {
              ndfsr_trace << a_->format_state(i->source) << " (-> "
                          << a_->format_state(i->dest) << ") ";
              target.emplace(i->source, *i);
            }
        }
      ndfsr_trace << std::endl;

      twa_run::step st = { current.source->clone(), current.label,
                            current.acc };
      run->cycle.push_back(st);

      while (!target.empty())
        {
          // find a minimal path from current.dest to any source state in
          // target.
          ndfsr_trace << "looking for a path from "
                      << a_->format_state(current.dest) << std::endl;
          typename m_source_trans::iterator i = target.find(current.dest);
          if (i == target.end())
            {
              min_path<true> s(this, a_, target, h_);
              const state* res = s.search(current.dest->clone(), run->cycle);
              // init current to the corresponding transition.
              SPOT_ASSERT(res);
              ndfsr_trace << a_->format_state(res) << " reached" << std::endl;
              i = target.find(res);
              SPOT_ASSERT(i != target.end());
            }
          else
            {
              ndfsr_trace << "this is a target" << std::endl;
            }
          current = i->second;
          // complete the path with the corresponding transition
          twa_run::step st = { current.source->clone(), current.label,
                                current.acc };
          run->cycle.push_back(st);
          // remove this source state of target
          target.erase(i);
        }

      if (current.dest->compare(begin) != 0)
        {
          // close the cycle by adding a path from the destination of the
          // last inserted transition to the source of the first one
          ndfsr_trace << std::endl << "looking for a path from "
                      << a_->format_state(current.dest) << " to "
                      << a_->format_state(begin) << std::endl;
          transition tmp;
          // Initialize to please GCC 4.0.1 (Darwin).
          tmp.source = tmp.dest = nullptr;
          tmp.acc = {};
          target.emplace(begin, tmp);
          min_path<true> s(this, a_, target, h_);
          const state* res = s.search(current.dest->clone(), run->cycle);
          SPOT_ASSERT(res);
          SPOT_ASSERT(res->compare(begin) == 0);
          (void)res;
        }
    }

    void construct_prefix(twa_run_ptr run)
    {
      m_source_trans target;
      transition tmp;
      tmp.source = tmp.dest = nullptr; // Initialize to please GCC 4.0.
      tmp.acc = {};

      // Register all states from the cycle as target of the BFS.
      for (twa_run::steps::const_iterator i = run->cycle.begin();
           i != run->cycle.end(); ++i)
        target.emplace(i->s, tmp);

      const state* prefix_start = a_->get_init_state();
      // There are two cases: either the initial state is already on
      // the cycle, or it is not.  If it is, we will have to rotate
      // the cycle so it begins on this position.  Otherwise we will shift
      // the cycle so it begins on the state that follows the prefix.
      // cycle_entry_point is that state.
      const state* cycle_entry_point;
      typename m_source_trans::const_iterator ps = target.find(prefix_start);
      if (ps != target.end())
        {
          // The initial state is on the cycle.
          prefix_start->destroy();
          cycle_entry_point = ps->first->clone();
        }
      else
        {
          // This initial state is outside the cycle.  Compute the prefix.
          min_path<false> s(this, a_, target, h_);
          cycle_entry_point = s.search(prefix_start, run->prefix);
          SPOT_ASSUME(cycle_entry_point);
          cycle_entry_point = cycle_entry_point->clone();
        }

      // Locate cycle_entry_point on the cycle.
      twa_run::steps::iterator cycle_ep_it;
      for (cycle_ep_it = run->cycle.begin();
           cycle_ep_it != run->cycle.end()
             && cycle_entry_point->compare(cycle_ep_it->s); ++cycle_ep_it)
        continue;
      SPOT_ASSERT(cycle_ep_it != run->cycle.end());
      cycle_entry_point->destroy();

      // Now shift the cycle so it starts on cycle_entry_point.
      run->cycle.splice(run->cycle.end(), run->cycle,
                        run->cycle.begin(), cycle_ep_it);
    }
  };

}

#undef ndfsr_trace
