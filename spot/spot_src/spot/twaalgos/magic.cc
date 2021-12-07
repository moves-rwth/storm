// -*- coding: utf-8 -*-
// Copyright (C) 2011, 2013-2020  Laboratoire de recherche et
// développement de l'Epita (LRDE).
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
#include <spot/twaalgos/magic.hh>
#include <spot/twaalgos/ndfs_result.hxx>

namespace spot
{
  namespace
  {
    enum color {WHITE, BLUE, RED};

    /// \brief Emptiness checker on spot::tgba automata having at most one
    /// acceptance condition (i.e. a TBA).
    template <typename heap>
    class magic_search_ final : public emptiness_check, public ec_statistics
    {
    public:
      /// \brief Initialize the Magic Search algorithm on the automaton \a a
      ///
      /// \pre The automaton \a a must have at most one acceptance
      /// condition (i.e. it is a TBA).
      magic_search_(const const_twa_ptr& a, size_t size,
                    option_map o = option_map())
        : emptiness_check(a, o),
          h(size)
      {
        if (!(a->prop_weak().is_true()
              || a->num_sets() == 0
              || a->acc().is_buchi()))
          throw std::runtime_error
            ("magic_search requires Büchi or weak automata");
      }

      virtual ~magic_search_()
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
      /// \return non null pointer iff the algorithm has found a
      /// new accepting path.
      ///
      /// check() can be called several times (until it returns a null
      /// pointer) to enumerate all the visited accepting paths. The method
      /// visits only a finite set of accepting paths.
      virtual emptiness_check_result_ptr check() override
      {
        auto t = std::static_pointer_cast<magic_search_>
          (this->emptiness_check::shared_from_this());
        if (st_red.empty())
          {
            assert(st_blue.empty());
            const state* s0 = a_->get_init_state();
            inc_states();
            h.add_new_state(s0, BLUE);
            push(st_blue, s0, bddfalse, {});
            if (dfs_blue())
              return std::make_shared<magic_search_result>(t, options());
          }
        else
          {
            h.pop_notify(st_red.front().s);
            pop(st_red);
            if (!st_red.empty() && dfs_red())
              return std::make_shared<magic_search_result>(t, options());
            else
              if (dfs_blue())
                return std::make_shared<magic_search_result>(t, options());
          }
        return nullptr;
      }

      virtual std::ostream& print_stats(std::ostream &os) const override
      {
        os << states() << " distinct nodes visited" << std::endl;
        os << transitions() << " transitions explored" << std::endl;
        os << max_depth() << " nodes for the maximal stack depth" << std::endl;
        if (!st_red.empty())
          {
            assert(!st_blue.empty());
            os << st_blue.size() + st_red.size() - 1
               << " nodes for the counter example" << std::endl;
          }
        return os;
      }

      virtual bool safe() const override
      {
        return heap::Safe;
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

      /// State targeted by the red dfs.
      const state* target;

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
                typename heap::color_ref c = h.get_color_ref(s_prime);
                if (c.is_white())
                  {
                    trace << "  It is white, go down" << std::endl;
                    inc_states();
                    h.add_new_state(s_prime, BLUE);
                    push(st_blue, s_prime, label, acc);
                  }
                else
                  {
                    if (a_->acc().accepting(acc) && c.get_color() != RED)
                      {
                        // the test 'c.get_color() != RED' is added to limit
                        // the number of runs reported by successive
                        // calls to the check method. Without this
                        // functionnality, the test can be ommited.
                        trace << "  It is blue and the arc is "
                              << "accepting, start a red dfs" << std::endl;
                        target = f.s;
                        c.set_color(RED);
                        push(st_red, s_prime, label, acc);
                        if (dfs_red())
                          return true;
                      }
                    else
                      {
                        trace << "  It is blue or red, pop it" << std::endl;
                        h.pop_notify(s_prime);
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
                typename heap::color_ref c = h.get_color_ref(f_dest.s);
                assert(!c.is_white());
                if (!st_blue.empty() &&
                    a_->acc().accepting(f_dest.acc) && c.get_color() != RED)
                  {
                    // the test 'c.get_color() != RED' is added to limit
                    // the number of runs reported by successive
                    // calls to the check method. Without this
                    // functionnality, the test can be ommited.
                    trace << "  It is blue and the arc from "
                          << a_->format_state(st_blue.front().s)
                          << " to it is accepting, start a red dfs"
                          << std::endl;
                    target = st_blue.front().s;
                    c.set_color(RED);
                    push(st_red, f_dest.s, f_dest.label, f_dest.acc);
                    if (dfs_red())
                      return true;
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

      bool dfs_red()
      {
        assert(!st_red.empty());
        if (target->compare(st_red.front().s) == 0)
          return true;

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
                typename heap::color_ref c = h.get_color_ref(s_prime);
                if (c.is_white())
                  {
                    // If the red dfs find a white here, it must have crossed
                    // the blue stack and the target must be reached soon.
                    // Notice that this property holds only for explicit search.
                    // Collisions in bit-state hashing search can also lead
                    // to the visit of white state. Anyway, it is not necessary
                    // to visit white states either if a cycle can be missed
                    // with bit-state hashing search.
                    trace << "  It is white, pop it" << std::endl;
                    s_prime->destroy();
                  }
                else if (c.get_color() == BLUE)
                  {
                    trace << "  It is blue, go down" << std::endl;
                    c.set_color(RED);
                    push(st_red, s_prime, label, acc);
                    if (target->compare(s_prime) == 0)
                      return true;
                  }
                else
                  {
                    trace << "  It is red, pop it" << std::endl;
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
        return false;
      }

      class result_from_stack final: public emptiness_check_result,
        public acss_statistics
      {
      public:
        result_from_stack(std::shared_ptr<magic_search_> ms)
          : emptiness_check_result(ms->automaton()), ms_(ms)
        {
        }

        virtual twa_run_ptr accepting_run() override
        {
          assert(!ms_->st_blue.empty());
          assert(!ms_->st_red.empty());

          auto run = std::make_shared<twa_run>(automaton());

          typename stack_type::const_reverse_iterator i, j, end;
          twa_run::steps* l;

          l = &run->prefix;

          i = ms_->st_blue.rbegin();
          end = ms_->st_blue.rend(); --end;
          j = i; ++j;
          for (; i != end; ++i, ++j)
            {
              twa_run::step s = { i->s->clone(), j->label, j->acc };
              l->emplace_back(s);
            }

          l = &run->cycle;

          j = ms_->st_red.rbegin();
          twa_run::step s = { i->s->clone(), j->label, j->acc };
          l->emplace_back(s);

          i = j; ++j;
          end = ms_->st_red.rend(); --end;
          for (; i != end; ++i, ++j)
            {
              twa_run::step s = { i->s->clone(), j->label, j->acc };
              l->emplace_back(s);
            }

          return run;
        }

        virtual unsigned acss_states() const override
        {
          return 0;
        }
      private:
        std::shared_ptr<magic_search_> ms_;
      };

#     define FROM_STACK "ar:from_stack"

      class magic_search_result final: public emptiness_check_result
      {
      public:
        magic_search_result(const std::shared_ptr<magic_search_>& m,
                            option_map o = option_map())
          : emptiness_check_result(m->automaton(), o), ms(m)
        {
          if (options()[FROM_STACK])
            computer = new result_from_stack(ms);
          else
            computer = new ndfs_result<magic_search_<heap>, heap>(ms);
        }

        virtual void options_updated(const option_map& old) override
        {
          if (old[FROM_STACK] && !options()[FROM_STACK])
            {
              delete computer;
              computer = new ndfs_result<magic_search_<heap>, heap>(ms);
            }
          else if (!old[FROM_STACK] && options()[FROM_STACK])
            {
              delete computer;
              computer = new result_from_stack(ms);
            }
        }

        virtual ~magic_search_result()
        {
          delete computer;
        }

        virtual twa_run_ptr accepting_run() override
        {
          return computer->accepting_run();
        }

        virtual const unsigned_statistics* statistics() const override
        {
          return computer->statistics();
        }

      private:
        emptiness_check_result* computer;
        std::shared_ptr<magic_search_> ms;
      };
    };

    class explicit_magic_search_heap final
    {
    public:
      enum { Safe = 1 };

      class color_ref final
      {
      public:
        color_ref(color* c) :p(c)
        {
        }
        color get_color() const
        {
          return *p;
        }
        void set_color(color c)
        {
          assert(!is_white());
          *p=c;
        }
        bool is_white() const
        {
          return !p;
        }
      private:
        color *p;
      };

      explicit_magic_search_heap(size_t)
        {
        }

      ~explicit_magic_search_heap()
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
            return color_ref(nullptr);
          if (s != it->first)
            {
              s->destroy();
              s = it->first;
            }
          return color_ref(&it->second);
        }

      void add_new_state(const state* s, color c)
        {
          assert(h.find(s) == h.end());
          h.emplace(s, c);
        }

      void pop_notify(const state*) const
        {
        }

      bool has_been_visited(const state* s) const
        {
          auto it = h.find(s);
          return (it != h.end());
        }

      enum { Has_Size = 1 };
      int size() const
        {
          return h.size();
        }

    private:
      state_map<color> h;
    };

    class bsh_magic_search_heap final
    {
    public:
      enum { Safe = 0 };

      class color_ref final
      {
      public:
        color_ref(unsigned char *b, unsigned char o): base(b), offset(o*2)
          {
          }
        color get_color() const
          {
            return color(((*base) >> offset) & 3U);
          }
        void set_color(color c)
          {
            *base =  (*base & ~(3U << offset)) | (c << offset);
          }
        bool is_white() const
          {
            return get_color() == WHITE;
          }
      private:
        unsigned char *base;
        unsigned char offset;
      };

      bsh_magic_search_heap(size_t s)
        {
          size_ = s;
          h = new unsigned char[size_];
          memset(h, WHITE, size_);
        }

      ~bsh_magic_search_heap()
        {
          delete[] h;
        }

      color_ref get_color_ref(const state*& s)
        {
          size_t ha = s->hash();
          return color_ref(&(h[ha%size_]), ha%4);
        }

      void add_new_state(const state* s, color c)
        {
          color_ref cr(get_color_ref(s));
          assert(cr.is_white());
          cr.set_color(c);
        }

      void pop_notify(const state* s) const
        {
          s->destroy();
        }

      bool has_been_visited(const state* s) const
        {
          size_t ha = s->hash();
          return color((h[ha%size_] >> ((ha%4)*2)) & 3U) != WHITE;
        }

      enum { Has_Size = 0 };

    private:
      size_t size_;
      unsigned char* h;
    };

  } // anonymous

  emptiness_check_ptr
  explicit_magic_search(const const_twa_ptr& a, option_map o)
  {
    return
      SPOT_make_shared_enabled__(magic_search_<explicit_magic_search_heap>,
                                 a, 0, o);
  }

  emptiness_check_ptr
  bit_state_hashing_magic_search(const const_twa_ptr& a,
                                 size_t size, option_map o)
  {
    return
      SPOT_make_shared_enabled__(magic_search_<bsh_magic_search_heap>,
                                 a, size, o);
  }

  emptiness_check_ptr
  magic_search(const const_twa_ptr& a, option_map o)
  {
    size_t size = o.get("bsh");
    if (size)
      return bit_state_hashing_magic_search(a, size, o);
    return explicit_magic_search(a, o);
  }

}
