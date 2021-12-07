// -*- coding: utf-8 -*-
// Copyright (C) 2017-2019 Laboratoire de Recherche et Développement
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
#include <spot/misc/minato.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/twaalgos/cleanacc.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/sbacc.hh>

#include <memory>

namespace spot
{
  namespace
  {
    class dualizer final
    {
    private:
      // Input automaton
      const const_twa_graph_ptr aut_;
      // State id to bdd variable association
      // [id] == bddtrue means the state will be accepting everything
      // (true state) in the dual automaton.
      // [id] == bddfalse means the state will be rejecting everything
      // (sink) in the dual automaton, which can lead to the removal of
      // the transitions going into that state and therefore to the removal
      // of the state when applying purge_unreachable_states().
      // When [id] != bddtrue/bddfalse, the value correspond to the state
      // bdd variable.
      std::vector<bdd> state_to_var_;
      // bdd variable to state id association
      std::map<int, unsigned> var_to_state_;
      // Acceptance mark to bdd variable association
      std::vector<int> mark_to_var_;
      // bdd variable to acceptance mark association
      std::map<int, unsigned> var_to_mark_;
      // bdd representing all the state variables
      bdd all_states_;
      // bdd representing all the marks variables
      bdd all_marks_;
      // bdd representing states & marks variables
      bdd all_vars_;
      // Id of the true state sink. -1U if none.
      unsigned true_state_;
      // In case the acceptance condition is never unsatisfied, but the
      // automaton is not complete, we do transform the acceptance condition
      // to Büchi, and need to mark all the existing transitions with the
      // proper mark, that will be stored in acc_. 0U if not used.
      acc_cond::mark_t acc_;
      // Whether aut_ has a state accepting all.
      bool has_sink;

      // Any sink state in the input automaton will become a true state in the
      // output. Knowing those allows us to simplify some universal transitions.
      // There could be more than one sink state in the input automaton, and in
      // this case they will be squashed into a single true state in the output
      // automaton.
      void find_sink_states(acc_cond::mark_t& second)
      {
        // Loop over the states and search a state that has no outgoing
        // transitions, or only self-loops labeled by the same non-accepting
        // mark. A similar test is done in complete_here().
        unsigned n = aut_->num_states();
        for (unsigned i = 0; i < n; ++i)
          {
            bool sinkable = true;
            bool first = true;
            acc_cond::mark_t commonacc = second;
            for (auto& t: aut_->out(i))
              {
                if (t.dst != i)
                  {
                    sinkable = false;
                    break;
                  }
                if (first)
                  {
                    commonacc = t.acc;
                    first = false;
                  }
                else if (t.acc != commonacc)
                  {
                    sinkable = false;
                    break;
                  }
              }
            if (sinkable && !aut_->acc().accepting(commonacc))
              {
                second = commonacc;
                state_to_var_[i] = bddtrue;
                true_state_ = i;
                has_sink = true;
              }
          }
      }
      // Any true state in the input automaton will become a sink state in the
      // output. Knowing those allow us to simplify some universal transitions.
      // There could be more than one true state in the input automaton. All
      // true states from the input automaton will be removed in the output
      // and the transitions leading to those states will be removed as well.
      void find_true_states()
      {
        // Loop over the states and search a state that has a self-loop on
        // any letter (bddtrue), with an accepting mark.
        unsigned n = aut_->num_states();
        for (unsigned i = 0; i < n; ++i)
          {
            bool acc_all = false;
            for (auto& t: aut_->out(i))
              {
                if (t.dst == i && t.cond == bddtrue
                               && aut_->acc().accepting(t.acc))
                  {
                    acc_all = true;
                    break;
                  }
              }
            if (acc_all)
              {
                state_to_var_[i] = bddfalse;
                has_sink = true;
              }
          }
      }

      void copy_edges(const twa_graph_ptr &res)
      {
        std::vector<unsigned> st;
        unsigned n = aut_->num_states();
        for (unsigned i = 0; i < n; ++i)
          {
            bdd delta = dualized_transition_function(i);
            bdd ap = bdd_exist(bdd_support(delta), all_vars_);
            bdd letters = bdd_exist(delta, all_vars_);

            while (letters != bddfalse)
              {
                bdd oneletter = bdd_satoneset(letters, ap, bddtrue);
                letters -= oneletter;

                minato_isop isop(delta & oneletter);
                bdd cube;

                while ((cube = isop.next()) != bddfalse)
                  {
                    bdd cond = bdd_exist(cube, all_vars_);
                    bdd dest = bdd_existcomp(cube, all_vars_);

                    st.clear();
                    acc_cond::mark_t m = bdd_to_state(dest, st);
                    if  (st.empty())
                      {
                        st.push_back(true_state_);
                        if (aut_->prop_state_acc())
                          m = aut_->state_acc_sets(i);
                      }
                    res->new_univ_edge(i, st.begin(), st.end(), cond, m);
                  }
              }
          }
      }

      // Handles the dualization of a universal initial transition.
      // In theory the transition would be split into several
      // existential initial transitions, but since Spot does not
      // allow multiple initial states, we rather use a trick: We add
      // a new initial state, and then copy all exiting transitions
      // from each destination states of the universal initial
      // transition.
      void univ_init(const twa_graph_ptr& res)
      {
        bdd comb = bddfalse;
        outedge_combiner oe(res);
        for (unsigned c : aut_->univ_dests(aut_->get_init_state_number()))
          comb |= oe(c);

        auto s = res->new_state();
        res->set_init_state(s);
        oe.new_dests(s, comb);
      }

      // Allocates the states and marks as variables into the bdd dictionary.
      // Also adds the corresponding mapping, and sets all_states_, all_marks_,
      // and all_vars_ to hold those variables as bdds.
      void allocate_dict_vars(const twa_graph_ptr& res)
      {
        auto dict = aut_->get_dict();

        unsigned numstates = aut_->num_states();
        all_states_ = bddtrue;
        for (unsigned i = 0; i < numstates; ++i)
          {
            int v = dict->register_anonymous_variables(1, this);
            if (state_to_var_[i] != bddtrue)
              state_to_var_[i] = bdd_ithvar(v);
            var_to_state_[v] = i;
            all_states_ &= bdd_ithvar(v);
          }

        unsigned numsets = res->num_sets();
        all_marks_ = bddtrue;
        for (unsigned i = 0; i < numsets; ++i)
          {
            int v = dict->register_anonymous_variables(1, this);
            mark_to_var_.push_back(v);
            var_to_mark_.emplace(v, i);
            all_marks_ &= bdd_ithvar(v);
          }
        all_vars_ = all_states_ & all_marks_;
      }

      // Returns the dualized transition function of any input state as a bdd.
      bdd dualized_transition_function(unsigned state_id)
      {
        if (state_to_var_[state_id] == bddtrue)
          return bddfalse;

        bdd res = bddtrue;
        for (auto& e : aut_->out(state_id))
          {
            bdd dest = bddfalse;
            for (unsigned d : aut_->univ_dests(e))
              dest |= state_to_var_[d];

            bdd mark_bdd = bddtrue;
            acc_cond::mark_t m = acc_ ? acc_ : e.acc;
            for (unsigned s: m.sets())
              mark_bdd &= bdd_ithvar(mark_to_var_[s]);

            res &= bdd_imp(e.cond, mark_bdd & dest);
          }
        return res;
      }

      // Given the bdd representation b of a transition, adds destination states
      // to s, and returns the marks on the transition. s being empty means the
      // transition goes toward a "forever true" state. s with size one
      // represents an existential transition, while size over one represents
      // a universal transition.
      acc_cond::mark_t bdd_to_state(bdd b, std::vector<unsigned>& s)
      {
        acc_cond::mark_t m = {};
        while (b != bddtrue)
          {
            assert(bdd_low(b) == bddfalse);
            int v = bdd_var(b);
            auto it = var_to_state_.find(v);
            if (it != var_to_state_.end())
              s.push_back(it->second);
            else
              m.set(var_to_mark_[v]);

            b = bdd_high(b);
          }
        return m;
      }

    public:
      dualizer(const const_twa_graph_ptr& aut)
        : aut_(is_universal(aut)
                ? aut
                : sbacc(std::const_pointer_cast<spot::twa_graph>(aut))),
          state_to_var_(aut_->num_states(), bddfalse),
          true_state_(-1U),
          acc_({}),
          has_sink(false)
      {
      }

      ~dualizer()
      {
        aut_->get_dict()->unregister_all_my_variables(this);
      }

      twa_graph_ptr run()
      {
        bool cmpl = is_complete(aut_);
        auto um = aut_->acc().unsat_mark();

        auto res = make_twa_graph(aut_->get_dict());
        res->copy_ap_of(aut_);

        if (!um.first && cmpl)
          {
            //Shortcut if dual is false
            res->new_states(1);
            res->new_edge(0, 0, bddtrue, {});
            res->set_init_state(0);
            res->set_acceptance(0, acc_cond::acc_code::f());

            res->prop_terminal(true);
            res->prop_complete(true);
            res->prop_universal(true);
            return res;
          }
        if (is_deterministic(aut_))
          {
            res = cleanup_acceptance_here(spot::complete(aut_));
            res->set_acceptance(res->num_sets(),
                                res->get_acceptance().complement());
            // Complementing the acceptance is likely to break the terminal
            // property, but not weakness.  We make a useless call to
            // prop_keep() just so we remember to update it in the future if a
            // new argument is added.
            res->prop_keep({true, true, true, true, true, true});
            res->prop_terminal(trival::maybe());
            return res;
          }

        const_twa_graph_ptr autptr;
        res->new_states(aut_->num_states());
        if (!cmpl)
          {
            if (!um.first)
              {
                acc_ = res->set_buchi();
                autptr = res;
              }
            else
              {
                find_sink_states(um.second);
                autptr = aut_;
              }
            if (true_state_ == -1U)
              true_state_ = res->new_state();
          }
        // This case does not cover cmpl && !um.first
        // Due to previous test shortcutting automata that accept all words.
        else
          {
            assert(um.first);
            find_sink_states(um.second);
            autptr = aut_;
          }
        if (true_state_ != -1U)
          res->new_edge(true_state_, true_state_, bddtrue, um.second);

        res->set_acceptance(autptr->num_sets(),
                            autptr->get_acceptance().complement());
        allocate_dict_vars(res);
        find_true_states();

        copy_edges(res);

        unsigned init_state = aut_->get_init_state_number();
        if (aut_->is_univ_dest(init_state))
          univ_init(res);
        else
          res->set_init_state(init_state);

        res->merge_edges();
        res->purge_unreachable_states();

        res->prop_copy(aut_, {true, true, false, false, false, true});
        res->prop_terminal(trival::maybe());
        if (!has_sink)
          res->prop_complete(true);

        cleanup_acceptance_here(res);
        return res;
      }
    };
  }

  twa_graph_ptr dualize(const const_twa_graph_ptr& aut)
  {
    dualizer du(aut);
    return du.run();
  }
}
