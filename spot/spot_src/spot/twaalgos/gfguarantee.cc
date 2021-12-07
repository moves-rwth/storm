// -*- coding: utf-8 -*-
// Copyright (C) 2018, 2021 Laboratoire de Recherche et Développement
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
#include "gfguarantee.hh"
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/dualize.hh>
#include <spot/twaalgos/isdet.hh>
// #include <spot/twa/bddprint.hh>

namespace spot
{
  namespace
  {
    // F(φ₁)&F(φ₂)&F(φ₃) ≡ F(φ₁ & F(φ₂ & F(φ₃))
    // because we assume this is all under G.
    static formula
    nest_f(formula input)
    {
      assert(input.is(op::And));
      formula res = formula::tt();
      unsigned n = input.size();
      do
        {
          --n;
          assert(input[n].is(op::F));
          res = formula::F(formula::And({input[n][0], res}));
        }
      while (n);
      return res;
    }


    static twa_graph_ptr
    do_g_f_terminal_inplace(scc_info& si, bool state_based)
    {
      bool want_merge_edges = false;
      twa_graph_ptr aut = std::const_pointer_cast<twa_graph>(si.get_aut());

      if (!is_terminal_automaton(aut, &si, true))
        throw std::runtime_error("g_f_terminal() expects a terminal automaton");

      unsigned ns = si.scc_count();
      std::vector<bool> term(ns, false);
      for (unsigned n = 0; n < ns; ++n)
        if (is_terminal_scc(si, n))
          term[n] = true;

      aut->prop_keep({ false, false, true, false, true, true });
      aut->prop_state_acc(state_based);
      aut->prop_inherently_weak(false);
      aut->set_buchi();

      unsigned init = aut->get_init_state_number();

      if (!state_based)
        {
          bool is_det = is_deterministic(aut);

          // If the initial state is a trivial SCC, we will be able to
          // remove it by combining the transitions leading to the
          // terminal state on the transitions leaving the initial
          // state.  However if some successor of the initial state is
          // also trivial, we might be able to remove it as well if we
          // are able to replay two step from the initial state: this
          // can be generalized to more depth and requires computing
          // some history for each state (i.e., a common suffix to all
          // finite words leading to this state).
          //
          // We will replay such histories regardless of whether there
          // is actually some trivial leading SCCs that could be
          // removed, because it reduces the size of cycles in the
          // automaton, and this helps getting smaller products when
          // combining several automata generated this way.
          std::vector<std::vector<bdd>> histories;
          bool initial_state_trivial = si.is_trivial(si.initial());

          if (is_det)
            {
              // Compute the max number of steps we want to keep in
              // the history of each state.  This is the length of the
              // maximal path we can build from the initial state.
              unsigned max_histories;
              {
                std::vector<unsigned> depths(ns, 0U);
                for (unsigned scc = 0; scc < ns; ++scc)
                  {
                    unsigned depth = 0;
                    for (unsigned succ: si.succ(scc))
                      depth = std::max(depth, depths[succ]);
                    depths[scc] = depth + si.states_of(scc).size();
                  }
                max_histories = depths[ns - 1] - 1;
              }

              unsigned numstates = aut->num_states();
              histories.resize(numstates);
              // Compute the one-letter history of each state.  If all
              // transition entering a state have the same label, the
              // history is that label.  Otherwise, we make a
              // disjunction of all those labels, so that what we have
              // is an over-approximation of the history.
              for (auto& e: aut->edges())
                {
                  std::vector<bdd>& hd = histories[e.dst];
                  if (hd.empty())
                    hd.push_back(e.cond);
                  else
                    hd[0] |= e.cond;
                }
              // check if there is a chance to build a larger history.
              bool should_continue = false;
              for (auto&h: histories)
                if (h.empty())
                  continue;
                else
                  should_continue = true;
              // Augment those histories with more letters.
              unsigned historypos = 0;
              while (should_continue && historypos + 1 < max_histories)
                {
                  ++historypos;
                  for (auto& e: aut->edges())
                    {
                      std::vector<bdd>& hd = histories[e.dst];
                      if (hd.size() >= historypos)
                        {
                          std::vector<bdd>& hs = histories[e.src];
                          if (hd.size() == historypos)
                            {
                              if (hs.size() >= historypos)
                                hd.push_back(hs[historypos - 1]);
                              else
                                hd.push_back(bddfalse);
                            }
                          else
                            {
                              if (hs.size() >= historypos)
                                hd[historypos] |= hs[historypos - 1];
                              else
                                hd[historypos] = bddfalse;
                            }
                        }
                    }
                  should_continue = false;
                  for (unsigned n = 0; n < numstates; ++n)
                    {
                      auto& h = histories[n];
                      if (h.size() <= historypos)
                        continue;
                      else if (h[historypos] == bddfalse)
                        h.pop_back();
                      else
                        should_continue = true;
                    }
                }
              // std::cerr << "computed histories:\n";
              // for (unsigned n = 0; n < numstates; ++n)
              //   {
              //     std::cerr << n << ": [";
              //     for (bdd b: histories[n])
              //       bdd_print_formula(std::cerr, aut->get_dict(), b) << ", ";
              //     std::cerr << '\n';
              //   }
            }

          unsigned new_init = -1U;
          // We do two the rewrite in two passes.  The first one
          // replays histories to detect the new source the edges
          // should synchronize with. We used to have single
          // loop, but replaying history on edges that have been modified
          // result in different automaton depending on the edge order.
          std::vector<unsigned> redirect_src;
          if (is_det)
            {
              redirect_src.resize(aut->edge_vector().size());
              for (auto& e: aut->edges())
                {
                  unsigned edge = aut->edge_number(e);
                  redirect_src[edge] = e.src; // no change by default
                  // Don't bother with terminal states, they won't be
                  // reachable anymore.
                  if (term[si.scc_of(e.src)])
                    continue;
                  // It will not loop
                  if (!term[si.scc_of(e.dst)])
                    continue;
                  // It will loop.

                  // If initial state cannot be reached from another
                  // state of the automaton, we can get rid of it by
                  // combining the edge reaching the terminal state
                  // with the edges leaving the initial state.
                  //
                  // However if we have some histories for e.src
                  // (which implies that the automaton is
                  // deterministic), we can try to replay that from
                  // the initial state first.
                  //
                  // One problem with those histories, is that we do
                  // not know how much of it to replay.  It's possible
                  // that we cannot find a matching transition (e.g. if
                  // the history if "b" but the choices are "!a" or "a"),
                  // and its also possible to that playing too much of
                  // history will get us back the terminal state.  In both
                  // cases, we should try again with a smaller history.
                  unsigned moved_init = init;
                  bdd econd = e.cond;

                  auto& h = histories[e.src];
                  int hsize = h.size();
                  for (int hlen = hsize - 1; hlen >= 0; --hlen)
                    {
                      for (int pos = hlen - 1; pos >= 0; --pos)
                        {
                          for (auto& e: aut->out(moved_init))
                            if (bdd_implies(h[pos], e.cond))
                              {
                                if (term[si.scc_of(e.dst)])
                                  goto failed;
                                moved_init = e.dst;
                                goto moved;
                              }
                          // if we reach this place, we failed to follow
                          // one step of the history.
                          goto failed;
                        moved:
                          continue;
                        }

                      // Make sure no successor of the new initial
                      // state will reach a terminal state; if
                      // that is the case, we have to shorten the
                      // history further.
                      if (hlen > 0)
                        for (auto& ei: aut->out(moved_init))
                          if ((ei.cond & econd) != bddfalse
                              && term[si.scc_of(ei.dst)])
                            goto failed;

                      redirect_src[edge] = moved_init;
                      break;
                    failed:
                      moved_init = init;
                      continue;
                    }
                }
            }

          // No we do the redirections.

          // We will modify most of the edges in place, but some cases
          // require the addition of new edges.  However we cannot add
          // edges during the iteration, because that may reallocate
          // the edge vector.  So we postpone all edge creations by
          // storing them into this array.  All postponed additions
          // correspond to Büchi accepting edges, so we do not have to
          // store the mark_t.
          struct edge_info {
            edge_info(unsigned s, unsigned d, bdd c) noexcept
              : src(s), dst(d), cond(c)
            {
            }
            unsigned src;
            unsigned dst;
            bdd cond;
          };
          std::vector<edge_info> new_edges;

          for (auto& e: aut->edges())
            {
              // Don't bother with terminal states, they won't be
              // reachable anymore.
              if (term[si.scc_of(e.src)])
                continue;
              // It will loop
              if (term[si.scc_of(e.dst)])
                {
                  // If the source state is the initial state, we
                  // should not try to combine it with itself...
                  //
                  // Also if the automaton is not deterministic
                  // and there is no trivial initial state, then
                  // we simply loop back to the initial state.
                  if (e.src == init
                      || (!is_det && !initial_state_trivial))
                    {
                      e.dst = init;
                      e.acc = {0};
                      new_init = init;
                      continue;
                    }
                  // If initial state cannot be reached from another
                  // state of the automaton, we can get rid of it by
                  // combining the edge reaching the terminal state
                  // with the edges leaving the initial state.
                  //
                  // However if we have some histories for e.src
                  // (which implies that the automaton is
                  // deterministic), we can try to replay that from
                  // the initial state first.
                  //
                  // One problem with those histories, is that we do
                  // not know how much of it to replay.  It's possible
                  // that we cannot find a matching transition (e.g. if
                  // the history if "b" but the choices are "!a" or "a"),
                  // and its also possible to that playing too much of
                  // history will get us back the terminal state.  In both
                  // cases, we should try again with a smaller history.
                  unsigned moved_init = init;
                  bdd econd = e.cond;
                  bool first;
                  if (is_det)
                    {
                      unsigned edge = aut->edge_number(e);
                      moved_init = redirect_src[edge];

                      first = true;
                      for (auto& ei: aut->out(moved_init))
                        {
                          bdd cond = ei.cond & econd;
                          if (cond != bddfalse)
                            {
                              // We should never reach the terminal state,
                              // unless the history is empty.  In that
                              // case (ts=true) we have to make a loop to
                              // the initial state without any
                              // combination.
                              bool ts = term[si.scc_of(ei.dst)];
                              if (ts)
                                want_merge_edges = true;
                              if (first)
                                {
                                  e.acc = {0};
                                  e.cond = cond;
                                  first = false;
                                  if (!ts)
                                    {
                                      e.dst = ei.dst;
                                      if (new_init == -1U)
                                        new_init = e.src;
                                    }
                                  else
                                    {
                                      new_init = e.dst = init;
                                    }
                                }
                              else
                                {
                                  unsigned dst = ei.dst;
                                  if (ts)
                                    new_init = dst = init;
                                  new_edges.emplace_back(e.src, dst, cond);
                                }
                            }
                        }
                    }
                  else
                    {
                      first = true;
                      for (auto& ei: aut->out(moved_init))
                        {
                          bdd cond = ei.cond & econd;
                          if (cond != bddfalse)
                            {
                              if (first)
                                {
                                  e.dst = ei.dst;
                                  e.acc = {0};
                                  e.cond = cond;
                                  first = false;
                                }
                              else
                                {
                                  new_edges.emplace_back(e.src, ei.dst, cond);
                                }
                            }
                        }

                    }
                }
              else
                {
                  e.acc = {};
                }
            }
          // Now that the iteration is over, commit all new edges.
          for (auto& e: new_edges)
            aut->new_edge(e.src, e.dst, e.cond, {0});
          // In a deterministic and suspendable automaton, all states
          // recognize the same language, so we can freely move the
          // initial state.  We decide to use the source of any
          // accepting transition, in the hope that it will make the
          // original initial state unreachable.
          if (is_det && new_init != -1U)
            aut->set_init_state(new_init);
        }
      else
        {
          // Replace all terminal state by a single accepting state.
          unsigned accstate = aut->new_state();
          for (auto& e: aut->edges())
            {
              if (term[si.scc_of(e.dst)])
                e.dst = accstate;
              e.acc = {};
            }
          // This accepting state has the same output as the initial
          // state.
          for (auto& e: aut->out(init))
            aut->new_edge(accstate, e.dst, e.cond, {0});
          // This is not mandatory, but starting on the accepting
          // state helps getting shorter accepting words and may
          // reader the original initial state unreachable, saving one
          // state.
          aut->set_init_state(accstate);
        }

      aut->purge_unreachable_states();
      if (want_merge_edges)
        aut->merge_edges();
      return aut;
    }
  }

  twa_graph_ptr
  g_f_terminal_inplace(twa_graph_ptr aut, bool state_based)
  {
    scc_info si(aut);
    return do_g_f_terminal_inplace(si, state_based);
  }

  twa_graph_ptr
  gf_guarantee_to_ba_maybe(formula gf, const bdd_dict_ptr& dict,
                           bool deterministic, bool state_based)
  {
    if (!gf.is(op::G))
      return nullptr;
    formula f = gf[0];
    if (!f.is(op::F))
      {
        // F(...)&F(...)&... is also OK.
        if (!f.is(op::And))
          return nullptr;
        for (auto c: f)
          if (!c.is(op::F))
            return nullptr;

        f = nest_f(f);
      }
    twa_graph_ptr aut = ltl_to_tgba_fm(f, dict, true);
    twa_graph_ptr reduced = minimize_obligation(aut, f);
    if (reduced == aut)
      return nullptr;
    scc_info si(reduced);
    if (!is_terminal_automaton(reduced, &si, true))
      return nullptr;
    do_g_f_terminal_inplace(si, state_based);
    if (!deterministic)
      {
        scc_info si2(aut);
        if (!is_terminal_automaton(aut, &si2, true))
          return reduced;
        do_g_f_terminal_inplace(si2, state_based);
        if (aut->num_states() < reduced->num_states())
          return aut;
      }
    return reduced;
  }

  twa_graph_ptr
  gf_guarantee_to_ba(formula gf, const bdd_dict_ptr& dict,
                     bool deterministic, bool state_based)
  {
    twa_graph_ptr res = gf_guarantee_to_ba_maybe(gf, dict,
                                                 deterministic, state_based);
    if (!res)
      throw std::runtime_error
        ("gf_guarantee_to_ba(): expects a formula of the form GF(guarantee)");
    return res;
  }

  twa_graph_ptr
  fg_safety_to_dca_maybe(formula fg, const bdd_dict_ptr& dict,
                         bool state_based)
  {
    if (!fg.is(op::F))
      return nullptr;
    formula g = fg[0];
    if (!g.is(op::G))
      {
        // G(...)|G(...)|... is also OK.
        if (!g.is(op::Or))
          return nullptr;
        for (auto c: g)
          if (!c.is(op::G))
            return nullptr;
      }

    formula gf = negative_normal_form(fg, true);
    twa_graph_ptr res =
      gf_guarantee_to_ba_maybe(gf, dict, true, state_based);
    if (!res)
      return nullptr;
    return dualize(res);
  }

  twa_graph_ptr
  fg_safety_to_dca(formula gf, const bdd_dict_ptr& dict,
                   bool state_based)
  {
    twa_graph_ptr res = fg_safety_to_dca_maybe(gf, dict, state_based);
    if (!res)
      throw std::runtime_error
        ("fg_safety_to_dca(): expects a formula of the form FG(safety)");
    return res;
  }
}
