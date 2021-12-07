// -*- coding: utf-8 -*-
// Copyright (C) 2012-2015, 2018, 2019 Laboratoire de Recherche et
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

#include "config.h"
#include <spot/twaalgos/compsusp.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/simulation.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/tl/print.hh>
#include <queue>
#include <sstream>
#include <spot/tl/environment.hh>

namespace spot
{
  namespace
  {
    typedef std::map<formula, bdd> formula_bdd_map;
    typedef std::vector<formula> vec;

    // Rewrite the suspendable subformulae "s" of an LTL formula in
    // the form Gg where "g" is an atomic proposition representing
    // "s".  At the same time, populate maps that associate "s" to "g"
    // and vice-versa.
    class ltl_suspender_visitor final
    {
    public:
      typedef std::map<formula, formula> fmap_t;
      ltl_suspender_visitor(fmap_t& g2s, fmap_t& a2o, bool oblig)
        : g2s_(g2s), a2o_(a2o), oblig_(oblig)
      {
      }

      formula
      visit(formula f)
      {
        switch (op o = f.kind())
          {
          case op::Or:
          case op::And:
            {
              vec res;
              vec oblig;
              vec susp;
              unsigned mos = f.size();
              for (unsigned i = 0; i < mos; ++i)
                {
                  formula c = f[i];
                  if (c.is_boolean())
                    res.emplace_back(c);
                  else if (oblig_ && c.is_syntactic_obligation())
                    oblig.emplace_back(c);
                  else if (c.is_eventual() && c.is_universal())
                    susp.emplace_back(c);
                  else
                    res.emplace_back(recurse(c));
                }
              if (!oblig.empty())
                {
                  res.emplace_back(recurse(formula::multop(o, oblig)));
                }
              if (!susp.empty())
                {
                  formula x = formula::multop(o, susp);
                  // Rewrite 'x' as 'G"x"'
                  formula g = recurse(x);
                  if (o == op::And)
                    {
                      res.emplace_back(g);
                    }
                  else
                    {
                      // res || susp -> (res && G![susp]) || G[susp])
                      auto r = formula::multop(o, res);
                      auto gn = formula::G(formula::Not(g[0]));
                      return formula::Or({formula::And({r, gn}), g});
                    }
                }
              return formula::multop(o, res);
            }
            break;
          default:
            return f.map([this](formula f)
                         {
                           return this->recurse(f);
                         });
          }
      }

      formula
      recurse(formula f)
      {
        formula res;
        if (f.is_boolean())
          return f;
        if (oblig_ && f.is_syntactic_obligation())
          {
            fmap_t::const_iterator i = assoc_.find(f);
            if (i != assoc_.end())
              return i->second;

            std::ostringstream s;
            print_psl(s << "〈", f) << "〉";
            res = formula::ap(s.str());
            a2o_[res] = f;
            assoc_[f] = res;
            return res;
          }
        if (f.is_eventual() && f.is_universal())
          {
            fmap_t::const_iterator i = assoc_.find(f);
            if (i != assoc_.end())
              return formula::G(i->second);

            std::ostringstream s;
            print_psl(s << '[', f) << "]$";
            res = formula::ap(s.str());
            g2s_[res] = f;
            assoc_[f] = res;
            return formula::G(res);
          }
        return visit(f);
      }

    private:
      fmap_t& g2s_;
      fmap_t assoc_;                // This one is only needed by the visitor.
      fmap_t& a2o_;
      bool oblig_;
    };


    typedef std::pair<const state*, const state*> state_pair;

    typedef std::map<state_pair, unsigned> pair_map;
    typedef std::deque<state_pair> pair_queue;

    static
    twa_graph_ptr
    susp_prod(const const_twa_ptr& left, formula f, bdd v)
    {
      bdd_dict_ptr dict = left->get_dict();
      auto right =
        iterated_simulations(scc_filter(ltl_to_tgba_fm(f, dict, true, true),
                                        false));

      twa_graph_ptr res = make_twa_graph(dict);
      {
        // Copy all atomic propositions, except the one corresponding
        // to the variable v used for synchronization.
        int vn = bdd_var(v);
        assert(dict->bdd_map[vn].type == bdd_dict::var);
        formula vf = dict->bdd_map[vn].f;
        for (auto a: left->ap())
          if (a != vf)
            res->register_ap(a);
        for (auto a: right->ap())
          if (a != vf)
            res->register_ap(a);
      }

      unsigned lsets = left->num_sets();
      res->set_generalized_buchi(lsets + right->num_sets());

      acc_cond::mark_t radd = right->acc().all_sets();

      pair_map seen;
      pair_queue todo;

      state_pair p(left->get_init_state(), nullptr);
      const state* ris = right->get_init_state();
      p.second = ris;
      unsigned i = res->new_state();
      seen[p] = i;
      todo.emplace_back(p);
      res->set_init_state(i);

      while (!todo.empty())
        {
          p = todo.front();
          todo.pop_front();
          const state* ls = p.first;
          const state* rs = p.second;
          int src = seen[p];

          for (auto li: left->succ(ls))
            {
              state_pair d(li->dst(), ris);
              bdd lc = li->cond();

              twa_succ_iterator* ri = nullptr;
              // Should we reset the right automaton?
              if ((lc & v) == lc)
                {
                  // No.
                  ri = right->succ_iter(rs);
                  ri->first();
                }
              // Yes.  Reset the right automaton.
              else
                {
                  p.second = ris;
                }

              // This loops over all the right edges
              // if RI is defined.  Otherwise this just makes
              // one iteration as if the right automaton was
              // looping in state 0 with "true".
              while (!ri || !ri->done())
                {
                  bdd cond = lc;
                  acc_cond::mark_t racc = radd;
                  if (ri)
                    {
                      cond = lc & ri->cond();
                      // Skip incompatible edges.
                      if (cond == bddfalse)
                        {
                          ri->next();
                          continue;
                        }
                      d.second = ri->dst();
                      racc = ri->acc();
                    }

                  int dest;
                  pair_map::const_iterator i = seen.find(d);
                  if (i != seen.end()) // Is this an existing state?
                    {
                      dest = i->second;
                    }
                  else
                    {
                      dest = res->new_state();
                      seen[d] = dest;
                      todo.emplace_back(d);
                    }

                  acc_cond::mark_t a = li->acc() | (racc << lsets);
                  res->new_edge(src, dest, bdd_exist(cond, v), a);

                  if (ri)
                    ri->next();
                  else
                    break;
                }
              if (ri)
                right->release_iter(ri);
            }
        }
      return res;
    }
  }


  twa_graph_ptr
  compsusp(formula f, const bdd_dict_ptr& dict,
           bool no_wdba, bool no_simulation,
           bool early_susp, bool no_susp_product, bool wdba_smaller,
           bool oblig)
  {
    ltl_suspender_visitor::fmap_t g2s;
    ltl_suspender_visitor::fmap_t a2o;
    ltl_suspender_visitor v(g2s, a2o, oblig);
    formula g = v.recurse(f);

    // Translate the patched formula, and remove useless SCCs.
    twa_graph_ptr res =
      scc_filter(ltl_to_tgba_fm(g, dict, true, true, false, false,
                                nullptr, nullptr),
                 false);

    if (!no_wdba)
      {
        twa_graph_ptr min = minimize_obligation(res, g,
                                                nullptr, wdba_smaller);
        if (min != res)
          {
            res = min;
            no_simulation = true;
          }
      }

    if (!no_simulation)
      res = iterated_simulations(res);

    // Create a map of suspended formulae to BDD variables.
    spot::formula_bdd_map susp;
    for (auto& it: g2s)
      {
        auto j = dict->var_map.find(it.first);
        // If no BDD variable of this suspended formula exist in the
        // BDD dict, it means the suspended subformulae was never
        // actually used in the automaton.  Just skip it.  FIXME: It
        // would be better if we had a way to check that the variable
        // is used in this automaton, and not in some automaton
        // (sharing the same dictionary.)
        if (j != dict->var_map.end())
          susp[it.second] = bdd_ithvar(j->second);
      }

    // Remove suspendable formulae from non-accepting SCCs.
    bdd suspvars = bddtrue;
    for (formula_bdd_map::const_iterator i = susp.begin();
         i != susp.end(); ++i)
      suspvars &= i->second;

    bdd allaccap = bddtrue; // set of atomic prop used in accepting SCCs.
    {
      scc_info si(res);

      // Restrict suspvars to the set of suspension labels that occur
      // in accepting SCC.
      unsigned sn = si.scc_count();
      for (unsigned n = 0; n < sn; n++)
        if (si.is_accepting_scc(n))
          allaccap &= si.scc_ap_support(n);

      bdd ignored = bdd_exist(suspvars, allaccap);
      suspvars = bdd_existcomp(suspvars, allaccap);
      res = scc_filter_susp(res, false, suspvars, ignored, early_susp, &si);
    }

    // Do we need to synchronize any suspended formula?
    if (!susp.empty() && !no_susp_product)
      for (formula_bdd_map::const_iterator i = susp.begin();
           i != susp.end(); ++i)
        if ((allaccap & i->second) == allaccap)
          res = susp_prod(res, i->first, i->second);

    return res;
  }
}
