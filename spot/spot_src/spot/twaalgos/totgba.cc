// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018 Laboratoire de Recherche et Développement
// de l'Epita.
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
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twa/twagraph.hh>
#include <deque>
#include <tuple>

#define TRACE 0
#if TRACE
#define trace std::cerr
#else
#define trace while (0) std::cerr
#endif

namespace spot
{
  namespace
  {
    class dnf_to_streett_converter
    {
    private:
      typedef std::pair<acc_cond::mark_t, acc_cond::mark_t> mark_pair;

      const const_twa_graph_ptr& in_;             // The given aut.
      scc_info si_;                               // SCC information.
      unsigned nb_scc_;                           // Number of SCC.
      unsigned max_set_in_;                       // Max acc. set nb of in_.
      bool state_based_;                          // Is in_ state_based ?
      unsigned init_st_in_;                       // Initial state of in_.
      bool init_reachable_;                       // Init reach from itself?
      twa_graph_ptr res_;                         // Resulting automaton.
      acc_cond::mark_t all_fin_;                  // All acc. set marked as
                                                  // Fin.
      acc_cond::mark_t all_inf_;                  // All acc. set marked as
                                                  // Inf.
      unsigned num_sets_res_;                     // Future nb of acc. set.
      std::vector<mark_pair> all_clauses_;        // All clauses.
      std::vector<acc_cond::mark_t> set_to_keep_; // Set to keep for each clause
      std::vector<acc_cond::mark_t> set_to_add_;  // New set for each clause.
      acc_cond::mark_t all_set_to_add_;           // All new set to add.
      std::vector<unsigned> assigned_sets_;       // Set that will be add.
      std::vector<std::vector<unsigned>> acc_clauses_; // Acc. clauses.
      unsigned res_init_;                         // Future initial st.

      // A state can be copied at most as many times as their are clauses for
      // which it is not rejecting and must be copied one time (to remain
      // consistent with the recognized language). This vector records each
      // created state following this format:
      // st_repr_[orig_st_nb] gives a vector<pair<clause, state>>.
      std::vector<std::vector<std::pair<unsigned, unsigned>>> st_repr_;

      // Split the DNF acceptance condition and get all the sets used in each
      // clause. It separates those that must be seen finitely often from
      // those that must be seen infinitely often.
      void
      split_dnf_clauses(const acc_cond::acc_code& code)
      {
        auto pos = &code.back();
        if (pos->sub.op == acc_cond::acc_op::Or)
          --pos;
        auto start = &code.front();
        while (pos > start)
          {
            const unsigned short size = pos[0].sub.size;
            if (pos[0].sub.op == acc_cond::acc_op::And)
              {
                acc_cond::mark_t fin = {};
                acc_cond::mark_t inf = {};
                for (int i = 1; i <= (int)size; i += 2)
                  {
                    if (pos[-i].sub.op == acc_cond::acc_op::Fin)
                      fin |= pos[-i - 1].mark;
                    else if (pos[-i].sub.op == acc_cond::acc_op::Inf)
                      inf |= pos[-i - 1].mark;
                    else
                      SPOT_UNREACHABLE();
                  }
                all_clauses_.emplace_back(fin, inf);
                set_to_keep_.emplace_back(fin | inf);
              }
            else if (pos[0].sub.op == acc_cond::acc_op::Fin) // Fin
              {
                auto m1 = pos[-1].mark;
                for (unsigned int s : m1.sets())
                  {
                    all_clauses_.emplace_back(acc_cond::mark_t({s}),
                                              acc_cond::mark_t({}));
                    set_to_keep_.emplace_back(acc_cond::mark_t({s}));
                  }
              }
            else if (pos[0].sub.op == acc_cond::acc_op::Inf) // Inf
              {
                auto m2 = pos[-1].mark;
                all_clauses_.emplace_back(acc_cond::mark_t({}), m2);
                set_to_keep_.emplace_back(m2);
              }
            else
              {
                SPOT_UNREACHABLE();
              }
            pos -= size + 1;
          }
#if TRACE
        trace << "\nPrinting all clauses\n";
        for (unsigned i = 0; i < all_clauses_.size(); ++i)
          {
            trace << i << " Fin:" << all_clauses_[i].first << " Inf:"
                  << all_clauses_[i].second << '\n';
          }
#endif
      }

      // Compute all the acceptance sets that will be needed:
      //   -Inf(x) will be converted to (Inf(x) | Fin(y)) with y appearing
      //     on every edge of the associated clone.
      //   -Fin(x) will be converted to (Inf(y) | Fin(x)) with y appearing
      //     nowhere.
      // In the second form, Inf(y) with no occurrence of y, can be
      // reused multiple times.  It's called "missing_set" below.
      void
      assign_new_sets()
      {
        unsigned int next_set = 0;
        unsigned int missing_set = -1U;
        assigned_sets_.resize(max_set_in_, -1U);

        acc_cond::mark_t all_m = all_fin_ | all_inf_;
        for (unsigned set = 0; set < max_set_in_; ++set)
          if (all_fin_.has(set))
            {
              if ((int)missing_set < 0)
                {
                  while (all_m.has(next_set))
                    ++next_set;
                  missing_set = next_set++;
                }

              assigned_sets_[set] = missing_set;
            }
          else if (all_inf_.has(set))
            {
              while (all_m.has(next_set))
                ++next_set;

              assigned_sets_[set] = next_set++;
            }

        num_sets_res_ = std::max(next_set, max_set_in_);
      }

      // Precompute:
      //   -the sets to add for each clause,
      //   -all sets to add.
      void
      find_set_to_add()
      {
        assign_new_sets();

        unsigned nb_clause = all_clauses_.size();
        for (unsigned clause = 0; clause < nb_clause; ++clause)
          {
            if (all_clauses_[clause].second)
              {
                acc_cond::mark_t m = {};
                for (unsigned set = 0; set < max_set_in_; ++set)
                  if (all_clauses_[clause].second.has(set))
                    {
                      assert((int)assigned_sets_[set] >= 0);
                      m |= acc_cond::mark_t({assigned_sets_[set]});
                    }
                set_to_add_.push_back(m);
              }
            else
              {
                set_to_add_.emplace_back(acc_cond::mark_t({}));
              }
          }

        all_set_to_add_ = {};
        for (unsigned s = 0; s < max_set_in_; ++s)
          if (all_inf_.has(s))
            {
              assert((int)assigned_sets_[s] >= 0);
              all_set_to_add_.set(assigned_sets_[s]);
            }
      }

      // Check whether the initial state is reachable from itself.
      bool
      is_init_reachable()
      {
        for (const auto& e : in_->edges())
          for (unsigned d : in_->univ_dests(e))
            if (d == init_st_in_)
              return true;
        return false;
      }

      // Get all non rejecting scc for each clause of the acceptance
      // condition. Actually, for each clause, an scc will be kept if it
      // contains all the 'Inf' acc. sets of the clause.
      void
      find_probably_accepting_scc(std::vector<std::vector<unsigned>>& res)
      {
        res.resize(nb_scc_);
        unsigned nb_clause = all_clauses_.size();
        for (unsigned scc = 0; scc < nb_scc_; ++scc)
          {
            if (si_.is_rejecting_scc(scc))
              continue;

            acc_cond::mark_t acc = si_.acc_sets_of(scc);
            for (unsigned clause = 0; clause < nb_clause; ++clause)
              {
                if ((acc & all_clauses_[clause].second)
                      == all_clauses_[clause].second)
                  res[scc].push_back(clause);
              }
          }
#if TRACE
        trace << "accepting clauses\n";
        for (unsigned i = 0; i < res.size(); ++i)
          {
            trace << "scc(" << i << ") -->";
            for (auto elt : res[i])
              trace << ' ' << elt;
            if (si_.is_rejecting_scc(i))
              trace << " rej";
            trace << '\n';
          }
        trace << '\n';
#endif
      }

      // Add all possible representatives of the original state provided.
      // Actually, this state will be copied as many times as there are clauses
      // for which its SCC is not rejecting.
      void
      add_state(unsigned st)
      {
        trace << "add_state(" << st << ")\n";
        if (st_repr_[st].empty())
          {
            unsigned st_scc = si_.scc_of(st);
            if (st == init_st_in_ && !init_reachable_)
              st_repr_[st].emplace_back(-1U, res_init_);

            else if (!acc_clauses_[st_scc].empty())
              for (const auto& clause : acc_clauses_[st_scc])
                st_repr_[st].emplace_back(clause, res_->new_state());

            else
              st_repr_[st].emplace_back(-1U, res_->new_state());
            trace << "added\n";
          }
      }

      // Compute the mark that will be set (instead of the provided e_acc)
      // according to the current clause in process. This function is only
      // called for accepting SCC.
      acc_cond::mark_t
      get_edge_mark(const acc_cond::mark_t& e_acc,
                    unsigned clause)
      {
        assert((int)clause >= 0);
        return (e_acc & set_to_keep_[clause]) | set_to_add_[clause];
      }

      // Set the acceptance condition once the resulting automaton is ready.
      void
      set_acc_condition()
      {
        acc_cond::acc_code p_code;
        for (unsigned set = 0; set < max_set_in_; ++set)
          {
            if (all_fin_.has(set))
              p_code &=
                acc_cond::acc_code::inf(acc_cond::mark_t({assigned_sets_[set]}))
                  | acc_cond::acc_code::fin(acc_cond::mark_t({set}));
            else if (all_inf_.has(set))
              p_code &=
                acc_cond::acc_code::inf(acc_cond::mark_t({set}))
                  | acc_cond::acc_code::fin(
                      acc_cond::mark_t({assigned_sets_[set]}));
          }
        res_->set_acceptance(num_sets_res_, p_code);
      }

    public:
      dnf_to_streett_converter(const const_twa_graph_ptr& in,
                               const acc_cond::acc_code& code)
        : in_(in),
          si_(scc_info(in, scc_info_options::TRACK_STATES
                           | scc_info_options::TRACK_SUCCS)),
          nb_scc_(si_.scc_count()),
          max_set_in_(code.used_sets().max_set()),
          state_based_(in->prop_state_acc() == true),
          init_st_in_(in->get_init_state_number()),
          init_reachable_(is_init_reachable())
      {
        trace << "State based ? " << state_based_ << '\n';
        std::tie(all_inf_, all_fin_) = code.used_inf_fin_sets();
        split_dnf_clauses(code);
        find_set_to_add();
        find_probably_accepting_scc(acc_clauses_);
      }

      ~dnf_to_streett_converter()
      {}

      twa_graph_ptr run(bool original_states)
      {
        res_ = make_twa_graph(in_->get_dict());
        res_->copy_ap_of(in_);
        st_repr_.resize(in_->num_states());
        res_init_ = res_->new_state();
        res_->set_init_state(res_init_);

        for (unsigned scc = 0; scc < nb_scc_; ++scc)
          {
            //if (!si_.is_useful_scc(scc))
            //  continue;
            trace << "scc #" << scc << '\n';

            bool rej_scc = acc_clauses_[scc].empty();
            for (auto st : si_.states_of(scc))
              {
                add_state(st);
                for (const auto& e : in_->out(st))
                  {
                    trace << "working_on_edge(" << st << ',' << e.dst << ")\n";

                    unsigned dst_scc = si_.scc_of(e.dst);
                    //if (!si_.is_useful_scc(dst_scc))
                    //    continue;
                    add_state(e.dst);
                    bool same_scc = scc == dst_scc;

                    if (st == init_st_in_)
                      {
                        for (const auto& p_dst : st_repr_[e.dst])
                          res_->new_edge(res_init_, p_dst.second, e.cond, {});
                        if (!init_reachable_)
                          continue;
                      }

                    if (!rej_scc)
                      for (const auto& p_src : st_repr_[st])
                        for (const auto& p_dst : st_repr_[e.dst])
                          {
                            trace << "repr(" << p_src.second << ','
                                  << p_dst.second << ")\n";

                            if (same_scc && p_src.first == p_dst.first)
                              res_->new_edge(p_src.second, p_dst.second, e.cond,
                                             get_edge_mark(e.acc, p_src.first));

                            else if (!same_scc)
                              res_->new_edge(p_src.second, p_dst.second, e.cond,
                                             state_based_ ?
                                               get_edge_mark(e.acc, p_src.first)
                                             : acc_cond::mark_t({}));
                          }
                    else
                      {
                        assert(st_repr_[st].size() == 1);
                        unsigned src = st_repr_[st][0].second;

                        acc_cond::mark_t m = {};
                        if (same_scc || state_based_)
                          m = e.acc | all_set_to_add_;

                        for (const auto& p_dst : st_repr_[e.dst])
                          res_->new_edge(src, p_dst.second, e.cond, m);
                      }
                  }
              }
          }

        // Mapping between each state of the resulting automaton and the
        // original state of the input automaton.
        if (original_states)
          {
            auto orig_states = new std::vector<unsigned>();
            orig_states->resize(res_->num_states(), -1U);
            res_->set_named_prop("original-states", orig_states);

            auto orig_clauses = new std::vector<unsigned>();
            orig_clauses->resize(res_->num_states(), -1U);
            res_->set_named_prop("original-clauses", orig_clauses);

            unsigned orig_num_states = in_->num_states();
            for (unsigned orig = 0; orig < orig_num_states; ++orig)
              {
                if (!si_.is_useful_scc(si_.scc_of(orig)))
                    continue;
                for (const auto& p : st_repr_[orig])
                  {
                    (*orig_states)[p.second] = orig;
                    (*orig_clauses)[p.second] = p.first;
                  }
              }
          }

        set_acc_condition();
        res_->prop_state_acc(state_based_);
        return res_;
      }
    };
  }


  twa_graph_ptr
  dnf_to_streett(const const_twa_graph_ptr& in, bool original_states)
  {
    const acc_cond::acc_code& code = in->get_acceptance();
    if (!code.is_dnf())
          throw std::runtime_error("dnf_to_streett() should only be"
                                   " called on automata with DNF acceptance");
    if (code.is_t() || code.is_f() || in->acc().is_streett() > 0)
      return make_twa_graph(in, twa::prop_set::all());

    dnf_to_streett_converter dnf_to_streett(in, code);
    return dnf_to_streett.run(original_states);
  }


  namespace
  {
    struct st2gba_state
    {
      acc_cond::mark_t pend;
      unsigned s;

      st2gba_state(unsigned st, acc_cond::mark_t bv = acc_cond::mark_t::all()):
        pend(bv), s(st)
      {
      }
    };

    struct st2gba_state_hash
    {
      size_t
      operator()(const st2gba_state& s) const noexcept
      {
        std::hash<acc_cond::mark_t> h;
        return s.s ^ h(s.pend);
      }
    };

    struct st2gba_state_equal
    {
      bool
      operator()(const st2gba_state& left,
                 const st2gba_state& right) const
      {
        if (left.s != right.s)
          return false;
        return left.pend == right.pend;
      }
    };

    typedef std::vector<acc_cond::mark_t> terms_t;

    terms_t cnf_terms(const acc_cond::acc_code& code)
    {
      assert(!code.empty());
      terms_t res;
      auto pos = &code.back();
      auto end = &code.front();
      if (pos->sub.op == acc_cond::acc_op::And)
        --pos;
      while (pos >= end)
        {
          auto term_end = pos - 1 - pos->sub.size;
          bool inor = pos->sub.op == acc_cond::acc_op::Or;
          if (inor)
            --pos;
          acc_cond::mark_t m = {};
          while (pos > term_end)
            {
              assert(pos->sub.op == acc_cond::acc_op::Inf);
              m |= pos[-1].mark;
              pos -= 2;
            }
          if (inor)
            res.emplace_back(m);
          else
            for (unsigned i: m.sets())
              res.emplace_back(acc_cond::mark_t({i}));
        }
      return res;
    }
  }


  //  Specialized conversion for Streett -> TGBA
  // ============================================
  //
  // Christof Löding's Diploma Thesis: Methods for the
  // Transformation of ω-Automata: Complexity and Connection to
  // Second Order Logic.  Section 3.4.3, gives a transition
  // from Streett with |Q| states to BA with |Q|*(4^n-3^n+2)
  // states, if n is the number of acceptance pairs.
  //
  // Duret-Lutz et al. (ATVA'2009): On-the-fly Emptiness Check of
  // Transition-based Streett Automata.  Section 3.3 contains a
  // conversion from transition-based Streett Automata to TGBA using
  // the generalized Büchi acceptance to limit the explosion.  It goes
  // from Streett with |Q| states to (T)GBA with |Q|*(2^n+1) states.
  // However the definition of the number of acceptance sets in that
  // paper is suboptimal: only n are needed, not 2^n.
  //
  // This implements this second version.
  twa_graph_ptr
  streett_to_generalized_buchi(const const_twa_graph_ptr& in)
  {
    // While "t" is Streett, it is also generalized Büchi, so
    // do not do anything.
    if (in->acc().is_generalized_buchi())
      return std::const_pointer_cast<twa_graph>(in);

    std::vector<acc_cond::rs_pair> pairs;
    bool res = in->acc().is_streett_like(pairs);
    if (!res)
      throw std::runtime_error("streett_to_generalized_buchi() should only be"
                               " called on automata with Streett-like"
                               " acceptance");

    // In Streett acceptance, inf sets are odd, while fin sets are
    // even.
    acc_cond::mark_t inf;
    acc_cond::mark_t fin;
    std::tie(inf, fin) = in->get_acceptance().used_inf_fin_sets();
    unsigned p = inf.count();
    // At some point we will remove anything that is not used as Inf.
    acc_cond::mark_t to_strip = in->acc().all_sets() - inf;
    acc_cond::mark_t inf_alone = {};
    acc_cond::mark_t fin_alone = {};

    if (!p)
      return remove_fin(in);

    unsigned numsets = in->acc().num_sets();
    std::vector<acc_cond::mark_t> fin_to_infpairs(numsets,
                                                  acc_cond::mark_t({}));
    std::vector<acc_cond::mark_t> inf_to_finpairs(numsets,
                                                  acc_cond::mark_t({}));
    for (auto pair: pairs)
      {
        if (pair.fin)
          for (unsigned mark: pair.fin.sets())
            fin_to_infpairs[mark] |= pair.inf;
        else
          inf_alone |= pair.inf;

        if (pair.inf)
          for (unsigned mark: pair.inf.sets())
            inf_to_finpairs[mark] |= pair.fin;
        else
          fin_alone |= pair.fin;
      }
    // If we have something like (Fin(0)|Inf(1))&Fin(0), then 0 is in
    // fin_alone, but we also have fin_to_infpair[0] = {1}.  This should
    // really be simplified to Fin(0).
    for (auto mark: fin_alone.sets())
      fin_to_infpairs[mark] = {};

    scc_info si(in, scc_info_options::NONE);

    // Compute the acceptance sets present in each SCC
    unsigned nscc = si.scc_count();
    std::vector<std::tuple<acc_cond::mark_t, acc_cond::mark_t,
                           bool, bool>> sccfi;
    sccfi.reserve(nscc);
    for (unsigned s = 0; s < nscc; ++s)
      {
        auto acc = si.acc_sets_of(s); // {0,1,2,3,4,6,7,9}
        auto acc_fin = acc & fin;     // {0,  2,  4,6}
        auto acc_inf = acc & inf;     // {  1,  3,    7,9}
        // Fin sets that are alone either because the acceptance
        // condition has no matching Inf, or because the SCC does not
        // intersect the matching Inf.
        acc_cond::mark_t fin_wo_inf = {};
        for (unsigned mark: acc_fin.sets())
          if (!fin_to_infpairs[mark] || (fin_to_infpairs[mark] - acc_inf))
            fin_wo_inf.set(mark);

        // Inf sets that *do* have a matching Fin in the acceptance
        // condition but without matching Fin in the SCC: they can be
        // considered as always present in the SCC.
        acc_cond::mark_t inf_wo_fin = {};
        for (unsigned mark: inf.sets())
          if (inf_to_finpairs[mark] && (inf_to_finpairs[mark] - acc_fin))
            inf_wo_fin.set(mark);

        sccfi.emplace_back(fin_wo_inf, inf_wo_fin,
                           !acc_fin, !acc_inf);
      }

    auto out = make_twa_graph(in->get_dict());
    out->copy_ap_of(in);
    out->prop_copy(in, {false, false, false, false, false, true});
    out->set_generalized_buchi(p);

    // Map st2gba pairs to the state numbers used in out.
    typedef std::unordered_map<st2gba_state, unsigned,
                               st2gba_state_hash,
                               st2gba_state_equal> bs2num_map;
    bs2num_map bs2num;

    // Queue of states to be processed.
    typedef std::deque<st2gba_state> queue_t;
    queue_t todo;

    st2gba_state s(in->get_init_state_number());
    bs2num[s] = out->new_state();
    todo.emplace_back(s);

    bool sbacc = in->prop_state_acc().is_true();

    // States of the original automaton are marked with s.pend == -1U.
    const acc_cond::mark_t orig_copy = acc_cond::mark_t::all();

    while (!todo.empty())
      {
        s = todo.front();
        todo.pop_front();
        unsigned src = bs2num[s];

        unsigned scc_src = si.scc_of(s.s);
        bool maybe_acc_scc = !si.is_rejecting_scc(scc_src);

        acc_cond::mark_t scc_fin_wo_inf;
        acc_cond::mark_t scc_inf_wo_fin;
        bool no_fin;
        bool no_inf;
        std::tie(scc_fin_wo_inf, scc_inf_wo_fin, no_fin, no_inf)
          = sccfi[scc_src];

        for (auto& t: in->out(s.s))
          {
            acc_cond::mark_t pend = s.pend;
            acc_cond::mark_t acc = {};

            bool maybe_acc = maybe_acc_scc && (scc_src == si.scc_of(t.dst));
            if (pend != orig_copy)
              {
                if (!maybe_acc)
                  continue;
                // No point going to some place we will never leave
                if (t.acc & scc_fin_wo_inf)
                  continue;
                // For any Fin set we see, we want to see the
                // corresponding Inf set.
                for (unsigned mark: (t.acc & fin).sets())
                  pend |= fin_to_infpairs[mark];

                // If we see some Inf set immediately, they are not
                // pending anymore.
                pend -= t.acc & inf;

                // Label this transition with all non-pending
                // inf sets.  The strip will shift everything
                // to the correct numbers in the targets.
                acc = (inf - pend).strip(to_strip);
                // Adjust the pending sets to what will be necessary
                // required on the destination state.
                if (sbacc)
                  {
                    auto a = in->state_acc_sets(t.dst);
                    if (a & scc_fin_wo_inf)
                      continue;
                    for (unsigned m: (a & fin).sets())
                      pend |= fin_to_infpairs[m];

                    pend -= a & inf;
                  }
                pend |= inf_alone;
              }
            else if (no_fin && maybe_acc)
              {
                // If the acceptance is (Fin(0) | Inf(1)) & Inf(2)
                // but we do not see any Fin set in this SCC, a
                // mark {2} should become {1,2} before striping.
                acc = (t.acc | scc_inf_wo_fin).strip(to_strip);
              }
            assert((acc & out->acc().all_sets()) == acc);

            st2gba_state d(t.dst, pend);
            // Have we already seen this destination?
            unsigned dest;
            auto dres = bs2num.emplace(d, 0);
            if (!dres.second)
              {
                dest = dres.first->second;
              }
            else                // No, this is a new state
              {
                dest = dres.first->second = out->new_state();
                todo.emplace_back(d);
              }
            out->new_edge(src, dest, t.cond, acc);

            // Nondeterministically jump to level ∅.  We need to do
            // that only once per cycle.  As an approximation, we
            // only do that for transitions where t.src >= t.dst as
            // this has to occur at least once per cycle.
            if (pend == orig_copy && (t.src >= t.dst) && maybe_acc && !no_fin)
              {
                acc_cond::mark_t stpend = {};
                if (sbacc)
                  {
                    auto a = in->state_acc_sets(t.dst);
                    if (a & scc_fin_wo_inf)
                      continue;
                    for (unsigned m: (a & fin).sets())
                      stpend |= fin_to_infpairs[m];

                    stpend -= a & inf;
                  }
                st2gba_state d(t.dst, stpend | inf_alone);
                // Have we already seen this destination?
                unsigned dest;
                auto dres = bs2num.emplace(d, 0);
                if (!dres.second)
                  {
                    dest = dres.first->second;
                  }
                else                // No, this is a new state
                  {
                    dest = dres.first->second = out->new_state();
                    todo.emplace_back(d);
                  }
                out->new_edge(src, dest, t.cond);
              }
          }
      }
    simplify_acceptance_here(out);
    if (out->acc().is_f())
      {
        // "f" is not generalized-Büchi.  Just return an
        // empty automaton instead.
        auto res = make_twa_graph(out->get_dict());
        res->set_generalized_buchi(0);
        res->set_init_state(res->new_state());
        res->prop_stutter_invariant(true);
        res->prop_weak(true);
        res->prop_complete(false);
        return res;
      }
    return out;
  }

  twa_graph_ptr
  streett_to_generalized_buchi_maybe(const const_twa_graph_ptr& in)
  {
    static unsigned min = [&]() {
      const char* c = getenv("SPOT_STREETT_CONV_MIN");
      if (!c)
        return 3;
      errno = 0;
      int val = strtol(c, nullptr, 10);
      if (val < 0 || errno != 0)
        throw std::runtime_error("unexpected value for SPOT_STREETT_CONV_MIN");
      return val;
    }();

    std::vector<acc_cond::rs_pair> pairs;
    bool res = in->acc().is_streett_like(pairs);
    if (!res || min == 0 || min > pairs.size())
      return nullptr;
    else
      return streett_to_generalized_buchi(in);
  }


  /// \brief Take an automaton with any acceptance condition and return
  /// an equivalent Generalized Büchi automaton.
  twa_graph_ptr
  to_generalized_buchi(const const_twa_graph_ptr& aut)
  {
    auto maybe = streett_to_generalized_buchi_maybe(aut);
    if (maybe)
      return maybe;

    auto res = remove_fin(cleanup_acceptance(aut));
    if (res->acc().is_generalized_buchi())
      return res;

    auto cnf = res->get_acceptance().to_cnf();
    // If we are very lucky, building a CNF actually gave us a GBA...
    if (cnf.empty() ||
        (cnf.size() == 2 && cnf.back().sub.op == acc_cond::acc_op::Inf))
      {
        res->set_acceptance(res->num_sets(), cnf);
        cleanup_acceptance_here(res);
        return res;
      }

    // Handle false specifically.  We want the output
    // an automaton with Acceptance: t, that has a single
    // state without successor.
    if (cnf.is_f())
      {
        assert(!cnf.front().mark);
        res = make_twa_graph(aut->get_dict());
        res->set_init_state(res->new_state());
        res->prop_state_acc(true);
        res->prop_weak(true);
        res->prop_universal(true);
        res->prop_stutter_invariant(true);
        return res;
      }

    auto terms = cnf_terms(cnf);
    unsigned nterms = terms.size();
    assert(nterms > 0);
    res->set_generalized_buchi(nterms);

    for (auto& t: res->edges())
      {
        acc_cond::mark_t cur_m = t.acc;
        acc_cond::mark_t new_m = {};
        for (unsigned n = 0; n < nterms; ++n)
          if (cur_m & terms[n])
            new_m.set(n);
        t.acc = new_m;
      }
    return res;
  }

  namespace
  {
    // If the DNF is
    //  Fin(1)&Inf(2)&Inf(4) | Fin(2)&Fin(3)&Inf(1) |
    //  Inf(1)&Inf(3) | Inf(1)&Inf(2) | Fin(4)
    // this returns the following vector of pairs:
    //  [({1}, {2,4})
    //   ({2,3}, {1}),
    //   ({}, {1,3}),
    //   ({}, {2}),
    //   ({4}, t)]
    static std::vector<std::pair<acc_cond::mark_t, acc_cond::mark_t>>
    split_dnf_acc(const acc_cond::acc_code& acc)
    {
      std::vector<std::pair<acc_cond::mark_t, acc_cond::mark_t>> res;
      if (acc.empty())
        {
          res.emplace_back(acc_cond::mark_t({}), acc_cond::mark_t({}));
          return res;
        }
      auto pos = &acc.back();
      if (pos->sub.op == acc_cond::acc_op::Or)
        --pos;
      auto start = &acc.front();
      while (pos > start)
        {
          if (pos->sub.op == acc_cond::acc_op::Fin)
            {
              // We have only a Fin term, without Inf.  In this case
              // only, the Fin() may encode a disjunction of sets.
              for (auto s: pos[-1].mark.sets())
                res.emplace_back(acc_cond::mark_t({s}), acc_cond::mark_t({}));
              pos -= pos->sub.size + 1;
            }
          else
            {
              // We have a conjunction of Fin and Inf sets.
              auto end = pos - pos->sub.size - 1;
              acc_cond::mark_t fin = {};
              acc_cond::mark_t inf = {};
              while (pos > end)
                {
                  switch (pos->sub.op)
                    {
                    case acc_cond::acc_op::And:
                      --pos;
                      break;
                    case acc_cond::acc_op::Fin:
                      fin |= pos[-1].mark;
                      assert(pos[-1].mark.count() == 1);
                      pos -= 2;
                      break;
                    case acc_cond::acc_op::Inf:
                      inf |= pos[-1].mark;
                      pos -= 2;
                      break;
                    case acc_cond::acc_op::FinNeg:
                    case acc_cond::acc_op::InfNeg:
                    case acc_cond::acc_op::Or:
                      SPOT_UNREACHABLE();
                      break;
                    }
                }
              assert(pos == end);
              res.emplace_back(fin, inf);
            }
        }
      return res;
    }


    static twa_graph_ptr
    to_generalized_rabin_aux(const const_twa_graph_ptr& aut,
                             bool share_inf, bool complement)
    {
      auto res = cleanup_acceptance(aut);
      auto oldacc = res->get_acceptance();
      if (complement)
        res->set_acceptance(res->acc().num_sets(), oldacc.complement());

      {
        std::vector<unsigned> pairs;
        if (res->acc().is_generalized_rabin(pairs))
          {
            if (complement)
              res->set_acceptance(res->acc().num_sets(), oldacc);
            return res;
          }
      }
      auto dnf = res->get_acceptance().to_dnf();
      if (dnf.is_f())
        {
          if (complement)
            res->set_acceptance(0, acc_cond::acc_code::t());
          return res;
        }

      auto v = split_dnf_acc(dnf);

      // Decide how we will rename each input set.
      //
      // inf_rename is only used if hoa_style=false, to
      // reuse previously used Inf sets.

      unsigned ns = res->num_sets();
      std::vector<acc_cond::mark_t> rename(ns);
      std::vector<unsigned> inf_rename(ns);

      unsigned next_set = 0;
      // The output acceptance conditions.
      acc_cond::acc_code code =
        complement ? acc_cond::acc_code::t() : acc_cond::acc_code::f();
      for (auto& i: v)
        {
          unsigned fin_set = 0U;

          if (!complement)
            {
              for (auto s: i.first.sets())
                rename[s].set(next_set);
              fin_set = next_set++;
            }

          acc_cond::mark_t infsets = {};

          if (share_inf)
            for (auto s: i.second.sets())
              {
                unsigned n = inf_rename[s];
                if (n == 0)
                  n = inf_rename[s] = next_set++;
                rename[s].set(n);
                infsets.set(n);
              }
          else                    // HOA style
            {
              for (auto s: i.second.sets())
                {
                  unsigned n = next_set++;
                  rename[s].set(n);
                  infsets.set(n);
                }
            }

          // The definition of Streett wants the Fin first in clauses,
          // so we do the same for generalized Streett since HOA does
          // not specify anything.  See
          // https://github.com/adl/hoaf/issues/62
          if (complement)
            {
              for (auto s: i.first.sets())
                rename[s].set(next_set);
              fin_set = next_set++;

              auto pair = acc_cond::inf({fin_set});
              pair |= acc_cond::acc_code::fin(infsets);
              pair &= std::move(code);
              code = std::move(pair);
            }
          else
            {
              auto pair = acc_cond::acc_code::inf(infsets);
              pair &= acc_cond::fin({fin_set});
              pair |= std::move(code);
              code = std::move(pair);
            }
        }

      // Fix the automaton
      res->set_acceptance(next_set, code);
      for (auto& e: res->edges())
        {
          acc_cond::mark_t m = {};
          for (auto s: e.acc.sets())
            m |= rename[s];
          e.acc = m;
        }
      return res;
    }


  }



  twa_graph_ptr
  to_generalized_rabin(const const_twa_graph_ptr& aut,
                       bool share_inf)
  {
    return to_generalized_rabin_aux(aut, share_inf, false);
  }

  twa_graph_ptr
  to_generalized_streett(const const_twa_graph_ptr& aut,
                         bool share_fin)
  {
    return to_generalized_rabin_aux(aut, share_fin, true);
  }
}
