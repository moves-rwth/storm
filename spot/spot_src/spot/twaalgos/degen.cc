// -*- coding: utf-8 -*-
// Copyright (C) 2012-2020 Laboratoire de Recherche
// et Développement de l'Epita (LRDE).
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
#include <spot/twaalgos/degen.hh>
#include <spot/twa/twagraph.hh>
#include <spot/misc/hash.hh>
#include <spot/misc/hashfunc.hh>
#include <deque>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/isdet.hh>

//#define DEGEN_DEBUG

namespace spot
{
  namespace
  {
    // A state in the degenalized automaton corresponds to a state in
    // the TGBA associated to a level.  The level is just an index in
    // the list of acceptance sets.
    typedef std::pair<unsigned, unsigned> degen_state;

    struct degen_state_hash
    {
      size_t
      operator()(const degen_state& s) const noexcept
      {
        return wang32_hash(s.first ^ wang32_hash(s.second));
      }
    };

    // Associate the degeneralized state to its number.
    typedef std::unordered_map<degen_state, unsigned,
                               degen_state_hash> ds2num_map;

    // Queue of state to be processed.
    typedef std::deque<degen_state> queue_t;

    // Acceptance set common to all outgoing or incoming edges (of the
    // same SCC -- we do not care about the others) of some state.
    class inout_acc final
    {
      const_twa_graph_ptr a_;
      typedef std::tuple<acc_cond::mark_t, // common out
                         acc_cond::mark_t, // union out
                         acc_cond::mark_t, // common in, then common in+out
                         bool,             // has self-loop
                         bool> cache_entry; // is true state
      std::vector<cache_entry> cache_;
      unsigned last_true_state_;
      const scc_info* sm_;

      unsigned scc_of(unsigned s) const
      {
        return sm_ ? sm_->scc_of(s) : 0;
      }

      void fill_cache(unsigned s)
      {
        unsigned s1 = scc_of(s);
        acc_cond::mark_t common = a_->acc().all_sets();
        acc_cond::mark_t union_ = {};
        bool has_acc_self_loop = false;
        bool is_true_state = false;
        bool seen = false;
        for (auto& t: a_->out(s))
          {
            // Ignore edges that leave the SCC of s.
            unsigned d = t.dst;
            if (scc_of(d) != s1)
              continue;

            common &= t.acc;
            union_ |= t.acc;
            std::get<2>(cache_[d]) &= t.acc;

            // an accepting self-loop?
            if ((t.dst == s) && a_->acc().accepting(t.acc))
              {
                has_acc_self_loop = true;
                if (t.cond == bddtrue)
                  {
                    is_true_state = true;
                    last_true_state_ = s;
                  }
              }
            seen = true;
          }
        if (!seen)
          common = {};
        std::get<0>(cache_[s]) = common;
        std::get<1>(cache_[s]) = union_;
        std::get<3>(cache_[s]) = has_acc_self_loop;
        std::get<4>(cache_[s]) = is_true_state;
      }

    public:
      inout_acc(const const_twa_graph_ptr& a, const scc_info* sm):
        a_(a), cache_(a->num_states()), sm_(sm)
      {
        unsigned n = a->num_states();
        acc_cond::mark_t all = a_->acc().all_sets();
        // slot 2 will hold acceptance mark that are common to the
        // incoming transitions of each state.  For now with all
        // marks if there is some incoming edge.  The next loop will
        // constrain this value.
        for (auto& e: a_->edges())
          if (scc_of(e.src) == scc_of(e.dst))
            std::get<2>(cache_[e.dst]) = all;
        for (unsigned s = 0; s < n; ++s)
          fill_cache(s);
        for (unsigned s = 0; s < n; ++s)
          std::get<2>(cache_[s]) |= std::get<0>(cache_[s]);
      }

      // Intersection of all outgoing acceptance sets
      acc_cond::mark_t common_out_acc(unsigned s) const
      {
        assert(s < cache_.size());
        return std::get<0>(cache_[s]);
      }

      // Union of all outgoing acceptance sets
      acc_cond::mark_t union_out_acc(unsigned s) const
      {
        assert(s < cache_.size());
        return std::get<1>(cache_[s]);
      }

      // Intersection of all incoming acceptance sets
      acc_cond::mark_t common_inout_acc(unsigned s) const
      {
        assert(s < cache_.size());
        return std::get<2>(cache_[s]);
      }

      bool has_acc_selfloop(unsigned s) const
      {
        assert(s < cache_.size());
        return std::get<3>(cache_[s]);
      }

      bool is_true_state(unsigned s) const
      {
        assert(s < cache_.size());
        return std::get<4>(cache_[s]);
      }

      unsigned last_true_state() const
      {
        return last_true_state_;
      }
    };

    // Order of accepting sets (for one SCC)
    class acc_order final
    {
      std::vector<std::pair<acc_cond::mark_t, unsigned>> found_;
      std::vector<unsigned> order_;

    public:
      acc_order(acc_cond::mark_t all)
      {
        unsigned count = all.count();
        found_.emplace_back(all, count);
        order_.insert(order_.begin(), count, 0);
      }

      unsigned
      next_level(unsigned slevel, acc_cond::mark_t set)
      {
        unsigned last = order_.size();
        if (last == 0)
          return slevel;
        unsigned index = order_[slevel];

        while (slevel < last
               && (found_[index].first & set) == found_[index].first)
          slevel += found_[index++].second;
        if (slevel == last)
          return slevel;
        if (acc_cond::mark_t match = (found_[index].first & set))
          {
            unsigned matchsize = match.count();
            found_[index].first -= match;
            found_[index].second -= matchsize;
            found_.emplace(found_.begin() + index, match, matchsize);
            slevel += matchsize;
            // update order_
            unsigned opos = 0;
            unsigned fpos = 0;
            for (auto& p: found_)
              {
                for (unsigned n = 0; n < p.second; ++n)
                  order_[opos++] = fpos;
                ++fpos;
              }
          }
        return slevel;
      }

      void
      print(int scc)
      {
        std::cout << "Order_" << scc << ":\t";
        for (auto i: found_)
          std::cout << i.first << ' ';
        std::cout << " // ";
        for (auto i: order_)
          std::cout << i << ' ';
        std::cout << '\n';
      }
    };

    // Accepting order for each SCC
    class scc_orders final
    {
      std::vector<acc_order> orders_;

    public:
      scc_orders(acc_cond::mark_t all, unsigned scc_count)
        : orders_(scc_count, acc_order(all))
      {
      }

      unsigned
      next_level(int scc, int slevel, acc_cond::mark_t set)
      {
        return orders_[scc].next_level(slevel, set);
      }

      void
      print()
      {
        unsigned sz = orders_.size();
        for (unsigned i = 0; i < sz; ++i)
          orders_[i].print(i);
      }
    };


    namespace
    {
      static void
      keep_bottommost_copies(twa_graph_ptr& res,
                             scc_info& si_orig,
                             std::vector<unsigned>* orig_states,
                             bool force_purge = false)
      {
        unsigned res_ns = res->num_states();
        auto a = si_orig.get_aut();
        if (res_ns <= a->num_states())
          return;

        scc_info si_res(res, scc_info_options::TRACK_STATES);
        unsigned res_scc_count = si_res.scc_count();
        if (res_scc_count <= si_orig.scc_count())
          {
            if (force_purge)
              res->purge_unreachable_states();
            return;
          }

        // If we reach this place, we have more SCCs in the output than
        // in the input.  This means that we have created some redundant
        // SCCs.  Often, these are trivial SCCs created in front of
        // their larger sisters, because we did not pick the correct
        // level when entering the SCC for the first time, and the level
        // we picked has not been seen again when exploring the SCC.
        // But it could also be the case that by entering the SCC in two
        // different ways, we create two clones of the SCC (I haven't
        // encountered any such case, but I do not want to rule it out
        // in the code below).
        //
        // Now we will iterate over the SCCs in topological order to
        // remember the "bottommost" SCCs that contain each original
        // state.  If an original state is duplicated in a higher SCC,
        // it can be shunted away.  Amen.
        unsigned maxorig = *std::max_element(orig_states->begin(),
                                             orig_states->end());
        std::vector<unsigned>
          bottommost_occurence(maxorig + 1);
        {
          unsigned n = res_scc_count;
          do
            for (unsigned s: si_res.states_of(--n))
              bottommost_occurence[(*orig_states)[s]] = s;
          while (n);
        }
        std::vector<unsigned> retarget(res_ns);
        for (unsigned n = 0; n < res_ns; ++n)
          {
            unsigned other = bottommost_occurence[(*orig_states)[n]];
            retarget[n] =
              (si_res.scc_of(n) != si_res.scc_of(other)) ? other : n;
          }
        for (auto& e: res->edges())
          e.dst = retarget[e.dst];
        res->set_init_state(retarget[res->get_init_state_number()]);
        res->purge_unreachable_states();
      }
    }

    template<bool want_sba>
    twa_graph_ptr
    degeneralize_aux(const const_twa_graph_ptr& a, bool use_z_lvl,
                     bool use_cust_acc_orders, int use_lvl_cache,
                     bool skip_levels, bool ignaccsl,
                     bool remove_extra_scc)
    {
      if (!a->acc().is_generalized_buchi())
        throw std::runtime_error
          ("degeneralize() only works with generalized Büchi acceptance");
      if (!a->is_existential())
        throw std::runtime_error
          ("degeneralize() does not support alternation");

      bool use_scc = (use_lvl_cache
                      || use_cust_acc_orders
                      || use_z_lvl
                      || remove_extra_scc);

      bdd_dict_ptr dict = a->get_dict();

      // The result automaton is an SBA.
      auto res = make_twa_graph(dict);
      res->copy_ap_of(a);
      res->set_buchi();
      if (want_sba)
        res->prop_state_acc(true);
      // Preserve determinism, weakness, and stutter-invariance
      res->prop_copy(a, { false, true, true, true, true, true });

      auto old_orig_states =
        a->get_named_prop<std::vector<unsigned>>("original-states");

      auto orig_states = new std::vector<unsigned>();
      auto levels = new std::vector<unsigned>();
      orig_states->reserve(a->num_states()); // likely more are needed.
      levels->reserve(a->num_states());
      res->set_named_prop("original-states", orig_states);
      res->set_named_prop("degen-levels", levels);

      // Create an order of acceptance conditions.  Each entry in this
      // vector correspond to an acceptance set.  Each index can
      // be used as a level in degen_state to indicate the next expected
      // acceptance set.  Level order.size() is a special level used to
      // denote accepting states.
      std::vector<unsigned> order;
      {
        // The order is arbitrary, but it turns out that using emplace_back
        // instead of push_front often gives better results because
        // acceptance sets at the beginning if the cycle are more often
        // used in the automaton.  (This surprising fact is probably
        // related to the order in which we declare the BDD variables
        // during the translation.)
        unsigned n = a->num_sets();
        for (unsigned i = n; i > 0; --i)
          order.emplace_back(i - 1);
      }

      // and vice-versa.
      ds2num_map ds2num;

      // This map is used to find edges that go to the same
      // destination with the same acceptance.  The integer key is
      // (dest*2+acc) where dest is the destination state number, and
      // acc is 1 iff the edge is accepting.  The source
      // is always that of the current iteration.
      typedef std::map<int, unsigned> tr_cache_t;
      tr_cache_t tr_cache;

      // State->level cache
      std::vector<std::pair<unsigned, bool>> lvl_cache(a->num_states());

      // Compute SCCs in order to use any optimization.
      std::unique_ptr<scc_info> m = use_scc
        ? std::make_unique<scc_info>(a, scc_info_options::NONE)
        : nullptr;

      // Initialize scc_orders
      std::unique_ptr<scc_orders> orders = use_cust_acc_orders
        ? std::make_unique<scc_orders>(a->acc().all_sets(), m->scc_count())
        : nullptr;

      // Cache for common outgoing/incoming acceptances.
      inout_acc inout(a, m.get());

      queue_t todo;

      degen_state s(a->get_init_state_number(), 0);

      // As a heuristic for building SBA, if the initial state has at
      // least one accepting self-loop, start the degeneralization on
      // the accepting level.
      if (want_sba && !ignaccsl && inout.has_acc_selfloop(s.first))
        s.second = order.size();
      // Otherwise, check for acceptance conditions common to all
      // outgoing edges, plus those common to all incoming edges, and
      // assume we have already seen these and start on the associated
      // level.
      if (s.second == 0)
        {
          auto set = inout.common_inout_acc(s.first);
          if (SPOT_UNLIKELY(use_cust_acc_orders))
            s.second = orders->next_level(m->initial(), s.second, set);
          else
            while (s.second < order.size() && set.has(order[s.second]))
              {
                ++s.second;
                if (!skip_levels)
                  break;
              }
          // There is no accepting level for TBA, let reuse level 0.
          if (!want_sba && s.second == order.size())
            s.second = 0;
        }

      auto new_state = [&](degen_state ds)
        {
          // Merge all true states into a single one.
          bool ts = inout.is_true_state(ds.first);
          if (ts)
            ds = {inout.last_true_state(), 0U};

          auto di = ds2num.find(ds);
          if (di != ds2num.end())
              return di->second;

          unsigned ns = res->new_state();
          ds2num[ds] = ns;
          if (ts)
            {
              res->new_acc_edge(ns, ns, bddtrue, true);
              // As we do not process all outgoing transition of
              // ds.first, it is possible that a non-deterministic
              // automaton becomes deterministic.
              if (res->prop_universal().is_false())
                res->prop_universal(trival::maybe());
            }
          else
            todo.emplace_back(ds);

          assert(ns == orig_states->size());
          unsigned orig = ds.first;
          if (old_orig_states)
            orig = (*old_orig_states)[orig];
          orig_states->emplace_back(orig);
          levels->emplace_back(ds.second);

          // Level cache stores one encountered level for each state
          // (the value of use_lvl_cache determinates which level
          // should be remembered).  This cache is used when
          // re-entering the SCC.
          if (use_lvl_cache)
            {
              unsigned lvl = ds.second;
              if (lvl_cache[ds.first].second)
                {
                  if (use_lvl_cache == 3)
                    lvl = std::max(lvl_cache[ds.first].first, lvl);
                  else if (use_lvl_cache == 2)
                    lvl = std::min(lvl_cache[ds.first].first, lvl);
                  else
                    lvl = lvl_cache[ds.first].first; // Do not change
                }
              lvl_cache[ds.first] = std::make_pair(lvl, true);
            }
          return ns;
        };
      new_state(s);

      while (!todo.empty())
        {
          s = todo.front();
          todo.pop_front();
          int src = ds2num[s];
          unsigned slevel = s.second;

          // If we have a state on the last level, it should be accepting.
          bool is_acc = slevel == order.size();
          // On the accepting level, start again from level 0.
          if (want_sba && is_acc)
            slevel = 0;

          // Check SCC for state s
          int s_scc = -1;
          if (use_scc)
            s_scc = m->scc_of(s.first);

          for (auto& i: a->out(s.first))
            {
              degen_state d(i.dst, 0);

              // Check whether the target SCC is accepting
              bool is_scc_acc;
              int scc;
              if (use_scc)
                {
                  scc = m->scc_of(d.first);
                  is_scc_acc = m->is_accepting_scc(scc);
                }
              else
                {
                  // If we have no SCC information, treat all SCCs as
                  // accepting.
                  scc = -1;
                  is_scc_acc = true;
                }

              // The old level is slevel.  What should be the new one?
              auto acc = i.acc;
              auto otheracc = inout.common_inout_acc(d.first);

              if (want_sba && is_acc)
                {
                  // Ignore the last expected acceptance set (the value of
                  // prev below) if it is common to all other outgoing
                  // edges (of the current state) AND if it is not
                  // used by any outgoing edge of the destination
                  // state.
                  //
                  // 1) It's correct to do that, because this acceptance
                  //    set is common to other outgoing edges.
                  //    Therefore if we make a cycle to this state we
                  //    will eventually see that acceptance set thanks
                  //    to the "pulling" of the common acceptance sets
                  //    of the destination state (d.first).
                  //
                  // 2) It's also desirable because it makes the
                  //    degeneralization idempotent (up to a renaming
                  //    of states).  Consider the following automaton
                  //    where 1 is initial and => marks accepting
                  //    edges: 1=>1, 1=>2, 2->2, 2->1. This is
                  //    already an SBA, with 1 as accepting state.
                  //    However if you try degeralize it without
                  //    ignoring *prev, you'll get two copies of state
                  //    2, depending on whether we reach it using 1=>2
                  //    or from 2->2.  If this example was not clear,
                  //    play with the "degenid.test" test case.
                  //
                  // 3) Ignoring all common acceptance sets would also
                  //    be correct, but it would make the
                  //    degeneralization produce larger automata in some
                  //    cases.  The current condition to ignore only one
                  //    acceptance set if is this not used by the next
                  //    state is a heuristic that is compatible with
                  //    point 2) above while not causing more states to
                  //    be generated in our benchmark of 188 formulae
                  //    from the literature.
                  if (!order.empty())
                    {
                      unsigned prev = order.size() - 1;
                      auto common = inout.common_out_acc(s.first);
                      if (common.has(order[prev]))
                        {
                          auto u = inout.union_out_acc(d.first);
                          if (!u.has(order[prev]))
                            acc -= a->acc().mark(order[prev]);
                        }
                    }
                }
              // A edge in the SLEVEL acceptance set should
              // be directed to the next acceptance set.  If the
              // current edge is also in the next acceptance
              // set, then go to the one after, etc.
              //
              // See Denis Oddoux's PhD thesis for a nice
              // explanation (in French).
              // @PhDThesis{    oddoux.03.phd,
              //   author     = {Denis Oddoux},
              //   title      = {Utilisation des automates alternants pour un
              //                model-checking efficace des logiques
              //                temporelles lin{\'e}aires.},
              //   school     = {Universit{\'e}e Paris 7},
              //   year       = {2003},
              //   address= {Paris, France},
              //   month      = {December}
              // }
              if (is_scc_acc)
                {
                  // If lvl_cache is used and switching SCCs, use level
                  // from cache
                  if (use_lvl_cache && s_scc != scc
                      && lvl_cache[d.first].second)
                    {
                      d.second = lvl_cache[d.first].first;
                    }
                  else
                    {
                      // Complete (or replace) the acceptance sets of
                      // this link with the acceptance sets common to
                      // all edges of the destination SCC entering or
                      // leaving the destination state.
                      if (s_scc == scc)
                        acc |= otheracc;
                      else
                        acc = otheracc;

                      // If use_z_lvl is on, start with level zero 0 when
                      // switching SCCs
                      unsigned next = (!use_z_lvl || s_scc == scc) ? slevel : 0;

                      // If using custom acc orders, get next level
                      // for this scc
                      if (use_cust_acc_orders)
                        {
                          d.second = orders->next_level(scc, next, acc);
                        }
                      // Else compute level according the global acc order
                      else
                        {
                          // As a heuristic, if we enter the SCC on a
                          // state that has at least one accepting
                          // self-loop, start the degeneralization on
                          // the accepting level.
                          if (s_scc != scc
                              && !ignaccsl
                              && inout.has_acc_selfloop(d.first))
                            {
                              d.second = order.size();
                            }
                          else
                            {
                              // Consider both the current acceptance
                              // sets, and the acceptance sets common
                              // to the outgoing edges of the
                              // destination state.  But don't do that
                              // if the state is accepting and we are
                              // not skipping levels.
                              if (skip_levels || !is_acc)
                                while (next < order.size()
                                       && acc.has(order[next]))
                                  {
                                    ++next;
                                    if (!skip_levels)
                                      break;
                                  }
                              d.second = next;
                            }
                        }
                    }
                }

              // In case we are building a TBA is_acc has to be
              // set differently for each edge, and
              // we do not need to stay on final level.
              if (!want_sba)
                {
                  is_acc = d.second == order.size();
                  if (is_acc)        // The edge is accepting
                    {
                      d.second = 0; // Make it go to the first level.
                      // Skip as many levels as possible.
                      if (!a->acc().accepting(acc) && skip_levels)
                        {
                          if (use_cust_acc_orders)
                            {
                              d.second = orders->next_level(scc, d.second, acc);
                            }
                          else
                            {
                              while (d.second < order.size() &&
                                     acc.has(order[d.second]))
                                ++d.second;
                            }
                        }
                    }
                }

              // Have we already seen this destination?
              int dest = new_state(d);

              unsigned& t = tr_cache[dest * 2 + is_acc];

              if (t == 0)        // Create edge.
                t = res->new_acc_edge(src, dest, i.cond, is_acc);
              else                // Update existing edge.
                res->edge_data(t).cond |= i.cond;
            }
          tr_cache.clear();
        }

#ifdef DEGEN_DEBUG
      std::cout << "Orig. order:  \t";
      for (auto i: order)
        std::cout << i << ", ";
      std::cout << '\n';
      orders->print();
#endif
      res->merge_edges();
      if (remove_extra_scc)
        keep_bottommost_copies(res, *(m.get()), orig_states);
      return res;
    }
  }

  twa_graph_ptr
  degeneralize(const const_twa_graph_ptr& a,
               bool use_z_lvl, bool use_cust_acc_orders,
               int use_lvl_cache, bool skip_levels, bool ignaccsl,
               bool remove_extra_scc)
  {
    // If this already a degeneralized digraph, there is nothing we
    // can improve.
    if (a->is_sba())
      return std::const_pointer_cast<twa_graph>(a);

    return degeneralize_aux<true>(a, use_z_lvl, use_cust_acc_orders,
                                  use_lvl_cache, skip_levels, ignaccsl,
                                  remove_extra_scc);
  }

  twa_graph_ptr
  degeneralize_tba(const const_twa_graph_ptr& a,
                   bool use_z_lvl, bool use_cust_acc_orders,
                   int use_lvl_cache, bool skip_levels, bool ignaccsl,
                   bool remove_extra_scc)
  {
    // If this already a degeneralized digraph, there is nothing we
    // can improve.
    if (a->acc().is_buchi())
      return std::const_pointer_cast<twa_graph>(a);

    return degeneralize_aux<false>(a, use_z_lvl, use_cust_acc_orders,
                                   use_lvl_cache, skip_levels, ignaccsl,
                                   remove_extra_scc);
  }

  namespace
  {
    static acc_cond::mark_t
    to_strip(const acc_cond::acc_code& code, acc_cond::mark_t todegen)
    {
      if (code.empty())
        return todegen;

      acc_cond::mark_t tostrip = todegen;
      unsigned pos = code.size();
      do
        {
          switch (code[pos - 1].sub.op)
            {
            case acc_cond::acc_op::And:
            case acc_cond::acc_op::Or:
              --pos;
              break;
            case acc_cond::acc_op::Fin:
            case acc_cond::acc_op::FinNeg:
            case acc_cond::acc_op::Inf:
            case acc_cond::acc_op::InfNeg:
              {
                pos -= 2;
                acc_cond::mark_t m = code[pos].mark;
                if (todegen.subset(m))
                  m -= todegen;
                tostrip -= m;
                break;
              }
            }
        }
      while (pos > 0);
      return tostrip;
    }

    static bool
    update_acc_for_partial_degen(acc_cond::acc_code& code,
                                 acc_cond::mark_t todegen,
                                 acc_cond::mark_t tostrip,
                                 acc_cond::mark_t accmark)
    {
      if (!todegen || code.empty())
        {
          code &= acc_cond::acc_code::inf(accmark);
          return true;
        }

      bool updated = false;
      unsigned pos = code.size();
      do
        {
          switch (code[pos - 1].sub.op)
            {
            case acc_cond::acc_op::And:
            case acc_cond::acc_op::Or:
              --pos;
              break;
            case acc_cond::acc_op::Fin:
            case acc_cond::acc_op::FinNeg:
            case acc_cond::acc_op::Inf:
            case acc_cond::acc_op::InfNeg:
              {
                pos -= 2;
                acc_cond::mark_t m = code[pos].mark;
                if (todegen.subset(m))
                  {
                    m -= todegen;
                    code[pos].mark = m.strip(tostrip) | accmark;
                    updated = true;
                  }
                else
                  {
                    code[pos].mark = m.strip(tostrip);
                  }
                break;
              }
            }
        }
      while (pos > 0);
      return updated;
    }

    [[noreturn]] static void
    report_invalid_partial_degen_arg(acc_cond::mark_t todegen,
                                     acc_cond::acc_code& cond)
    {
      std::ostringstream err;
      err << "partial_degeneralize(): " << todegen
          << " does not match any degeneralizable subformula of "
          << cond << '.';
      throw std::runtime_error(err.str());
    }
  }

  acc_cond::mark_t
  is_partially_degeneralizable(const const_twa_graph_ptr& aut,
                               bool allow_inf, bool allow_fin,
                               std::vector<acc_cond::mark_t> forbid)
  {
    auto& code = aut->get_acceptance();

    if (code.empty())
      return {};

    acc_cond::mark_t res = {};
    unsigned res_sz = -1U;
    auto update = [&](const acc_cond::mark_t& m)
    {
      unsigned sz = m.count();
      if (sz > 1 && sz < res_sz)
        {
          res_sz = sz;
          res = m;
        }
      // If we have found a pair to degeneralize, we
      // won't find
      return res_sz == 2;
    };

    unsigned pos = code.size();
    do
      {
        switch (code[pos - 1].sub.op)
          {
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            --pos;
            break;
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::FinNeg:
            pos -= 2;
            if (allow_fin)
            {
              auto m = code[pos].mark;
              if (!std::count(forbid.begin(), forbid.end(), m) && update(m))
                return res;
            }
            break;
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::InfNeg:
            pos -= 2;
            if (allow_inf)
            {
              auto m = code[pos].mark;
              if (!std::count(forbid.begin(), forbid.end(), m) && update(m))
                return res;
            }
            break;
          }
      }
    while (pos > 0);
    return res;
  }


  twa_graph_ptr
  partial_degeneralize(const const_twa_graph_ptr& a,
                       acc_cond::mark_t todegen)

  {
    auto res = make_twa_graph(a->get_dict());
    res->copy_ap_of(a);
    acc_cond::acc_code acc = a->get_acceptance();

    acc_cond::mark_t tostrip = to_strip(acc, todegen);
    acc_cond::mark_t keep = a->acc().all_sets() - tostrip;
    acc_cond::mark_t degenmark = {keep.count()};

    if (!update_acc_for_partial_degen(acc, todegen, tostrip, degenmark))
      report_invalid_partial_degen_arg(todegen, acc);
    res->set_acceptance(acc);

    // auto* names = new std::vector<std::string>;
    // res->set_named_prop("state-names", names);
    auto orig_states = new std::vector<unsigned>();
    auto levels = new std::vector<unsigned>();
    unsigned ns = a->num_states();
    orig_states->reserve(ns); // likely more are needed.
    levels->reserve(a->num_states());
    res->set_named_prop("original-states", orig_states);
    res->set_named_prop("degen-levels", levels);

    scc_info si_orig(a, scc_info_options::NONE);

    auto marks = propagate_marks_vector(a, &si_orig);

    std::vector<unsigned> highest_level(ns, 0);
    // Compute the marks that are common to all incoming or all
    // outgoing transitions of each state, ignoring self-loops and
    // out-of-SCC transitions.  Note that because
    // propagate_marks_verctor() has been used, the intersection
    // of all incoming marks is equal to the intersection of all
    // outgoing marks unless the state has no predecessor or no
    // successor.  We take the outgoing marks because states without
    // successor are useless (but states without predecessors are not).
    std::vector<acc_cond::mark_t> inout(ns, todegen);
    for (auto& e: a->edges())
      if (e.src != e.dst && si_orig.scc_of(e.src) == si_orig.scc_of(e.dst))
        {
          unsigned idx = a->edge_number(e);
          inout[e.src] &= marks[idx];
        }

    scc_orders orders(todegen, si_orig.scc_count());
    unsigned ordersize = todegen.count();

    // degen_states -> new state numbers
    ds2num_map ds2num;

    queue_t todo;

    auto new_state = [&](degen_state ds)
                     {
                       auto di = ds2num.find(ds);
                       if (di != ds2num.end())
                         return di->second;

                       highest_level[ds.first] =
                         std::max(highest_level[ds.first], ds.second);

                       unsigned ns = res->new_state();
                       ds2num[ds] = ns;
                       todo.emplace_back(ds);

                       // std::ostringstream os;
                       // os << ds.first << ',' << ds.second;
                       // names->push_back(os.str());

                       unsigned orig = ds.first;
                       assert(ns == orig_states->size());
                       orig_states->emplace_back(orig);
                       levels->emplace_back(ds.second);
                       return ns;
                     };

    unsigned init = a->get_init_state_number();
    degen_state s(init, 0);
    // The initial level can be anything.  As a heuristic, pretend we
    // have just seen the acceptance condition common to incoming
    // edges.
    s.second = orders.next_level(si_orig.scc_of(init), 0, inout[init]);
    if (s.second == ordersize)
      s.second = 0;

    new_state(s);

    // A list of edges are that "all accepting" and whose destination
    // could be redirected to a higher level.
    std::vector<unsigned> allaccedges;

    while (!todo.empty())
      {
        s = todo.back();
        todo.pop_back();

        int src = ds2num[s];
        unsigned slevel = s.second;
        unsigned orig_src = s.first;
        unsigned scc_src = si_orig.scc_of(orig_src);
        for (auto& e: a->out(orig_src))
          {
            bool saveedge = false;
            unsigned nextlvl = slevel;
            acc_cond::mark_t accepting = {};
            if (si_orig.scc_of(e.dst) == scc_src)
              {
                unsigned idx = a->edge_number(e);
                acc_cond::mark_t acc = marks[idx] & todegen;
                nextlvl = orders.next_level(scc_src, nextlvl, acc);
                if (nextlvl == ordersize)
                  {
                    accepting = degenmark;
                    nextlvl = 0;
                    if ((acc & todegen) != todegen)
                      {
                        nextlvl = orders.next_level(scc_src, nextlvl, acc);
                      }
                    else
                      {
                        // Because we have seen all sets on this
                        // transition, we can jump to any level we
                        // like.  As a heuristic, let's jump to the
                        // highest existing level.
                        nextlvl = highest_level[e.dst];
                        if (nextlvl == 0 // probably a new state ->
                                         // use inout for now
                            && inout[e.dst] != todegen)
                          nextlvl = orders.next_level(scc_src, nextlvl,
                                                      inout[e.dst]);
                        // It's possible that we have not yet seen the
                        // highest level yet, so let's save this edge
                        // and revisit this issue at the end.
                        saveedge = true;
                      }
                  }
                accepting |= e.acc.strip(tostrip);
              }
            else
              {
                nextlvl = 0;
              }

            degen_state ds_dst(e.dst, nextlvl);
            unsigned dst = new_state(ds_dst);
            unsigned idx = res->new_edge(src, dst, e.cond, accepting);
            if (saveedge)
              allaccedges.push_back(idx);
          }
      }
    // Raise the destination of the "all-accepting" edges to the
    // highest existing level.  If we do such a redirection, we need
    // to force keep_bottommost_copies to purge unreachable_states.
    bool force_purge = false;
    auto& ev = res->edge_vector();
    for (unsigned idx: allaccedges)
      {
        unsigned dst = ev[idx].dst;
        unsigned orig_dst = (*orig_states)[dst];
        unsigned hl = highest_level[orig_dst];
        unsigned new_dst = ds2num[degen_state{orig_dst, hl}];
        if (dst != new_dst)
          {
            ev[idx].dst = new_dst;
            force_purge = true;
          }
      }
    // compose original-states with the any previously existing one.
    // We do that now, because the above loop uses orig_states to
    // find the local source, but for the bottommost copy below, it's better
    // if we compose everything.
    if (auto old_orig_states =
        a->get_named_prop<std::vector<unsigned>>("original-states"))
      for (auto& s: *orig_states)
        s = (*old_orig_states)[s];
    //orders.print();
    res->merge_edges();
    keep_bottommost_copies(res, si_orig, orig_states, force_purge);
    return res;
  }

  twa_graph_ptr
  partial_degeneralize(twa_graph_ptr a)
  {
    while (acc_cond::mark_t m = is_partially_degeneralizable(a))
      a = partial_degeneralize(a, m);
    return a;
  }

  std::vector<acc_cond::mark_t>
  propagate_marks_vector(const const_twa_graph_ptr& aut,
                         const scc_info* si)
  {
    bool own_si = true;
    if (si)
      own_si = false;
    else
      si = new scc_info(aut);

    unsigned ns = aut->num_states();
    acc_cond::mark_t allm = aut->acc().all_sets();
    unsigned es = aut->edge_vector().size();
    std::vector<acc_cond::mark_t> marks(es, acc_cond::mark_t{});
    const auto& edges = aut->edge_vector();
    for (unsigned e = 1; e < es; ++e)
      marks[e] = edges[e].acc;

    std::vector<acc_cond::mark_t> common_in(ns, allm);
    std::vector<acc_cond::mark_t> common_out(ns, allm);

    for (;;)
      {
        bool changed = false;
        for (auto& e: aut->edges())
          if (e.src != e.dst && si->scc_of(e.src) == si->scc_of(e.dst))
            {
              unsigned idx = aut->edge_number(e);
              common_in[e.dst] &= marks[idx];
              common_out[e.src] &= marks[idx];
            }
        for (auto& e: aut->edges())
          if (e.src != e.dst && si->scc_of(e.src) == si->scc_of(e.dst))
            {
              unsigned idx = aut->edge_number(e);
              auto acc = marks[idx] | common_in[e.src] | common_out[e.dst];
              if (acc != marks[idx])
                {
                  marks[idx] = acc;
                  changed = true;
                }
            }
        if (!changed)
          break;
        std::fill(common_in.begin(), common_in.end(), allm);
        std::fill(common_out.begin(), common_out.end(), allm);
      }
    if (own_si)
      delete si;
    return marks;
  }

  void propagate_marks_here(twa_graph_ptr& aut, const scc_info* si)
  {
    auto marks = propagate_marks_vector(aut, si);
    for (auto& e: aut->edges())
      {
        unsigned idx = aut->edge_number(e);
        e.acc = marks[idx];
      }
  }
}
