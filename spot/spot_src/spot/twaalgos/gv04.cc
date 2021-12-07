// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2010, 2011, 2013-2020 Laboratoire de
// recherche et développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <iostream>
#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

#include <cassert>
#include <utility>
#include <vector>
#include <spot/twa/twa.hh>
#include <spot/misc/hash.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <spot/twaalgos/gv04.hh>
#include <spot/twaalgos/bfssteps.hh>

namespace spot
{
  namespace
  {
    struct stack_entry
    {
      const state* s;                  // State stored in stack entry.
      twa_succ_iterator* lasttr; // Last transition explored from this state.
      int lowlink;                  // Lowlink value if this entry.
      int pre;                          // DFS predecessor.
      int acc;                          // Accepting state link.
    };

    struct gv04: public emptiness_check, public ec_statistics
    {
      // Map of visited states.
      state_map<size_t> h;

      // Stack of visited states on the path.
      std::vector<stack_entry> stack;

      int top;                        // Top of SCC stack.
      int dftop;                // Top of DFS stack.
      bool violation;                // Whether an accepting run was found.

      gv04(const const_twa_ptr& a, option_map o)
        : emptiness_check(a, o)
      {
        if (!(a->prop_weak().is_true()
              || a->num_sets() == 0
              || a->acc().is_buchi()))
          throw std::runtime_error
            ("gv04 requires Büchi or weak automata");
      }

      ~gv04()
      {
        for (auto i: stack)
          a_->release_iter(i.lasttr);
        auto s = h.begin();
        while (s != h.end())
          {
            // Advance the iterator before deleting the "key" pointer.
            const state* ptr = s->first;
            ++s;
            ptr->destroy();
          }
      }

      virtual emptiness_check_result_ptr
      check() override
      {
        top = dftop = -1;
        violation = false;
        push(a_->get_init_state(), false);

        while (!violation && dftop >= 0)
          {
            trace << "Main iteration (top = " << top
                  << ", dftop = " << dftop
                  << ", s = " << a_->format_state(stack[dftop].s)
                  << ')' << std::endl;

            twa_succ_iterator* iter = stack[dftop].lasttr;
            bool cont;
            if (!iter)
              {
                iter = stack[dftop].lasttr = a_->succ_iter(stack[dftop].s);
                cont = iter->first();
              }
            else
              {
                cont = iter->next();
              }

            if (!cont)
              {
                trace << " No more successors" << std::endl;
                pop();
              }
            else if (SPOT_UNLIKELY(iter->cond() == bddfalse))
              {
                continue;
              }
            else
              {
                const state* s_prime = iter->dst();
                bool acc =
                  a_->acc().accepting(iter->acc());
                inc_transitions();

                trace << " Next successor: s_prime = "
                      << a_->format_state(s_prime)
                      << (acc ? " (with accepting link)" : "");

                auto i = h.find(s_prime);

                if (i == h.end())
                  {
                    trace << " is a new state." << std::endl;
                    push(s_prime, acc);
                  }
                else
                  {
                    if (i->second < stack.size()
                        && stack[i->second].s->compare(s_prime) == 0)
                      {
                        // s_prime has a clone on stack
                        trace << " is on stack." << std::endl;
                        // This is an addition to GV04 to support TBA.
                        violation |= acc;
                        lowlinkupdate(dftop, i->second);
                      }
                    else
                      {
                        trace << " has been seen, but is no longer on stack."
                              << std::endl;
                      }

                    s_prime->destroy();
                  }
              }
            set_states(h.size());
          }
        if (violation)
          return std::make_shared<result>(*this);
        return nullptr;
      }

      void
      push(const state* s, bool accepting)
      {
        trace << "  push(s = " << a_->format_state(s)
              << ", accepting = " << accepting << ")\n";

        h[s] = ++top;

        stack_entry ss = { s, nullptr, top, dftop, 0 };

        if (accepting)
          ss.acc = top - 1;        // This differs from GV04 to support TBA.
        else if (dftop >= 0)
          ss.acc = stack[dftop].acc;
        else
          ss.acc = -1;

        trace << "    s.lowlink = " << top << std::endl
              << "    s.acc = " << ss.acc << std::endl;

        stack.emplace_back(ss);
        dftop = top;
        inc_depth();
      }

      void
      pop()
      {
        trace << "  pop()\n";

        int p = stack[dftop].pre;
        if (p >= 0)
          lowlinkupdate(p, dftop);
        if (stack[dftop].lowlink == dftop)
          {
            assert(static_cast<unsigned int>(top + 1) == stack.size());
            for (int i = top; i >= dftop; --i)
              {
                a_->release_iter(stack[i].lasttr);
                stack.pop_back();
                dec_depth();
              }
            top = dftop - 1;
          }
        dftop = p;
      }

      void
      lowlinkupdate(int f, int t)
      {
        trace << "  lowlinkupdate(f = " << f << ", t = " << t
              << ")\n    t.lowlink = " << stack[t].lowlink
              << "\n    f.lowlink = " << stack[f].lowlink
              << "\n    f.acc = " << stack[f].acc << '\n';
        int stack_t_lowlink = stack[t].lowlink;
        if (stack_t_lowlink <= stack[f].lowlink)
          {
            if (stack_t_lowlink <= stack[f].acc)
              violation = true;
            stack[f].lowlink = stack_t_lowlink;
            trace << "    f.lowlink updated to "
                  << stack[f].lowlink << '\n';
          }
      }

      virtual std::ostream&
      print_stats(std::ostream& os) const override
      {
        os << h.size() << " unique states visited\n";
        os << transitions() << " transitions explored\n";
        os << max_depth() << " items max on stack\n";
        return os;
      }

      struct result:
        public emptiness_check_result,
        public acss_statistics
      {
        gv04& data;

        result(gv04& data)
          : emptiness_check_result(data.automaton(), data.options()),
            data(data)
        {
        }

        void
        update_lowlinks()
        {
          // Transitively update the lowlinks, so we can use them in
          // to check SCC inclusion
          for (int i = 0; i <= data.top; ++i)
            {
              int l = data.stack[i].lowlink;
              if (l < i)
                {
                  int ll = data.stack[i].lowlink = data.stack[l].lowlink;
                  for (int j = i - 1; data.stack[j].lowlink != ll; --j)
                    data.stack[j].lowlink = ll;
                }
            }
        }

        virtual unsigned
        acss_states() const override
        {
          // Gross!
          const_cast<result*>(this)->update_lowlinks();

          int scc = data.stack[data.dftop].lowlink;
          int j = data.dftop;
          int s = 0;
          while (j >= 0 && data.stack[j].lowlink == scc)
            {
              --j;
              ++s;
            }
          assert(s > 0);
          return s;
        }

        virtual twa_run_ptr
        accepting_run() override
        {
          auto res = std::make_shared<twa_run>(automaton());

          update_lowlinks();
#ifdef TRACE
          for (int i = 0; i <= data.top; ++i)
            {
              trace << "state " << i << " ("
                    << data.a_->format_state(data.stack[i].s)
                    << ") has lowlink = " << data.stack[i].lowlink << std::endl;
            }
#endif

          // We will use the root of the last SCC as the start of the
          // cycle.
          int scc_root = data.stack[data.dftop].lowlink;
          assert(scc_root >= 0);

          // Construct the prefix by unwinding the DFS stack before
          // scc_root.
          int father = data.stack[scc_root].pre;
          while (father >= 0)
            {
              twa_run::step st =
                {
                  data.stack[father].s->clone(),
                  data.stack[father].lasttr->cond(),
                  data.stack[father].lasttr->acc()
                };
              res->prefix.push_front(st);
              father = data.stack[father].pre;
            }

          // Construct the cycle in two phases.  A first BFS finds the
          // shortest path from scc_root to an accepting transition.
          // A second BFS then search a path back to scc_root.  If
          // there is no acceptance conditions we just use the second
          // BFS to find a cycle around scc_root.

          struct first_bfs: bfs_steps
          {
            const gv04& data;
            int scc_root;
            result* r;

            first_bfs(result* r, int scc_root)
              : bfs_steps(r->data.automaton()), data(r->data),
                scc_root(scc_root), r(r)
            {
            }

            virtual const state*
            filter(const state* s) override
            {
              // Do not escape the SCC
              auto j = data.h.find(s);
              if (// This state was never visited so far.
                  j == data.h.end()
                  // Or it was discarded
                  || j->second >= data.stack.size()
                  // Or it was discarded (but its stack slot reused)
                  || data.stack[j->second].s->compare(s)
                  // Or it is still on the stack but not in the SCC
                  || data.stack[j->second].lowlink < scc_root)
                {
                  s->destroy();
                  return nullptr;
                }
              r->inc_ars_cycle_states();
              s->destroy();
              return j->first;
            }

            virtual bool
            match(twa_run::step& step, const state*) override
            {
              return !!step.acc;
            }
          };

          struct second_bfs final: first_bfs
          {
            const state* target;
            second_bfs(result* r, int scc_root, const state* target)
              : first_bfs(r, scc_root), target(target)
            {
            }

            virtual bool
            match(twa_run::step&, const state* s) override
            {
              return s == target;
            }
          };

          const state* bfs_start = data.stack[scc_root].s;
          const state* bfs_end = bfs_start;
          if (a_->num_sets() > 0)
            {
              first_bfs b1(this, scc_root);
              bfs_start = b1.search(bfs_start, res->cycle);
              assert(bfs_start);
            }
          if (bfs_start != bfs_end || res->cycle.empty())
            {
              second_bfs b2(this, scc_root, bfs_end);
              bfs_start = b2.search(bfs_start, res->cycle);
              assert(bfs_start == bfs_end);
            }

          assert(res->cycle.begin() != res->cycle.end());
          return res;
        }
      };


    };

  } // anonymous

  emptiness_check_ptr
  explicit_gv04_check(const const_twa_ptr& a, option_map o)
  {
    return SPOT_make_shared_enabled__(gv04, a, o);
  }
}
