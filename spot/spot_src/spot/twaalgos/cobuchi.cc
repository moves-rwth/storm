// -*- coding: utf-8 -*-
// Copyright (C) 2017-2018 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/cobuchi.hh>

#include <spot/misc/bddlt.hh>
#include <spot/misc/bitvect.hh>
#include <spot/misc/hash.hh>
#include <spot/twa/acc.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/powerset.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/sbacc.hh>  // For issue #317
#include <stack>
#include <unordered_map>

#define TRACE 0
#if TRACE
#define trace std::cerr
#else
#define trace while (0) std::cout
#endif

namespace spot
{
  namespace
  {
    typedef std::pair<unsigned, unsigned> pair_state_nca;

    // Helper function that returns the called 'augmented subset construction'
    // of the given automaton, i.e. the product of the automaton  with its
    // powerset construction.
    //
    // 'aut_power' is the automaton that will be used for the powerset
    // construction and 'aut_prod' is the one that will be used for the
    // product. They may be confusing in the sense that why the same automaton
    // is not used for the product and the powerset construction. Actually,
    // when dealing with an automaton A with Rabin acceptance, it is firstly
    // converted into an automaton B with Streett-like acceptance. The powerset
    // construction of B happens to be isomorphic with the powerset construction
    // of A. Therefore, you would like to use A (which is smaller than B) for
    // the powerset construction and B for the product.
    static
    twa_graph_ptr
    aug_subset_cons(const const_twa_graph_ptr& aut_prod,
                    const const_twa_graph_ptr& aut_power,
                    bool named_states,
                    struct power_map& pmap)
    {
      twa_graph_ptr res = nullptr;

      if (is_deterministic(aut_prod))
        {
          res = make_twa_graph(aut_prod, twa::prop_set::all());
          auto v = new product_states;
          res->set_named_prop("product-states", v);
          for (unsigned s = 0; s < aut_power->num_states(); ++s)
            v->emplace_back(s, s);
          for (unsigned s = 0; s < aut_power->num_states(); ++s)
            pmap.map_.push_back({s});
        }
      else
        {
          res = product(aut_prod, tgba_powerset(aut_power, pmap));
        }

      if (named_states)
        {
          const product_states* res_map = res->get_named_prop
            <product_states>("product-states");

          auto v = new std::vector<std::string>;
          res->set_named_prop("state-names", v);

          auto get_st_name =
            [&](const pair_state_nca& x)
            {
              std::stringstream os;
              os << x.first << ",{";
              bool not_first = false;
              for (auto& a : pmap.states_of(x.second))
                {
                  if (not_first)
                    os << ',';
                  else
                    not_first = true;
                  os << a;
                }
              os << '}';
              return os.str();
            };

          unsigned num_states = res->num_states();
          for (unsigned i = 0; i < num_states; ++i)
            v->emplace_back(get_st_name((*res_map)[i]));
        }
      return res;
    }

    class nsa_to_nca_converter final
    {
      protected:
        struct power_map pmap_;                 // Sets of sts (powerset cons.).

        const_twa_graph_ptr aut_;               // The given automaton.
        bool state_based_;                      // Is it state based?
        std::vector<acc_cond::rs_pair> pairs_;  // All pairs of the acc. cond.
        unsigned nb_pairs_;                     // Nb pair in the acc. cond.
        bool named_states_;                     // Name states for display?
        twa_graph_ptr res_;                     // The augmented subset const.
        product_states* res_map_;               // States of the aug. sub. cons.
        scc_info si_;                           // SCC information.
        unsigned nb_states_;                    // Number of states.
        unsigned was_dnf_;                      // Was it DNF before Streett?
        std::vector<unsigned>* orig_states_;    // Match old Rabin st. from new.
        std::vector<unsigned>* orig_clauses_;   // Associated Rabin clauses.
        unsigned orig_num_st_;                  // Rabin original nb states.

        // Keep information of states that are wanted to be seen infinitely
        // often (cf Header).
        void save_inf_nca_st(unsigned s, vect_nca_info* nca_info)
        {
          const pair_state_nca& st = (*res_map_)[s];
          auto bv = make_bitvect(orig_num_st_);
          for (unsigned state : pmap_.states_of(st.second))
            bv->set(state);
          unsigned clause = 0;
          unsigned state = st.first;
          if (was_dnf_)
            {
              clause = (*orig_clauses_)[state];
              assert((int)clause >= 0);
              state = (*orig_states_)[state];
              assert((int)state >= 0);
            }
          nca_info->push_back(new nca_st_info(clause, state, bv));
        }

        // Helper function that marks states that we want to see finitely often
        // and save some information about states that we want to see infinitely
        // often (cf Header).
        void set_marks_using(std::vector<bool>& nca_is_inf_state,
                             vect_nca_info* nca_info)
        {
          for (unsigned s = 0; s < nb_states_; ++s)
            {
              unsigned src_scc = si_.scc_of(s);
              if (nca_is_inf_state[s])
                {
                  for (auto& e : res_->out(s))
                    e.acc = {};

                  if (nca_info)
                    save_inf_nca_st(s, nca_info);
                }
              else
                {
                  for (auto& e : res_->out(s))
                    {
                      if (si_.scc_of(e.dst) == src_scc || state_based_)
                          e.acc = acc_cond::mark_t({0});
                      else
                        e.acc = {};
                    }
                }
            }
        }

      public:

        nsa_to_nca_converter(const const_twa_graph_ptr ref_prod,
                             const const_twa_graph_ptr ref_power,
                             std::vector<acc_cond::rs_pair>& pairs,
                             bool named_states = false,
                             bool was_dnf = false,
                             unsigned orig_num_st = 0)
          : aut_(ref_prod),
            state_based_(aut_->prop_state_acc().is_true()),
            pairs_(pairs),
            nb_pairs_(pairs.size()),
            named_states_(named_states),
            res_(aug_subset_cons(ref_prod, ref_power, named_states_, pmap_)),
            res_map_(res_->get_named_prop<product_states>("product-states")),
            si_(res_, (scc_info_options::TRACK_STATES
                       | scc_info_options::TRACK_SUCCS)),
            nb_states_(res_->num_states()),
            was_dnf_(was_dnf),
            orig_num_st_(orig_num_st ? orig_num_st : ref_prod->num_states())
        {
          if (was_dnf)
            {
              orig_states_ = ref_prod->get_named_prop<std::vector<unsigned>>
                ("original-states");
              orig_clauses_ = ref_prod->get_named_prop<std::vector<unsigned>>
                ("original-clauses");
            }
        }

        ~nsa_to_nca_converter()
        {}

        twa_graph_ptr run(vect_nca_info* nca_info)
        {
          std::vector<bool> nca_is_inf_state;    // Accepting or rejecting sts.
          nca_is_inf_state.resize(nb_states_, false);

          // Iterate over all SCCs and check for accepting states. A state 's'
          // is accepting if there is a cycle containing 's' that visits
          // finitely often all acceptance sets marked as Fin or infinitely
          // often acceptance sets marked by Inf.
          unsigned nb_scc = si_.scc_count();
          for (unsigned scc = 0; scc < nb_scc; ++scc)
            for (unsigned st : si_.states_on_acc_cycle_of(scc))
              nca_is_inf_state[st] = true;

          set_marks_using(nca_is_inf_state, nca_info);

          res_->prop_state_acc(state_based_);
          res_->set_co_buchi();
          res_->merge_edges();
          return res_;
        }
    };
  }


  twa_graph_ptr
  nsa_to_nca(const_twa_graph_ptr ref,
             bool named_states,
             vect_nca_info* nca_info)
  {
    if (ref->acc().is_parity())
      ref = to_generalized_streett(ref);

    std::vector<acc_cond::rs_pair> pairs;
    if (!ref->acc().is_streett_like(pairs))
      throw std::runtime_error("nsa_to_nca() only works with Streett-like or "
                               "Parity acceptance condition");

    // FIXME: At the moment this algorithm does not support
    // transition-based acceptance.  See issue #317.  Once that is
    // fixed we may remove the next line.
    ref = sbacc(std::const_pointer_cast<twa_graph>(ref));

    nsa_to_nca_converter nca_converter(ref, ref, pairs, named_states, false);
    return nca_converter.run(nca_info);
  }


  twa_graph_ptr
  dnf_to_nca(const_twa_graph_ptr ref, bool named_states,
             vect_nca_info* nca_info)
  {
    const acc_cond::acc_code& code = ref->get_acceptance();
    if (!code.is_dnf())
      throw std::runtime_error("dnf_to_nca() only works with DNF acceptance "
                               "condition");

    // FIXME: At the moment this algorithm does not support
    // transition-based acceptance.  See issue #317.  Once that is
    // fixed we may remove the next line.
    ref = sbacc(std::const_pointer_cast<twa_graph>(ref));

    auto streett_aut = spot::dnf_to_streett(ref, true);

    std::vector<acc_cond::rs_pair> pairs;
    if (!streett_aut->acc().is_streett_like(pairs))
      throw std::runtime_error("dnf_to_nca() could not convert the original "
          "automaton into an intermediate Streett-like automaton");

    nsa_to_nca_converter nca_converter(streett_aut,
                                       ref,
                                       pairs,
                                       named_states,
                                       true,
                                       ref->num_states());
    return nca_converter.run(nca_info);
  }

  namespace
  {
    twa_graph_ptr
    weak_to_cobuchi(const const_twa_graph_ptr& aut)
    {
      trival iw = aut->prop_inherently_weak();
      if (iw.is_false())
        return nullptr;
      scc_info si(aut);
      if (iw.is_maybe() && !is_weak_automaton(aut, &si))
        return nullptr;
      auto res = make_twa_graph(aut->get_dict());
      res->copy_ap_of(aut);
      res->prop_copy(aut, twa::prop_set::all());
      res->new_states(aut->num_states());
      si.determine_unknown_acceptance();
      unsigned ns = si.scc_count();
      for (unsigned s = 0; s < ns; ++s)
        {
          acc_cond::mark_t m = {};
          if (si.is_rejecting_scc(s))
            m = acc_cond::mark_t{0};
          else
            assert(si.is_accepting_scc(s));

          for (auto& e: si.edges_of(s))
            res->new_edge(e.src, e.dst, e.cond, m);
        }
      res->set_co_buchi();
      res->set_init_state(aut->get_init_state_number());
      res->prop_weak(true);
      res->prop_state_acc(true);
      return res;
    }
  }

  twa_graph_ptr
  to_nca(const_twa_graph_ptr aut, bool named_states)
  {
    if (aut->acc().is_co_buchi())
      return make_twa_graph(aut, twa::prop_set::all());

    if (auto weak = weak_to_cobuchi(aut))
      return weak;

    const acc_cond::acc_code& code = aut->get_acceptance();

    std::vector<acc_cond::rs_pair> pairs;
    if (aut->acc().is_streett_like(pairs) || aut->acc().is_parity())
      return nsa_to_nca(aut, named_states);
    else if (code.is_dnf())
      return dnf_to_nca(aut, named_states);

    auto tmp = make_twa_graph(aut, twa::prop_set::all());
    tmp->set_acceptance(aut->acc().num_sets(),
                        aut->get_acceptance().to_dnf());
    return to_nca(tmp, named_states);
  }

  namespace
  {
    struct mp_hash
    {
      size_t
      operator()(std::pair<unsigned, const bitvect_array*> bv) const noexcept
      {
        size_t res = 0;
        size_t size = bv.second->size();
        for (unsigned i = 0; i < size; ++i)
          res = wang32_hash(res ^ wang32_hash(bv.second->at(i).hash()));
        res = wang32_hash(res ^ bv.first);
        return res;
      }
    };

    struct mp_equal
    {
      bool operator()(std::pair<unsigned, const bitvect_array*> bvl,
                      std::pair<unsigned, const bitvect_array*> bvr) const
      {
        if (bvl.first != bvr.first)
          return false;
        size_t size = bvl.second->size();
        for (unsigned i = 0; i < size; ++i)
          if (bvl.second->at(i) != bvr.second->at(i))
            return false;
        return true;
      }
    };

    typedef std::unordered_map
      <std::pair<unsigned, const bitvect_array*>, unsigned, mp_hash, mp_equal>
      dca_st_mapping;
    class dca_breakpoint_cons final
    {
      protected:
        const_twa_graph_ptr aut_;                        // The given automaton.
        vect_nca_info* nca_info_;                        // Info (cf Header).
        unsigned nb_copy_;                               // != 0 if was Rabin.
        bdd ap_;                                         // All AP.
        std::vector<bdd> num2bdd_;                       // Get bdd from AP num.
        std::map<bdd, unsigned, bdd_less_than> bdd2num_; // Get AP num from bdd.

        // Each state is characterized by a bitvect_array of 2 bitvects:
        // bv1 -> the set of original states that it represents
        // bv2 -> a set of marked states (~)
        // To do so, we keep a correspondance between a state number and its
        // bitvect representation.
        dca_st_mapping bv_to_num_;
        std::vector<std::pair<unsigned, bitvect_array*>> num_2_bv_;

        // Next states to process.
        std::stack<std::pair<std::pair<unsigned, bitvect_array*>, unsigned>>
          todo_;
        // All allocated bitvect that must be freed at the end.
        std::vector<const bitvect_array*> toclean_;

      public:
        dca_breakpoint_cons(const const_twa_graph_ptr aut,
                            vect_nca_info* nca_info,
                            unsigned nb_copy)
          : aut_(aut),
            nca_info_(nca_info),
            nb_copy_(nb_copy),
            ap_(aut->ap_vars())
        {
          //  Get all bdds.
          bdd all = bddtrue;
          for (unsigned i = 0; all != bddfalse; ++i)
            {
              bdd one = bdd_satoneset(all, ap_, bddfalse);
              num2bdd_.push_back(one);
              bdd2num_[one] = i;
              all -= one;
            }
        }

        ~dca_breakpoint_cons()
        {
          for (auto p : *nca_info_)
            delete p;
        }

        twa_graph_ptr run(bool named_states)
        {
          unsigned ns = aut_->num_states();
          unsigned nc = num2bdd_.size();

          // Fill bv_aut_trans_ which is a bitvect of all possible transitions
          // of each state for each letter.
          auto bv_aut_trans_ = std::unique_ptr<bitvect_array>(
              make_bitvect_array(ns, ns * nc));
          for (unsigned src = 0; src < ns; ++src)
            {
              size_t base = src * nc;
              for (auto& t: aut_->out(src))
                {
                  bdd all = t.cond;
                  while (all != bddfalse)
                    {
                      bdd one = bdd_satoneset(all, ap_, bddfalse);
                      all -= one;
                      bv_aut_trans_->at(base + bdd2num_[one]).set(t.dst);
                    }
                }
            }
          trace << "All_states:\n" << *bv_aut_trans_ << '\n';

          twa_graph_ptr res = make_twa_graph(aut_->get_dict());
          res->copy_ap_of(aut_);
          res->set_co_buchi();

          // Rename states of resulting automata (for display purposes).
          std::vector<std::string>* state_name = nullptr;
          if (named_states)
            {
              state_name = new std::vector<std::string>();
              res->set_named_prop("state-names", state_name);
            }

          // Function used to add a new state. A new state number is associated
          // to the state if has never been added before, otherwise the old
          // state number is returned.
          auto new_state = [&](std::pair<unsigned, bitvect_array*> bv_st)
            -> unsigned
          {
            auto p = bv_to_num_.emplace(bv_st, 0);
            if (!p.second)
              return p.first->second;

            p.first->second = res->new_state();
            todo_.emplace(bv_st, p.first->second);
            assert(num_2_bv_.size() == p.first->second);
            num_2_bv_.push_back(bv_st);

            // For display purposes only.
            if (named_states)
              {
                assert(p.first->second == state_name->size());
                std::ostringstream os;
                bool not_first = false;
                os << '{';
                for (unsigned s = 0; s < ns; ++s)
                  {
                    if (bv_st.second->at(1).get(s))
                      {
                        if (not_first)
                          os << ',';
                        else
                          not_first = true;
                        os << '~';
                      }
                    if (bv_st.second->at(0).get(s))
                      os << s;
                  }
                os << '|' << bv_st.first << '}';
                state_name->emplace_back(os.str());
              }
            return p.first->second;
          };

          // Set init state
          auto bv_init = make_bitvect_array(ns, 2);
          toclean_.push_back(bv_init);
          bv_init->at(0).set(aut_->get_init_state_number());
          res->set_init_state(new_state(std::make_pair(0, bv_init)));

          // Processing loop
          while (!todo_.empty())
            {
              auto top = todo_.top();
              todo_.pop();

              // Bitvect array of all possible moves for each letter.
              auto bv_trans = make_bitvect_array(ns, nc);
              for (unsigned s = 0; s < ns; ++s)
                if (top.first.second->at(0).get(s))
                  for (unsigned l = 0; l < nc; ++l)
                    bv_trans->at(l) |= bv_aut_trans_->at(s * nc + l);
              toclean_.push_back(bv_trans);

              // Bitvect array of all possible moves for each state marked
              // for each letter. If no state is marked (breakpoint const.),
              // nothing is set.
              bool marked = !top.first.second->at(1).is_fully_clear();
              auto bv_trans_mark = make_bitvect_array(ns, nc);
              if (marked)
                for (unsigned s = 0; s < ns; ++s)
                  if (top.first.second->at(1).get(s))
                    for (unsigned l = 0; l < nc; ++l)
                      bv_trans_mark->at(l) |= bv_aut_trans_->at(s * nc + l);
              toclean_.push_back(bv_trans_mark);

              trace << "src:" << top.second;
              if (named_states)
                trace << ' ' << (*state_name)[top.second];
              trace << '\n';

              for (unsigned l = 0; l < nc; ++l)
                {
                  trace << "l: "
                        << bdd_format_formula(aut_->get_dict(), num2bdd_[l])
                        << '\n';

                  auto bv_res = make_bitvect_array(ns, 2);
                  toclean_.push_back(bv_res);
                  bv_res->at(0) |= bv_trans->at(l);
                  // If this state has not any outgoing edges.
                  if (bv_res->at(0).is_fully_clear())
                    continue;

                  // Set states that must be marked.
                  for (const auto& p : *nca_info_)
                    {
                      if (p->clause_num != top.first.first)
                        continue;

                      if (*p->all_dst == bv_res->at(0))
                        if ((marked && bv_trans_mark->at(l).get(p->state_num))
                             || (!marked && bv_res->at(0).get(p->state_num)))
                          bv_res->at(1).set(p->state_num);
                    }

                  unsigned i = bv_res->at(1).is_fully_clear() ?
                                (top.first.first + 1) % (nb_copy_ + 1)
                              : top.first.first;

                  trace << "dest\n" << *bv_res << "i: " << i << '\n';
                  res->new_edge(top.second,
                                new_state(std::make_pair(i, bv_res)),
                                num2bdd_[l]);
                }
              trace << '\n';
            }

          // Set rejecting states.
          scc_info si(res);
          bool state_based = (bool)aut_->prop_state_acc();
          unsigned res_size = res->num_states();
          for (unsigned s = 0; s < res_size; ++s)
            {
              unsigned s_scc = si.scc_of(s);
              if (num_2_bv_[s].second->at(1).is_fully_clear())
                for (auto& edge : res->out(s))
                  if (state_based || si.scc_of(edge.dst) == s_scc)
                    edge.acc = acc_cond::mark_t({0});
            }

          // Delete all bitvect arrays allocated by this method (run).
          for (auto v: toclean_)
            delete v;

          res->merge_edges();
          return res;
        }
    };
  }


  twa_graph_ptr
  nsa_to_dca(const_twa_graph_ptr aut, bool named_states)
  {
    trace << "NSA_to_dca\n";
    std::vector<acc_cond::rs_pair> pairs;
    if (!aut->acc().is_streett_like(pairs) && !aut->acc().is_parity())
      throw std::runtime_error("nsa_to_dca() only works with Streett-like or "
                               "Parity acceptance condition");

    // FIXME: At the moment this algorithm does not support
    // transition-based acceptance.  See issue #317.  Once that is
    // fixed we may remove the next line.
    aut = sbacc(std::const_pointer_cast<twa_graph>(aut));

    // Get states that must be visited infinitely often in NCA.
    vect_nca_info nca_info;
    nsa_to_nca(aut, named_states, &nca_info);

#if TRACE
    trace << "PRINTING INFO\n";
    for (unsigned i = 0; i < nca_info.size(); ++i)
      trace << '<' << nca_info[i]->clause_num << ',' << nca_info[i]->state_num
            << ',' << *nca_info[i]->all_dst << ">\n";
#endif

    dca_breakpoint_cons dca(aut, &nca_info, 0);
    return dca.run(named_states);
  }


  twa_graph_ptr
  dnf_to_dca(const_twa_graph_ptr aut, bool named_states)
  {
    trace << "DNF_to_dca\n";
    const acc_cond::acc_code& code = aut->get_acceptance();
    if (!code.is_dnf())
      throw std::runtime_error("dnf_to_dca() only works with DNF (Rabin-like "
                               "included) acceptance condition");

    // FIXME: At the moment this algorithm does not support
    // transition-based acceptance.  See issue #317.  Once that is
    // fixed we may remove the next line.
    aut = sbacc(std::const_pointer_cast<twa_graph>(aut));

    // Get states that must be visited infinitely often in NCA.
    vect_nca_info nca_info;
    dnf_to_nca(aut, false, &nca_info);

#if TRACE
    trace << "PRINTING INFO\n";
    for (unsigned i = 0; i < nca_info.size(); ++i)
      trace << '<' << nca_info[i]->clause_num << ',' << nca_info[i]->state_num
            << ',' << *nca_info[i]->all_dst << ">\n";
#endif

    unsigned nb_copy = 0;
    for (const auto& p : nca_info)
      if (nb_copy < p->clause_num)
        nb_copy = p->clause_num;

    dca_breakpoint_cons dca(aut, &nca_info, nb_copy);
    return dca.run(named_states);
  }


  twa_graph_ptr
  to_dca(const_twa_graph_ptr aut, bool named_states)
  {
    if (is_deterministic(aut))
      {
        if (aut->acc().is_co_buchi())
          return make_twa_graph(aut, twa::prop_set::all());
        if (auto weak = weak_to_cobuchi(aut))
          return weak;
      }

    const acc_cond::acc_code& code = aut->get_acceptance();

    std::vector<acc_cond::rs_pair> pairs;
    if (aut->acc().is_streett_like(pairs) || aut->acc().is_parity())
      return nsa_to_dca(aut, named_states);
    else if (code.is_dnf())
      return dnf_to_dca(aut, named_states);

    auto tmp = make_twa_graph(aut, twa::prop_set::all());
    tmp->set_acceptance(aut->acc().num_sets(),
                        aut->get_acceptance().to_dnf());
    return to_nca(tmp, named_states);
  }
}
