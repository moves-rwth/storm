// -*- coding: utf-8 -*-
// Copyright (C) 2014-2020 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
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
#include <spot/twaalgos/stutter.hh>
#include <spot/twa/twa.hh>
#include <spot/misc/hash.hh>
#include <spot/misc/hashfunc.hh>
#include <spot/tl/apcollect.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/tl/remove_x.hh>
#include <spot/twaalgos/product.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/complement.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/word.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/twa/bddprint.hh>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <numeric>

namespace spot
{
  namespace
  {
    class state_tgbasl final: public state
    {
    public:
      state_tgbasl(const state* s, bdd cond) : s_(s), cond_(cond)
      {
      }

      virtual
      ~state_tgbasl()
      {
        s_->destroy();
      }

      virtual int
      compare(const state* other) const override
      {
        const state_tgbasl* o =
          down_cast<const state_tgbasl*>(other);
        int res = s_->compare(o->real_state());
        if (res != 0)
          return res;
        return cond_.id() - o->cond_.id();
      }

      virtual size_t
      hash() const override
      {
        return wang32_hash(s_->hash()) ^ wang32_hash(cond_.id());
      }

      virtual
      state_tgbasl* clone() const override
      {
        return new state_tgbasl(s_, cond_);
      }

      const state*
      real_state() const
      {
        return s_;
      }

      bdd
      cond() const
      {
        return cond_;
      }

    private:
      const state* s_;
      bdd cond_;
    };

    class twasl_succ_iterator final : public twa_succ_iterator
    {
    public:
      twasl_succ_iterator(twa_succ_iterator* it, const state_tgbasl* state,
                           bdd_dict_ptr d, bdd atomic_propositions)
        : it_(it), state_(state), aps_(atomic_propositions), d_(d)
      {
      }

      virtual
      ~twasl_succ_iterator()
      {
        delete it_;
      }

      // iteration

      virtual bool
      first() override
      {
        loop_ = false;
        done_ = false;
        need_loop_ = true;
        if (it_->first())
          {
            cond_ = it_->cond();
            next_edge();
          }
        return true;
      }

      virtual bool
      next() override
      {
        if (cond_ != bddfalse)
          {
            next_edge();
            return true;
          }
        if (!it_->next())
          {
            if (loop_ || !need_loop_)
              done_ = true;
            loop_ = true;
            return !done_;
          }
        else
          {
            cond_ = it_->cond();
            next_edge();
            return true;
          }
      }

      virtual bool
      done() const override
      {
        return it_->done() && done_;
      }

      // inspection

      virtual state_tgbasl*
      dst() const override
      {
        if (loop_)
          return new state_tgbasl(state_->real_state(), state_->cond());
        return new state_tgbasl(it_->dst(), one_);
      }

      virtual bdd
      cond() const override
      {
        if (loop_)
          return state_->cond();
        return one_;
      }

      virtual acc_cond::mark_t
      acc() const override
      {
        if (loop_)
          return {};
        return it_->acc();
      }

    private:
      void
      next_edge()
      {
        one_ = bdd_satoneset(cond_, aps_, bddtrue);
        cond_ -= one_;
        if (need_loop_ && (state_->cond() == one_)
            && (state_ == it_->dst()))
          need_loop_ = false;
      }

      twa_succ_iterator* it_;
      const state_tgbasl* state_;
      bdd cond_;
      bdd one_;
      bdd aps_;
      bdd_dict_ptr d_;
      bool loop_;
      bool need_loop_;
      bool done_;
    };


    class tgbasl final : public twa
    {
    public:
      tgbasl(const_twa_ptr a)
        : twa(a->get_dict()), a_(a)
      {
        copy_ap_of(a);
        copy_acceptance_of(a_);
      }

      virtual const state* get_init_state() const override
      {
        return new state_tgbasl(a_->get_init_state(), bddfalse);
      }

      virtual twa_succ_iterator* succ_iter(const state* state) const override
      {
        const state_tgbasl* s = down_cast<const state_tgbasl*>(state);
        return new twasl_succ_iterator(a_->succ_iter(s->real_state()), s,
                                       a_->get_dict(), ap_vars());
      }

      virtual std::string format_state(const state* state) const override
      {
        const state_tgbasl* s = down_cast<const state_tgbasl*>(state);
        return (a_->format_state(s->real_state())
                + ", "
                + bdd_format_formula(a_->get_dict(), s->cond()));
      }

    private:
      const_twa_ptr a_;
    };

    typedef std::shared_ptr<tgbasl> tgbasl_ptr;

    inline tgbasl_ptr make_tgbasl(const const_twa_ptr& aut)
    {
      return std::make_shared<tgbasl>(aut);
    }



    typedef std::pair<unsigned, bdd> stutter_state;

    struct stutter_state_hash
    {
      size_t
      operator()(const stutter_state& s) const noexcept
      {
        return wang32_hash(s.first) ^ wang32_hash(s.second.id());
      }
    };

    // Associate the stutter state to its number.
    typedef std::unordered_map<stutter_state, unsigned,
                               stutter_state_hash> ss2num_map;

    // Queue of state to be processed.
    typedef std::deque<stutter_state> queue_t;
  }

  twa_graph_ptr
  sl(const_twa_graph_ptr a)
  {
    // The result automaton uses numbered states.
    twa_graph_ptr res = make_twa_graph(a->get_dict());
    bdd atomic_propositions = a->ap_vars();
    // We use the same BDD variables as the input.
    res->copy_ap_of(a);
    res->copy_acceptance_of(a);

    // We are going to create self-loop labeled by {},
    // and those should not be accepting.  If they are,
    // we need to upgrade the acceptance condition.
    acc_cond::mark_t toadd = {};
    if (res->acc().accepting({}))
      {
        unsigned ns = res->num_sets();
        auto na = res->get_acceptance() & acc_cond::acc_code::inf({ns});
        res->set_acceptance(ns + 1, na);
        toadd = {ns};
      }

    // These maps make it possible to convert stutter_state to number
    // and vice-versa.
    ss2num_map ss2num;

    queue_t todo;

    unsigned s0 = a->get_init_state_number();
    stutter_state s(s0, bddfalse);
    ss2num[s] = 0;
    res->new_state();
    todo.emplace_back(s);

    while (!todo.empty())
      {
        s = todo.front();
        todo.pop_front();
        unsigned src = ss2num[s];

        bool self_loop_needed = true;

        for (auto& t : a->out(s.first))
          {
            bdd all = t.cond;
            while (all != bddfalse)
              {
                bdd one = bdd_satoneset(all, atomic_propositions, bddtrue);
                all -= one;

                stutter_state d(t.dst, one);

                auto r = ss2num.emplace(d, ss2num.size());
                unsigned dest = r.first->second;

                if (r.second)
                  {
                    todo.emplace_back(d);
                    unsigned u = res->new_state();
                    assert(u == dest);
                    (void)u;
                  }

                // Create the edge.
                res->new_edge(src, dest, one, t.acc | toadd);

                if (src == dest)
                  self_loop_needed = false;
              }
          }

        if (self_loop_needed && s.second != bddfalse)
          res->new_edge(src, src, s.second, {});
      }
    res->merge_edges();
    return res;
  }

  twa_graph_ptr
  sl2_inplace(twa_graph_ptr a)
  {
    // We are going to create self-loop labeled by {},
    // and those should not be accepting.  If they are,
    // we need to upgrade the acceptance condition.
    if (a->acc().accepting({}))
      {
        unsigned ns = a->num_sets();
        auto na = a->get_acceptance() & acc_cond::acc_code::inf({ns});
        a->set_acceptance(ns + 1, na);
        acc_cond::mark_t toadd = {ns};
        for (auto& e: a->edges())
          e.acc |= toadd;
      }

    // The self-loops we add should not be accepting, so try to find
    // an unsat mark, and upgrade the acceptance condition if
    // necessary.
    //
    // UM is a pair (bool, mark).  If the Boolean is false, the
    // acceptance is always satisfiable.  Otherwise, MARK is an
    // example of unsatisfiable mark.
    auto um = a->acc().unsat_mark();
    if (!um.first)
      {
        auto m = a->set_buchi();
        for (auto& e: a->edges())
          e.acc = m;
        um.second = {};
      }
    acc_cond::mark_t unsat = um.second;

    bdd atomic_propositions = a->ap_vars();
    unsigned num_states = a->num_states();
    unsigned num_edges = a->num_edges();
    std::vector<bdd> selfloops(num_states, bddfalse);
    std::map<std::pair<unsigned, int>, unsigned> newstates;
    // Record all the conditions for which we can selfloop on each
    // state.
    for (auto& t: a->edges())
      if (t.src == t.dst)
        selfloops[t.src] |= t.cond;
    for (unsigned t = 1; t <= num_edges; ++t)
      {
        auto& td = a->edge_storage(t);
        if (a->is_dead_edge(td))
          continue;

        unsigned src = td.src;
        unsigned dst = td.dst;
        if (src != dst)
          {
            bdd all = td.cond;
            // If there is a self-loop with the whole condition on
            // either end of the edge, do not bother with it.
            if (bdd_implies(all, selfloops[src])
                || bdd_implies(all, selfloops[dst]))
              continue;
            // Do not use td in the loop because the new_edge()
            // might invalidate it.
            auto acc = td.acc;
            while (all != bddfalse)
              {
                bdd one = bdd_satoneset(all, atomic_propositions, bddtrue);
                all -= one;
                // Skip if there is a loop for this particular letter.
                if (bdd_implies(one, selfloops[src])
                    || bdd_implies(one, selfloops[dst]))
                  continue;
                auto p = newstates.emplace(std::make_pair(dst, one.id()), 0);
                if (p.second)
                  p.first->second = a->new_state();
                unsigned tmp = p.first->second; // intermediate state
                unsigned i = a->new_edge(src, tmp, one, acc);
                assert(i > num_edges);
                i = a->new_edge(tmp, tmp, one, unsat);
                assert(i > num_edges);
                // unsat acceptance here to preserve the state-based property.
                i = a->new_edge(tmp, dst, one, unsat);
                assert(i > num_edges);
                (void)i;
              }
          }
      }
    if (num_states != a->num_states())
      a->prop_keep({true,         // state_based
                    false,        // inherently_weak
                    false, false, // deterministic
                    true,         // complete
                    false,        // stutter inv.
                   });
    a->merge_edges();
    return a;
  }

  twa_graph_ptr
  sl2(const_twa_graph_ptr a)
  {
    return sl2_inplace(make_twa_graph(a, twa::prop_set::all()));
  }

  twa_graph_ptr
  closure_inplace(twa_graph_ptr a)
  {
    // In the fin-less version of the closure, we can merge edges that
    // have the same src, letter, and destination by taking the union
    // of their marks.  If some Fin() is used, we cannot do that.
    bool fin_less = !a->acc().uses_fin_acceptance();

    a->prop_keep({false,        // state_based
                  false,        // inherently_weak
                  false, false, // deterministic
                  true,         // complete
                  false,        // stutter inv.
                 });

    unsigned n = a->num_states();
    std::vector<unsigned> todo;
    std::vector<std::vector<unsigned> > dst2trans(n);

    for (unsigned state = 0; state < n; ++state)
      {
        auto trans = a->out(state);

        for (auto it = trans.begin(); it != trans.end(); ++it)
          {
            todo.emplace_back(it.trans());
            dst2trans[it->dst].emplace_back(it.trans());
          }

        while (!todo.empty())
          {
            auto t1 = a->edge_storage(todo.back());
            todo.pop_back();

            for (auto& t2 : a->out(t1.dst))
              {
                bdd cond = t1.cond & t2.cond;
                if (cond != bddfalse)
                  {
                    bool need_new_trans = true;
                    acc_cond::mark_t acc = t1.acc | t2.acc;
                    for (auto& t: dst2trans[t2.dst])
                      {
                        auto& ts = a->edge_storage(t);
                        if (acc == ts.acc)
                          {
                            if (!bdd_implies(cond, ts.cond))
                              {
                                ts.cond |= cond;
                                if (std::find(todo.begin(), todo.end(), t)
                                    == todo.end())
                                  todo.emplace_back(t);
                              }
                            need_new_trans = false;
                            break;
                          }
                        else if (fin_less && cond == ts.cond)
                          {
                            acc |= ts.acc;
                            if (ts.acc != acc)
                              {
                                ts.acc = acc;
                                if (std::find(todo.begin(), todo.end(), t)
                                    == todo.end())
                                  todo.emplace_back(t);
                              }
                            need_new_trans = false;
                            break;
                          }
                      }
                    if (need_new_trans)
                      {
                        // Load t2.dst first, because t2 can be
                        // invalidated by new_edge().
                        auto dst = t2.dst;
                        auto i = a->new_edge(state, dst, cond, acc);
                        dst2trans[dst].emplace_back(i);
                        todo.emplace_back(i);
                      }
                  }
              }
          }
        for (auto& it: dst2trans)
          it.clear();
      }
    return a;
  }

  twa_graph_ptr
  closure(const_twa_graph_ptr a)
  {
    return closure_inplace(make_twa_graph(a, twa::prop_set::all()));
  }

  namespace
  {
    // The stutter check algorithm to use can be overridden via an
    // environment variable.
    static int default_stutter_check_algorithm()
    {
      static const char* stutter_check = getenv("SPOT_STUTTER_CHECK");
      if (stutter_check)
        {
          char* endptr;
          long res = strtol(stutter_check, &endptr, 10);
          if (*endptr || res < 0 || res > 9)
            throw
              std::runtime_error("invalid value for SPOT_STUTTER_CHECK.");
          return res;
        }
      else
        {
          return 8;     // The best variant, according to our benchmarks.
        }
    }
  }

  namespace
  {
    // The own_f and own_nf tell us whether we can modify the aut_f
    // and aut_nf automata inplace.
    static bool do_si_check(const_twa_graph_ptr aut_f, bool own_f,
                            const_twa_graph_ptr aut_nf, bool own_nf,
                            int algo)
    {
      auto cl = [](const_twa_graph_ptr a, bool own) {
        if (own)
          return closure_inplace(std::const_pointer_cast<twa_graph>
                                 (std::move(a)));
        return closure(std::move(a));
      };
      auto sl_2 = [](const_twa_graph_ptr a, bool own) {
        if (own)
          return sl2_inplace(std::const_pointer_cast<twa_graph>(std::move(a)));
        return sl2(std::move(a));
      };

      switch (algo)
        {
        case 1: // sl(aut_f) x sl(aut_nf)
          return product(sl(std::move(aut_f)),
                         sl(std::move(aut_nf)))->is_empty();
        case 2: // sl(cl(aut_f)) x aut_nf
          return product(sl(cl(std::move(aut_f), own_f)),
                         std::move(aut_nf))->is_empty();
        case 3: // (cl(sl(aut_f)) x aut_nf
          return product(closure_inplace(sl(std::move(aut_f))),
                         std::move(aut_nf))->is_empty();
        case 4: // sl2(aut_f) x sl2(aut_nf)
          return product(sl_2(std::move(aut_f), own_f),
                         sl_2(std::move(aut_nf), own_nf))
            ->is_empty();
        case 5: // sl2(cl(aut_f)) x aut_nf
          return product(sl2_inplace(cl(std::move(aut_f), own_f)),
                         std::move(aut_nf))->is_empty();
        case 6: // (cl(sl2(aut_f)) x aut_nf
          return product(closure_inplace(sl_2(std::move(aut_f), own_f)),
                         std::move(aut_nf))->is_empty();
        case 7: // on-the-fly sl(aut_f) x sl(aut_nf)
          return otf_product(make_tgbasl(std::move(aut_f)),
                             make_tgbasl(std::move(aut_nf)))->is_empty();
        case 8: // cl(aut_f) x cl(aut_nf)
          return product(cl(std::move(aut_f), own_f),
                         cl(std::move(aut_nf), own_nf))->is_empty();
        default:
          throw std::runtime_error("is_stutter_invariant(): "
                                   "invalid algorithm number");
          SPOT_UNREACHABLE();
        }
    }

    bool
    is_stutter_invariant_aux(twa_graph_ptr aut_f,
                             bool own_f,
                             const_twa_graph_ptr aut_nf = nullptr,
                             int algo = 0)
    {
      trival si = aut_f->prop_stutter_invariant();
      if (si.is_known())
        return si.is_true();
      if (aut_nf)
        {
          trival si_n = aut_nf->prop_stutter_invariant();
          if (si_n.is_known())
            {
              bool res = si_n.is_true();
              aut_f->prop_stutter_invariant(res);
              return res;
            }
        }

      if (algo == 0)
        algo = default_stutter_check_algorithm();

      bool own_nf = false;
      if (!aut_nf)
        {
          aut_nf = complement(aut_f);
          own_nf = true;
        }
      bool res = do_si_check(aut_f, own_f,
                             std::move(aut_nf), own_nf,
                             algo);
      aut_f->prop_stutter_invariant(res);
      return res;
    }

  }

  bool
  is_stutter_invariant(twa_graph_ptr aut_f,
                       const_twa_graph_ptr aut_nf,
                       int algo)
  {
    return is_stutter_invariant_aux(aut_f, false, aut_nf, algo);
  }

  bool
  is_stutter_invariant(formula f, twa_graph_ptr aut_f)
  {
    if (f.is_ltl_formula() && f.is_syntactic_stutter_invariant())
      return true;

    int algo = default_stutter_check_algorithm();

    if (algo == 0 || algo == 9)
      // Etessami's check via syntactic transformation.
      {
        if (!f.is_ltl_formula())
          throw std::runtime_error("Cannot use the syntactic "
                                   "stutter-invariance check "
                                   "for non-LTL formulas");
        formula g = remove_x(f);
        bool res;
        if (algo == 0)                // Equivalence check
          {
            tl_simplifier ls;
            res = ls.are_equivalent(f, g);
          }
        else
          {
            formula h = formula::Xor(f, g);
            res = ltl_to_tgba_fm(h, make_bdd_dict())->is_empty();
          }
        return res;
      }

    // Prepare for an automata-based check.
    translator trans(aut_f ? aut_f->get_dict() : make_bdd_dict());
    bool own_f = false;
    if (!aut_f)
      {
        aut_f = trans.run(f);
        own_f = true;
      }
    return is_stutter_invariant_aux(aut_f, own_f, trans.run(formula::Not(f)));
  }

  trival
  check_stutter_invariance(twa_graph_ptr aut, formula f,
                           bool do_not_determinize,
                           bool find_counterexamples)
  {
    trival is_stut = aut->prop_stutter_invariant();
    if (!find_counterexamples && is_stut.is_known())
      return is_stut.is_true();

    twa_graph_ptr neg = nullptr;
    if (f)
      neg = translator(aut->get_dict()).run(formula::Not(f));
    else if (!is_deterministic(aut) && do_not_determinize)
      return trival::maybe();

    if (!find_counterexamples)
      return is_stutter_invariant(aut, std::move(neg));

    // Procedure that may find a counterexample.

    if (!neg)
      neg = complement(aut);

    auto aword = product(closure(aut), closure(neg))->accepting_word();
    if (!aword)
      {
        aut->prop_stutter_invariant(true);
        return true;
      }
    aword->simplify();
    aword->use_all_aps(aut->ap_vars());
    auto aaut = aword->as_automaton();
    twa_word_ptr rword;
    if (aaut->intersects(aut))
      {
        rword = sl2(aaut)->intersecting_word(neg);
        rword->simplify();
      }
    else
      {
        rword = aword;
        aword = sl2(aaut)->intersecting_word(aut);
        aword->simplify();
      }
    std::ostringstream os;
    os << *aword;
    aut->set_named_prop<std::string>("accepted-word",
                                     new std::string(os.str()));
    os.str("");
    os << *rword;
    aut->set_named_prop<std::string>("rejected-word",
                                     new std::string(os.str()));
    aut->prop_stutter_invariant(false);
    return false;
  }

  std::vector<bool>
  stutter_invariant_states(const_twa_graph_ptr pos, formula f)
  {
    if (f.is_syntactic_stutter_invariant()
        || pos->prop_stutter_invariant().is_true())
      return std::vector<bool>(pos->num_states(), true);
    auto neg = translator(pos->get_dict()).run(formula::Not(f));
    return stutter_invariant_states(pos, neg);
  }

  // Based on an idea by Joachim Klein.
  std::vector<bool>
  stutter_invariant_states(const_twa_graph_ptr pos,
                           const_twa_graph_ptr neg)
  {
    std::vector<bool> res(pos->num_states(), true);
    if (pos->prop_stutter_invariant().is_true())
      return res;

    if (neg == nullptr)
      neg = complement(pos);

    auto product_states = [](const const_twa_graph_ptr& a)
      {
        return (a->get_named_prop<std::vector<std::pair<unsigned, unsigned>>>
                ("product-states"));
      };

    // Get the set of states (x,y) that appear in the product P1=pos*neg.
    std::set<std::pair<unsigned, unsigned>> pairs = [&]()
      {
        twa_graph_ptr prod = spot::product(pos, neg);
        auto goodstates = product_states(prod);
        std::set<std::pair<unsigned, unsigned>> pairs(goodstates->begin(),
                                                      goodstates->end());
        return pairs;
      }();

    // Compute P2=cl(pos)*cl(neg).  A state x of pos is stutter-sensitive
    // if there exists a state (x,y) in both P1 and P2 that as a successor
    // in the useful part of P2 and that is not in P1.
    twa_graph_ptr prod = spot::product(closure(pos), closure(neg));
    auto prod_pairs = product_states(prod);
    scc_info si(prod, scc_info_options::TRACK_SUCCS
                | scc_info_options::TRACK_STATES_IF_FIN_USED);
    si.determine_unknown_acceptance();
    unsigned n = prod->num_states();
    bool sinv = true;

    for (unsigned s = 0; s < n; ++s)
      {
        if (!si.is_useful_scc(si.scc_of(s)))
          continue;
        if (pairs.find((*prod_pairs)[s]) == pairs.end())
          continue;
        for (auto& e: prod->out(s))
          if (si.is_useful_scc(si.scc_of(e.dst)))
            res[(*prod_pairs)[s].first] = sinv = false;
      }
    std::const_pointer_cast<twa_graph>(pos)->prop_stutter_invariant(sinv);
    std::const_pointer_cast<twa_graph>(neg)->prop_stutter_invariant(sinv);
    return res;
  }

  std::vector<bdd>
  stutter_invariant_letters(const_twa_graph_ptr pos, formula f)
  {
    if (f.is_syntactic_stutter_invariant()
        || pos->prop_stutter_invariant().is_true())
      {
        std::const_pointer_cast<twa_graph>(pos)->prop_stutter_invariant(true);
        return stutter_invariant_letters(pos);
      }
    auto neg = translator(pos->get_dict()).run(formula::Not(f));
    return stutter_invariant_letters(pos, neg);
  }

  std::vector<bdd>
  stutter_invariant_letters(const_twa_graph_ptr pos,
                            const_twa_graph_ptr neg)
  {
    unsigned ns = pos->num_states();
    std::vector<bdd> res(ns, bddtrue);
    if (pos->prop_stutter_invariant().is_true())
      return res;

    if (neg == nullptr)
      neg = complement(pos);

    auto product_states = [](const const_twa_graph_ptr& a)
      {
        return (a->get_named_prop<std::vector<std::pair<unsigned, unsigned>>>
                ("product-states"));
      };

    // Get the set of states (x,y) that appear in the product P1=pos*neg.
    std::set<std::pair<unsigned, unsigned>> pairs = [&]()
      {
        twa_graph_ptr prod = spot::product(pos, neg);
        auto goodstates = product_states(prod);
        std::set<std::pair<unsigned, unsigned>> pairs(goodstates->begin(),
                                                      goodstates->end());
        return pairs;
      }();

    // Compute P2=cl(pos)*cl(neg).  A state x of pos is stutter-sensitive
    // if there exists a state (x,y) in both P1 and P2 that as a successor
    // in the useful part of P2 and that is not in P1.
    twa_graph_ptr prod = spot::product(closure(pos), closure(neg));
    auto prod_pairs = product_states(prod);
    scc_info si(prod, scc_info_options::TRACK_SUCCS
                | scc_info_options::TRACK_STATES_IF_FIN_USED);
    si.determine_unknown_acceptance();
    unsigned n = prod->num_states();
    bool sinv = true;

    for (unsigned s = 0; s < n; ++s)
      {
        if (!si.is_useful_scc(si.scc_of(s)))
          continue;
        if (pairs.find((*prod_pairs)[s]) == pairs.end())
          continue;
        for (auto& e: prod->out(s))
          if (si.is_useful_scc(si.scc_of(e.dst)))
            {
              sinv = false;
              res[(*prod_pairs)[s].first] -= e.cond;
            }
      }
    std::const_pointer_cast<twa_graph>(pos)->prop_stutter_invariant(sinv);
    std::const_pointer_cast<twa_graph>(neg)->prop_stutter_invariant(sinv);
    return res;
  }

  namespace
  {
    static
    void highlight_vector(twa_graph_ptr aut,
                          const std::vector<bool>& v,
                          unsigned color)
    {
      // Create the highlight-states property only if it does not
      // exist already.
      auto hs =
        aut->get_named_prop<std::map<unsigned, unsigned>>("highlight-states");
      if (!hs)
        {
          hs = new std::map<unsigned, unsigned>;
          aut->set_named_prop("highlight-states", hs);
        }

      unsigned n = v.size();
      for (unsigned i = 0; i < n; ++i)
        if (v[i])
          (*hs)[i] = color;
    }
  }

  void
  highlight_stutter_invariant_states(twa_graph_ptr pos,
                                     formula f, unsigned color)
  {
    highlight_vector(pos, stutter_invariant_states(pos, f), color);
  }

  void
  highlight_stutter_invariant_states(twa_graph_ptr pos,
                                     const_twa_graph_ptr neg,
                                     unsigned color)
  {
    highlight_vector(pos, stutter_invariant_states(pos, neg), color);
  }


  namespace
  {
    [[noreturn]] static void sistates_has_wrong_size(const char* fun)
    {
      throw std::runtime_error(std::string(fun) +
                               "(): vector size should match "
                               "the number of states");
    }
  }

  int
  is_stutter_invariant_forward_closed(twa_graph_ptr aut,
                                      const std::vector<bool>& sistates)
  {
    unsigned ns = aut->num_states();
    if (SPOT_UNLIKELY(sistates.size() != ns))
      sistates_has_wrong_size("is_stutter_invariant_forward_closed");
    for (unsigned s = 0; s < ns; ++s)
      {
        if (!sistates[s])
          continue;
        for (auto& e : aut->out(s))
          {
            if (!sistates[e.dst])
              return e.dst;
          }
      }
    return -1;
  }

  std::vector<bool>
  make_stutter_invariant_forward_closed_inplace
  (twa_graph_ptr aut, const std::vector<bool>& sistates)
  {
    unsigned ns = aut->num_states();
    if (SPOT_UNLIKELY(sistates.size() != ns))
      sistates_has_wrong_size("make_stutter_invariant_forward_closed_inplace");
    // Find the set of SI states that can jump into non-SI states.
    std::vector<unsigned> seed_states;
    bool pb = false;
    for (unsigned s = 0; s < ns; ++s)
      {
        if (!sistates[s])
          continue;
        for (auto& e : aut->out(s))
          if (!sistates[e.dst])
            {
              seed_states.push_back(s);
              pb = true;
              break;
            }
      }
    if (!pb)                    // Nothing to change
      return sistates;

    // Find the set of non-SI states that are reachable from a seed
    // state, and give each of them a new number.
    std::vector<unsigned> new_number(ns, 0);
    std::vector<unsigned> prob_states;
    prob_states.reserve(ns);
    unsigned base = ns;
    std::vector<unsigned> dfs;
    dfs.reserve(ns);
    for (unsigned pb: seed_states)
      {
        dfs.push_back(pb);
        while (!dfs.empty())
          {
            unsigned src = dfs.back();
            dfs.pop_back();
            for (auto& e: aut->out(src))
              {
                unsigned dst = e.dst;
                if (sistates[dst] || new_number[dst])
                  continue;
                new_number[dst] = base++;
                dfs.push_back(dst);
                prob_states.push_back(dst);
              }
          }
      }

    // Actually duplicate the problematic states
    assert(base > ns);
    aut->new_states(base - ns);
    assert(aut->num_states() == base);
    for (unsigned ds: prob_states)
      {
        unsigned new_src = new_number[ds];
        assert(new_src > 0);
        for (auto& e: aut->out(ds))
          {
            unsigned dst = new_number[e.dst];
            if (!dst)
              dst = e.dst;
            aut->new_edge(new_src, dst, e.cond, e.acc);
          }
      }

    // Redirect the transition coming out of the seed states
    // to the duplicate state.
    for (unsigned pb: seed_states)
      for (auto& e: aut->out(pb))
        {
          unsigned ndst = new_number[e.dst];
          if (ndst)
            e.dst = ndst;
        }

    // Create a new SI-state vector.
    std::vector<bool> new_sistates;
    new_sistates.reserve(base);
    new_sistates.insert(new_sistates.end(), sistates.begin(), sistates.end());
    new_sistates.insert(new_sistates.end(), base - ns, true);
    return new_sistates;
  }

}
