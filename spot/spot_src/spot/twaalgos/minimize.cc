// -*- coding: utf-8 -*-
// Copyright (C) 2010-2020 Laboratoire de Recherche et Développement
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


//#define TRACE
#ifdef TRACE
#  define trace std::cerr
#else
#  define trace while (0) std::cerr
#endif

#include "config.h"
#include <queue>
#include <deque>
#include <set>
#include <list>
#include <vector>
#include <sstream>
#include <spot/twaalgos/minimize.hh>
#include <spot/misc/hash.hh>
#include <spot/misc/bddlt.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/alternation.hh>
#include <spot/tl/hierarchy.hh>

namespace spot
{
  // This is called hash_set for historical reason, but we need the
  // order inside hash_set to be deterministic.
  typedef std::set<unsigned> hash_set;

  namespace
  {
    static std::ostream&
    dump_hash_set(const hash_set* hs,
                  std::ostream& out)
    {
      out << '{';
      const char* sep = "";
      for (auto i: *hs)
        {
          out << sep << i;
          sep = ", ";
        }
      out << '}';
      return out;
    }

    static std::string
    format_hash_set(const hash_set* hs)
    {
      std::ostringstream s;
      dump_hash_set(hs, s);
      return s.str();
    }

    // Find all states of an automaton.
    static void
    build_state_set(const const_twa_graph_ptr& a, hash_set* seen)
    {
      std::stack<unsigned> todo;
      unsigned init = a->get_init_state_number();
      todo.push(init);
      seen->insert(init);
      while (!todo.empty())
        {
          unsigned s = todo.top();
          todo.pop();
          for (auto& e: a->out(s))
            if (seen->insert(e.dst).second)
              todo.push(e.dst);
        }
    }

    // From the base automaton and the list of sets, build the minimal
    // resulting automaton
    static twa_graph_ptr
    build_result(const const_twa_graph_ptr& a,
                 std::list<hash_set*>& sets,
                 hash_set* final)
    {
      auto dict = a->get_dict();
      auto res = make_twa_graph(dict);
      res->copy_ap_of(a);
      res->prop_state_acc(true);

      // For each set, create a state in the output automaton.  For an
      // input state s, state_num[s] is the corresponding the state in
      // the output automaton.
      std::vector<unsigned> state_num(a->num_states(), -1U);
      {
        unsigned num = res->new_states(sets.size());
        for (hash_set* h: sets)
          {
            for (unsigned s: *h)
              state_num[s] = num;
            ++num;
          }
      }

      if (!final->empty())
        res->set_buchi();

      // For each transition in the initial automaton, add the
      // corresponding transition in res.
      for (hash_set* h: sets)
        {
          // Pick one state.
          unsigned src = *h->begin();
          unsigned src_num = state_num[src];
          bool accepting = (final->find(src) != final->end());

          // Connect it to all destinations.
          for (auto& e: a->out(src))
            {
              unsigned dn = state_num[e.dst];
              if ((int)dn < 0)  // Ignore useless destinations.
                continue;
              res->new_acc_edge(src_num, dn, e.cond, accepting);
            }
        }
      res->merge_edges();
      if (res->num_states() > 0)
        res->set_init_state(state_num[a->get_init_state_number()]);
      else
        res->set_init_state(res->new_state());
      return res;
    }

    static twa_graph_ptr minimize_dfa(const const_twa_graph_ptr& det_a,
                                      hash_set* final, hash_set* non_final)
    {
      typedef std::list<hash_set*> partition_t;
      partition_t cur_run;
      partition_t next_run;

      // The list of equivalent states.
      partition_t done;

      std::vector<unsigned> state_set_map(det_a->num_states(), -1U);

      // Size of det_a
      unsigned size = final->size() + non_final->size();
      // Use bdd variables to number sets.  set_num is the first variable
      // available.
      unsigned set_num =
        det_a->get_dict()->register_anonymous_variables(size, det_a);

      std::set<int> free_var;
      for (unsigned i = set_num; i < set_num + size; ++i)
        free_var.insert(i);
      std::map<int, int> used_var;

      hash_set* final_copy;

      if (!final->empty())
        {
          unsigned s = final->size();
          used_var[set_num] = s;
          free_var.erase(set_num);
          if (s > 1)
            cur_run.emplace_back(final);
          else
            done.emplace_back(final);
          for (auto i: *final)
            state_set_map[i] = set_num;

          final_copy = new hash_set(*final);
        }
      else
        {
          final_copy = final;
        }

      if (!non_final->empty())
        {
          unsigned s = non_final->size();
          unsigned num = set_num + 1;
          used_var[num] = s;
          free_var.erase(num);
          if (s > 1)
            cur_run.emplace_back(non_final);
          else
            done.emplace_back(non_final);
          for (auto i: *non_final)
            state_set_map[i] = num;
        }
      else
        {
          delete non_final;
        }

      // A bdd_states_map is a list of formulae (in a BDD form)
      // associated with a destination set of states.
      typedef std::map<bdd, hash_set*, bdd_less_than> bdd_states_map;

      bool did_split = true;

      while (did_split)
        {
          did_split = false;
          while (!cur_run.empty())
            {
              // Get a set to process.
              hash_set* cur = cur_run.front();
              cur_run.pop_front();

              trace << "processing " << format_hash_set(cur)
                    << std::endl;

              bdd_states_map bdd_map;
              for (unsigned src: *cur)
                {
                  bdd f = bddfalse;
                  for (auto si: det_a->out(src))
                    {
                      unsigned i = state_set_map[si.dst];
                      if ((int)i < 0)
                        // The destination state is not in our
                        // partition.  This can happen if the initial
                        // FINAL and NON_FINAL supplied to the algorithm
                        // do not cover the whole automaton (because we
                        // want to ignore some useless states).  Simply
                        // ignore these states here.
                        continue;
                      f |= (bdd_ithvar(i) & si.cond);
                    }

                  // Have we already seen this formula ?
                  bdd_states_map::iterator bsi = bdd_map.find(f);
                  if (bsi == bdd_map.end())
                    {
                      // No, create a new set.
                      hash_set* new_set = new hash_set;
                      new_set->insert(src);
                      bdd_map[f] = new_set;
                    }
                  else
                    {
                      // Yes, add the current state to the set.
                      bsi->second->insert(src);
                    }
                }

              auto bsi = bdd_map.begin();
              if (bdd_map.size() == 1)
                {
                  // The set was not split.
                  trace << "set " << format_hash_set(bsi->second)
                        << " was not split" << std::endl;
                  next_run.emplace_back(bsi->second);
                }
              else
                {
                  did_split = true;
                  for (; bsi != bdd_map.end(); ++bsi)
                    {
                      hash_set* set = bsi->second;
                      // Free the number associated to these states.
                      unsigned num = state_set_map[*set->begin()];
                      assert(used_var.find(num) != used_var.end());
                      unsigned left = (used_var[num] -= set->size());
                      // Make sure LEFT does not become negative (hence bigger
                      // than SIZE when read as unsigned)
                      assert(left < size);
                      if (left == 0)
                        {
                          used_var.erase(num);
                          free_var.insert(num);
                        }
                      // Pick a free number
                      assert(!free_var.empty());
                      num = *free_var.begin();
                      free_var.erase(free_var.begin());
                      used_var[num] = set->size();
                      for (unsigned s: *set)
                        state_set_map[s] = num;
                      // Trivial sets can't be split any further.
                      if (set->size() == 1)
                        {
                          trace << "set " << format_hash_set(set)
                                << " is minimal" << std::endl;
                          done.emplace_back(set);
                        }
                      else
                        {
                          trace << "set " << format_hash_set(set)
                                << " should be processed further" << std::endl;
                          next_run.emplace_back(set);
                        }
                    }
                }
              delete cur;
            }
          if (did_split)
            trace << "splitting did occur during this pass." << std::endl;
          else
            trace << "splitting did not occur during this pass." << std::endl;
          std::swap(cur_run, next_run);
        }

      done.splice(done.end(), cur_run);

#ifdef TRACE
      trace << "Final partition: ";
      for (hash_set* hs: done)
        trace << format_hash_set(hs) << ' ';
      trace << std::endl;
#endif

      // Build the result.
      auto res = build_result(det_a, done, final_copy);

      // Free all the allocated memory.
      delete final_copy;

      for (hash_set* hs: done)
        delete hs;

      return res;
    }
  }

  twa_graph_ptr minimize_monitor(const const_twa_graph_ptr& a)
  {
    if (!a->is_existential())
      throw std::runtime_error
        ("minimize_monitor() does not support alternation");

    hash_set* final = new hash_set;
    hash_set* non_final = new hash_set;
    twa_graph_ptr det_a = tgba_powerset(a);

    // non_final contain all states.
    // final is empty: there is no acceptance condition
    build_state_set(det_a, non_final);
    auto res = minimize_dfa(det_a, final, non_final);
    res->prop_copy(a, { false, false, false, false, true, true });
    res->prop_universal(true);
    res->prop_weak(true);
    res->prop_state_acc(true);
    // Quickly check if this is a terminal automaton
    for (auto& e: res->edges())
      if (e.src == e.dst && e.cond == bddtrue)
        {
          res->prop_terminal(true);
          break;
        }
    return res;
  }

  twa_graph_ptr minimize_wdba(const const_twa_graph_ptr& a,
                              const output_aborter* aborter)
  {
    if (!a->is_existential())
      throw std::runtime_error
        ("minimize_wdba() does not support alternation");

    hash_set* final;
    hash_set* non_final;

    twa_graph_ptr det_a;

    {
      power_map pm;
      bool input_is_det = is_deterministic(a);
      if (input_is_det)
        {
          det_a = std::const_pointer_cast<twa_graph>(a);
        }
      else
        {
          det_a = tgba_powerset(a, pm, aborter);
          if (!det_a)
            return nullptr;
        }

      // For each SCC of the deterministic automaton, determine if it
      // is accepting or not.

      // This corresponds to the algorithm in Fig. 1 of "Efficient
      // minimization of deterministic weak omega-automata" written by
      // Christof Löding and published in Information Processing
      // Letters 79 (2001) pp 105--109.

      // We also keep track of whether an SCC is useless
      // (i.e., it is not the start of any accepting word).

      scc_info sm(det_a);
      unsigned scc_count = sm.scc_count();

      // SCCs of det_a are assumed accepting if any of their loop
      // corresponds to an accepted word in the original automaton.
      // If the automaton is the same as det_a, we can simply ask that
      // to sm.
      std::vector<char> is_accepting_scc(scc_count, 0);
      if (input_is_det)
        {
          sm.determine_unknown_acceptance();
          for (unsigned m = 0; m < scc_count; ++m)
            is_accepting_scc[m] = sm.is_accepting_scc(m);
        }
      else
        {
          twa_graph_ptr prod = spot::product(a, det_a, aborter);
          if (!prod)
            return nullptr;

          const product_states* pmap =
            prod->get_named_prop<product_states>("product-states");
          assert(pmap);
          scc_info sip(prod, scc_info_options::TRACK_STATES_IF_FIN_USED);
          sip.determine_unknown_acceptance();
          unsigned prod_scc_count = sip.scc_count();
          for (unsigned m = 0; m < prod_scc_count; ++m)
            if (sip.is_accepting_scc(m))
              {
                unsigned right_state = (*pmap)[sip.one_state_of(m)].second;
                is_accepting_scc[sm.scc_of(right_state)] = true;
              }
        }

      final = new hash_set;
      non_final = new hash_set;

      // SCC that have been marked as useless.
      std::vector<bool> useless(scc_count);
      // The "color".  Even number correspond to
      // accepting SCCs.
      std::vector<unsigned> d(scc_count);

      // An even number larger than scc_count.
      unsigned k = (scc_count | 1) + 1;

      // SCC are numbered in topological order
      // (but in the reverse order as Löding's)
      for (unsigned m = 0; m < scc_count; ++m)
        {
          bool is_useless = true;
          bool transient = sm.is_trivial(m);
          auto& succ = sm.succ(m);

          // Compute the minimum color l of the successors.  Also SCCs
          // are useless if all their successor are useless.  Note
          // that Löding uses k-1 as level for non-final SCCs without
          // successors but that seems bogus: using k+1 will make sure
          // that a non-final SCCs without successor (i.e., a useless
          // SCC) will be ignored in the computation of the level.
          unsigned l = k + 1;
          for (unsigned j: succ)
            {
              is_useless &= useless[j];
              unsigned dj = d[j];
              if (dj < l)
                l = dj;
            }

          if (transient)
            {
              d[m] = l;
            }
          else
            {
              if (is_accepting_scc[m])
                {
                  is_useless = false;
                  d[m] = l & ~1; // largest even number inferior or equal
                }
              else
                {
                  if (succ.empty())
                    is_useless = true;
                  d[m] = (l - 1) | 1; // largest odd number inferior or equal
                }
            }

          useless[m] = is_useless;

          if (!is_useless)
            {
              hash_set* dest_set = (d[m] & 1) ? non_final : final;
              auto& con = sm.states_of(m);
              dest_set->insert(con.begin(), con.end());
            }
        }
    }

    auto res = minimize_dfa(det_a, final, non_final);
    res->prop_copy(a, { false, false, false, false, false, true });
    res->prop_universal(true);
    res->prop_weak(true);
    // If the input was terminal, then the output is also terminal.
    // FIXME:
    // (1) We should have a specialized version of this function for
    // the case where the input is terminal.  See issue #120.
    // (2) It would be nice to have a more precise detection of
    // terminal automata in the output.  Calling
    // is_terminal_automaton() seems overkill here.  But maybe we can
    // add a quick check inside minimize_dfa.
    if (a->prop_terminal())
      res->prop_terminal(true);
    return res;
  }

  // Declared in tl/hierarchy.cc, but defined here because it relies on
  // other internal functions from this file.
  SPOT_LOCAL bool is_wdba_realizable(formula f, twa_graph_ptr aut = nullptr);

  bool is_wdba_realizable(formula f, twa_graph_ptr aut)
  {
    if (f.is_syntactic_obligation())
      return true;

    if (aut == nullptr)
      aut = ltl_to_tgba_fm(f, make_bdd_dict(), true);

    if (!aut->is_existential())
      throw std::runtime_error
        ("is_wdba_realizable() does not support alternation");

    if ((f.is_syntactic_persistence() || aut->prop_weak())
        && (f.is_syntactic_recurrence() || is_deterministic(aut)))
      return true;

    if (is_terminal_automaton(aut))
      return true;

    // FIXME: we do not need to minimize the wdba to test realizability.
    auto min_aut = minimize_wdba(aut);

    twa_graph_ptr aut_neg;
    if (is_deterministic(aut))
      {
        aut_neg = remove_fin(dualize(aut));
      }
    else
      {
        aut_neg = ltl_to_tgba_fm(formula::Not(f), aut->get_dict());
        aut_neg = scc_filter(aut_neg, true);
      }

    if (is_terminal_automaton(aut_neg))
      return true;

    return product(min_aut, aut_neg)->is_empty();
  }

  bool minimize_obligation_garanteed_to_work(const const_twa_graph_ptr& aut_f,
                                             formula f)
  {
    // WDBA-minimization necessarily work for obligations
    return ((f && f.is_syntactic_obligation())
            // Weak deterministic automata are obligations
            || (aut_f->prop_weak().is_true() && is_deterministic(aut_f))
            // Guarantee automata are obligations as well.
            || is_terminal_automaton(aut_f));
  }

  twa_graph_ptr
  minimize_obligation(const const_twa_graph_ptr& aut_f,
                      formula f,
                      const_twa_graph_ptr aut_neg_f,
                      bool reject_bigger,
                      const output_aborter* aborter)
  {
    if (!aut_f->is_existential())
      throw std::runtime_error
        ("minimize_obligation() does not support alternation");

    bool minimization_will_be_correct = false;
    if (minimize_obligation_garanteed_to_work(aut_f, f))
      {
        minimization_will_be_correct = true;
      }
    else if (!aut_neg_f)
      {
        // The minimization might not be correct and will need to
        // be checked.   Are we able to build aut_neg_f?
        if (!(is_deterministic(aut_f) || f || is_very_weak_automaton(aut_f)))
          return nullptr;
      }

    // FIXME: We should build scc_info once, and reuse it between
    //  minimize_wdba is_terminal_automaton(), is_weak_automaton(),
    //  and is_very_weak_automaton().
    auto min_aut_f = minimize_wdba(aut_f, aborter);

    if (!min_aut_f)
      return std::const_pointer_cast<twa_graph>(aut_f);
    if (reject_bigger)
      {
        // Abort if min_aut_f has more states than aut_f.
        unsigned orig_states = aut_f->num_states();
        if (orig_states < min_aut_f->num_states())
          return std::const_pointer_cast<twa_graph>(aut_f);
      }

    if (minimization_will_be_correct)
      return min_aut_f;

    // The minimization might not be correct and will need to
    // be checked.   Build negation automaton if not supplied.
    if (!aut_neg_f)
      {
        if (is_deterministic(aut_f))
          {
            // If the automaton is deterministic, complementing is
            // easy.
            aut_neg_f = dualize(aut_f);
          }
        else if (f)
          {
            // If we know the formula, simply build the automaton for
            // its negation.
            aut_neg_f = ltl_to_tgba_fm(formula::Not(f), aut_f->get_dict());
            // Remove useless SCCs.
            aut_neg_f = scc_filter(aut_neg_f, true);
          }
        else if (is_very_weak_automaton(aut_f))
          {
            // Very weak automata are easy to complement.
            aut_neg_f = remove_alternation(dualize(aut_f));
          }
        else
          {
            // Otherwise, we don't try to complement the automaton and
            // therefore we cannot check if the minimization is safe.
            return nullptr;
          }
      }
    // Make sure the minimized WDBA does not accept more words than
    // the input.
    auto prod = product(min_aut_f, aut_neg_f, aborter);
    if (prod && prod->is_empty())
      {
        assert((bool)min_aut_f->prop_weak());
        return min_aut_f;
      }
    else
      {
        return std::const_pointer_cast<twa_graph>(aut_f);
      }
  }
}
