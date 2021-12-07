// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2011, 2014-2016, 2018-2020 Laboratoire de
// Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005, 2006 Laboratoire d'Informatique de
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
#include <iostream>
// #define TRACE
#ifdef TRACE
#define trace std::cerr
#define SPOT_TRACE 1
#else
#define trace while (0) std::cerr
#endif

#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/gtec/ce.hh>
#include <spot/misc/memusage.hh>

namespace spot
{
  namespace
  {
    typedef std::pair<const spot::state*, twa_succ_iterator*> pair_state_iter;
  }

  couvreur99_check::couvreur99_check(const const_twa_ptr& a, option_map o)
    : emptiness_check(a, o),
      removed_components(0)
  {
    poprem_ = o.get("poprem", 1);
    ecs_ = std::make_shared<couvreur99_check_status>(a);
    stats["removed components"] =
        static_cast<spot::unsigned_statistics::unsigned_fun>
        (&couvreur99_check::get_removed_components);
    stats["vmsize"] =
        static_cast<spot::unsigned_statistics::unsigned_fun>
        (&couvreur99_check::get_vmsize);
  }

  couvreur99_check::~couvreur99_check()
  {
  }

  unsigned
  couvreur99_check::get_removed_components() const
  {
    return removed_components;
  }

  unsigned
  couvreur99_check::get_vmsize() const
  {
    int size = memusage();
    if (size > 0)
      return size;
    return 0;
  }

  void
  couvreur99_check::remove_component(const state* from)
  {
    ++removed_components;
    // If rem has been updated, removing states is very easy.
    if (poprem_)
      {
        assert(!ecs_->root.rem().empty());
        dec_depth(ecs_->root.rem().size());
        for (auto i: ecs_->root.rem())
          ecs_->h[i] = -1;
        // ecs_->root.rem().clear();
        return;
      }

    // Remove from H all states which are reachable from state FROM.

    // Stack of iterators towards states to remove.
    std::stack<twa_succ_iterator*> to_remove;

    // Remove FROM itself, and prepare to remove its successors.
    // (FROM should be in H, otherwise it means all reachable
    // states from FROM have already been removed and there is no
    // point in calling remove_component.)
    ecs_->h[from] = -1;
    twa_succ_iterator* i = ecs_->aut->succ_iter(from);

    for (;;)
      {
        // Remove each destination of this iterator.
        if (i->first())
          do
            {
              inc_transitions();
              if (SPOT_UNLIKELY(i->cond() == bddfalse))
                continue;

              const state* s = i->dst();
              auto j = ecs_->h.find(s);
              assert(j != ecs_->h.end());
              s->destroy();

              if (j->second != -1)
                {
                  j->second = -1;
                  to_remove.push(ecs_->aut->succ_iter(j->first));
                }
            }
          while (i->next());
        ecs_->aut->release_iter(i);
        if (to_remove.empty())
          break;
        i = to_remove.top();
        to_remove.pop();
      }
  }

  emptiness_check_result_ptr
  couvreur99_check::check()
  {
    {
      auto acc = ecs_->aut->acc();
      if (acc.get_acceptance().is_f())
        return nullptr;
      if (acc.uses_fin_acceptance())
        throw std::runtime_error
          ("Fin acceptance is not supported by couvreur99()");
    }

    // We use five main data in this algorithm:
    // * couvreur99_check::root, a stack of strongly connected components (SCC),
    // * couvreur99_check::h, a hash of all visited nodes, with their order,
    //   (it is called "Hash" in Couvreur's paper)
    // * arc, a stack of acceptance conditions between each of these SCC,
    std::stack<acc_cond::mark_t> arc;
    // * num, the number of visited nodes.  Used to set the order of each
    //   visited node,
    int num = 1;
    // * todo, the depth-first search stack.  This holds pairs of the
    //   form (STATE, ITERATOR) where ITERATOR is a twa_succ_iterator
    //   over the successors of STATE.  In our use, ITERATOR should
    //   always be freed when TODO is popped, but STATE should not because
    //   it is also used as a key in H.
    std::stack<pair_state_iter> todo;

    // Setup depth-first search from the initial state.
    {
      const state* init = ecs_->aut->get_init_state();
      ecs_->h[init] = 1;
      ecs_->root.push(1);
      arc.push({});
      twa_succ_iterator* iter = ecs_->aut->succ_iter(init);
      iter->first();
      todo.emplace(init, iter);
      inc_depth();
    }

    while (!todo.empty())
      {
        assert(ecs_->root.size() == arc.size());

        // We are looking at the next successor in SUCC.
        twa_succ_iterator* succ = todo.top().second;

        // If there is no more successor, backtrack.
        if (succ->done())
          {
            // We have explored all successors of state CURR.
            const state* curr = todo.top().first;

            // Backtrack TODO.
            todo.pop();
            dec_depth();

            // If poprem is used, fill rem with any component removed,
            // so that remove_component() does not have to traverse
            // the SCC again.
            auto i = ecs_->h.find(curr);
            assert(i != ecs_->h.end());
            if (poprem_)
              {
                ecs_->root.rem().push_front(i->first);
                inc_depth();
              }
            // When backtracking the root of an SCC, we must also
            // remove that SCC from the ARC/ROOT stacks.  We must
            // discard from H all reachable states from this SCC.
            assert(!ecs_->root.empty());
            if (ecs_->root.top().index == i->second)
              {
                assert(!arc.empty());
                arc.pop();
                remove_component(curr);
                ecs_->root.pop();
              }
            ecs_->aut->release_iter(succ);
            // Do not destroy CURR: it is a key in H.
            continue;
          }

        // We have a successor to look at.
        inc_transitions();
        // Fetch the values (destination state, acceptance conditions
        // of the arc) we are interested in...
        const state* dest = succ->dst();
        acc_cond::mark_t acc = succ->acc();
        trace << "-> " << dest << ' ' << acc << ' ' << succ->cond();
        // ... and point the iterator to the next successor, for
        // the next iteration.
        {
          bdd cond = succ->cond();
          succ->next();
          if (SPOT_UNLIKELY(cond == bddfalse))
            continue;
        }
        // We do not need SUCC from now on.

        // Are we going to a new state?
        auto p = ecs_->h.emplace(dest, num + 1);
        if (p.second)
          {
            // Yes.  Bump number, stack the stack, and register its
            // successors for later processing.
            ecs_->root.push(++num);
            arc.push(acc);
            twa_succ_iterator* iter = ecs_->aut->succ_iter(dest);
            iter->first();
            todo.emplace(dest, iter);
            inc_depth();
            continue;
          }
        dest->destroy();

        // If we have reached a dead component, ignore it.
        if (p.first->second == -1)
          continue;

        // Now this is the most interesting case.  We have reached a
        // state S1 which is already part of a non-dead SCC.  Any such
        // non-dead SCC has necessarily been crossed by our path to
        // this state: there is a state S2 in our path which belongs
        // to this SCC too.  We are going to merge all states between
        // this S1 and S2 into this SCC.
        //
        // This merge is easy to do because the order of the SCC in
        // ROOT is ascending: we just have to merge all SCCs from the
        // top of ROOT that have an index greater to the one of
        // the SCC of S2 (called the "threshold").
        int threshold = p.first->second;
        std::list<const state*> rem;
        while (threshold < ecs_->root.top().index)
          {
            assert(!ecs_->root.empty());
            assert(!arc.empty());
            acc |= ecs_->root.top().condition;
            acc |= arc.top();
            rem.splice(rem.end(), ecs_->root.rem());
            ecs_->root.pop();
            arc.pop();
          }
        // Note that we do not always have
        //  threshold == ecs_->root.top().index
        // after this loop, the SCC whose index is threshold might have
        // been merged with a lower SCC.

        // Accumulate all acceptance conditions into the merged SCC.
        ecs_->root.top().condition |= acc;
        ecs_->root.rem().splice(ecs_->root.rem().end(), rem);

        if (ecs_->aut->acc().accepting(ecs_->root.top().condition))
          {
            // We have found an accepting SCC.
            // Release all iterators in TODO.
            while (!todo.empty())
              {
                ecs_->aut->release_iter(todo.top().second);
                todo.pop();
                dec_depth();
              }
            // Use this state to start the computation of an accepting
            // cycle.
            ecs_->cycle_seed = p.first->first;
            set_states(ecs_->states());
            return std::make_shared<couvreur99_check_result>(ecs_, options());
          }
      }
    // This automaton recognizes no word.
    set_states(ecs_->states());
    return nullptr;
  }

  std::shared_ptr<const couvreur99_check_status>
  couvreur99_check::result() const
  {
    return ecs_;
  }

  std::ostream&
  couvreur99_check::print_stats(std::ostream& os) const
  {
    ecs_->print_stats(os);
    os << transitions() << " transitions explored" << std::endl;
    os << max_depth() << " items max in DFS search stack" << std::endl;
    return os;
  }

  //////////////////////////////////////////////////////////////////////

  couvreur99_check_shy::todo_item::todo_item(const state* s, int n,
                                             couvreur99_check_shy* shy)
        : s(s), n(n)
  {
    for (auto iter: shy->ecs_->aut->succ(s))
      {
        shy->inc_transitions();
        if (SPOT_UNLIKELY(iter->cond() == bddfalse))
          continue;
        q.emplace_back(iter->acc(),
                       iter->dst());
        shy->inc_depth();
      }
  }

  couvreur99_check_shy::couvreur99_check_shy(const const_twa_ptr& a,
                                             option_map o)
    : couvreur99_check(a, o), num(1)
  {
    group_ = o.get("group", 1);
    group2_ = o.get("group2", 0);
    group_ |= group2_;

    // Setup depth-first search from the initial state.
    const state* i = ecs_->aut->get_init_state();
    ecs_->h[i] = ++num;
    ecs_->root.push(num);
    todo.emplace_back(i, num, this);
    inc_depth(1);
  }

  couvreur99_check_shy::~couvreur99_check_shy()
  {
  }

  void
  couvreur99_check_shy::clear_todo()
  {
    // We must destroy all states appearing in TODO
    // unless they are used as keys in H.
    while (!todo.empty())
      {
        succ_queue& queue = todo.back().q;
        for (auto& q: queue)
          {
            // Destroy the state if it is a clone of a state in the
            // heap or if it is an unknown state.
            auto i = ecs_->h.find(q.s);
            if (i == ecs_->h.end() || i->first != q.s)
              q.s->destroy();
          }
        dec_depth(todo.back().q.size() + 1);
        todo.pop_back();
      }
    dec_depth(ecs_->root.clear_rem());
    assert(depth() == 0);
  }

#ifdef TRACE
  namespace
  {
    template<class T>
    void dump_queue(const T& todo)
    {
      trace << "--- TODO ---\n";
      unsigned lvl = 0;
      for (auto& ti: todo)
        {
          ++lvl;
          trace << '#' << lvl << " s:" << ti.s << " n:" << ti.n
                << " q:{";
          for (auto qi = ti.q.begin(); qi != ti.q.end();)
            {
              trace << qi->s;
              ++qi;
              if (qi != ti.q.end())
                trace << ", ";
            }
          trace << "}\n";
        }
    }
  }
#endif

  emptiness_check_result_ptr
  couvreur99_check_shy::check()
  {
    {
      auto acc = ecs_->aut->acc();
      if (acc.get_acceptance().is_f())
        return nullptr;
      if (acc.uses_fin_acceptance())
        throw std::runtime_error
          ("Fin acceptance is not supported by couvreur99()");
    }
    // Position in the loop seeking known successors.
    pos = todo.back().q.begin();

    for (;;)
      {
#ifdef TRACE
        dump_queue(todo);
#endif

        assert(ecs_->root.size() == 1 + arc.size());

        // Get the successors of the current state.
        succ_queue& queue = todo.back().q;

        // If there is no more successor, backtrack.
        if (queue.empty())
          {
            trace << "backtrack" << std::endl;

            // We have explored all successors of state CURR.
            const state* curr = todo.back().s;
            int index = todo.back().n;

            // Backtrack TODO.
            todo.pop_back();
            dec_depth();

            if (todo.empty())
              {
                // This automaton recognizes no word.
                set_states(ecs_->states());
                assert(poprem_ || depth() == 0);
                return nullptr;
              }

            pos = todo.back().q.begin();

            // If poprem is used, fill rem with any component removed,
            // so that remove_component() does not have to traverse
            // the SCC again.
            if (poprem_)
              {
                auto i = ecs_->h.find(curr);
                assert(i != ecs_->h.end());
                assert(i->first == curr);
                ecs_->root.rem().push_front(i->first);
                inc_depth();
              }

            // When backtracking the root of an SCC, we must also
            // remove that SCC from the ARC/ROOT stacks.  We must
            // discard from H all reachable states from this SCC.
            assert(!ecs_->root.empty());
            if (ecs_->root.top().index == index)
              {
                assert(!arc.empty());
                arc.pop();
                remove_component(curr);
                ecs_->root.pop();
              }
            continue;
          }

        // We always make a first pass over the successors of a state
        // to check whether it contains some state we have already seen.
        // This way we hope to merge the most SCCs before stacking new
        // states.
        //
        // So are we checking for known states ?  If yes, POS tells us
        // which state we are considering.  Otherwise just pick the
        // first one.
        succ_queue::iterator old;
        if (pos == queue.end())
          old = queue.begin();
        else
          old = pos;
        if (pos != queue.end())
          ++pos;
        //int* i = sip.second;

        successor succ = *old;
        trace << "picked state " << succ.s << '\n';
        auto i = ecs_->h.find(succ.s);

        if (i == ecs_->h.end())
          {
            // It's a new state.
            // If we are seeking known states, just skip it.
            if (pos != queue.end())
              continue;

            trace << "new state\n";

            // Otherwise, number it and stack it so we recurse.
            queue.erase(old);
            dec_depth();
            ecs_->h[succ.s] = ++num;
            ecs_->root.push(num);
            arc.push(succ.acc);
            todo.emplace_back(succ.s, num, this);
            pos = todo.back().q.begin();
            inc_depth();
            continue;
          }

        // It's an known state.  Use i->first from now on.
        succ.s->destroy();

        queue.erase(old);
        dec_depth();

        // Skip dead states.
        if (i->second == -1)
          {
            trace << "dead state\n";
            continue;
          }

        trace << "merging...\n";

        // Now this is the most interesting case.  We have
        // reached a state S1 which is already part of a
        // non-dead SCC.  Any such non-dead SCC has
        // necessarily been crossed by our path to this
        // state: there is a state S2 in our path which
        // belongs to this SCC too.  We are going to merge
        // all states between this S1 and S2 into this
        // SCC.
        //
        // This merge is easy to do because the order of
        // the SCC in ROOT is ascending: we just have to
        // merge all SCCs from the top of ROOT that have
        // an index greater to the one of the SCC of S2
        // (called the "threshold").
        int threshold = i->second;
        std::list<const state*> rem;
        acc_cond::mark_t acc = succ.acc;
        while (threshold < ecs_->root.top().index)
          {
            assert(!ecs_->root.empty());
            assert(!arc.empty());
            acc |= ecs_->root.top().condition;
            acc |= arc.top();
            rem.splice(rem.end(), ecs_->root.rem());
            ecs_->root.pop();
            arc.pop();
          }
        // Note that we do not always have
        //   threshold == ecs_->root.top().index
        // after this loop, the SCC whose index is threshold
        // might have been merged with a lower SCC.

        // Accumulate all acceptance conditions into the
        // merged SCC.
        ecs_->root.top().condition |= acc;
        ecs_->root.rem().splice(ecs_->root.rem().end(), rem);

        // Have we found all acceptance conditions?
        if (ecs_->aut->acc().accepting(ecs_->root.top().condition))
          {
            // Use this state to start the computation of an accepting
            // cycle.
            ecs_->cycle_seed = i->first;

            // We have found an accepting SCC.  Clean up TODO.
            clear_todo();
            set_states(ecs_->states());
            return std::make_shared<couvreur99_check_result>(ecs_, options());
          }
        // Group the pending successors of formed SCC if requested.
        if (group_)
          {
            assert(todo.back().s);
            while (ecs_->root.top().index < todo.back().n)
              {
                todo_list::reverse_iterator prev = todo.rbegin();
                todo_list::reverse_iterator last = prev++;
                // If group2 is used we insert the last->q in front
                // of prev->q so that the states in prev->q are checked
                // for existence again after we have processed the states
                // of last->q.  Otherwise we just append to the end.
                prev->q.splice(group2_ ? prev->q.begin() : prev->q.end(),
                               last->q);

                if (poprem_)
                  {
                    const state* s = todo.back().s;
                    auto i = ecs_->h.find(s);
                    assert(i != ecs_->h.end());
                    assert(i->first == s);
                    ecs_->root.rem().push_front(i->first);
                    // Don't change the stack depth, since
                    // we are just moving the state from TODO to REM.
                  }
                else
                  {
                    dec_depth();
                  }
                todo.pop_back();
              }
            pos = todo.back().q.begin();
          }
      }
  }

  emptiness_check_ptr
  couvreur99(const const_twa_ptr& a, option_map o)
  {
    if (o.get("shy"))
      return SPOT_make_shared_enabled__(couvreur99_check_shy, a, o);
    return SPOT_make_shared_enabled__(couvreur99_check, a, o);
  }

}
