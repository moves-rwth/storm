// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011, 2012, 2014-2016, 2018 Laboratoire
// de Recherche et Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <ostream>
#include <sstream>
#include <spot/twaalgos/neverclaim.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twa/twagraph.hh>
#include <spot/tl/print.hh>
#include <spot/twa/formula2bdd.hh>

using namespace std::string_literals;

namespace spot
{
  namespace
  {
    class never_claim_output final
    {
    public:
      std::ostream& os_;
      bool opt_comments_ = false;
      std::vector<std::string>* sn_ = nullptr;
      bool opt_624_ = false;
      const_twa_graph_ptr aut_;
      bool fi_needed_ = false;
      bool need_accept_all_ = false;
      unsigned accept_all_ = 0;

    public:
      never_claim_output(std::ostream& os, const char* options)
        : os_(os)
      {
        if (options)
          while (char c = *options++)
            switch (c)
              {
              case '6':
                opt_624_ = true;
                break;
              case 'c':
                opt_comments_ = true;
                break;
              default:
                throw std::runtime_error
                  ("unknown option for print_never_claim(): "s + c);
              }
      }

      void
      start() const
      {
        os_ << "never {";
        auto n = aut_->get_named_prop<std::string>("automaton-name");
        if (n)
          os_ << " /* " << *n << " */";
        os_ << '\n';
      }

      void
      end() const
      {
        if (need_accept_all_)
          {
            os_ << "accept_all:";
            print_comment(accept_all_);
            os_ << "\n  skip\n";
          }
        os_ << '}' << std::endl;
      }

      bool is_sink(unsigned n) const
      {
        auto ts = aut_->out(n);
        assert(ts.begin() != ts.end());
        auto it = ts.begin();
        return (it->cond == bddtrue) && (it->dst == n) && (++it == ts.end());
      }

      void
      print_comment(unsigned n) const
      {
        if (sn_)
          if (n < sn_->size() && !(*sn_)[n].empty())
            os_ << " /* " << (*sn_)[n] << " */";
      }

      void
      print_state(unsigned n) const
      {
        bool acc = aut_->state_is_accepting(n);
        if (n == aut_->get_init_state_number())
          {
            if (acc)
              os_ << "accept_init";
            else
              os_ << "T0_init";
          }
        else
          {
            if (!acc)
              os_ << "T0_S" << n;
            else if (is_sink(n))
              os_ << "accept_all";
            else
              os_ << "accept_S" << n;
          }
      }

      void process_state(unsigned n)
      {
        if (aut_->state_is_accepting(n) && is_sink(n)
            && n != aut_->get_init_state_number())
          {
            // We want the accept_all state at the end of the never claim.
            need_accept_all_ = true;
            accept_all_ = n;
            return;
          }

        print_state(n);
        os_ << ':';
        print_comment(n);
        os_ << (opt_624_ ? "\n  do\n" : "\n  if\n");
        bool did_output = false;
        for (auto& t: aut_->out(n))
          {
            did_output = true;
            bool atom =
              opt_624_ && aut_->state_is_accepting(t.dst) && is_sink(t.dst);
            if (atom)
              os_ << "  :: atomic { (";
            else
              os_ << "  :: (";
            formula f = bdd_to_formula(t.cond, aut_->get_dict());
            // This is actually a Boolean formula, but the LTL printer
            // is all we have.
            print_spin_ltl(os_, f, true);
            if (atom)
              {
                os_ << ") -> assert(!(";
                print_spin_ltl(os_, f, true);
                os_ << ")) }";
              }
            else
              {
                os_ << ") -> goto ";
                print_state(t.dst);
              }
            os_ << '\n';
          }
        if (!did_output)
          {
            if (opt_624_)
              {
                os_ << "  :: atomic { (false) -> assert(!(false)) }";
              }
            else
              {
                os_ << "  :: (false) -> goto ";
                print_state(n);
              }
            os_ << '\n';
          }
        os_ << (opt_624_ ? "  od;\n" : "  fi;\n");
      }

      void print(const const_twa_graph_ptr& aut)
      {
        aut_ = aut;
        if (opt_comments_)
          sn_ = aut->get_named_prop<std::vector<std::string>>("state-names");
        start();
        unsigned init = aut_->get_init_state_number();
        unsigned ns = aut_->num_states();
        process_state(init);
        for (unsigned n = 0; n < ns; ++n)
          if (n != init)
            process_state(n);
        end();
      }
    };
  } // anonymous namespace

  std::ostream&
  print_never_claim(std::ostream& os, const const_twa_ptr& g,
                        const char* options)
  {
    if (!(g->acc().is_buchi() || g->acc().is_all()))
      throw std::runtime_error
        ("Never claim output only supports Büchi acceptance");
    never_claim_output d(os, options);
    auto aut = std::dynamic_pointer_cast<const twa_graph>(g);
    if (!aut)
      aut = make_twa_graph(g, twa::prop_set::all());
    d.print(aut);
    return os;
  }
}
