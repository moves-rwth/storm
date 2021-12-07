// -*- coding: utf-8 -*-
// Copyright (C) 2014-2020 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/sccinfo.hh>
#include <stack>
#include <algorithm>
#include <queue>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/bfssteps.hh>
#include <spot/twaalgos/mask.hh>
#include <spot/twaalgos/genem.hh>
#include <spot/misc/escape.hh>

namespace spot
{
  void scc_info::report_need_track_states()
  {
    throw std::runtime_error
      ("scc_info was not run with option TRACK_STATES");
  }

  void scc_info::report_need_track_succs()
  {
    throw std::runtime_error
      ("scc_info was not run with option TRACK_SUCCS");
  }

  void scc_info::report_incompatible_stop_on_acc()
  {
    throw std::runtime_error
      ("scc_info was run with option STOP_ON_ACC");
  }

  // this one is not yet needed in the hh file
  static void report_need_stop_on_acc()
  {
    throw std::runtime_error
      ("scc_info was not run with option STOP_ON_ACC");
  }

  namespace
  {
    struct scc
    {
    public:
      scc(int index, acc_cond::mark_t in_acc):
        in_acc(in_acc), index(index)
      {
      }

      acc_cond::mark_t in_acc; // Acceptance sets on the incoming transition
      acc_cond::mark_t acc = {}; // union of all acceptance marks in the SCC
      // intersection of all marks in the SCC
      acc_cond::mark_t common = acc_cond::mark_t::all();
      int index;                     // Index of the SCC
      bool trivial = true;           // Whether the SCC has no cycle
      bool accepting = false;        // Necessarily accepting
    };
  }

  scc_info::scc_info(const scc_and_mark_filter& filt, scc_info_options options)
    : scc_info(filt.get_aut(), filt.start_state(),
               filt.get_filter(),
               const_cast<scc_and_mark_filter*>(&filt), options)
  {
  }

  scc_info::scc_info(const_twa_graph_ptr aut,
                     unsigned initial_state,
                     edge_filter filter,
                     void* filter_data,
                     scc_info_options options)
    : aut_(aut), initial_state_(initial_state),
      filter_(filter), filter_data_(filter_data),
      options_(options)
  {
    unsigned n = aut->num_states();

    if (initial_state != -1U && n <= initial_state)
      throw std::runtime_error
        ("scc_info: supplied initial state does not exist");

    sccof_.resize(n, -1U);

    if (!!(options & scc_info_options::TRACK_STATES_IF_FIN_USED)
        && aut->acc().uses_fin_acceptance())
      options_ = options = options | scc_info_options::TRACK_STATES;

    std::vector<unsigned> live;
    live.reserve(n);
    std::deque<scc> root_;        // Stack of SCC roots.
    std::vector<int> h_(n, 0);
    // Map of visited states.  Values > 0 designate maximal SCC.
    // Values < 0 number states that are part of incomplete SCCs being
    // completed.  0 denotes non-visited states.

    int num_ = 0;               // Number of visited nodes, negated.

    struct stack_item {
      unsigned src;
      unsigned out_edge;
      unsigned univ_pos;
    };
    // DFS stack.  Holds (STATE, TRANS, UNIV_POS) pairs where TRANS is
    // the current outgoing transition of STATE, and UNIV_POS is used
    // when the transition is universal to iterate over all possible
    // destinations.
    std::stack<stack_item> todo_;
    auto& gr = aut->get_graph();

    std::deque<unsigned> init_states;
    std::vector<bool> init_seen(n, false);
    auto push_init = [&](unsigned s)
      {
        if (h_[s] != 0 || init_seen[s])
          return;
        init_seen[s] = true;
        init_states.push_back(s);
      };

    bool track_states = !!(options & scc_info_options::TRACK_STATES);
    bool track_succs = !!(options & scc_info_options::TRACK_SUCCS);
    auto backtrack = [&](unsigned curr)
      {
        if (root_.back().index == h_[curr])
          {
            unsigned num = node_.size();
            acc_cond::mark_t acc = root_.back().acc;
            acc_cond::mark_t common = root_.back().common;
            bool triv = root_.back().trivial;
            node_.emplace_back(acc, common, triv);

            auto& succ = node_.back().succ_;
            unsigned np1 = num + 1;
            auto s = live.rbegin();
            do
              {
                sccof_[*s] = num;
                h_[*s] = np1;

                // Gather all successor SCCs
                if (track_succs)
                  for (auto& t: aut->out(*s))
                    if (SPOT_LIKELY(t.cond != bddfalse))
                      for (unsigned d: aut->univ_dests(t))
                        {
                          unsigned n = sccof_[d];
                          if (n == num || n == -1U)
                            continue;
                          // If edges are cut, we are not able to
                          // maintain proper successor information.
                          if (filter_)
                            switch (filter_(t, d, filter_data_))
                              {
                              case edge_filter_choice::keep:
                                break;
                              case edge_filter_choice::ignore:
                              case edge_filter_choice::cut:
                                continue;
                              }
                          succ.emplace_back(n);
                        }
              }
            while (*s++ != curr);

            if (track_states)
              {
                auto& nbs = node_.back().states_;
                nbs.insert(nbs.end(), s.base(), live.end());
              }

            node_.back().one_state_ = curr;
            live.erase(s.base(), live.end());

            if (track_succs)
              {
                std::sort(succ.begin(), succ.end());
                succ.erase(std::unique(succ.begin(), succ.end()), succ.end());
              }

            bool accept = !triv && root_.back().accepting;
            node_.back().accepting_ = accept;
            if (accept)
              one_acc_scc_ = num;
            bool reject = triv ||
              aut->acc().maybe_accepting(acc, common).is_false();
            node_.back().rejecting_ = reject;
            root_.pop_back();
          }
      };

    // Setup depth-first search from the initial state.  But we may
    // have a conjunction of initial state in alternating automata.
    if (initial_state_ == -1U)
      initial_state_ = aut->get_init_state_number();
    for (unsigned init: aut->univ_dests(initial_state_))
      push_init(init);

    while (!init_states.empty())
      {
        unsigned init = init_states.front();
        init_states.pop_front();
        int spi = h_[init];
        if (spi > 0)
          continue;
        assert(spi == 0);
        h_[init] = --num_;
        root_.emplace_back(num_, acc_cond::mark_t({}));
        todo_.emplace(stack_item{init, gr.state_storage(init).succ, 0});
        live.emplace_back(init);

        while (!todo_.empty())
          {
            // We are looking at the next successor in SUCC.
            unsigned tr_succ = todo_.top().out_edge;

            // If there is no more successor, backtrack.
            if (!tr_succ)
              {
                // We have explored all successors of state CURR.
                unsigned curr = todo_.top().src;

                // Backtrack TODO_.
                todo_.pop();

                // When backtracking the root of an SCC, we must also
                // remove that SCC from the ARC/ROOT stacks.  We must
                // discard from H all reachable states from this SCC.
                assert(!root_.empty());
                backtrack(curr);
                continue;
              }

            // We have a successor to look at.
            // Fetch the values we are interested in...
            auto& e = gr.edge_storage(tr_succ);

            // Skip false edges.
            if (SPOT_UNLIKELY(e.cond == bddfalse))
              {
                todo_.top().out_edge = e.next_succ;
                continue;
              }

            unsigned dest = e.dst;
            if ((int) dest < 0)
              {
                // Iterate over all destinations of a universal edge.
                if (todo_.top().univ_pos == 0)
                  todo_.top().univ_pos = ~dest + 1;
                const auto& v = gr.dests_vector();
                dest = v[todo_.top().univ_pos];
                // Last universal destination?
                if (~e.dst + v[~e.dst] == todo_.top().univ_pos)
                  {
                    todo_.top().out_edge = e.next_succ;
                    todo_.top().univ_pos = 0;
                  }
                else
                  {
                    ++todo_.top().univ_pos;
                  }
              }
            else
              {
                todo_.top().out_edge = e.next_succ;
              }

            // Do we really want to look at this
            if (filter_)
              switch (filter_(e, dest, filter_data_))
                {
                case edge_filter_choice::keep:
                  break;
                case edge_filter_choice::ignore:
                  continue;
                case edge_filter_choice::cut:
                  push_init(e.dst);
                  continue;
                }

            acc_cond::mark_t acc = e.acc;

            // Are we going to a new state?
            int spi = h_[dest];
            if (spi == 0)
              {
                // Yes.  Number it, stack it, and register its successors
                // for later processing.
                h_[dest] = --num_;
                root_.emplace_back(num_, acc);
                todo_.emplace(stack_item{dest, gr.state_storage(dest).succ, 0});
                live.emplace_back(dest);
                continue;
              }

            // We already know the state.

            // Have we reached a maximal SCC?
            if (spi > 0)
              continue;

            // Now this is the most interesting case.  We have reached a
            // state S1 which is already part of a non-dead SCC.  Any such
            // non-dead SCC has necessarily been crossed by our path to
            // this state: there is a state S2 in our path which belongs
            // to this SCC too.  We are going to merge all states between
            // this S1 and S2 into this SCC..
            //
            // This merge is easy to do because the order of the SCC in
            // ROOT is descending: we just have to merge all SCCs from the
            // top of ROOT that have an index lesser than the one of
            // the SCC of S2 (called the "threshold").
            int threshold = spi;
            bool is_accepting = false;
            // If this is a self-loop, check its acceptance alone.
            if (dest == e.src)
              is_accepting = aut->acc().accepting(acc);

            acc_cond::mark_t common = acc;
            assert(!root_.empty());
            while (threshold > root_.back().index)
              {
                acc |= root_.back().acc;
                acc_cond::mark_t in_acc = root_.back().in_acc;
                acc |= in_acc;
                common &= root_.back().common;
                common &= in_acc;
                is_accepting |= root_.back().accepting;
                root_.pop_back();
                assert(!root_.empty());
              }

            // Note that we do not always have
            //  threshold == root_.back().index
            // after this loop, the SCC whose index is threshold might have
            // been merged with a higher SCC.

            root_.back().acc |= acc;
            root_.back().common &= common;
            root_.back().accepting |= is_accepting
              || aut->acc().accepting(root_.back().acc);
            // This SCC is no longer trivial.
            root_.back().trivial = false;

            if (root_.back().accepting
                && !!(options & scc_info_options::STOP_ON_ACC))
              {
                while (!todo_.empty())
                  {
                    unsigned curr = todo_.top().src;
                    todo_.pop();
                    backtrack(curr);
                  }
                return;
              }
          }
      }
    if (track_succs && !(options & scc_info_options::STOP_ON_ACC))
      determine_usefulness();
  }

  void scc_info::determine_usefulness()
  {
    // An SCC is useful if it is not rejecting or it has a successor
    // SCC that is useful.
    unsigned scccount = scc_count();
    for (unsigned i = 0; i < scccount; ++i)
      {
        if (!node_[i].is_rejecting())
          {
            node_[i].useful_ = true;
            continue;
          }
        node_[i].useful_ = false;
        for (unsigned j: node_[i].succ())
          if (node_[j].is_useful())
            {
              node_[i].useful_ = true;
              break;
            }
      }
  }

  std::set<acc_cond::mark_t> scc_info::marks_of(unsigned scc) const
  {
    std::set<acc_cond::mark_t> res;
    for (auto& t: inner_edges_of(scc))
      res.insert(t.acc);
    return res;
  }

  std::vector<std::set<acc_cond::mark_t>> scc_info::marks() const
  {
    unsigned n = aut_->num_states();
    std::vector<std::set<acc_cond::mark_t>> result(scc_count());

    for (unsigned src = 0; src < n; ++src)
      {
        unsigned src_scc = scc_of(src);
        if (src_scc == -1U || is_rejecting_scc(src_scc))
          continue;
        auto& s = result[src_scc];
        for (auto& t: aut_->out(src))
          {
            if (scc_of(t.dst) != src_scc || SPOT_UNLIKELY(t.cond == bddfalse))
              continue;
            s.insert(t.acc);
          }
      }
    return result;
  }

  std::vector<bool> scc_info::weak_sccs() const
  {
    unsigned n = scc_count();
    std::vector<bool> result(scc_count());
    auto acc = marks();
    for (unsigned s = 0; s < n; ++s)
      result[s] = is_rejecting_scc(s) || acc[s].size() == 1;
    return result;
  }

  bdd scc_info::scc_ap_support(unsigned scc) const
  {
    bdd support = bddtrue;
    for (auto& t: edges_of(scc))
      support &= bdd_support(t.cond);
    return support;
  }

  bool scc_info::check_scc_emptiness(unsigned n) const
  {
    if (SPOT_UNLIKELY(!aut_->is_existential()))
      throw std::runtime_error("scc_info::check_scc_emptiness() "
                               "does not support alternating automata");
    if (SPOT_UNLIKELY(!(options_ & scc_info_options::TRACK_STATES)))
      report_need_track_states();
    return generic_emptiness_check_for_scc(*this, n);
  }

  void scc_info::determine_unknown_acceptance()
  {
    unsigned s = scc_count();
    bool changed = false;
    // iterate over SCCs in topological order
    do
      {
        --s;
        if (!is_rejecting_scc(s) && !is_accepting_scc(s))
          {
            if (SPOT_UNLIKELY(!aut_->is_existential()))
              throw std::runtime_error(
                  "scc_info::determine_unknown_acceptance() "
                  "does not support alternating automata");
            auto& node = node_[s];
            if (check_scc_emptiness(s))
              {
                node.rejecting_ = true;
              }
            else
              {
                node.accepting_ = true;
                if (one_acc_scc_ < 0)
                  one_acc_scc_ = s;
              }
            changed = true;
          }
      }
    while (s);
    if (changed && !!(options_ & scc_info_options::TRACK_SUCCS))
      determine_usefulness();
  }

  // A reimplementation of spot::bfs_steps for explicit automata.
  // bool filter(const twa_graph::edge_storage_t&) returns true if the
  // transition has to be filtered out.
  // bool match(const twa_graph::edge_storage_t&) returns true if the BFS
  // has to stop after this transition.
  // Returns the destination of the matched transition, or -1 if no match has
  // been found.
  template <typename edge_filter_type,
            typename step_matcher_type>
  static int explicit_bfs_steps(const const_twa_graph_ptr aut, unsigned start,
                                twa_run::steps& steps,
                                edge_filter_type filter,
                                step_matcher_type match)
  {
    auto& gr = aut->get_graph();
    // The backlink of each state is the index of the edge that
    // discovered it in the BFS, so BACKLINKS effectively describes a
    // tree rooted at START.
    std::vector<unsigned> backlinks(aut->num_states(), 0);
    std::deque<unsigned> bfs_queue;
    bfs_queue.emplace_back(start);
    while (!bfs_queue.empty())
      {
        unsigned src = bfs_queue.front();
        bfs_queue.pop_front();
        for (auto& t: aut->out(src))
          {
            if (SPOT_UNLIKELY(t.cond == bddfalse) || filter(t))
              continue;

            if (match(t))
              {
                // Build the path from START to T.DST by following the
                // backlinks, starting at the end.
                twa_run::steps path;
                path.emplace_front(aut->state_from_number(t.src),
                                   t.cond, t.acc);
                unsigned src = t.src;
                while (src != start)
                  {
                    unsigned bl_num = backlinks[src];
                    assert(bl_num);
                    auto& bl_edge = gr.edge_storage(bl_num);
                    src = bl_edge.src;
                    path.emplace_front(aut->state_from_number(src),
                                       bl_edge.cond, bl_edge.acc);
                  }
                steps.splice(steps.end(), path);
                return t.dst;
              }

            if (!backlinks[t.dst])
              {
                backlinks[t.dst] = aut->edge_number(t);
                bfs_queue.push_back(t.dst);
              }
          }
      }
    return -1;
  }

  void scc_info::get_accepting_run(unsigned scc, twa_run_ptr r) const
  {
    const scc_info::scc_node& node = node_[scc];
    if (!node.is_accepting())
      throw std::runtime_error("scc_info::get_accepting_cycle needs to be "
                               "called on an accepting scc");
    if (SPOT_UNLIKELY(!(options_ & scc_info_options::STOP_ON_ACC)))
      report_need_stop_on_acc();

    unsigned init = aut_->get_init_state_number();

    // The accepting cycle should honor any edge filter we have.
    auto filter = [this](const twa_graph::edge_storage_t& t)
                  {
                    if (!filter_)
                      return false;
                    // Filter out ignored and cut transitions.
                    return filter_(t, t.dst, filter_data_)
                      != edge_filter_choice::keep;
                  };

    // The SCC exploration has a small optimization that can flag SCCs
    // as accepting if they contain accepting self-loops, even if the
    // SCC itself has some Fin acceptance to check.  So we have to
    // deal with this situation before we look for the more complex
    // case of satisfying the condition with a larger cycle.  We do
    // this first, because it's good to return a small cycle if we
    // can.
    const acc_cond& acccond = aut_->acc();
    unsigned num_states = aut_->num_states();
    for (unsigned s = 0; s < num_states; ++s)
      {
        // We scan the entire state to find those in the SCC, because
        // we cannot rely on TRACK_STATES being on.
        if (scc_of(s) != scc)
          continue;
        for (auto& e: aut_->out(s))
          if (e.src == e.dst && SPOT_LIKELY(e.cond != bddfalse)
              && !filter(e) && acccond.accepting(e.acc))
            {
              // We have found an accepting self-loop.  That's the cycle
              // part of our accepting run.
              r->cycle.clear();
              r->cycle.emplace_front(aut_->state_from_number(e.src),
                                     e.cond, e.acc);
              // Add the prefix.
              r->prefix.clear();
              if (e.src != init)
                explicit_bfs_steps(aut_, init, r->prefix,
                                   [](const twa_graph::edge_storage_t&)
                                   {
                                     return false; // Do not filter.
                                   },
                                   [&](const twa_graph::edge_storage_t& t)
                                   {
                                     return t.dst == e.src;
                                   });
              return;
            }
      }

    // Prefix search

    r->prefix.clear();
    int substart;
    if (scc_of(init) == scc)
      {
        // The initial state is in the target SCC: no prefix needed.
        substart = init;
      }
    else
      {
        substart = explicit_bfs_steps(aut_, init, r->prefix,
            [](const twa_graph::edge_storage_t&)
            {
              return false; // Do not filter.
            },
            [&](const twa_graph::edge_storage_t& t)
            {
              // Match any state in the SCC.
              return scc_of(t.dst) == scc;
            });
      }

    const unsigned start = (unsigned)substart;

    // Cycle search
    acc_cond actual_cond = acccond.restrict_to(node.acc_marks())
      .force_inf(node.acc_marks());
    assert(!actual_cond.uses_fin_acceptance());
    assert(!actual_cond.is_f());
    acc_cond::mark_t acc_to_see = actual_cond.accepting_sets(node.acc_marks());
    r->cycle.clear();

    do
      {
        substart = explicit_bfs_steps(aut_, substart, r->cycle,
            [&](const twa_graph::edge_storage_t& t)
            {
              // Stay in the specified SCC.
              return scc_of(t.dst) != scc || filter(t);
            },
            [&](const twa_graph::edge_storage_t& t)
            {
              if (!acc_to_see) // We have seen all the marks, go back to start.
                return t.dst == start;
              if (t.acc & acc_to_see)
                {
                  acc_to_see -= t.acc;
                  return true;
                }
              return false;
            });
        assert(0 <= substart);
      }
    while (acc_to_see || (unsigned)substart != start);
  }

  std::ostream&
  dump_scc_info_dot(std::ostream& out,
                    const_twa_graph_ptr aut, scc_info* sccinfo)
  {
    scc_info* m = sccinfo ? sccinfo : new scc_info(aut);

    out << "digraph G {\n  i [label=\"\", style=invis, height=0]\n";
    int start = m->scc_of(aut->get_init_state_number());
    out << "  i -> " << start << std::endl;

    std::vector<bool> seen(m->scc_count());
    seen[start] = true;

    std::queue<int> q;
    q.push(start);
    while (!q.empty())
      {
        int state = q.front();
        q.pop();

        out << "  " << state << " [shape=box,"
            << (aut->acc().accepting(m->acc_sets_of(state)) ?
                "style=bold," : "")
            << "label=\"" << state;
        {
          size_t n = m->states_of(state).size();
          out << " (" << n << " state";
          if (n > 1)
            out << 's';
          out << ')';
        }
        out << "\"]\n";

        for (unsigned dest: m->succ(state))
          {
            out << "  " << state << " -> " << dest << '\n';
            if (seen[dest])
              continue;
            seen[dest] = true;
            q.push(dest);
          }
      }

    out << "}\n";
    if (!sccinfo)
      delete m;
    return out;
  }

  std::vector<twa_graph_ptr>
  scc_info::split_on_sets(unsigned scc, acc_cond::mark_t sets,
                          bool preserve_names) const
  {
    if (SPOT_UNLIKELY(!(options_ & scc_info_options::TRACK_STATES)))
      report_need_track_states();
    std::vector<twa_graph_ptr> res;

    std::vector<bool> seen(aut_->num_states(), false);
    std::vector<bool> cur(aut_->num_states(), false);

    for (unsigned init: states_of(scc))
      {
        if (seen[init])
          continue;
        cur.assign(aut_->num_states(), false);

        auto copy = make_twa_graph(aut_->get_dict());
        copy->copy_ap_of(aut_);
        copy->copy_acceptance_of(aut_);
        copy->prop_state_acc(aut_->prop_state_acc());
        transform_accessible(aut_, copy, [&](unsigned src,
                                             bdd& cond,
                                             acc_cond::mark_t& m,
                                             unsigned dst)
                             {
                               cur[src] = seen[src] = true;
                               if (filter_)
                                 {
                                   twa_graph::edge_storage_t e;
                                   e.cond = cond;
                                   e.acc = m;
                                   e.src = src;
                                   e.dst = dst;
                                   if (filter_(e, dst, filter_data_)
                                       != edge_filter_choice::keep)
                                     {
                                       cond = bddfalse;
                                       return;
                                     }
                                 }
                               if (scc_of(dst) != scc
                                   || (m & sets)
                                   || (seen[dst] && !cur[dst]))
                                 {
                                   cond = bddfalse;
                                   return;
                                 }
                             },
                             init);
        if (copy->num_edges())
          {
            if (preserve_names)
              copy->copy_state_names_from(aut_);
            res.push_back(copy);
          }
      }
    return res;
  }

  void
  scc_info::states_on_acc_cycle_of_rec(unsigned scc,
                                       acc_cond::mark_t all_fin,
                                       acc_cond::mark_t all_inf,
                                       unsigned nb_pairs,
                                       std::vector<acc_cond::rs_pair>& pairs,
                                       std::vector<unsigned>& res,
                                       std::vector<unsigned>& old) const
  {
    if (is_useful_scc(scc) && !is_rejecting_scc(scc))
      {
        acc_cond::mark_t all_acc = acc_sets_of(scc);
        acc_cond::mark_t fin = all_fin & all_acc;
        acc_cond::mark_t inf = all_inf & all_acc;

        // Get all Fin acceptance set that appears in the SCC and does not have
        // their corresponding Inf appearing in the SCC.
        acc_cond::mark_t m = {};
        if (fin)
          for (unsigned p = 0; p < nb_pairs; ++p)
            if (fin & pairs[p].fin && !(inf & pairs[p].inf))
              m |= pairs[p].fin;

        if (m)
          for (const twa_graph_ptr& aut : split_on_sets(scc, m))
            {
              auto orig_sts = aut->get_named_prop
                <std::vector<unsigned>>("original-states");

              // Update mapping of state numbers between the current automaton
              // and the starting one.
              for (unsigned i = 0; i < orig_sts->size(); ++i)
                (*orig_sts)[i] = old[(*orig_sts)[i]];

              scc_info si_tmp(aut, scc_info_options::TRACK_STATES
                                   | scc_info_options::TRACK_SUCCS);
              unsigned scccount_tmp = si_tmp.scc_count();
              for (unsigned scc_tmp = 0; scc_tmp < scccount_tmp; ++scc_tmp)
                si_tmp.states_on_acc_cycle_of_rec(scc_tmp, all_fin, all_inf,
                                                  nb_pairs, pairs, res,
                                                  *orig_sts);
            }

        else  // Accepting cycle found.
          for (unsigned s : states_of(scc))
            res.push_back(old[s]);
      }
  }

  std::vector<unsigned>
  scc_info::states_on_acc_cycle_of(unsigned scc) const
  {
    std::vector<acc_cond::rs_pair> pairs;
    if (!aut_->acc().is_streett_like(pairs))
      throw std::runtime_error("states_on_acc_cycle_of only works with "
                               "Streett-like acceptance condition");
    unsigned nb_pairs = pairs.size();

    std::vector<unsigned> res;
    if (is_useful_scc(scc) && !is_rejecting_scc(scc))
      {
        std::vector<unsigned> old;
        unsigned nb_states = aut_->num_states();
        for (unsigned i = 0; i < nb_states; ++i)
          old.push_back(i);

        acc_cond::mark_t all_fin = {};
        acc_cond::mark_t all_inf = {};
        std::tie(all_inf, all_fin) = aut_->get_acceptance().used_inf_fin_sets();

        states_on_acc_cycle_of_rec(scc, all_fin, all_inf, nb_pairs, pairs, res,
                                   old);
      }

    return res;
  }

    scc_info::edge_filter_choice
    scc_and_mark_filter::filter_scc_and_mark_
    (const twa_graph::edge_storage_t& e, unsigned dst, void* data)
    {
      auto& d = *reinterpret_cast<scc_and_mark_filter*>(data);
      if (d.lower_si_->scc_of(dst) != d.lower_scc_)
        return scc_info::edge_filter_choice::ignore;
      if (d.cut_sets_ & e.acc)
        return scc_info::edge_filter_choice::cut;
      return scc_info::edge_filter_choice::keep;
    }

    scc_info::edge_filter_choice
    scc_and_mark_filter::filter_mark_
    (const twa_graph::edge_storage_t& e, unsigned, void* data)
    {
      auto& d = *reinterpret_cast<scc_and_mark_filter*>(data);
      if (d.cut_sets_ & e.acc)
        return scc_info::edge_filter_choice::cut;
      return scc_info::edge_filter_choice::keep;
    }

}
