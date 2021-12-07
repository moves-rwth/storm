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
#include <spot/twaalgos/product.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/isdet.hh>
#include <deque>
#include <unordered_map>
#include <spot/misc/hash.hh>

namespace spot
{
  namespace
  {
    typedef std::pair<unsigned, unsigned> product_state;

    struct product_state_hash
    {
      size_t
      operator()(product_state s) const noexcept
      {
        return wang32_hash(s.first ^ wang32_hash(s.second));
      }
    };

    template<typename T>
    static
    void product_main(const const_twa_graph_ptr& left,
                      const const_twa_graph_ptr& right,
                      unsigned left_state,
                      unsigned right_state,
                      twa_graph_ptr& res, T merge_acc,
                      const output_aborter* aborter)
    {
      std::unordered_map<product_state, unsigned, product_state_hash> s2n;
      std::deque<std::pair<product_state, unsigned>> todo;

      auto v = new product_states;
      res->set_named_prop("product-states", v);

      auto new_state =
        [&](unsigned left_state, unsigned right_state) -> unsigned
        {
          product_state x(left_state, right_state);
          auto p = s2n.emplace(x, 0);
          if (p.second)                // This is a new state
            {
              p.first->second = res->new_state();
              todo.emplace_back(x, p.first->second);
              assert(p.first->second == v->size());
              v->emplace_back(x);
            }
          return p.first->second;
        };

      res->set_init_state(new_state(left_state, right_state));
      if (res->acc().is_f())
        // Do not bother doing any work if the resulting acceptance is
        // false.
        return;
      while (!todo.empty())
        {
          if (aborter && aborter->too_large(res))
            {
              res = nullptr;
              return;
            }
          auto top = todo.front();
          todo.pop_front();
          for (auto& l: left->out(top.first.first))
            for (auto& r: right->out(top.first.second))
              {
                auto cond = l.cond & r.cond;
                if (cond == bddfalse)
                  continue;
                auto dst = new_state(l.dst, r.dst);
                res->new_edge(top.second, dst, cond,
                              merge_acc(l.acc, r.acc));
                // If right is deterministic, we can abort immediately!
              }
        }
    }

    enum acc_op { and_acc, or_acc, xor_acc, xnor_acc };


    static
    twa_graph_ptr product_aux(const const_twa_graph_ptr& left,
                              const const_twa_graph_ptr& right,
                              unsigned left_state,
                              unsigned right_state,
                              acc_op aop,
                              const output_aborter* aborter)
    {
      if (SPOT_UNLIKELY(!(left->is_existential() && right->is_existential())))
        throw std::runtime_error
          ("product() does not support alternating automata");
      if (SPOT_UNLIKELY(left->get_dict() != right->get_dict()))
        throw std::runtime_error("product: left and right automata should "
                                 "share their bdd_dict");

      auto res = make_twa_graph(left->get_dict());
      res->copy_ap_of(left);
      res->copy_ap_of(right);

      //bool leftweak = left->prop_weak().is_true();
      //bool rightweak = right->prop_weak().is_true();
      bool leftweak = false;
      bool rightweak = false;
      // We have optimization to the standard product in case one
      // of the arguments is weak.
      if (leftweak || rightweak)
        {
          // If both automata are weak, we can restrict the result to
          // t, f, Büchi or co-Büchi.  We use co-Büchi only when
          // t and f cannot be used, and both acceptance conditions
          // are in {t,f,co-Büchi}.
          if (leftweak && rightweak)
            {
            weak_weak:
              res->prop_weak(true);
              acc_cond::mark_t accmark = {0};
              acc_cond::mark_t rejmark = {};
              auto& lacc = left->acc();
              auto& racc = right->acc();
              if ((lacc.is_co_buchi() && (racc.is_co_buchi()
                                         || racc.num_sets() == 0))
                  || (lacc.num_sets() == 0 && racc.is_co_buchi()))
                {
                  res->set_co_buchi();
                  std::swap(accmark, rejmark);
                }
              else if ((aop == and_acc && lacc.is_t() && racc.is_t())
                       || (aop == or_acc && (lacc.is_t() || racc.is_t()))
                       || (aop == xnor_acc && ((lacc.is_t() && racc.is_t()) ||
                                               (lacc.is_f() && racc.is_f())))
                       || (aop == xor_acc && ((lacc.is_t() && racc.is_f()) ||
                                              (lacc.is_f() && racc.is_t()))))
                {
                  res->set_acceptance(0, acc_cond::acc_code::t());
                  accmark = {};
                }
              else if ((aop == and_acc && (lacc.is_f() || racc.is_f()))
                       || (aop == or_acc && lacc.is_f() && racc.is_f())
                       || (aop == xor_acc && ((lacc.is_t() && racc.is_t()) ||
                                              (lacc.is_f() && racc.is_f())))
                       || (aop == xnor_acc && ((lacc.is_t() && racc.is_f()) ||
                                               (lacc.is_f() && racc.is_t()))))
                {
                  res->set_acceptance(0, acc_cond::acc_code::f());
                  accmark = {};
                }
              else
                {
                  res->set_buchi();
                }
              switch (aop)
                {
                case and_acc:
                  product_main(left, right, left_state, right_state, res,
                               [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                               {
                                 if (lacc.accepting(ml) && racc.accepting(mr))
                                   return accmark;
                                 else
                                   return rejmark;
                               }, aborter);
                  break;
                case or_acc:
                  product_main(left, right, left_state, right_state, res,
                               [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                               {
                                 if (lacc.accepting(ml) || racc.accepting(mr))
                                   return accmark;
                                 else
                                   return rejmark;
                               }, aborter);
                  break;
                case xor_acc:
                  product_main(left, right, left_state, right_state, res,
                               [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                               {
                                 if (lacc.accepting(ml) ^ racc.accepting(mr))
                                   return accmark;
                                 else
                                   return rejmark;
                               }, aborter);
                  break;
                case xnor_acc:
                  product_main(left, right, left_state, right_state, res,
                               [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                               {
                                 if (lacc.accepting(ml) == racc.accepting(mr))
                                   return accmark;
                                 else
                                   return rejmark;
                               }, aborter);
                  break;
                }
            }
          else if (!rightweak)
            {
              switch (aop)
                {
                case and_acc:
                  {
                    auto rightunsatmark = right->acc().unsat_mark();
                    if (!rightunsatmark.first)
                      {
                        // Left is weak.  Right was not weak, but it is
                        // always accepting.  We can therefore pretend
                        // that right is weak.
                        goto weak_weak;
                      }
                    res->copy_acceptance_of(right);
                    acc_cond::mark_t rejmark = rightunsatmark.second;
                    auto& lacc = left->acc();
                    product_main(left, right, left_state, right_state, res,
                                 [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                                 {
                                   if (lacc.accepting(ml))
                                     return mr;
                                   else
                                     return rejmark;
                                 }, aborter);
                    break;
                  }
                case or_acc:
                  {
                    auto rightsatmark = right->acc().sat_mark();
                    if (!rightsatmark.first)
                      {
                        // Left is weak.  Right was not weak, but it is
                        // always rejecting.  We can therefore pretend
                        // that right is weak.
                        goto weak_weak;
                      }
                    res->copy_acceptance_of(right);
                    acc_cond::mark_t accmark = rightsatmark.second;
                    auto& lacc = left->acc();
                    product_main(left, right, left_state, right_state, res,
                                 [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                                 {
                                   if (!lacc.accepting(ml))
                                     return mr;
                                   else
                                     return accmark;
                                 }, aborter);
                    break;
                  }
                case xor_acc:
                case xnor_acc:
                  {
                    auto rightsatmark = right->acc().sat_mark();
                    auto rightunsatmark = right->acc().unsat_mark();
                    if (!rightunsatmark.first || !rightsatmark.first)
                      {
                        // Left is weak.  Right was not weak, but it
                        // is either always rejecting or always
                        // accepting.  We can therefore pretend that
                        // right is weak.
                        goto weak_weak;
                      }
                    goto generalcase;
                    break;
                  }
                }
            }
          else // right weak
            {
              assert(!leftweak);
              switch (aop)
                {
                case and_acc:
                  {
                    auto leftunsatmark = left->acc().unsat_mark();
                    if (!leftunsatmark.first)
                      {
                        // Right is weak.  Left was not weak, but it is
                        // always accepting.  We can therefore pretend
                        // that left is weak.
                        goto weak_weak;
                      }
                    res->copy_acceptance_of(left);
                    acc_cond::mark_t rejmark = leftunsatmark.second;
                    auto& racc = right->acc();
                    product_main(left, right, left_state, right_state, res,
                                 [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                                 {
                                   if (racc.accepting(mr))
                                     return ml;
                                   else
                                     return rejmark;
                                 }, aborter);
                    break;
                  }
                case or_acc:
                  {
                    auto leftsatmark = left->acc().sat_mark();
                    if (!leftsatmark.first)
                      {
                        // Right is weak.  Left was not weak, but it is
                        // always rejecting.  We can therefore pretend
                        // that left is weak.
                        goto weak_weak;
                      }
                    res->copy_acceptance_of(left);
                    acc_cond::mark_t accmark = leftsatmark.second;
                    auto& racc = right->acc();
                    product_main(left, right, left_state, right_state, res,
                                 [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                                 {
                                   if (!racc.accepting(mr))
                                     return ml;
                                   else
                                     return accmark;
                                 }, aborter);

                    break;
                  }
                case xor_acc:
                case xnor_acc:
                  {
                    auto leftsatmark = left->acc().sat_mark();
                    auto leftunsatmark = left->acc().unsat_mark();
                    if (!leftunsatmark.first || !leftsatmark.first)
                      {
                        // Right is weak.  Left was not weak, but it
                        // is either always rejecting or always
                        // accepting.  We can therefore pretend that
                        // left is weak.
                        goto weak_weak;
                      }
                    goto generalcase;
                    break;
                  }
                }
            }
        }
      else // general case
        {
        generalcase:
          auto left_num = left->num_sets();
          auto& left_acc = left->get_acceptance();
          auto right_acc = right->get_acceptance() << left_num;
          switch (aop)
            {
            case and_acc:
              right_acc &= left_acc;
              break;
            case or_acc:
              right_acc |= left_acc;
              break;
            case xor_acc:
              {
                auto tmp = right_acc.complement() & left_acc;
                right_acc &= left_acc.complement();
                right_acc |= tmp;
                break;
              }
            case xnor_acc:
              {
                auto tmp = right_acc.complement() & left_acc.complement();
                right_acc &= left_acc;
                tmp |= right_acc;
                std::swap(tmp, right_acc);
                break;
              }
            }
          res->set_acceptance(left_num + right->num_sets(), right_acc);
          product_main(left, right, left_state, right_state, res,
                       [&] (acc_cond::mark_t ml, acc_cond::mark_t mr)
                       {
                         return ml | (mr << left_num);
                       }, aborter);
        }

      if (!res)                 // aborted
        return nullptr;

      if (res->acc().is_f())
        {
          assert(res->num_edges() == 0);
          res->prop_universal(true);
          res->prop_complete(false);
          res->prop_stutter_invariant(true);
          res->prop_terminal(true);
          res->prop_state_acc(true);
        }
      else
        {
          // The product of two non-deterministic automata could be
          // deterministic.  Likewise for non-complete automata.
          if (left->prop_universal() && right->prop_universal())
            res->prop_universal(true);
          if (left->prop_complete() && right->prop_complete())
            res->prop_complete(true);
          if (left->prop_stutter_invariant() && right->prop_stutter_invariant())
            res->prop_stutter_invariant(true);
          if (left->prop_inherently_weak() && right->prop_inherently_weak())
            res->prop_inherently_weak(true);
          if (left->prop_weak() && right->prop_weak())
            res->prop_weak(true);
          if (left->prop_terminal() && right->prop_terminal())
            res->prop_terminal(true);
          res->prop_state_acc(left->prop_state_acc()
                              && right->prop_state_acc());
        }
      return res;
    }
  }

  twa_graph_ptr product(const const_twa_graph_ptr& left,
                        const const_twa_graph_ptr& right,
                        unsigned left_state,
                        unsigned right_state,
                        const output_aborter* aborter)
  {
    return product_aux(left, right, left_state, right_state, and_acc, aborter);
  }

  twa_graph_ptr product(const const_twa_graph_ptr& left,
                        const const_twa_graph_ptr& right,
                        const output_aborter* aborter)
  {
    return product(left, right,
                   left->get_init_state_number(),
                   right->get_init_state_number(), aborter);
  }

  twa_graph_ptr product_or(const const_twa_graph_ptr& left,
                           const const_twa_graph_ptr& right,
                           unsigned left_state,
                           unsigned right_state)
  {
    return product_aux(complete(left), complete(right),
                       left_state, right_state, or_acc, nullptr);
  }

  twa_graph_ptr product_or(const const_twa_graph_ptr& left,
                           const const_twa_graph_ptr& right)
  {
    return product_or(left, right,
                      left->get_init_state_number(),
                      right->get_init_state_number());
  }

  twa_graph_ptr product_xor(const const_twa_graph_ptr& left,
                            const const_twa_graph_ptr& right)
  {
    if (SPOT_UNLIKELY(!is_deterministic(left) || !is_deterministic(right)))
      throw std::runtime_error
        ("product_xor() only works with deterministic automata");

    return product_aux(complete(left), complete(right),
                       left->get_init_state_number(),
                       right->get_init_state_number(),
                       xor_acc, nullptr);
  }

  twa_graph_ptr product_xnor(const const_twa_graph_ptr& left,
                             const const_twa_graph_ptr& right)
  {
    if (SPOT_UNLIKELY(!is_deterministic(left) || !is_deterministic(right)))
      throw std::runtime_error
        ("product_xnor() only works with deterministic automata");

    return product_aux(complete(left), complete(right),
                       left->get_init_state_number(),
                       right->get_init_state_number(),
                       xnor_acc, nullptr);
  }


  namespace
  {

    template<typename T>
    static void
    product_susp_aux(const const_twa_graph_ptr& left,
                     const const_twa_graph_ptr& right,
                     twa_graph_ptr res, bool and_acc,
                     bool sync_all, acc_cond::mark_t rejmark, T merge_acc)
    {
      std::unordered_map<product_state, unsigned, product_state_hash> s2n;
      std::deque<std::pair<product_state, unsigned>> todo;

      scc_info si(left,
                  and_acc ? scc_info_options::TRACK_STATES_IF_FIN_USED
                  : (scc_info_options::TRACK_STATES_IF_FIN_USED
                     | scc_info_options::TRACK_SUCCS));
      si.determine_unknown_acceptance();

      auto new_state =
        [&](unsigned left_state, unsigned right_state) -> unsigned
        {
          product_state x(left_state, right_state);
          auto p = s2n.emplace(x, 0);
          if (p.second)                // This is a new state
            {
              p.first->second = res->new_state();
              todo.emplace_back(x, p.first->second);
            }
          return p.first->second;
        };

      unsigned right_init = right->get_init_state_number();
      unsigned left_init = left->get_init_state_number();
      unsigned res_init;

      auto target_scc = [&](unsigned scc) -> bool
        {
          return (!si.is_trivial(scc)
                  && (sync_all || si.is_accepting_scc(scc) == and_acc));
        };

      if (target_scc(si.scc_of(left_init)))
        res_init = new_state(left_init, right_init);
      else
        res_init = new_state(left_init, -1U);
      res->set_init_state(res_init);

      bool sbacc = res->prop_state_acc().is_true();

      while (!todo.empty())
        {
          auto top = todo.front();
          todo.pop_front();
          for (auto& l: left->out(top.first.first))
            if (!target_scc(si.scc_of(l.dst)))
              {
                acc_cond::mark_t right_acc =
                  (sbacc && top.first.second != -1U)
                  // This edge leaves a target SCC, but we build a
                  // state-based automaton, so make sure we still use
                  // the same acceptance marks as in the SCC we leave.
                  ? right->state_acc_sets(top.first.second)
                  : rejmark;
                res->new_edge(top.second, new_state(l.dst, -1U), l.cond,
                              merge_acc(l.acc, right_acc));
              }
            else
              {
                unsigned right_state = top.first.second;
                if (top.first.second == -1U)
                  right_state = right_init;
                for (auto& r: right->out(right_state))
                  {
                    auto cond = l.cond & r.cond;
                    if (cond == bddfalse)
                      continue;
                    auto dst = new_state(l.dst, r.dst);

                    // For state-based automata, we cannot use the
                    // right-mark when entering a target SCC, because
                    // another sibling transition might be going to a
                    // non-target SCC without this mark.
                    acc_cond::mark_t right_acc =
                      (sbacc && top.first.second == -1U) ? rejmark : r.acc;
                    res->new_edge(top.second, dst, cond,
                                  merge_acc(l.acc, right_acc));
                  }
              }
        }
    }

    static twa_graph_ptr
    product_susp_main(const const_twa_graph_ptr& left,
                      const const_twa_graph_ptr& right,
                      bool and_acc = true)
    {
      if (SPOT_UNLIKELY(!(left->is_existential() && right->is_existential())))
        throw std::runtime_error
          ("product_susp() does not support alternating automata");
      if (SPOT_UNLIKELY(left->get_dict() != right->get_dict()))
        throw std::runtime_error("product_susp(): left and right automata "
                                 "should share their bdd_dict");

      auto false_or_left = [&] (bool ff)
        {
          if (ff)
            {
              auto res = make_twa_graph(left->get_dict());
              res->new_state();
              res->prop_terminal(true);
              res->prop_stutter_invariant(true);
              res->prop_universal(true);
              res->prop_complete(false);
              return res;
            }
          return make_twa_graph(left, twa::prop_set::all());
        };

      // We assume RIGHT is suspendable, but we want to deal with some
      // trivial true/false cases so we can later assume right has
      // more than one acceptance set.
      // Note: suspendable with "t" acceptance = universal language.
      if (SPOT_UNLIKELY(right->num_sets() == 0))
        {
          if (and_acc)
            return false_or_left(right->is_empty());
          else if (right->is_empty()) // left OR false = left
            return make_twa_graph(left, twa::prop_set::all());
          else // left OR true = true
            return make_twa_graph(right, twa::prop_set::all());
        }

      auto res = make_twa_graph(left->get_dict());
      res->copy_ap_of(left);
      res->copy_ap_of(right);
      bool leftweak = left->prop_weak().is_true();

      res->prop_state_acc(left->prop_state_acc() && right->prop_state_acc());

      auto rightunsatmark = right->acc().unsat_mark();
      if (SPOT_UNLIKELY(!rightunsatmark.first))
        return false_or_left(and_acc);
      acc_cond::mark_t rejmark = rightunsatmark.second;

      if (leftweak)
        {
          res->copy_acceptance_of(right);
          if (and_acc)
            {
              product_susp_aux(left, right, res, true, false, rejmark,
                               [&] (acc_cond::mark_t,
                                    acc_cond::mark_t mr)
                               {
                                 return mr;
                               });
            }
          else
            {
              auto rightsatmark = right->acc().sat_mark();
              if (!rightsatmark.first)
                // Right is always rejecting, no point in making a product_or
                return make_twa_graph(left, twa::prop_set::all());
              acc_cond::mark_t accmark = rightsatmark.second;
              auto& lacc = left->acc();
              product_susp_aux(left, right, res, false, false, rejmark,
                               [&] (acc_cond::mark_t ml,
                                    acc_cond::mark_t mr)
                               {
                                 if (!lacc.accepting(ml))
                                   return mr;
                                 else
                                   return accmark;
                               });
            }
        }
      else // general case
        {
          auto left_num = left->num_sets();
          auto right_acc = right->get_acceptance() << left_num;
          if (and_acc)
            right_acc &= left->get_acceptance();
          else
            right_acc |= left->get_acceptance();
          res->set_acceptance(left_num + right->num_sets(), right_acc);

          product_susp_aux(left, right, res, and_acc, !and_acc, rejmark,
                           [&] (acc_cond::mark_t ml,
                                acc_cond::mark_t mr)
                           {
                             return ml | (mr << left_num);
                           });
        }

      // The product of two non-deterministic automata could be
      // deterministic.  Likewise for non-complete automata.
      if (left->prop_universal() && right->prop_universal())
        res->prop_universal(true);
      if (left->prop_complete() && right->prop_complete())
        res->prop_complete(true);
      if (left->prop_stutter_invariant() && right->prop_stutter_invariant())
        res->prop_stutter_invariant(true);
      if (left->prop_inherently_weak() && right->prop_inherently_weak())
        res->prop_inherently_weak(true);
      if (left->prop_weak() && right->prop_weak())
        res->prop_weak(true);
      if (left->prop_terminal() && right->prop_terminal())
        res->prop_terminal(true);
      return res;
    }
  }

  twa_graph_ptr product_susp(const const_twa_graph_ptr& left,
                             const const_twa_graph_ptr& right)
  {
    return product_susp_main(left, right);
  }

  twa_graph_ptr product_or_susp(const const_twa_graph_ptr& left,
                                const const_twa_graph_ptr& right)
  {
    return product_susp_main(complete(left), right, false);
  }

}
