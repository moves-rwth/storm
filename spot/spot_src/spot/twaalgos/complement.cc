// -*- coding: utf-8 -*-
// Copyright (C) 2013-2015, 2017-2020 Laboratoire de Recherche et
// Développement de l'Epita.
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
#include <deque>
#include <map>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  twa_graph_ptr
  dtwa_complement(const const_twa_graph_ptr& aut)
  {
    if (!is_deterministic(aut))
      throw
        std::runtime_error("dtwa_complement() requires a deterministic input");

    return dualize(aut);
  }

  namespace
  {
    enum ncsb
      {
        ncsb_n = 0,       // non deterministic
        ncsb_c = 2,       // needs check
        ncsb_cb = 3,      // needs check AND in breakpoint
        ncsb_s = 4,       // safe
        ncsb_m = 1,       // missing
      };

    typedef std::vector<ncsb> mstate;
    typedef std::vector<std::pair<unsigned, ncsb>> small_mstate;

    struct small_mstate_hash
    {
      size_t
      operator()(small_mstate s) const noexcept
      {
        size_t hash = 0;
        for (const auto& p: s)
          {
            hash = wang32_hash(hash ^ ((p.first<<2) | p.second));
          }
        return hash;
      }
    };

    class ncsb_complementation
    {
    private:
      // The source automaton.
      const const_twa_graph_ptr aut_;

      // SCCs information of the source automaton.
      scc_info si_;

      // Number of states in the input automaton.
      unsigned nb_states_;

      // The complement being built.
      twa_graph_ptr res_;

      // Association between NCSB states and state numbers of the
      // complement.
      std::unordered_map<small_mstate, unsigned, small_mstate_hash> ncsb2n_;

      // States to process.
      std::deque<std::pair<mstate, unsigned>> todo_;

      // Support for each state of the source automaton.
      std::vector<bdd> support_;

      // Propositions compatible with all transitions of a state.
      std::vector<bdd> compat_;

      // Whether a SCC is deterministic or not
      std::vector<bool> is_deter_;

      // Whether a state only has accepting transitions
      std::vector<bool> is_accepting_;

      // State names for graphviz display
      std::vector<std::string>* names_;

      // Show NCSB states in state name to help debug
      bool show_names_;

      std::string
      get_name(const small_mstate& ms)
      {
        std::string res = "{";

        bool first_state = true;
        for (const auto& p: ms)
          if (p.second == ncsb_n)
            {
              if (!first_state)
                res += ",";
              first_state = false;
              res += std::to_string(p.first);
            }

        res += "},{";

        first_state = true;
        for (const auto& p: ms)
          if (p.second & ncsb_c)
            {
              if (!first_state)
                res += ",";
              first_state = false;
              res += std::to_string(p.first);
            }

        res += "},{";

        first_state = true;
        for (const auto& p: ms)
          if (p.second == ncsb_s)
            {
              if (!first_state)
                res += ",";
              first_state = false;
              res += std::to_string(p.first);
            }

        res += "},{";

        first_state = true;
        for (const auto& p: ms)
          if (p.second == ncsb_cb)
            {
              if (!first_state)
                res += ",";
              first_state = false;
              res += std::to_string(p.first);
            }

        return res + "}";
      }

      small_mstate
      to_small_mstate(const mstate& ms)
      {
        unsigned count = 0;
        for (unsigned i = 0; i < nb_states_; ++i)
          count+= (ms[i] != ncsb_m);
        small_mstate small;
        small.reserve(count);
        for (unsigned i = 0; i < nb_states_; ++i)
          if (ms[i] != ncsb_m)
            small.emplace_back(i, ms[i]);
        return small;
      }

      // From a NCSB state, looks for a duplicate in the map before
      // creating a new state if needed.
      unsigned
      new_state(mstate&& s)
      {
        auto p = ncsb2n_.emplace(to_small_mstate(s), 0);
        if (p.second) // This is a new state
          {
            p.first->second = res_->new_state();
            if (show_names_)
              names_->push_back(get_name(p.first->first));
            todo_.emplace_back(std::move(s), p.first->second);
          }
        return p.first->second;
      }

      void
      ncsb_successors(mstate&& ms, unsigned origin, bdd letter)
      {
        std::vector<mstate> succs;
        succs.emplace_back(nb_states_, ncsb_m);

        // Handle S states.
        //
        // Treated first because we can escape early if the letter
        // leads to an accepting transition for a Safe state.
        for (unsigned i = 0; i < nb_states_; ++i)
          {
            if (ms[i] != ncsb_s)
              continue;

            for (const auto& t: aut_->out(i))
              {
                if (!bdd_implies(letter, t.cond))
                  continue;
                if (t.acc || is_accepting_[t.dst])
                  // Exit early; transition is forbidden for safe
                  // state.
                  return;

                succs[0][t.dst] = ncsb_s;

                // No need to look for other compatible transitions
                // for this state; it's in the deterministic part of
                // the automaton
                break;
              }
          }

        // Handle C states.
        for (unsigned i = 0; i < nb_states_; ++i)
          {
            if (!(ms[i] & ncsb_c))
              continue;

            bool has_succ = false;
            for (const auto& t: aut_->out(i))
              {
                if (!bdd_implies(letter, t.cond))
                  continue;

                has_succ = true;

                // state can become safe, if transition is accepting
                // and destination isn't an accepting state
                if (t.acc)
                  {
                    // double all the current possible states
                    unsigned length = succs.size();
                    for (unsigned j = 0; j < length; ++j)
                      {
                        if (succs[j][t.dst] == ncsb_m)
                          {
                            if (!is_accepting_[t.dst])
                              {
                                succs.push_back(succs[j]);
                                succs.back()[t.dst] = ncsb_s;
                              }
                            succs[j][t.dst] = ncsb_c;
                          }
                      }
                  }
                else       // state stays in check
                  {
                    // remove states that should stay in s (ncsb_s),
                    // and mark the other as ncsb_c.
                    // The first two loops form a kind of remove_if()
                    // that set the non-removed states to ncsb_c.
                    auto it = succs.begin();
                    auto end = succs.end();
                    for (; it != end; ++it)
                      if ((*it)[t.dst] != ncsb_s)
                        (*it)[t.dst] = ncsb_c;
                      else
                        break;
                    if (it != end)
                      for (auto it2 = it; ++it2 != end;)
                        if ((*it2)[t.dst] != ncsb_s)
                          *it++ = std::move(*it2);
                    succs.erase(it, end);
                  }
                // No need to look for other compatible transitions
                // for this state; it's in the deterministic part of
                // the automaton
                break;
              }
            if (!has_succ && !is_accepting_[i])
              return;
          }

        // Handle N states.
        for (unsigned i = 0; i < nb_states_; ++i)
          {
            if (ms[i] != ncsb_n)
              continue;
            for (const auto& t: aut_->out(i))
              {
                if (!bdd_implies(letter, t.cond))
                  continue;

                if (is_deter_[si_.scc_of(t.dst)])
                  {
                    // double all the current possible states
                    unsigned length = succs.size();
                    for (unsigned j = 0; j < length; ++j)
                      {
                        if (succs[j][t.dst] == ncsb_m)
                          {
                            // Can become safe if the destination is
                            // not an accepting state.
                            if (!is_accepting_[t.dst])
                              {
                                succs.push_back(succs[j]);
                                succs.back()[t.dst] = ncsb_s;
                              }
                            succs[j][t.dst] = ncsb_c;
                          }
                      }
                  }
                else
                  for (auto& succ: succs)
                    succ[t.dst] = ncsb_n;
              }
          }

        // Revisit B states to see if they still exist in successors.
        // This is done at the end because we need to know all of the
        // states present in C before this stage
        bool b_empty = true;
        for (unsigned i = 0; i < nb_states_; ++i)
          {
            if (ms[i] != ncsb_cb)
              continue;

            // The original B set wasn't empty
            b_empty = false;

            for (const auto& t: aut_->out(i))
              {
                if (!bdd_implies(letter, t.cond))
                  continue;

                for (auto& succ: succs)
                  {
                    if (succ[t.dst] == ncsb_c)
                      succ[t.dst] = ncsb_cb;
                  }

                // No need to look for other compatible transitions
                // for this state; it's in the deterministic part of
                // the automaton
                break;
              }
          }

        // If B was empty, then set every c_not_b to cb in successors
        if (b_empty)
          for (auto& succ: succs)
            for (unsigned i = 0; i < succ.size(); ++i)
              if (succ[i] == ncsb_c)
                succ[i] = ncsb_cb;

        // create the automaton states
        for (auto& succ: succs)
          {
            bool b_empty = true;
            for (const auto& state: succ)
              if (state == ncsb_cb)
                {
                  b_empty = false;
                  break;
                }
            if (b_empty) // becomes accepting
              {
                for (unsigned i = 0; i < succ.size(); ++i)
                  if (succ[i] == ncsb_c)
                    succ[i] = ncsb_cb;
                unsigned dst = new_state(std::move(succ));
                res_->new_edge(origin, dst, letter, {0});
              }
            else
              {
                unsigned dst = new_state(std::move(succ));
                res_->new_edge(origin, dst, letter);
              }
          }
      }

    public:
      ncsb_complementation(const const_twa_graph_ptr& aut, bool show_names)
        : aut_(aut),
          si_(aut),
          nb_states_(aut->num_states()),
          support_(nb_states_),
          compat_(nb_states_),
          is_accepting_(nb_states_),
          show_names_(show_names)
      {
        res_ = make_twa_graph(aut->get_dict());
        res_->copy_ap_of(aut);
        res_->set_buchi();

        // Generate bdd supports and compatible options for each state.
        // Also check if all its transitions are accepting.
        for (unsigned i = 0; i < nb_states_; ++i)
          {
            bdd res_support = bddtrue;
            bdd res_compat = bddfalse;
            bool accepting = true;
            bool has_transitions = false;
            for (const auto& out: aut->out(i))
              {
                has_transitions = true;
                res_support &= bdd_support(out.cond);
                res_compat |= out.cond;
                if (!out.acc)
                  accepting = false;
              }
            support_[i] = res_support;
            compat_[i] = res_compat;
            is_accepting_[i] = accepting && has_transitions;
          }



        // Compute which SCCs are part of the deterministic set.
        is_deter_ = semidet_sccs(si_);

        if (show_names_)
          {
            names_ = new std::vector<std::string>();
            res_->set_named_prop("state-names", names_);
          }

        // Because we only handle one initial state, we assume it
        // belongs to the N set. (otherwise the automaton would be
        // deterministic)
        unsigned init_state = aut->get_init_state_number();
        mstate new_init_state(nb_states_, ncsb_m);
        new_init_state[init_state] = ncsb_n;
        res_->set_init_state(new_state(std::move(new_init_state)));
      }

      twa_graph_ptr
      run()
      {
        // Main stuff happens here

        while (!todo_.empty())
          {
            auto top = todo_.front();
            todo_.pop_front();

            mstate ms = top.first;

            // Compute support of all available states.
            bdd msupport = bddtrue;
            bdd n_s_compat = bddfalse;
            bdd c_compat = bddtrue;
            bool c_empty = true;
            for (unsigned i = 0; i < nb_states_; ++i)
              if (ms[i] != ncsb_m)
                {
                  msupport &= support_[i];
                  if (ms[i] == ncsb_n || ms[i] == ncsb_s || is_accepting_[i])
                    n_s_compat |= compat_[i];
                  else
                    {
                      c_empty = false;
                      c_compat &= compat_[i];
                    }
                }

            bdd all;
            if (!c_empty)
              all = c_compat;
            else
              {
                all = n_s_compat;
                if (all != bddtrue)
                  {
                    mstate empty_state(nb_states_, ncsb_m);
                    res_->new_edge(top.second,
                                   new_state(std::move(empty_state)),
                                   !all,
                                   {0});
                  }
              }
            while (all != bddfalse)
              {
                bdd one = bdd_satoneset(all, msupport, bddfalse);
                all -= one;

                // Compute all new states available from the generated
                // letter.
                ncsb_successors(std::move(ms), top.second, one);
              }
          }

        res_->merge_edges();
        return res_;
      }
    };

  }

  twa_graph_ptr
  complement_semidet(const const_twa_graph_ptr& aut, bool show_names)
  {
    if (!is_semi_deterministic(aut))
      throw std::runtime_error
        ("complement_semidet() requires a semi-deterministic input");

    auto ncsb = ncsb_complementation(aut, show_names);
    return ncsb.run();
  }

  twa_graph_ptr
  complement(const const_twa_graph_ptr& aut, const output_aborter* aborter)
  {
    if (!aut->is_existential() || is_universal(aut))
      return dualize(aut);
    if (is_very_weak_automaton(aut))
      return remove_alternation(dualize(aut), aborter);
    // Determinize
    spot::option_map m;
    if (aborter)
      {
        m.set("det-max-states", aborter->max_states());
        m.set("det-max-edges", aborter->max_edges());
      }
    if (aut->num_states() > 32)
      {
        m.set("ba-simul", 0);
        m.set("simul", 0);
      }
    spot::postprocessor p(&m);
    p.set_type(spot::postprocessor::Generic);
    p.set_pref(spot::postprocessor::Deterministic);
    p.set_level(spot::postprocessor::Low);
    auto det = p.run(std::const_pointer_cast<twa_graph>(aut));
    if (!det || !is_universal(det))
      return nullptr;
    return dualize(det);
  }
}
