// -*- coding: utf-8 -*-
// Copyright (C) 2016-2020 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/couvreurnew.hh>

#include <spot/twa/twa.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/bfssteps.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>

#include <stack>

namespace spot
{
  namespace
  {
    using explicit_iterator = twa_graph::graph_t::const_iterator;

    // A proxy class that allows to manipulate an iterator from the
    // explicit interface as an iterator from the abstract interface.
    class explicitproxy
    {
    public:
      explicitproxy(explicit_iterator it)
        : it_(it)
      {
      }

      const explicitproxy*
      operator->() const
      {
        return this;
      }

      explicitproxy*
      operator->()
      {
        return this;
      }

      bool
      done() const
      {
        return !it_;
      }

      unsigned
      dst() const
      {
        return it_->dst;
      }

      acc_cond::mark_t
      acc() const
      {
        return it_->acc;
      }

      bdd
      cond() const
      {
        return it_->cond;
      }

      void
      next()
      {
        ++it_;
      }

    private:
      explicit_iterator it_;
    };

    template<bool is_explicit>
    class twa_iteration
    {
    };

    template<>
    class twa_iteration<false>
    {
    public:
      using state_t = const state*;
      using iterator_t = twa_succ_iterator*;
      template<class val>
      using state_map = spot::state_map<val>;

      template<class val>
      static
      std::pair<std::pair<state_t, val>, bool>
      h_emplace(state_map<val>& h, state_t s, val i)
      {
        auto p = h.emplace(s, i);
        return std::make_pair(*p.first, p.second);
      }

      static
      std::pair<state_t, int>
      h_find(const state_map<int>& h, state_t s)
      {
        auto p = h.find(s);
        if (p == h.end())
          return std::make_pair(nullptr, 0);
        else
          return std::make_pair(p->first, p->second);
      }

      static
      unsigned
      h_count(const state_map<int>& h, const std::function<bool(int)>& p)
      {
        unsigned count = 0;
        for (auto i : h)
          if (p(i.second))
            ++count;
        return count;
      }

      static
      state_t
      initial_state(const const_twa_ptr& twa_p)
      {
        return twa_p->get_init_state();
      }

      static
      iterator_t
      succ(const const_twa_ptr& twa_p, state_t s)
      {
        auto res = twa_p->succ_iter(s);
        res->first();
        return res;
      }

      static
      void
      destroy(state_t s)
      {
        s->destroy();
      }

      static
      const state*
      to_state(const const_twa_ptr&, state_t s)
      {
        return s;
      }

      static
      state_t
      from_state(const const_twa_ptr&, const state* s)
      {
        return s;
      }

      static
      void
      it_destroy(const const_twa_ptr& twa_p, iterator_t it)
      {
        twa_p->release_iter(it);
      }
    };

    template<>
    class twa_iteration<true>
    {
    public:
      using state_t = unsigned;
      using iterator_t = explicitproxy;
      template<class val>
      using state_map = std::vector<val>;

      template<class val>
      static
      std::pair<std::pair<state_t, val>, bool>
      h_emplace(state_map<val>& h, state_t s, val i)
      {
        if (h[s] == val())
        {
          h[s] = i;
          return std::make_pair(std::make_pair(s, h[s]), true);
        }
        else
        {
          return std::make_pair(std::make_pair(s, h[s]), false);
        }
      }

      static
      std::pair<state_t, int>
      h_find(const state_map<int>& h, state_t s)
      {
        assert(s < h.size());
        return std::make_pair(s, h[s]);
      }

      static
      unsigned
      h_count(const state_map<int>& h, const std::function<bool(int)>& p)
      {
        unsigned count = 0;
        for (auto i : h)
          if (p(i))
            ++count;
        return count;
      }

      static
      state_t
      initial_state(const const_twa_graph_ptr& twa_p)
      {
        if (!twa_p->is_existential())
          throw std::runtime_error
            ("couvreur99_new does not support alternation");
        return twa_p->get_init_state_number();
      }

      static
      iterator_t
      succ(const const_twa_graph_ptr& twa_p, state_t s)
      {
        return explicitproxy(twa_p->out(s).begin());
      }

      static
      const state*
      to_state(const const_twa_graph_ptr& twa_p, state_t s)
      {
        return twa_p->state_from_number(s);
      }

      static
      state_t
      from_state(const const_twa_graph_ptr& twa_p, const state* s)
      {
        return twa_p->state_number(s);
      }

      static
      void
      destroy(state_t)
      {
      }

      static
      void
      it_destroy(const const_twa_ptr&, iterator_t)
      {
      }
    };

    // A simple struct representing an SCC.
    struct scc
    {
      scc(int i): index(i), condition({}) {}

      int index;
      acc_cond::mark_t condition;
    };

    template<bool is_explicit>
    using automaton_ptr = typename std::conditional<is_explicit,
                                                    const_twa_graph_ptr,
                                                    const_twa_ptr>::type;

    // The status of the emptiness-check on success.
    // It contains everyting needed to build a counter-example:
    // the automaton, the stack of SCCs traversed by the counter-example,
    // and the heap of visited states with their indices.
    template<bool is_explicit>
    struct couvreur99_new_status
    {
      using T = twa_iteration<is_explicit>;

      couvreur99_new_status(const automaton_ptr<is_explicit>& a)
        : aut(a)
      {
        // Appropriate version is resolved through SFINAE.
        init<is_explicit>();
      }

      ~couvreur99_new_status()
      {
        uninit<is_explicit>();
      }

      automaton_ptr<is_explicit> aut;
      std::stack<scc> root;
      typename T::template state_map<int> h;
      typename T::state_t cycle_seed;

    private:
      template<bool U>
      typename std::enable_if<U>::type
      init()
      {
        // Initialize h with the right size.
        h.resize(aut->num_states(), 0);
      }

      template<bool U>
      typename std::enable_if<U>::type
      uninit()
      {
      }

      template<bool U>
      typename std::enable_if<!U>::type
      init()
      {
      }

      template<bool U>
      typename std::enable_if<!U>::type
      uninit()
      {
        auto i = h.begin();
        while (i != h.end())
          i++->first->destroy();
      }
    };

    template<bool is_explicit>
    using couvreur99_new_status_ptr =
      std::shared_ptr<couvreur99_new_status<is_explicit>>;

    template<bool is_explicit>
    class couvreur99_new_result final :
      public emptiness_check_result,
      public acss_statistics
    {
      using T = twa_iteration<is_explicit>;
    public:
      couvreur99_new_result(const couvreur99_new_status_ptr<is_explicit>& ecs)
        : emptiness_check_result(ecs->aut), ecs_(ecs)
      {}

      virtual
      unsigned
      acss_states() const override
      {
        int scc_root = ecs_->root.top().index;
        return T::h_count(ecs_->h,
            [scc_root](int s) noexcept { return s >= scc_root; });
      }

      twa_run_ptr
      accepting_run() override;
    private:
      couvreur99_new_status_ptr<is_explicit> ecs_;
      twa_run_ptr run_;

      void
      accepting_cycle()
      {
        acc_cond::mark_t acc_to_traverse =
          ecs_->aut->acc().accepting_sets(ecs_->root.top().condition);
        // Compute an accepting cycle using successive BFS that are
        // restarted from the point reached after we have discovered a
        // transition with a new acceptance condition.
        //
        // This idea is taken from Product<T>::findWitness in LBTT 1.1.2,
        // which in turn is probably inspired from
        // @Article{latvala.00.fi,
        //   author     = {Timo Latvala and Keijo Heljanko},
        //   title      = {Coping With Strong Fairness},
        //   journal    = {Fundamenta Informaticae},
        //   year       = {2000},
        //   volume     = {43},
        //   number     = {1--4},
        //   pages      = {1--19},
        //   publisher  = {IOS Press}
        // }
        const state* substart = T::to_state(ecs_->aut, ecs_->cycle_seed);
        const state* cycle_seed = T::to_state(ecs_->aut, ecs_->cycle_seed);
        do
          {
            struct scc_bfs final: bfs_steps
            {
              const couvreur99_new_status<is_explicit>* ecs;
              couvreur99_new_result<is_explicit>* r;
              acc_cond::mark_t& acc_to_traverse;
              int scc_root;

              scc_bfs(const couvreur99_new_status<is_explicit>* ecs,
                      couvreur99_new_result<is_explicit>* r,
                      acc_cond::mark_t& acc_to_traverse)
                : bfs_steps(ecs->aut), ecs(ecs), r(r)
                , acc_to_traverse(acc_to_traverse)
                , scc_root(ecs->root.top().index)
              {
              }

              virtual const state*
              filter(const state* s) override
              {
                auto i = T::h_find(ecs->h, T::from_state(ecs->aut, s));
                s->destroy();
                // Ignore unknown states.
                if (i.second == 0)
                  return nullptr;
                // Stay in the final SCC.
                if (i.second < scc_root)
                  return nullptr;
                r->inc_ars_cycle_states();
                return T::to_state(ecs->aut, i.first);
              }

              virtual bool
              match(twa_run::step& st, const state* s) override
              {
                acc_cond::mark_t less_acc =
                  acc_to_traverse - st.acc;
                if (less_acc != acc_to_traverse
                    || (!acc_to_traverse
                        && T::from_state(ecs->aut, s) == ecs->cycle_seed))
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
        while (acc_to_traverse || substart != cycle_seed);
      }
    };

    template<bool is_explicit>
    class shortest_path final : public bfs_steps
    {
      using T = twa_iteration<is_explicit>;
    public:
      shortest_path(const state_set* t,
                    const couvreur99_new_status_ptr<is_explicit>& ecs,
                    couvreur99_new_result<is_explicit>* r)
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
        auto i = T::h_find(ecs->h, T::from_state(ecs->aut, s));
        s->destroy();
        // Ignore unknown states ...
        if (i.second == 0)
          return nullptr;
        // ... as well as dead states
        if (i.second == -1)
          return nullptr;
        return T::to_state(ecs->aut, i.first);
      }

      bool
      match(twa_run::step&, const state* dest) override
      {
        return target->find(dest) != target->end();
      }
    private:
      state_set seen;
      const state_set* target;
      couvreur99_new_status_ptr<is_explicit> ecs;
      couvreur99_new_result<is_explicit>* r;
    };

    template<bool is_explicit>
    twa_run_ptr
    couvreur99_new_result<is_explicit>::accepting_run()
    {
      run_ = std::make_shared<twa_run>(ecs_->aut);

      assert(!ecs_->root.empty());

      // Compute an accepting cycle.
      accepting_cycle();

      // Compute the prefix: it is the shortest path from the initial
      // state of the automaton to any state of the cycle.

      // Register all states from the cycle as targets of the BFS.
      state_set ss;
      for (twa_run::steps::const_iterator i = run_->cycle.begin();
          i != run_->cycle.end(); ++i)
        ss.insert(i->s);
      shortest_path<is_explicit> shpath(&ss, ecs_, this);

      const state* prefix_start =
        T::to_state(ecs_->aut, T::initial_state(ecs_->aut));
      // There are two cases: either the initial state is already in
      // the cycle, or it is not.  If it is, we will have to rotate
      // the cycle so it begins at this position.  Otherwise we will shift
      // the cycle so it begins at the state that follows the prefix.
      // cycle_entry_point is that state.
      const state* cycle_entry_point;
      state_set::const_iterator ps = ss.find(prefix_start);
      if (ps != ss.end())
        {
          // The initial state is in the cycle.
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

      // Now shift the cycle so it starts on cycle_entry_point
      run_->cycle.splice(run_->cycle.end(), run_->cycle,
                         run_->cycle.begin(), cycle_ep_it);

      return run_;
    }

    // A simple enum for the different automata strengths.
    enum twa_strength { STRONG, WEAK, TERMINAL };

    template<bool is_explicit, twa_strength strength>
    class couvreur99_new : public emptiness_check, public ec_statistics
    {
      using T = twa_iteration<is_explicit>;
      using state_t = typename T::state_t;
      using iterator_t = typename T::iterator_t;
      using pair_state_iter = std::pair<state_t, iterator_t>;

      couvreur99_new_status_ptr<is_explicit> ecs_;

    public:
      couvreur99_new(const automaton_ptr<is_explicit>& a,
                     option_map o = option_map())
        : emptiness_check(a, o)
        , ecs_(std::make_shared<couvreur99_new_status<is_explicit>>(a))
      {
        if (a->acc().uses_fin_acceptance())
          throw std::runtime_error
            ("couvreur99_new requires Fin-less acceptance");
      }

      virtual
      emptiness_check_result_ptr
      check() override
      {
        if (ecs_->aut->acc().get_acceptance().is_f())
          return nullptr;
        return check_impl<true>();
      }

      // The following two methods anticipe the future interface of the
      // class emptiness_check.  Following the interface of twa, the class
      // emptiness_check_result should not be exposed.
      bool
      is_empty()
      {
        return check_impl<false>();
      }

      twa_run
      accepting_run()
      {
        return check_impl<true>()->accepting_run();
      }

    private:
      // A union-like struct to store the result of an emptiness.
      // If the caller only wants to test emptiness, it is sufficient to
      // store the Boolean result.
      // Otherwise, we need to store all the information needed to build
      // an accepting run.
      struct check_result
      {
        enum { BOOL, PTR } tag;
        union
        {
          bool res;
          emptiness_check_result_ptr ecr;
        };

        // Copy constructor.
        check_result(const check_result& o)
          : tag(o.tag)
        {
          if (tag == BOOL)
            res = o.res;
          else
            ecr = o.ecr;
        }
        check_result(bool r)
          : tag(BOOL), res(r)
        {
        }
        check_result(std::nullptr_t)
          : tag(PTR), ecr(nullptr)
        {
        }
        check_result(const emptiness_check_result_ptr& p)
          : tag(PTR), ecr(p)
        {
        }
        // A destructor to properly dereference the shared_pointer.
        ~check_result()
        {
          if (tag == PTR)
            ecr.~emptiness_check_result_ptr();
        }

        // Handy cast operators.
        operator bool() const
        {
          if (tag == BOOL)
            return res;
          else
            return !!ecr;
        }

        operator emptiness_check_result_ptr() const
        {
          if (tag == PTR)
            return ecr;
          else
            throw std::runtime_error("accepting run was not requested");
        }
      };

      template<bool need_accepting_run>
      check_result
      check_impl()
      {
        {
          // check trivial acceptance condition
          auto acc = ecs_->aut->acc();
          if (acc.is_f())
            return nullptr;
          // check whether fin acceptance conditions are used
          if (acc.uses_fin_acceptance())
            throw std::runtime_error
              ("Fin acceptance is not supported by couvreur99()");
        }

        // We use five main data in this algorithm:
        // * root, a stack of strongly connected components (SCC),
        // * h, a hash of all visited nodes with their order,
        //    ("Hash" in Couvreur's paper)
        // * arc, a stack of acceptance conditions between each of these SCC.
        std::stack<acc_cond::mark_t> arc;
        // * num, the number of visited nodes.  Used to set the order of each
        //   visited node.
        int num = 1;
        // * todo, the depth-first-search stack.  This holds pairs of the form
        //   (STATE, ITERATOR) where ITERATOR is a twa_succ_iterator over the
        //   successors of STATE.  In our use, ITERATOR should always be freed
        //   when todo is popped, but STATE should not because it is also used
        //   as a key in h.
        std::stack<pair_state_iter> todo;
        // * live, a stack of live nodes
        std::deque<state_t> live;

        // Setup depth-first search from the initial state.
        {
          state_t init = T::initial_state(ecs_->aut);
          ecs_->h[init] = 1;
          ecs_->root.push(1);
          if (strength == STRONG)
            arc.push({});
          auto iter = T::succ(ecs_->aut, init);
          todo.emplace(init, iter);
          live.emplace_back(init);
          inc_depth();
        }

        while (!todo.empty())
          {
            if (strength == STRONG)
              assert(ecs_->root.size() == arc.size());

            // We are looking at the next successor in SUCC.
            auto& succ = todo.top().second;

            // If there are no more successors, backtrack.
            if (succ->done())
              {
                // We have explored all successors of state CURR.
                state_t curr = todo.top().first;

                // Backtrack todo.
                todo.pop();

                // When backtracking the root of an SCC, we must also remove
                // that SCC from the ARC/ROOT stacks. We must discard from H
                // all reachable states from this SCC.
                assert(!ecs_->root.empty());
                if (ecs_->root.top().index == ecs_->h[curr])
                  {
                    if (strength == STRONG)
                      {
                        assert(!arc.empty());
                        arc.pop();
                      }
                    // Remove all elements of this SCC from the live stack.
                    auto i = std::find(live.rbegin(), live.rend(), curr);
                    assert(i != live.rend());
                    ++i; // Because base() does -1
                    for (auto it = i.base(); it != live.end(); ++it)
                    {
                      ecs_->h[*it] = -1;
                    }
                    live.erase(i.base(), live.end());
                    ecs_->root.pop();
                  }
                T::it_destroy(ecs_->aut, succ);
                // Do not destroy curr: it is a key in h.
                continue;
              }

            // We have a successor to look at.
            inc_transitions();

            // Ignore false edges
            if (SPOT_UNLIKELY(succ->cond() == bddfalse))
              {
                succ->next();
                continue;
              }

            // Fetch the values we are interested in...
            auto acc = succ->acc();
            if (!need_accepting_run)
              if (strength == TERMINAL && ecs_->aut->acc().accepting(acc))
                {
                  // We have found an accepting SCC.
                  // Release all iterators in todo.
                  while (!todo.empty())
                    {
                      T::it_destroy(ecs_->aut, todo.top().second);
                      todo.pop();
                      dec_depth();
                    }
                  // We do not need an accepting run.
                  return true;
                }
            state_t dest = succ->dst();
            // ... and point the iterator to the next successor, for
            // the next iteration.
            succ->next();

            // We do not need succ from now on.

            // Are we going to a new state?
            auto p = T::h_emplace(ecs_->h, dest, num+1);
            if (p.second)
              {
                // Yes.  Bump number, stack the stack, and register its
                // successors for later processing.
                ecs_->root.push(++num);
                if (strength == STRONG)
                  arc.push(acc);
                iterator_t iter = T::succ(ecs_->aut, dest);
                todo.emplace(dest, iter);
                live.emplace_back(dest);
                inc_depth();
                continue;
              }
            T::destroy(dest);

            // If we have reached a dead component, ignore it.
            if (p.first.second == -1)
              continue;

            // Now this is the most interesting case.  We have reached a
            // state S1 which is already part of a non-dead SCC.  Any such
            // non-dead SCC has necessarily been crossed by our path to
            // this state: there is a state S2 in our path which belongs
            // to this SCC too.  We are going to merge all states between
            // this S1 and S2 into this SCC.
            //
            // This merge is easy to do because the order of the SCC in
            // root is ascending: we just have to merge all SCCs from the
            // top of root that have an index greater than the one of
            // the SCC of S2 (called the "threshold").
            int threshold = p.first.second;
            // TODO optimize if this is a self-loop?
            while (threshold < ecs_->root.top().index)
              {
                assert(!ecs_->root.empty());
                if (strength == STRONG)
                  {
                    assert(!arc.empty());
                    acc |= ecs_->root.top().condition;
                    acc |= arc.top();
                    arc.pop();
                  }
                ecs_->root.pop();
              }
            // Note that we do not always have
            //  threshold == root.top().index
            // after this loop, the SCC whose index is threshold might have
            // been merged with a lower SCC.

            // Accumulate all acceptance conditions into the merged SCC.
            ecs_->root.top().condition |= acc;

            if (ecs_->aut->acc().accepting(ecs_->root.top().condition))
              {
                // We have found an accepting SCC.
                // Release all iterators in todo.
                while (!todo.empty())
                  {
                    T::it_destroy(ecs_->aut, todo.top().second);
                    todo.pop();
                    dec_depth();
                  }
                // Use this state to start the computation of an
                // accepting cycle.
                ecs_->cycle_seed = p.first.first;
                set_states(states());
                if (need_accepting_run)
                  return check_result(
                    std::make_shared<couvreur99_new_result<is_explicit>>(ecs_));
                else
                  return true;
              }
          }
        // This automaton recognizes no word.
        set_states(states());
        return nullptr;
      }
    };

  } // anonymous namespace

  template<twa_strength strength>
  using cna = couvreur99_new<false, strength>;
  template<twa_strength strength>
  using cne = couvreur99_new<true, strength>;

  emptiness_check_ptr
  get_couvreur99_new_abstract(const const_twa_ptr& a, option_map o)
  {
    // NB: The order of the if's matter.
    if (a->prop_terminal())
      return SPOT_make_shared_enabled__(cna<TERMINAL>, a, o);
    if (a->prop_weak())
      return SPOT_make_shared_enabled__(cna<WEAK>, a, o);
    return SPOT_make_shared_enabled__(cna<STRONG>, a, o);
  }

  emptiness_check_ptr
  get_couvreur99_new(const const_twa_ptr& a, spot::option_map o)
  {
    const_twa_graph_ptr ag = std::dynamic_pointer_cast<const twa_graph>(a);
    if (ag) // the automaton is explicit
      {
        // NB: The order of the if's matter.
        if (a->prop_terminal())
          return SPOT_make_shared_enabled__(cne<TERMINAL>, ag, o);
        if (a->prop_weak())
          return SPOT_make_shared_enabled__(cne<WEAK>, ag, o);
        return SPOT_make_shared_enabled__(cne<STRONG>, ag, o);
      }
    else // the automaton is abstract
      {
        return get_couvreur99_new_abstract(a, o);
      }
  }

  emptiness_check_result_ptr
  couvreur99_new_check(const const_twa_ptr& a)
  {
    return get_couvreur99_new(a, spot::option_map())->check();
  }

} // namespace spot
