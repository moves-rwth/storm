// -*- coding: utf-8 -*-
// Copyright (C) 2009-2018 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  namespace
  {
    // BDD.id -> Acc number
    typedef std::map<int, unsigned> accremap_t;
    typedef std::vector<accremap_t> remap_table_t;

    typedef std::tuple<bool, bdd, acc_cond::mark_t> filtered_trans;


    // SCC filters are objects with two methods:
    //  state(src) return true iff s should be kept
    //  trans(src, dst, cond, acc) returns a triplet
    //     (keep, cond2, acc2) where keep is a Boolean stating if the
    //                      edge should be kept, and cond2/acc2
    //                      give replacement values for cond/acc
    struct id_filter
    {
      scc_info* si;
      id_filter(scc_info* si)
        : si(si)
      {
      }

      // Accept all states
      bool state(unsigned)
      {
        return true;
      }

      void fix_acceptance(const twa_graph_ptr& out)
      {
        out->copy_acceptance_of(this->si->get_aut());
      }

      // Accept all edges, unmodified
      filtered_trans trans(unsigned, unsigned, bdd cond, acc_cond::mark_t acc)
      {
        return filtered_trans{true, cond, acc};
      }
    };

    // Remove useless states.
    template <class next_filter = id_filter>
    struct state_filter: next_filter
    {
      template<typename... Args>
      state_filter(scc_info* si, Args&&... args)
        : next_filter(si, std::forward<Args>(args)...)
      {
      }

      bool state(unsigned s)
      {
        return this->next_filter::state(s) && this->si->is_useful_state(s);
      }
    };

    // Suspension filter, used only by compsusp.cc
    template <class next_filter = id_filter>
    struct susp_filter: next_filter
    {
      bdd suspvars;
      bdd ignoredvars;
      bool early_susp;

      template<typename... Args>
      susp_filter(scc_info* si,
                  bdd suspvars, bdd ignoredvars, bool early_susp,
                  Args&&... args)
        : next_filter(si, std::forward<Args>(args)...),
          suspvars(suspvars),
          ignoredvars(ignoredvars),
          early_susp(early_susp)
      {
      }

      filtered_trans trans(unsigned src, unsigned dst,
                           bdd cond, acc_cond::mark_t acc)
      {
        bool keep;
        std::tie(keep, cond, acc) =
          this->next_filter::trans(src, dst, cond, acc);

        if (keep)
          {
            // Always remove ignored variables
            cond = bdd_exist(cond, ignoredvars);

            // Remove the suspension variables only if
            // the destination in a rejecting SCC,
            // or if we are between SCC with early_susp unset.
            unsigned u = this->si->scc_of(dst);
            if (this->si->is_rejecting_scc(u)
                || (!early_susp && (u != this->si->scc_of(src))))
              cond = bdd_exist(cond, suspvars);
          }

        return filtered_trans(keep, cond, acc);
      }
    };

    // Transform inherently weak automata into weak Büchi automata.
    template <bool buchi, class next_filter = id_filter>
    struct weak_filter: next_filter
    {
      acc_cond::mark_t acc_m = {0};
      acc_cond::mark_t rej_m = {};

      template<typename... Args>
      weak_filter(scc_info* si, Args&&... args)
        : next_filter(si, std::forward<Args>(args)...)
      {
        if (!buchi)
          {
            acc_m = {};
            if (si->get_aut()->acc().is_co_buchi())
              rej_m = {0};
          }
      }

      filtered_trans trans(unsigned src, unsigned dst,
                           bdd cond, acc_cond::mark_t acc)
      {

        bool keep;
        std::tie(keep, cond, acc) =
          this->next_filter::trans(src, dst, cond, acc);

        if (keep)
          {
            unsigned ss = this->si->scc_of(src);
            if (this->si->is_accepting_scc(ss))
              acc = acc_m;
            else
              acc = rej_m;
          }
        return filtered_trans(keep, cond, acc);
      }

      void fix_acceptance(const twa_graph_ptr& out)
      {
        if (buchi)
          out->set_buchi();
        else
          out->copy_acceptance_of(this->si->get_aut());
      }
    };

    // Remove acceptance conditions from all edges outside of
    // non-accepting SCCs.  If "RemoveAll" is false, keep those on
    // transitions entering accepting SCCs.  If "PreserveSBA", is set
    // only touch a transition if all its neighbor siblings can be
    // touched as well.
    template <bool RemoveAll, bool PreserveSBA, class next_filter = id_filter>
    struct acc_filter_mask: next_filter
    {
      acc_cond::mark_t accmask;

      template<typename... Args>
      acc_filter_mask(scc_info* si, Args&&... args)
        : next_filter(si, std::forward<Args>(args)...)
      {
        acc_cond::mark_t fin;
        acc_cond::mark_t inf;
        std::tie(inf, fin) =
          si->get_aut()->acc().get_acceptance().used_inf_fin_sets();
        // If an SCC is rejecting, we can mask all the sets that are
        // used only as Inf in the acceptance.
        accmask = ~(inf - fin);
      }

      filtered_trans trans(unsigned src, unsigned dst,
                           bdd cond, acc_cond::mark_t acc)
      {
        bool keep;
        std::tie(keep, cond, acc) =
          this->next_filter::trans(src, dst, cond, acc);

        if (keep)
          {
            unsigned u = this->si->scc_of(src);
            unsigned v = this->si->scc_of(dst);
            // The basic rules are as follows:
            //
            // - If an edge is between two SCCs, it is OK to remove
            //   all acceptance sets, as this edge cannot be part
            //   of any loop.
            // - If an edge is in an non-accepting SCC, we can only
            //   remove the Inf sets, as removinf the Fin sets
            //   might make the SCC accepting.
            //
            // The above rules are made more complex with two flags:
            //
            // - If PreserveSBA is set, we have to tree a transition
            //   leaving an SCC as other transitions inside the SCC,
            //   otherwise we will break the property that all
            //   transitions leaving the same state have identical set
            //   membership.
            // - If RemoveAll is false, we like to keep the membership
            //   of transitions entering an SCC.  This can only be
            //   done if PreserveSBA is unset, unfortunately.
            if (u == v)
              {
                if (this->si->is_rejecting_scc(u))
                  acc &= accmask;
              }
            else if (PreserveSBA && this->si->is_rejecting_scc(u))
              {
                if (!this->si->is_trivial(u))
                  acc &= accmask; // No choice.
                else if (RemoveAll)
                  acc = {};
              }
            else if (!PreserveSBA)
              {
                if (RemoveAll)
                  acc = {};
                else if (this->si->is_rejecting_scc(v))
                  acc &= accmask;
              }
          }
        return filtered_trans(keep, cond, acc);
      }
    };

    // Simplify redundant acceptance sets used in each SCCs.
    template <class next_filter = id_filter>
    struct acc_filter_simplify: next_filter
    {
      // Acceptance sets to strip in each SCC.
      std::vector<acc_cond::mark_t> strip_;

      template<typename... Args>
      acc_filter_simplify(scc_info* si, Args&&... args)
        : next_filter(si, std::forward<Args>(args)...)
      {
      }

      void fix_acceptance(const twa_graph_ptr& out)
      {
        auto& acc = this->si->get_aut()->acc();
        if (!acc.is_generalized_buchi())
          throw std::runtime_error
            ("simplification of SCC acceptance works only with "
             "generalized Büchi acceptance");

        unsigned scc_count = this->si->scc_count();
        auto used_acc = this->si->marks();
        assert(used_acc.size() == scc_count);
        strip_.resize(scc_count);
        std::vector<unsigned> cnt(scc_count); // # of useful sets in each SCC
        unsigned max = 0;                     // Max number of useful sets
        for (unsigned n = 0; n < scc_count; ++n)
          {
            if (this->si->is_rejecting_scc(n))
              continue;
            strip_[n] = acc.useless(used_acc[n].begin(), used_acc[n].end());
            cnt[n] = acc.num_sets() - strip_[n].count();
            if (cnt[n] > max)
              max = cnt[n];
          }
        // Now that we know about the max number of acceptance
        // conditions, add extra acceptance conditions to those SCC
        // that do not have enough.
        for (unsigned n = 0; n < scc_count; ++n)
          {
            if (this->si->is_rejecting_scc(n))
              continue;
            if (cnt[n] < max)
              strip_[n].remove_some(max - cnt[n]);
          }

        out->set_generalized_buchi(max);
      }

      filtered_trans trans(unsigned src, unsigned dst, bdd cond,
                           acc_cond::mark_t acc)
      {
        bool keep;
        std::tie(keep, cond, acc) =
          this->next_filter::trans(src, dst, cond, acc);

        if (keep && acc)
          {
            unsigned u = this->si->scc_of(dst);

            if (this->si->is_rejecting_scc(u))
              acc = {};
            else
              acc = acc.strip(strip_[u]);
          }
        return filtered_trans{keep, cond, acc};
      }
    };


    template<class F, typename... Args>
    twa_graph_ptr scc_filter_apply(const_twa_graph_ptr aut,
                                   scc_info* given_si, Args&&... args)
    {
      if (!aut->is_existential())
        throw std::runtime_error
          ("scc_filter() does yet not support alternation");

      unsigned in_n = aut->num_states();

      twa_graph_ptr filtered = make_twa_graph(aut->get_dict());
      filtered->copy_ap_of(aut);

      // Compute scc_info if not supplied.
      scc_info* si = given_si;
      if (!si)
        si = new scc_info(aut, scc_info_options::TRACK_SUCCS
                          | scc_info_options::TRACK_STATES_IF_FIN_USED);
      si->determine_unknown_acceptance();

      F filter(si, std::forward<Args>(args)...);

      // Renumber all useful states.
      unsigned out_n = 0;          // Number of output states.
      std::vector<unsigned> inout; // Associate old states to new ones.
      inout.reserve(in_n);
      for (unsigned i = 0; i < in_n; ++i)
        if (filter.state(i))
          inout.emplace_back(out_n++);
        else
          inout.emplace_back(-1U);

      filter.fix_acceptance(filtered);
      filtered->new_states(out_n);
      for (unsigned isrc = 0; isrc < in_n; ++isrc)
        {
          unsigned osrc = inout[isrc];
          if (osrc >= out_n)
            continue;
          for (auto& t: aut->out(isrc))
            {
              unsigned odst = inout[t.dst];
              if (odst >= out_n)
                continue;
              bool want;
              bdd cond;
              acc_cond::mark_t acc;
              std::tie(want, cond, acc) =
                filter.trans(isrc, t.dst, t.cond, t.acc);
              if (want)
                filtered->new_edge(osrc, odst, cond, acc);
            }
        }
      if (!given_si)
        delete si;
      // If the initial state has been filtered out, we have to create
      // a new one (not doing so may cause empty automata, which in turn
      // cause all sort of issue with algorithms assuming an automaton
      // has one initial state).
      auto init = inout[aut->get_init_state_number()];
      filtered->set_init_state(init < out_n ? init : filtered->new_state());

      if (auto* names =
          aut->get_named_prop<std::vector<std::string>>("state-names"))
        {
          unsigned size = names->size();
          if (size > in_n)
            size = in_n;
          auto* new_names = new std::vector<std::string>(out_n);
          filtered->set_named_prop("state-names", new_names);
          for (unsigned s = 0; s < size; ++s)
            {
              unsigned new_s = inout[s];
              if (new_s != -1U)
                (*new_names)[new_s] = (*names)[s];
            }
        }
      if (auto hs =
          aut->get_named_prop<std::map<unsigned, unsigned>>("highlight-states"))
        {
          auto* new_hs = new std::map<unsigned, unsigned>;
          filtered->set_named_prop("highlight-states", new_hs);
          for (auto p: *hs)
            {
              unsigned new_s = inout[p.first];
              if (new_s != -1U)
                new_hs->emplace(new_s, p.second);
            }
        }
      return filtered;
    }
  }

  twa_graph_ptr
  scc_filter_states(const const_twa_graph_ptr& aut, bool remove_all_useless,
                    scc_info* given_si)
  {
    twa_graph_ptr res;
    // For weak automata, scc_filter() is already doing the right
    // thing and preserves state-based acceptance.
    if (aut->prop_inherently_weak())
      return scc_filter(aut, remove_all_useless, given_si);
    if (remove_all_useless)
      res = scc_filter_apply<state_filter
                             <acc_filter_mask<true, true>>>(aut, given_si);
    else
      res = scc_filter_apply<state_filter
                             <acc_filter_mask<false, true>>>(aut, given_si);
    res->prop_copy(aut, { true, true, false, true, false, true });
    return res;
  }

  twa_graph_ptr
  scc_filter(const const_twa_graph_ptr& aut, bool remove_all_useless,
             scc_info* given_si)
  {
    twa_graph_ptr res;
    scc_info* si = given_si;
    if (aut->prop_inherently_weak())
      {
        // Create scc_info here, because we will need it to decide
        // very-weakness.
        if (!si)
          si = new scc_info(aut, scc_info_options::TRACK_SUCCS
                            | scc_info_options::TRACK_STATES_IF_FIN_USED);
        if (aut->acc().is_t() || aut->acc().is_co_buchi())
          res =
            scc_filter_apply<state_filter<weak_filter<false>>>(aut, si);
        else
          res =
            scc_filter_apply<state_filter<weak_filter<true>>>(aut, si);
      }
    else if (aut->acc().is_generalized_buchi())
      {
        // acc_filter_simplify only works for generalized Büchi
        if (remove_all_useless)
          res =
            scc_filter_apply<state_filter
                             <acc_filter_mask
                              <true, false,
                               acc_filter_simplify<>>>>(aut, given_si);
        else
          res =
            scc_filter_apply<state_filter
                             <acc_filter_mask
                              <false, false,
                               acc_filter_simplify<>>>>(aut, given_si);
      }
    else
      {
        if (remove_all_useless)
          res = scc_filter_apply<state_filter
                                 <acc_filter_mask
                                  <true, false>>>(aut, given_si);
        else
          res = scc_filter_apply<state_filter
                                 <acc_filter_mask
                                  <false, false>>>(aut, given_si);
      }
    res->merge_edges();
    res->prop_copy(aut,
                   { false,  // state-based acceptance is not preserved
                     true,
                     false,
                     true,      // determinism improved
                     false,
                     true,
                   });
    if (aut->prop_inherently_weak())
      {
        res->prop_weak(true);
        res->prop_state_acc(true);
        if (si->scc_count() == aut->num_states())
          res->prop_very_weak(true);
        if (!given_si)
          delete si;
      }
    return res;
  }

  twa_graph_ptr
  scc_filter_susp(const const_twa_graph_ptr& aut, bool remove_all_useless,
                  bdd suspvars, bdd ignoredvars, bool early_susp,
                  scc_info* given_si)
  {
    twa_graph_ptr res;
    if (remove_all_useless)
      res = scc_filter_apply<susp_filter
                             <state_filter
                              <acc_filter_mask
                               <true, false,
                                acc_filter_simplify<>>>>>(aut, given_si,
                                                          suspvars,
                                                          ignoredvars,
                                                          early_susp);
    else
      res = scc_filter_apply<susp_filter
                             <state_filter
                              <acc_filter_mask
                               <false, false,
                                acc_filter_simplify<>>>>>(aut, given_si,
                                                          suspvars,
                                                          ignoredvars,
                                                          early_susp);
    res->merge_edges();
    res->prop_copy(aut,
                   { false,  // state-based acceptance is not preserved
                     true,
                     false, false, // determinism may not be preserved
                     false,
                     false,  // stutter inv. of suspvars probably altered
                   });
    return res;
  }

}
