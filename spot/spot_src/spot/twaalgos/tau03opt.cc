// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2019  Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

/// FIXME:
/// * Test some heuristics on the order of visit of the successors in the blue
///   dfs:
///   - favorize the arcs conducting to the blue stack (the states of color
///     cyan)
///   - in this category, favorize the labelled arcs
///   - for the remaining ones, favorize the arcs labelled by the greatest
///     number of new acceptance conditions (notice that this number may evolve
///     after the visit of previous successors).
///
/// * Add a bit-state hashing version.

//#define TRACE

#include "config.h"
#include <iostream>
#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

#include <cassert>
#include <vector>
#include <stack>
#include <spot/misc/hash.hh>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <spot/twaalgos/tau03opt.hh>
#include <spot/priv/weight.hh>
#include <spot/twaalgos/ndfs_result.hxx>

namespace spot
{
  namespace
  {
    enum color {WHITE, CYAN, BLUE};

    /// \brief Emptiness checker on spot::tgba automata having a finite number
    /// of acceptance conditions (i.e. a TGBA).
    template <typename heap>
    class tau03_opt_search : public emptiness_check, public ec_statistics
    {
    public:
      /// \brief Initialize the search algorithm on the automaton \a a
      tau03_opt_search(const const_twa_ptr& a, size_t size, option_map o)
        : emptiness_check(a, o),
          current_weight(a->acc()),
          h(size),
          use_condition_stack(o.get("condstack")),
          use_ordering(use_condition_stack && o.get("ordering")),
          use_weights(o.get("weights", 1)),
          use_red_weights(use_weights && o.get("redweights", 1))
      {
        if (a->acc().uses_fin_acceptance())
          throw std::runtime_error("tau03opt requires Fin-less acceptance");
      }

      virtual ~tau03_opt_search()
      {
        // Release all iterators on the stacks.
        while (!st_blue.empty())
          {
            h.pop_notify(st_blue.front().s);
            a_->release_iter(st_blue.front().it);
            st_blue.pop_front();
          }
        while (!st_red.empty())
          {
            h.pop_notify(st_red.front().s);
            a_->release_iter(st_red.front().it);
            st_red.pop_front();
          }
      }

      /// \brief Perform an emptiness check.
      ///
      /// \return non null pointer iff the algorithm has found an
      /// accepting path.
      virtual emptiness_check_result_ptr check() override
      {
        if (!st_blue.empty())
            return nullptr;
        assert(st_red.empty());
        const state* s0 = a_->get_init_state();
        inc_states();
        h.add_new_state(s0, CYAN, current_weight);
        push(st_blue, s0, bddfalse, {});
        auto t = std::static_pointer_cast<tau03_opt_search>
          (this->emptiness_check::shared_from_this());
        if (dfs_blue())
          return std::make_shared<ndfs_result<tau03_opt_search<heap>, heap>>(t);
        return nullptr;
      }

      virtual std::ostream& print_stats(std::ostream &os) const override
      {
        os << states() << " distinct nodes visited" << std::endl;
        os << transitions() << " transitions explored" << std::endl;
        os << max_depth() << " nodes for the maximal stack depth" << std::endl;
        return os;
      }

      const heap& get_heap() const
        {
          return h;
        }

      const stack_type& get_st_blue() const
        {
          return st_blue;
        }

      const stack_type& get_st_red() const
        {
          return st_red;
        }

    private:
      void push(stack_type& st, const state* s,
                const bdd& label, acc_cond::mark_t acc)
      {
        inc_depth();
        twa_succ_iterator* i = a_->succ_iter(s);
        i->first();
        st.emplace_front(s, i, label, acc);
      }

      void pop(stack_type& st)
      {
        dec_depth();
        a_->release_iter(st.front().it);
        st.pop_front();
      }

      acc_cond::mark_t project_acc(acc_cond::mark_t acc) const
      {
        if (!use_ordering)
          return acc;
        // FIXME: This should be improved.
        std::vector<unsigned> res;
        unsigned max = a_->num_sets();
        for (unsigned n = 0; n < max && acc.has(n); ++n)
          res.emplace_back(n);
        return acc_cond::mark_t(res.begin(), res.end());
      }

      /// \brief weight of the state on top of the blue stack.
      weight current_weight;

      /// \brief Stack of the blue dfs.
      stack_type st_blue;

      /// \brief Stack of the red dfs.
      stack_type st_red;

      /// \brief Map where each visited state is colored
      /// by the last dfs visiting it.
      heap h;

      /// Whether to use the "condition stack".
      bool use_condition_stack;
      /// Whether to use an ordering between the acceptance conditions.
      /// Effective only if using the condition stack.
      bool use_ordering;
      /// Whether to use weights to abort earlier.
      bool use_weights;
      /// Whether to use weights in the red dfs.
      bool use_red_weights;

      bool dfs_blue()
      {
        while (!st_blue.empty())
          {
            stack_item& f = st_blue.front();
            trace << "DFS_BLUE treats: " << a_->format_state(f.s) << std::endl;
            if (!f.it->done())
              {
                const state *s_prime = f.it->dst();
                trace << "  Visit the successor: "
                      << a_->format_state(s_prime) << std::endl;
                bdd label = f.it->cond();
                auto acc = f.it->acc();
                // Go down the edge (f.s, <label, acc>, s_prime)
                f.it->next();
                inc_transitions();
                typename heap::color_ref c_prime = h.get_color_ref(s_prime);
                if (c_prime.is_white())
                  {
                    trace << "  It is white, go down" << std::endl;
                    if (use_weights)
                      current_weight.add(acc);
                    inc_states();
                    h.add_new_state(s_prime, CYAN, current_weight);
                    push(st_blue, s_prime, label, acc);
                  }
                else
                  {
                    typename heap::color_ref c = h.get_color_ref(f.s);
                    assert(!c.is_white());
                    if (c_prime.get_color() == CYAN
                        && a_->acc().accepting
                        (current_weight.diff(a_->acc(), c_prime. get_weight())
                         | c.get_acc() | acc | c_prime.get_acc()))
                      {
                        trace << "  It is cyan and acceptance condition "
                              << "is reached, report cycle" << std::endl;
                        c_prime.cumulate_acc(a_->acc().all_sets());
                        push(st_red, s_prime, label, acc);
                        return true;
                      }
                    else
                      {
                        trace << "  It is cyan or blue and";
                        auto acu = acc | c.get_acc();
                        auto acp = project_acc(acu);
                        if ((c_prime.get_acc() & acp) != acp)
                          {
                            trace << "  a propagation is needed, "
                                  << "start a red dfs" << std::endl;
                            c_prime.cumulate_acc(acp);
                            push(st_red, s_prime, label, acc);
                            if (dfs_red(acu))
                              return true;
                          }
                        else
                          {
                            trace << " no propagation is needed, pop it."
                                  << std::endl;
                            h.pop_notify(s_prime);
                          }
                      }
                  }
              }
            else
            // Backtrack the edge
            //        (predecessor of f.s in st_blue, <f.label, f.acc>, f.s)
              {
                trace << "  All the successors have been visited" << std::endl;
                stack_item f_dest(f);
                pop(st_blue);
                if (use_weights)
                  current_weight.sub(f_dest.acc);
                typename heap::color_ref c_prime = h.get_color_ref(f_dest.s);
                assert(!c_prime.is_white());
                c_prime.set_color(BLUE);
                if (!st_blue.empty())
                  {
                    typename heap::color_ref c =
                                          h.get_color_ref(st_blue.front().s);
                    assert(!c.is_white());
                    auto acu = f_dest.acc | c.get_acc();
                    auto acp = project_acc(acu);
                    if ((c_prime.get_acc() & acp) != acp)
                      {
                        trace << "  The arc from "
                              << a_->format_state(st_blue.front().s)
                              << " to the current state implies to "
                              << " start a red dfs" << std::endl;
                        c_prime.cumulate_acc(acp);
                        push(st_red, f_dest.s, f_dest.label, f_dest.acc);
                        if (dfs_red(acu))
                            return true;
                      }
                    else
                      {
                        trace << "  Pop it" << std::endl;
                        h.pop_notify(f_dest.s);
                      }
                  }
                else
                  {
                    trace << "  Pop it" << std::endl;
                    h.pop_notify(f_dest.s);
                  }
              }
          }
        return false;
      }

      bool
      dfs_red(acc_cond::mark_t acu)
      {
        assert(!st_red.empty());

        // These are useful only when USE_CONDITION_STACK is set.
        typedef std::pair<acc_cond::mark_t, unsigned> cond_level;
        std::stack<cond_level> condition_stack;
        unsigned depth = 1;
        condition_stack.emplace(acc_cond::mark_t({}), 0);

        while (!st_red.empty())
          {
            stack_item& f = st_red.front();
            trace << "DFS_RED treats: " << a_->format_state(f.s) << std::endl;
            if (!f.it->done())
              {
                const state *s_prime = f.it->dst();
                trace << "  Visit the successor: "
                      << a_->format_state(s_prime) << std::endl;
                bdd label = f.it->cond();
                auto acc = f.it->acc();
                // Go down the edge (f.s, <label, acc>, s_prime)
                f.it->next();
                inc_transitions();
                typename heap::color_ref c_prime = h.get_color_ref(s_prime);
                if (c_prime.is_white())
                  {
                    trace << "  It is white, pop it" << std::endl;
                    s_prime->destroy();
                    continue;
                  }
                else if (c_prime.get_color() == CYAN &&
                         a_->acc().accepting
                         (acc | acu | c_prime.get_acc() |
                          (use_red_weights ?
                           current_weight.diff(a_->acc(),
                                               c_prime.
                                               get_weight())
                           : acc_cond::mark_t({}))))
                  {
                    trace << "  It is cyan and acceptance condition "
                          << "is reached, report cycle" << std::endl;
                    c_prime.cumulate_acc(a_->acc().all_sets());
                    push(st_red, s_prime, label, acc);
                    return true;
                  }
                acc_cond::mark_t acp;
                if (use_ordering)
                  acp = project_acc(acu | acc | c_prime.get_acc());
                else if (use_condition_stack)
                  acp = acu | acc;
                else
                  acp = acu;
                if ((c_prime.get_acc() & acp) != acp)
                  {
                    trace << "  It is cyan or blue and propagation "
                          << "is needed, go down"
                          << std::endl;
                    c_prime.cumulate_acc(acp);
                    push(st_red, s_prime, label, acc);
                    if (use_condition_stack)
                      {
                        auto old = acu;
                        acu |= acc;
                        condition_stack.emplace(acu - old, depth);
                      }
                    ++depth;
                  }
                else
                  {
                    trace << "  It is cyan or blue and no propagation "
                          << "is needed , pop it" << std::endl;
                    h.pop_notify(s_prime);
                  }
              }
            else // Backtrack
              {
                trace << "  All the successors have been visited, pop it"
                      << std::endl;
                h.pop_notify(f.s);
                pop(st_red);
                --depth;
                if (condition_stack.top().second == depth)
                  {
                    acu -= condition_stack.top().first;
                    condition_stack.pop();
                  }
              }
          }
        assert(depth == 0);
        assert(condition_stack.empty());
        return false;
      }

    };

    class explicit_tau03_opt_search_heap final
    {
      typedef state_map<std::pair<weight, acc_cond::mark_t>> hcyan_type;
      typedef state_map<std::pair<color, acc_cond::mark_t>> hash_type;
    public:
      class color_ref final
      {
      public:
        color_ref(hash_type* h, hcyan_type* hc, const state* s,
                  const weight* w, acc_cond::mark_t* a)
          : is_cyan(true), w(w), ph(h), phc(hc), ps(s), pc(nullptr), acc(a)
          {
          }
        color_ref(color* c, acc_cond::mark_t* a)
          : is_cyan(false), pc(c), acc(a)
          {
          }
        color get_color() const
          {
            if (is_cyan)
              return CYAN;
            return *pc;
          }
        const weight& get_weight() const
          {
            assert(is_cyan);
            return *w;
          }
        void set_color(color c)
          {
            assert(!is_white());
            if (is_cyan)
              {
                assert(c != CYAN);
                std::pair<hash_type::iterator, bool> p;
                p = ph->emplace(ps, std::make_pair(c, *acc));
                assert(p.second);
                acc = &(p.first->second.second);
                int i = phc->erase(ps);
                assert(i == 1);
                (void)i;
              }
            else
              {
                *pc=c;
              }
          }
        acc_cond::mark_t get_acc() const
          {
            assert(!is_white());
            return *acc;
          }
        void cumulate_acc(acc_cond::mark_t a)
          {
            assert(!is_white());
            *acc |= a;
          }
        bool is_white() const
          {
            return !is_cyan && !pc;
          }
      private:
        bool is_cyan;
        const weight* w; // point to the weight of a state in hcyan
        hash_type* ph; //point to the main hash table
        hcyan_type* phc; // point to the hash table hcyan
        const state* ps; // point to the state in hcyan
        color *pc; // point to the color of a state stored in main hash table
        acc_cond::mark_t* acc; // point to the acc set of a state stored
                                // in main hash table  or hcyan
      };

      explicit_tau03_opt_search_heap(size_t)
        {
        }

      ~explicit_tau03_opt_search_heap()
        {
          hcyan_type::const_iterator sc = hc.begin();
          while (sc != hc.end())
            {
              const state* ptr = sc->first;
              ++sc;
              ptr->destroy();
            }
          hash_type::const_iterator s = h.begin();
          while (s != h.end())
            {
              const state* ptr = s->first;
              ++s;
              ptr->destroy();
            }
        }

      color_ref get_color_ref(const state*& s)
        {
          hcyan_type::iterator ic = hc.find(s);
          if (ic == hc.end())
            {
              hash_type::iterator it = h.find(s);
              if (it == h.end())
                // white state
                return color_ref(nullptr, nullptr);
              if (s != it->first)
                {
                  s->destroy();
                  s = it->first;
                }
              // blue or red state
              return color_ref(&it->second.first, &it->second.second);
            }
          if (s != ic->first)
            {
              s->destroy();
              s = ic->first;
            }
          // cyan state
          return color_ref(&h, &hc, ic->first,
                           &ic->second.first, &ic->second.second);
        }

      void add_new_state(const state* s, color c, const weight& w)
        {
          assert(hc.find(s) == hc.end() && h.find(s) == h.end());
          assert(c == CYAN);
          (void)c;
          hc.emplace(std::piecewise_construct,
                     std::forward_as_tuple(s),
                     std::forward_as_tuple(w, acc_cond::mark_t({})));
        }

      void pop_notify(const state*) const
        {
        }

      bool has_been_visited(const state* s) const
        {
          hcyan_type::const_iterator ic = hc.find(s);
          if (ic == hc.end())
            {
              hash_type::const_iterator it = h.find(s);
              return (it != h.end());
            }
          return true;
        }

      enum { Has_Size = 1 };
      int size() const
        {
          return h.size() + hc.size();
        }

    private:

      // associate to each blue and red state its color and its acceptance set
      hash_type h;
      // associate to each cyan state its weight and its acceptance set
      hcyan_type hc;
    };

  } // anonymous

  emptiness_check_ptr
  explicit_tau03_opt_search(const const_twa_ptr& a, option_map o)
  {
    return SPOT_make_shared_enabled__
      (tau03_opt_search<explicit_tau03_opt_search_heap>, a, 0, o);
  }

}
