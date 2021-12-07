// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017-2020 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/cleanacc.hh>

namespace spot
{
  twa_graph_ptr cleanup_acceptance_here(twa_graph_ptr aut, bool strip)
  {
    auto& acc = aut->acc();
    if (acc.num_sets() == 0)
      return aut;

    auto c = aut->get_acceptance();
    acc_cond::mark_t used_in_cond = c.used_sets();

    acc_cond::mark_t used_in_aut = {};
    acc_cond::mark_t used_on_all_edges = used_in_cond;
    for (auto& t: aut->edges())
      {
        used_in_aut |= t.acc;
        used_on_all_edges &= t.acc;
      }

    auto useful = used_in_aut & used_in_cond;
    auto useless = strip ? acc.comp(useful) : (used_in_cond - used_in_aut);

    useless |= used_on_all_edges;

    if (!useless)
      return aut;

    // Remove useless marks from the automaton
    if (strip)
      for (auto& t: aut->edges())
        t.acc = t.acc.strip(useless);

    // if x appears on all edges, then
    //   Fin(x) = false and Inf(x) = true
    if (used_on_all_edges)
      c = c.remove(used_on_all_edges, false);

    // Remove useless marks from the acceptance condition
    if (strip)
      aut->set_acceptance(useful.count(), c.strip(useless, true));
    else
      aut->set_acceptance(aut->num_sets(), c.remove(useless, true));

    // This may in turn cause even more set to be unused, because of
    // some simplifications in the acceptance condition, so do it again.
    return cleanup_acceptance_here(aut, strip);
  }

  twa_graph_ptr cleanup_acceptance(const_twa_graph_ptr aut, bool strip)
  {
    return cleanup_acceptance_here(make_twa_graph(aut, twa::prop_set::all()),
                                   strip);
  }

  namespace
  {
    twa_graph_ptr merge_identical_marks_here(twa_graph_ptr aut)
    {
      // /!\ This assumes that the acceptance condition has been
      // cleaned up first.  If some mark appears in the acceptance
      // condition but not in the automaton, the result is undefined.

      auto& acc = aut->acc();
      auto& c = acc.get_acceptance();
      acc_cond::mark_t used_in_cond = c.used_sets();

      if (!used_in_cond)
        return aut;

      unsigned num_sets = acc.num_sets();
      std::vector<acc_cond::mark_t> always_together(num_sets);

      for (unsigned i = 0; i < num_sets; ++i)
        if (used_in_cond.has(i))
          always_together[i] = used_in_cond;
        else
          always_together[i] = acc_cond::mark_t({i});

      acc_cond::mark_t previous_a = {};
      for (auto& t: aut->edges())
        {
          acc_cond::mark_t a = t.acc & used_in_cond;
          if (a == previous_a)
            continue;
          previous_a = a;
          for (unsigned m: a.sets())
            {
              acc_cond::mark_t at = always_together[m];
              acc_cond::mark_t newm = at & a;

              for (unsigned rem: (at - newm).sets())
                always_together[rem] -= newm;

              always_together[m] = newm;
            }
        }

      acc_cond::mark_t to_remove = {};
      for (unsigned i = 0; i < num_sets; ++i)
        {
          auto oldm = always_together[i];
          if (oldm == acc_cond::mark_t({i}))
            continue;

          acc_cond::mark_t newm = oldm.lowest();
          to_remove |= oldm - newm;
          always_together[i] = newm;
        }
      for (auto& t: aut->edges())
        t.acc -= to_remove;

      // Replace the marks in the acceptance condition
      auto pos = &c.back();
      auto end = &c.front();
      while (pos > end)
        {
          switch (pos->sub.op)
            {
            case acc_cond::acc_op::And:
            case acc_cond::acc_op::Or:
              --pos;
              break;
            case acc_cond::acc_op::Fin:
            case acc_cond::acc_op::Inf:
            case acc_cond::acc_op::FinNeg:
            case acc_cond::acc_op::InfNeg:
              acc_cond::mark_t replace = pos[-1].mark & to_remove;
              pos[-1].mark -= replace;
              for (unsigned m: replace.sets())
                pos[-1].mark |= always_together[m];
              pos -= 2;
              break;
            }
        }
      return aut;
    }

    // Remove complementary marks from the acceptance condition.
    acc_cond::acc_code remove_compl_rec(const acc_cond::acc_word* pos,
                                        const std::vector<acc_cond::mark_t>&
                                                          complement)
    {
      auto start = pos - pos->sub.size;
      auto boolop =
        [&](acc_cond::acc_op opand, acc_cond::acc_op opfin,
            acc_cond::acc_op opinf,
            acc_cond::acc_code&& truecode,
            acc_cond::acc_code&& falsecode)
        {
          --pos;
          acc_cond::acc_code res = truecode;
          acc_cond::mark_t seen_fin = {};
          do
            {
              auto tmp = remove_compl_rec(pos, complement);

              if (!tmp.empty() && (tmp.back().sub.op == opfin
                                   && tmp.front().mark.is_singleton()))
                seen_fin |= tmp.front().mark;

              if (opand == acc_cond::acc_op::And)
                tmp &= std::move(res);
              else
                tmp |= std::move(res);
              std::swap(tmp, res);
              pos -= pos->sub.size + 1;
            }
          while (pos > start);
          if (res.empty() || res.back().sub.op != opand)
            return res;
          // Locate the position of any Inf element.
          // A property of the &= operator is that there
          // is only one Inf in a conjunction.
          auto rbegin = &res.back();
          auto rend = rbegin - rbegin->sub.size;
          --rbegin;
          do
            {
              if (rbegin->sub.op == opinf)
                break;
              rbegin -= rbegin->sub.size + 1;
            }
          while (rbegin > rend);
          if (rbegin <= rend)
            return res;
          // Fin(i) & Inf(i) = f;
          if (rbegin[-1].mark & seen_fin)
            return std::move(falsecode);
          for (auto m: seen_fin.sets())
            {
              acc_cond::mark_t cm = complement[m];
              // Fin(i) & Fin(!i) = f;
              if (cm & seen_fin)
                return std::move(falsecode);
              // Inf({!i}) & Fin({i}) = Fin({i})
              rbegin[-1].mark -= complement[m];
            }
          if (rbegin[-1].mark)
            return res;
          // Inf(i) has been rewritten as t, we need to remove it.  Sigh!
          acc_cond::acc_code res2 = std::move(truecode);
          rbegin = &res.back() - 1;
          do
            {
              acc_cond::acc_code tmp(rbegin);
              if (opand == acc_cond::acc_op::And)
                tmp &= std::move(res);
              else
                tmp |= std::move(res);
              std::swap(tmp, res2);
              rbegin -= rbegin->sub.size + 1;
            }
          while (rbegin > rend);
          return res2;
        };

      switch (pos->sub.op)
        {
          case acc_cond::acc_op::And:
            return boolop(acc_cond::acc_op::And,
                          acc_cond::acc_op::Fin,
                          acc_cond::acc_op::Inf,
                          acc_cond::acc_code::t(),
                          acc_cond::acc_code::f());
          case acc_cond::acc_op::Or:
            return boolop(acc_cond::acc_op::Or,
                          acc_cond::acc_op::Inf,
                          acc_cond::acc_op::Fin,
                          acc_cond::acc_code::f(),
                          acc_cond::acc_code::t());
          case acc_cond::acc_op::Fin:
            return acc_cond::acc_code::fin(pos[-1].mark);
          case acc_cond::acc_op::Inf:
            return acc_cond::acc_code::inf(pos[-1].mark);
          case acc_cond::acc_op::FinNeg:
          case acc_cond::acc_op::InfNeg:
            SPOT_UNREACHABLE();
        };
        SPOT_UNREACHABLE();
        return {};
    }

    // Always cleanup_acceptance_here with stripping after calling this function
    // As complementary marks might be simplified in the acceptance condition.
    twa_graph_ptr simplify_complementary_marks_here(twa_graph_ptr aut)
    {
      auto& acc = aut->acc();
      auto c = acc.get_acceptance();
      acc_cond::mark_t used_in_cond = c.used_sets();
      if (!used_in_cond)
        return aut;

      // complement[i] holds sets that appear when set #i does not.
      unsigned num_sets = acc.num_sets();
      std::vector<acc_cond::mark_t> complement(num_sets);

      for (unsigned i = 0; i < num_sets; ++i)
        if (used_in_cond.has(i))
          complement[i] = used_in_cond - acc_cond::mark_t({i});

      // Let's visit all edges to update complement[i].  To skip some
      // duplicated work, prev_acc remember the "acc" sets of the
      // previous edge, so we can skip consecutive edges with
      // identical "acc" sets.  Note that there is no value of
      // prev_acc that would allow us to fail the comparison on the
      // first edge (this was issue #315), so we have to deal with
      // that first edge specifically.
      acc_cond::mark_t prev_acc = {};
      const auto& edges = aut->edges();
      auto b = edges.begin();
      auto e = edges.end();
      auto update = [&](acc_cond::mark_t tacc)
        {
          prev_acc = tacc;
          for (unsigned m: used_in_cond.sets())
            {
              if (tacc.has(m))
                complement[m] -= tacc;
              else
                complement[m] &= tacc;
            }
        };
      if (b != e)
        {
          update(b->acc);
          ++b;
          while (b != e)
            {
              if (b->acc != prev_acc)
                update(b->acc);
              ++b;
            }
        }
      aut->set_acceptance(num_sets,
                          remove_compl_rec(&acc.get_acceptance().back(),
                                           complement));
      return aut;
    }


    acc_cond::acc_code acc_rewrite_rec(const acc_cond::acc_word* pos)
    {
      auto start = pos - pos->sub.size;
      switch (pos->sub.op)
        {
          case acc_cond::acc_op::And:
            {
              --pos;
              auto res = acc_cond::acc_code::t();
              do
                {
                  auto tmp = acc_rewrite_rec(pos);
                  tmp &= std::move(res);
                  std::swap(tmp, res);
                  pos -= pos->sub.size + 1;
                }
              while (pos > start);
              return res;
            }
         case acc_cond::acc_op::Or:
            {
              --pos;
              auto res = acc_cond::acc_code::f();
              do
                {
                  auto tmp = acc_rewrite_rec(pos);
                  tmp |= std::move(res);
                  std::swap(tmp, res);
                  pos -= pos->sub.size + 1;
                }
              while (pos > start);
              return res;
            }
          case acc_cond::acc_op::Fin:
            return acc_cond::acc_code::fin(pos[-1].mark);
          case acc_cond::acc_op::Inf:
            return acc_cond::acc_code::inf(pos[-1].mark);
          case acc_cond::acc_op::FinNeg:
          case acc_cond::acc_op::InfNeg:
            SPOT_UNREACHABLE();
        };
        SPOT_UNREACHABLE();
        return {};
    }

    acc_cond::mark_t find_interm_rec(const acc_cond::acc_word* pos)
    {
      acc_cond::acc_op wanted;
      auto topop = pos->sub.op;
      if (topop == acc_cond::acc_op::Or)
        {
          wanted = acc_cond::acc_op::Fin;
        }
      else
        {
          wanted = acc_cond::acc_op::Inf;
          assert(topop == acc_cond::acc_op::And);
        }
      acc_cond::mark_t res = {};
      const acc_cond::acc_word* rend = pos - (pos->sub.size + 1);
      --pos;
      do
        switch (auto op = pos->sub.op)
          {
          case acc_cond::acc_op::Inf:
          case acc_cond::acc_op::Fin:
          case acc_cond::acc_op::InfNeg:
          case acc_cond::acc_op::FinNeg:
            {
              auto m = pos[-1].mark;
              if (op == wanted)
                {
                  res |= m;
                }
              else
                {
                  return {};
                }
              pos -= 2;
              break;
            }
          case acc_cond::acc_op::And:
          case acc_cond::acc_op::Or:
            if (op == topop)
              {
                if (auto m = find_interm_rec(pos))
                  res |= m;
                else
                  return {};
                pos -= pos->sub.size + 1;
              }
            else
              {
                auto posend = pos - (pos->sub.size + 1);
                --pos;
                bool seen = false;
                do
                  {
                    switch (auto op = pos->sub.op)
                      {
                      case acc_cond::acc_op::Inf:
                      case acc_cond::acc_op::Fin:
                      case acc_cond::acc_op::InfNeg:
                      case acc_cond::acc_op::FinNeg:
                        if (op == wanted)
                          {
                            auto m = pos[-1].mark;
                            if (!seen)
                              {
                                seen = true;
                                res |= m;
                              }
                            else
                              {
                                return {};
                              }
                          }
                        pos -= 2;
                        break;
                      case acc_cond::acc_op::And:
                      case acc_cond::acc_op::Or:
                        return {};
                      }
                  }
                while (pos > posend);
              }
            break;
          }
      while (pos > rend);
      return res;
    }

    // Replace Inf(i)|Inf(j) by Inf(k)
    //   or    Fin(i)&Fin(j) by Fin(k)
    // For this to work, k must be one of i or j,
    //                   k must be used only once in the acceptance
    // the transitions have to be updated: every transition marked
    // by i or j should be marked by k.
    void fuse_marks_here(twa_graph_ptr aut)
    {
      acc_cond::acc_code acccopy = aut->get_acceptance();
      acc_cond::mark_t once = acccopy.used_once_sets();
      if (!once)
        return;

      acc_cond::acc_word* pos = &acccopy.back();
      const acc_cond::acc_word* front = &acccopy.front();

      // a list of pairs ({i}, {j, k, l, ...}) where i is a set
      // occurring once that can be removed if all transitions in set
      // i are added to sets j,k,l, ...
      std::vector<std::pair<acc_cond::mark_t, acc_cond::mark_t>> to_fuse;

      auto find_fusable = [&](acc_cond::acc_word* pos)
                          {
                            acc_cond::acc_op wanted;
                            auto topop = pos->sub.op;
                            if (topop == acc_cond::acc_op::And)
                              {
                                wanted = acc_cond::acc_op::Fin;
                              }
                            else
                              {
                                wanted = acc_cond::acc_op::Inf;
                                assert(topop == acc_cond::acc_op::Or);
                              }

                            // Build a vector of "singleton-sets" of
                            // the wanted type in the operand of the
                            // pointed Or/And operator.  For instance,
                            // assuming wanted=Inf and pos points to
                            //
                            // Inf({1})|Inf({2,3})|Fin({4})
                            //         |Inf({5})|Inf({5,6})
                            //
                            // This returns [({1}, Inf({1})),
                            //               ({5}, Inf({5}))]].
                            std::vector<std::pair<acc_cond::mark_t,
                                                  acc_cond::acc_word*>>
                                        singletons;
                            const acc_cond::acc_word* rend =
                              pos - (pos->sub.size + 1);
                            --pos;
                            do
                              {
                                switch (auto op = pos->sub.op)
                                  {
                                  case acc_cond::acc_op::InfNeg:
                                  case acc_cond::acc_op::FinNeg:
                                  case acc_cond::acc_op::Inf:
                                  case acc_cond::acc_op::Fin:
                                    {
                                      if (op == wanted)
                                        singletons.emplace_back(pos[-1].mark,
                                                                pos);
                                      pos -= 2;
                                    }
                                    break;
                                  case acc_cond::acc_op::And:
                                  case acc_cond::acc_op::Or:
                                    // On Fin(a)&(Fin(b)&Inf(c)|Fin(d))
                                    // we'd like to build [({a},...),
                                    // ({b,d},...)] and decide later that
                                    // {b,d} can receive {a} if they
                                    // (b and d) are both used once.
                                    if (auto m = find_interm_rec(pos))
                                      singletons.emplace_back(m, pos);
                                    pos -= pos->sub.size + 1;
                                    break;
                                  }
                              }
                            while (pos > rend);

                            // sort the singletons vector: we want
                            // those that are not really singleton to
                            // be first to they can become recipient
                            std::partition(singletons.begin(), singletons.end(),
                                           [&] (auto s)
                                           { return !s.first.is_singleton(); });

                            acc_cond::mark_t can_receive = {};
                            for (auto p: singletons)
                              if ((p.first & once) == p.first)
                                {
                                  can_receive = p.first;
                                  break;
                                }
                            if (!can_receive)
                              return;
                            for (auto p: singletons)
                              if (p.first != can_receive
                                  && p.first.is_singleton())
                                {
                                  // Mark fused singletons as false,
                                  // so that a future call to
                                  // find_fusable() ignores them.
                                  if (p.second->sub.op == acc_cond::acc_op::Fin)
                                    p.second->sub.op = acc_cond::acc_op::Inf;
                                  else if (p.second->sub.op ==
                                           acc_cond::acc_op::Inf)
                                    p.second->sub.op = acc_cond::acc_op::Fin;
                                  else
                                    continue;
                                  p.second[-1].mark = {};
                                  to_fuse.emplace_back(p.first, can_receive);
                                }
                          };

      do
        {
          switch (pos->sub.op)
            {
            case acc_cond::acc_op::And:
            case acc_cond::acc_op::Or:
              find_fusable(pos);
              // Don't skip to entire operands, as we might find
              // fusable sub-parts.
              --pos;
              break;
            case acc_cond::acc_op::Inf:
            case acc_cond::acc_op::InfNeg:
            case acc_cond::acc_op::FinNeg:
            case acc_cond::acc_op::Fin:
              pos -= 2;
              break;
            }
        }
      while (pos >= front);

      if (to_fuse.empty())
        return;

      // Update the transition according to to_fuse.
      for (auto pair: to_fuse)
        if (pair.first & once) // can we remove pair.first?
          {
            assert(pair.first.is_singleton());
            for (auto& e: aut->edges())
              if (e.acc & pair.first)
                e.acc = (e.acc - pair.first) | pair.second;
          }
        else
          {
            for (auto& e: aut->edges())
              if (e.acc & pair.first)
                e.acc |= pair.second;
          }

      // Now rewrite the acceptance condition, removing all the "to_kill" terms.
      aut->set_acceptance(aut->num_sets(), acc_rewrite_rec(&acccopy.back()));
    }
  }

  twa_graph_ptr simplify_acceptance_here(twa_graph_ptr aut)
  {
    for (;;)
      {
        cleanup_acceptance_here(aut, false);
        merge_identical_marks_here(aut);
        if (aut->acc().is_generalized_buchi())
          break;
        acc_cond::acc_code old = aut->get_acceptance();
        aut->set_acceptance(aut->acc().unit_propagation());
        simplify_complementary_marks_here(aut);
        fuse_marks_here(aut);
        if (old == aut->get_acceptance())
          break;
      }
    cleanup_acceptance_here(aut, true);
    return aut;
  }

  twa_graph_ptr simplify_acceptance(const_twa_graph_ptr aut)
  {
    return simplify_acceptance_here(make_twa_graph(aut, twa::prop_set::all()));
  }
}
