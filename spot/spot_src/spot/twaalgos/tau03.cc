// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2020  Laboratoire de Recherche et
// Developpement de l'Epita (LRDE).
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

/// FIXME: Add
/// - a bit-state hashing version.

//#define TRACE

#include "config.h"
#include <iostream>
#ifdef TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

#include <cassert>
#include <list>
#include <spot/misc/hash.hh>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>
#include <spot/twaalgos/tau03.hh>
#include <spot/twaalgos/ndfs_result.hxx>

namespace spot
{
  namespace
  {
    enum color {WHITE, BLUE};

    /// \brief Emptiness checker on spot::tgba automata having at most one
    /// acceptance condition (i.e. a TBA).
    template <typename heap>
    class tau03_search final : public emptiness_check, public ec_statistics
    {
    public:
      /// \brief Initialize the search algorithm on the automaton \a a
      tau03_search(const const_twa_ptr a, size_t size, option_map o)
        : emptiness_check(a, o),
          h(size)
      {
        if (!(a->num_sets() > 0 && a->acc().is_generalized_buchi()))
          throw std::runtime_error
            ("tau03 requires generalized Büchi with at least one set");
      }

      virtual ~tau03_search()
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

      /// \brief Perform a Magic Search.
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
        h.add_new_state(s0, BLUE);
        push(st_blue, s0, bddfalse, {});
        auto t = std::static_pointer_cast<tau03_search>
          (this->emptiness_check::shared_from_this());
        if (dfs_blue())
          return std::make_shared<ndfs_result<tau03_search<heap>, heap>>(t);
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

      /// \brief Stack of the blue dfs.
      stack_type st_blue;

      /// \brief Stack of the red dfs.
      stack_type st_red;

      /// \brief Map where each visited state is colored
      /// by the last dfs visiting it.
      heap h;

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
                if (SPOT_UNLIKELY(label == bddfalse))
                  continue;
                typename heap::color_ref c_prime = h.get_color_ref(s_prime);
                if (c_prime.is_white())
                  {
                    trace << "  It is white, go down" << std::endl;
                    inc_states();
                    h.add_new_state(s_prime, BLUE);
                    push(st_blue, s_prime, label, acc);
                  }
                else
                  {
                    trace << "  It is blue, pop it" << std::endl;
                    h.pop_notify(s_prime);
                  }
              }
            else
            // Backtrack the edge
            //        (predecessor of f.s in st_blue, <f.label, f.acc>, f.s)
              {
                trace << "  All the successors have been visited"
                      << ", rescan this successors"
                      << std::endl;
                typename heap::color_ref c = h.get_color_ref(f.s);
                assert(!c.is_white());
                for (auto i: a_->succ(f.s))
                  {
                   inc_transitions();
                   const state *s_prime = i->dst();
                   bdd label = i->cond();
                   auto acc = i->acc();
                   if (SPOT_UNLIKELY(label == bddfalse))
                     continue;
                   trace << "DFS_BLUE rescanning the arc from "
                         << a_->format_state(f.s) << "  to "
                         << a_->format_state(s_prime) << std::endl;
                    typename heap::color_ref c_prime = h.get_color_ref(s_prime);
                    assert(!c_prime.is_white());
                    auto acu = acc | c.get_acc();
                    if ((c_prime.get_acc() & acu) != acu)
                      {
                        trace << "  a propagation is needed, go down"
                              << std::endl;
                        c_prime.cumulate_acc(acu);
                        push(st_red, s_prime, label, acc);
                        dfs_red(acu);
                     }
                  }
                if (a_->acc().accepting(c.get_acc()))
                  {
                    trace << "DFS_BLUE propagation is successful, report a"
                          << " cycle" << std::endl;
                    return true;
                  }
                else
                  {
                    trace << "DFS_BLUE propagation is unsuccessful, pop it"
                          << std::endl;
                    h.pop_notify(f.s);
                    pop(st_blue);
                 }
              }
          }
        return false;
      }

      void dfs_red(acc_cond::mark_t acu)
      {
        assert(!st_red.empty());

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
                if (SPOT_UNLIKELY(label == bddfalse))
                  continue;
                typename heap::color_ref c_prime = h.get_color_ref(s_prime);
                if (c_prime.is_white())
                  {
                    trace << "  It is white, pop it" << std::endl;
                    s_prime->destroy();
                  }
                else if ((c_prime.get_acc() & acu) != acu)
                  {
                    trace << "  It is blue and propagation "
                          << "is needed, go down" << std::endl;
                    c_prime.cumulate_acc(acu);
                    push(st_red, s_prime, label, acc);
                  }
                else
                  {
                    trace << "  It is blue and no propagation "
                          << "is needed, pop it" << std::endl;
                    h.pop_notify(s_prime);
                  }
              }
            else // Backtrack
              {
                trace << "  All the successors have been visited, pop it"
                      << std::endl;
                h.pop_notify(f.s);
                pop(st_red);
              }
          }
      }

    };

    class explicit_tau03_search_heap final
    {
    public:
      class color_ref final
      {
      public:
        color_ref(color* c, acc_cond::mark_t* a) :p(c), acc(a)
          {
          }
        color get_color() const
          {
            return *p;
          }
        void set_color(color c)
          {
            assert(!is_white());
            *p = c;
          }
        acc_cond::mark_t get_acc() const
          {
            SPOT_ASSUME(!is_white());
            return *acc;
          }
        void cumulate_acc(acc_cond::mark_t a)
          {
            assert(!is_white());
            *acc |= a;
          }
        bool is_white() const
          {
            return !p;
          }
      private:
        color *p;
        acc_cond::mark_t* acc;
      };

      explicit_tau03_search_heap(size_t)
        {
        }

      ~explicit_tau03_search_heap()
        {
          auto s = h.begin();
          while (s != h.end())
            {
              // Advance the iterator before deleting the "key" pointer.
              const state* ptr = s->first;
              ++s;
              ptr->destroy();
            }
        }

      color_ref get_color_ref(const state*& s)
        {
          auto it = h.find(s);
          if (it == h.end())
            return color_ref(nullptr, nullptr);
          if (s != it->first)
            {
              s->destroy();
              s = it->first;
            }
          return color_ref(&it->second.first, &it->second.second);
        }

      void add_new_state(const state* s, color c)
        {
          assert(h.find(s) == h.end());
          h.emplace(std::piecewise_construct,
                    std::forward_as_tuple(s),
                    std::forward_as_tuple(c, acc_cond::mark_t({})));
        }

      void pop_notify(const state*) const
        {
        }

      bool has_been_visited(const state* s) const
        {
          return h.find(s) != h.end();
        }

      enum { Has_Size = 1 };
      int size() const
        {
          return h.size();
        }
    private:
      state_map<std::pair<color, acc_cond::mark_t>> h;
    };

  } // anonymous

  emptiness_check_ptr
  explicit_tau03_search(const const_twa_ptr& a, option_map o)
  {
    return
      SPOT_make_shared_enabled__(tau03_search<explicit_tau03_search_heap>,
                                 a, 0, o);
  }

}
