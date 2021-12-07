// -*- coding: utf-8 -*-
// Copyright (C) 2011-2016, 2018 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003-2005 Laboratoire d'Informatique de Paris 6
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

#include "config.h"
#include <spot/twaalgos/lbtt.hh>
#include <map>
#include <string>
#include <ostream>
#include <spot/twa/formula2bdd.hh>
#include <spot/twaalgos/reachiter.hh>
#include <spot/misc/bddlt.hh>
#include "spot/priv/accmap.hh"
#include <spot/tl/print.hh>

using namespace std::string_literals;

namespace spot
{
  namespace
  {
    class lbtt_bfs final : public twa_reachable_iterator_breadth_first
    {
    public:
      lbtt_bfs(const const_twa_ptr& a, std::ostream& os, bool sba_format)
        : twa_reachable_iterator_breadth_first(a),
          os_(os),
          sba_format_(sba_format),
          sba_(nullptr)
      {
        // Check if the automaton can be converted into a
        // twa_graph. This makes the state_is_accepting() function
        // more efficient.
        if (a->is_sba())
          sba_ = std::dynamic_pointer_cast<const twa_graph>(a);
      }

      acc_cond::mark_t
      state_acc_sets(const state* s) const
      {
        // If the automaton has a SBA type, it's easier to just query the
        // state_is_accepting() method.
        if (sba_)
          return sba_->state_acc_sets(sba_->state_number(s));

        // Otherwise, since we are dealing with a degeneralized
        // automaton nonetheless, the transitions leaving an accepting
        // state are either all accepting, or all non-accepting.  So
        // we just check the acceptance of the first transition.  This
        // is not terribly efficient since we have to create the
        // iterator.
        twa_succ_iterator* it = aut_->succ_iter(s);
        acc_cond::mark_t res = {};
        if (it->first())
          res = it->acc();
        aut_->release_iter(it);
        return res;
      }


      void
      process_state(const state* s, int n, twa_succ_iterator*) override
      {
        --n;
        if (n == 0)
          body_ << "0 1";
        else
          body_ << "-1\n" << n << " 0";
        // Do we have state-based acceptance?
        if (sba_format_)
          {
            for (auto i: state_acc_sets(s).sets())
              body_ << ' ' << i;
            body_ << " -1";
          }
        body_ << '\n';
      }

      void
      process_link(const state*, int,
                   const state*, int out, const twa_succ_iterator* si) override
      {
        body_ << out - 1 << ' ';
        if (!sba_format_)
          {
            for (auto s: si->acc().sets())
              body_ << s << ' ';
            body_ << "-1 ";
          }
        print_lbt_ltl(body_, bdd_to_formula(si->cond(),
                                            aut_->get_dict())) << '\n';
      }

      void
      end() override
      {
        os_ << seen.size() << ' ';
        if (sba_format_)
          os_ << aut_->num_sets();
        else
          os_ << aut_->num_sets() << 't';
        os_ << '\n' << body_.str() << "-1" << std::endl;
      }

    private:
      std::ostream& os_;
      std::ostringstream body_;
      bdd all_acc_conds_;
      bool sba_format_;
      const_twa_graph_ptr sba_;
    };
  }

  std::ostream&
  print_lbtt(std::ostream& os, const const_twa_ptr& g, const char* opt)
  {
    if (!g->acc().is_generalized_buchi())
      throw std::runtime_error
        ("LBTT only supports generalized Büchi acceptance");

    bool sba = g->prop_state_acc().is_true();
    if (opt)
      switch (char c = *opt++)
        {
        case 't':
          sba = false;
          break;
        default:
          throw std::runtime_error("unknown option for print_lbtt(): "s + c);
        }

    lbtt_bfs b(g, os, sba);
    b.run();
    return os;
  }
}
