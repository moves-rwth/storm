// -*- coding: utf-8 -*-
// Copyright (C) 2016-2019 Laboratoire de Recherche et Développement
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
#include <algorithm>
#include <sstream>
#include <spot/twaalgos/alternation.hh>
#include <spot/misc/minato.hh>
#include <spot/twaalgos/strength.hh>

namespace spot
{
  outedge_combiner::outedge_combiner(const twa_graph_ptr& aut)
    : aut_(aut), vars_(bddtrue)
  {
  }

  outedge_combiner::~outedge_combiner()
  {
    aut_->get_dict()->unregister_all_my_variables(this);
  }

  bdd outedge_combiner::operator()(unsigned st)
  {
    const auto& dict = aut_->get_dict();
    bdd res = bddtrue;
    for (unsigned d1: aut_->univ_dests(st))
      {
        bdd res2 = bddfalse;
        for (auto& e: aut_->out(d1))
          {
            bdd out = bddtrue;
            for (unsigned d: aut_->univ_dests(e.dst))
              {
                auto p = state_to_var.emplace(d, 0);
                if (p.second)
                  {
                    int v = dict->register_anonymous_variables(1, this);
                    p.first->second = v;
                    var_to_state.emplace(v, d);
                    vars_ &= bdd_ithvar(v);
                  }
                out &= bdd_ithvar(p.first->second);
              }
            res2 |= e.cond & out;
          }
        res &= res2;
      }
    return res;
  }



  void outedge_combiner::new_dests(unsigned st, bdd out) const
  {
    minato_isop isop(out);
    bdd cube;
    std::vector<unsigned> univ_dest;
    while ((cube = isop.next()) != bddfalse)
      {
        bdd cond = bdd_exist(cube, vars_);
        bdd dest = bdd_existcomp(cube, vars_);
        while (dest != bddtrue)
          {
            assert(bdd_low(dest) == bddfalse);
            auto it = var_to_state.find(bdd_var(dest));
            assert(it != var_to_state.end());
            univ_dest.push_back(it->second);
            dest = bdd_high(dest);
          }
        std::sort(univ_dest.begin(), univ_dest.end());
        aut_->new_univ_edge(st, univ_dest.begin(), univ_dest.end(), cond);
        univ_dest.clear();
      }
  }



  namespace
  {
    class alternation_remover final
    {
    protected:
      const_twa_graph_ptr aut_;
      scc_info si_;
      enum scc_class : char { accept, reject_1, reject_more };
      std::vector<scc_class> class_of_;
      bool has_reject_more_ = false;
      unsigned reject_1_count_ = 0;
      std::set<unsigned> true_states_;

      bool ensure_weak_scc(unsigned scc)
      {
        bool first = true;
        bool reject_cycle = false;
        acc_cond::mark_t m = {};
        for (unsigned src: si_.states_of(scc))
          for (auto& t: aut_->out(src))
            for (unsigned d: aut_->univ_dests(t.dst))
              if (si_.scc_of(d) == scc)
                {
                  if (first)
                    {
                      first = false;
                      m = t.acc;
                      reject_cycle = !aut_->acc().accepting(m);
                    }
                  else if (m != t.acc)
                    {
                      throw std::runtime_error
                        ("remove_alternation() only works with weak "
                         "alternating automata");
                    }
                  // In case of a universal edge we only
                  // need to check the first destination
                  // inside the SCC, because the other
                  // have the same t.acc.
                  break;
                }
        return reject_cycle;
      }

      void classify_each_scc()
      {
        auto& g = aut_->get_graph();
        unsigned sc = si_.scc_count();
        for (unsigned n = 0; n < sc; ++n)
          {
            if (si_.is_trivial(n))
              continue;
            if (si_.states_of(n).size() == 1)
              {
                if (si_.is_rejecting_scc(n))
                  {
                    class_of_[n] = scc_class::reject_1;
                    ++reject_1_count_;
                  }
                else
                  {
                    // For size one, scc_info should always be able to
                    // decide rejecting/accepting.
                    assert(si_.is_accepting_scc(n));

                    // Catch unsupported types of automata
                    bool rejecting = ensure_weak_scc(n);
                    assert(!rejecting);
                    (void) rejecting;
                    // Detect if it is a "true state"
                    unsigned s = si_.states_of(n).front();
                    auto& ss = g.state_storage(s);
                    if (ss.succ == ss.succ_tail)
                      {
                        auto& es = g.edge_storage(ss.succ);
                        if (es.cond == bddtrue && !aut_->is_univ_dest(es.dst))
                          true_states_.emplace(s);
                      }
                  }
              }
            else if (ensure_weak_scc(n))
              {
                class_of_[n] = reject_more;
                has_reject_more_ = true;
              }
          }
      }

      std::vector<int> state_to_var_;
      std::map<int, unsigned> var_to_state_;
      std::vector<int> scc_to_var_;
      std::map<int, acc_cond::mark_t> var_to_mark_;
      std::vector<unsigned> mark_to_state_;
      bdd all_vars_;
      bdd all_marks_;
      bdd all_states_;

      void allocate_state_vars()
      {
        auto d = aut_->get_dict();
        // We need one BDD variable per possible output state.  If
        // that state is in a reject_more SCC we actually use two
        // variables for the breakpoint.
        unsigned ns = aut_->num_states();
        state_to_var_.reserve(ns);
        bdd all_states = bddtrue;
        for (unsigned s = 0; s < ns; ++s)
          {
            if (!si_.reachable_state(s))
              {
                state_to_var_.push_back(0);
                continue;
              }
            scc_class c = class_of_[si_.scc_of(s)];
            bool r = c == scc_class::reject_more;
            int v = d->register_anonymous_variables(1 + r, this);
            state_to_var_.push_back(v);
            var_to_state_[v] = s;
            all_states &= bdd_ithvar(v);
            if (r)
              {
                var_to_state_[v + 1] = ~s;
                all_states &= bdd_ithvar(v + 1);
              }
          }
        // We also use one BDD variable per reject_1 SCC.  Each of
        // these variables will represent a bit in mark_t.  We reserve
        // the first bit for the break_point construction if we have
        // some reject_more SCC.
        unsigned nc = si_.scc_count();
        scc_to_var_.reserve(nc);
        unsigned mark_pos = has_reject_more_;
        mark_to_state_.resize(mark_pos);
        bdd all_marks = bddtrue;
        for (unsigned s = 0; s < nc; ++s)
          {
            scc_class c = class_of_[s];
            if (c == scc_class::reject_1)
              {
                int v = d->register_anonymous_variables(1, this);
                scc_to_var_.emplace_back(v);
                mark_to_state_.push_back(si_.one_state_of(s));
                var_to_mark_.emplace(v, acc_cond::mark_t({mark_pos++}));
                bdd bv = bdd_ithvar(v);
                all_marks &= bv;
              }
            else
              {
                scc_to_var_.emplace_back(0);
              }
          }
        all_marks_ = all_marks;
        all_states_ = all_states;
        all_vars_ = all_states & all_marks;
      }

      std::map<unsigned, bdd> state_as_bdd_cache_;

      bdd state_as_bdd(unsigned s)
      {
        auto p = state_as_bdd_cache_.emplace(s, bddfalse);
        if (!p.second)
          return p.first->second;

        bool marked = (int)s < 0;
        if (marked)
          s = ~s;

        unsigned scc_s = si_.scc_of(s);
        bdd res = bddfalse;
        for (auto& e: aut_->out(s))
          {
            bdd dest = bddtrue;
            for (unsigned d: aut_->univ_dests(e.dst))
              {
                unsigned scc_d = si_.scc_of(d);
                scc_class c = class_of_[scc_d];
                bool mark =
                  marked && (scc_s == scc_d) && (c == scc_class::reject_more);
                dest &= bdd_ithvar(state_to_var_[d] + mark);
                if (c == scc_class::reject_1 && scc_s == scc_d)
                  dest &= bdd_ithvar(scc_to_var_[scc_d]);
              }
            res |= e.cond & dest;
          }

        p.first->second = res;
        return res;
      }

      acc_cond::mark_t bdd_to_state(bdd b, std::vector<unsigned>& s)
      {
        acc_cond::mark_t m = {};
        while (b != bddtrue)
          {
            assert(bdd_low(b) == bddfalse);
            int v = bdd_var(b);
            auto it = var_to_state_.find(v);
            if (it != var_to_state_.end())
              {
                s.push_back(it->second);
              }
            else
              {
                auto it2 = var_to_mark_.find(v);
                assert(it2 != var_to_mark_.end());
                m |= it2->second;
              }
            b = bdd_high(b);
          }
        return m;
      }

      void simplify_state_set(std::vector<unsigned>& ss)
      {
        auto to_remove = true_states_;
        for (unsigned i: ss)
          if ((int)i < 0)
            to_remove.emplace(~i);

        auto i =
          std::remove_if(ss.begin(), ss.end(),
                         [&] (unsigned s) {
                           return to_remove.find(s) != to_remove.end();
                         });
        ss.erase(i, ss.end());
        std::sort(ss.begin(), ss.end());
      }

      bool has_mark(const std::vector<unsigned>& ss)
      {
        for (unsigned i: ss)
          if ((int)i < 0)
            return true;
        return false;
      }

      void set_mark(std::vector<unsigned>& ss)
      {
        for (unsigned& s: ss)
          if (class_of_[si_.scc_of(s)] == scc_class::reject_more)
            s = ~s;
      }

    public:
      alternation_remover(const const_twa_graph_ptr& aut)
        : aut_(aut), si_(aut, scc_info_options::TRACK_STATES),
          class_of_(si_.scc_count(), scc_class::accept)
      {
      }

      ~alternation_remover()
      {
        aut_->get_dict()->unregister_all_my_variables(this);
      }


      twa_graph_ptr run(bool named_states, const output_aborter* aborter)
      {
        // First, we classify each SCC into three possible classes:
        //
        // 1) trivial or accepting
        // 2) rejecting of size 1
        // 3) rejecting of size >1
        classify_each_scc();

        // Rejecting SCCs of size 1 can be handled using genralized
        // Büchi acceptance, using one set per SCC, as in Gastin &
        // Oddoux CAV'01.  See also Boker & et al. ICALP'10.  Larger
        // rejecting SCCs require a more expensive procedure known as
        // the break point construction.  See Miyano & Hayashi (TCS
        // 1984).  We are currently combining the two constructions.
        auto res = make_twa_graph(aut_->get_dict());
        res->copy_ap_of(aut_);
        // We preserve deterministic-like properties, and
        // stutter-invariance.
        res->prop_copy(aut_, {false, false, false, true, true, true});
        res->set_generalized_buchi(has_reject_more_ + reject_1_count_);

        // We for easier computation of outgoing sets, we will
        // represent states using BDD variables.
        allocate_state_vars();

        // Conversion between state-sets and states.
        typedef std::vector<unsigned> state_set;
        std::vector<state_set> s_to_ss;
        std::map<state_set, unsigned> ss_to_s;
        std::stack<unsigned> todo;

        std::vector<std::string>* state_name = nullptr;
        if (named_states)
          {
            state_name = new std::vector<std::string>();
            res->set_named_prop("state-names", state_name);
          }

        auto new_state = [&](state_set& ss, bool& need_mark)
        {
          simplify_state_set(ss);

          if (has_reject_more_)
            {
              need_mark = has_mark(ss);
              if (!need_mark)
                set_mark(ss);
            }

          auto p = ss_to_s.emplace(ss, 0);
          if (!p.second)
            return p.first->second;
          unsigned s = res->new_state();
          assert(s == s_to_ss.size());
          p.first->second = s;
          s_to_ss.emplace_back(ss);
          todo.emplace(s);

          if (named_states)
            {
              std::ostringstream os;
              bool notfirst = false;
              for (unsigned s: ss)
                {
                  if (notfirst)
                    os << ',';
                  else
                    notfirst = true;
                  if ((int)s < 0)
                    {
                      os << '~';
                      s = ~s;
                    }
                  os << s;
                }
              if (!notfirst)
                os << "{}";
              state_name->emplace_back(os.str());
            }
          return s;
        };

        const auto& i = aut_->univ_dests(aut_->get_init_state_number());
        state_set is(i.begin(), i.end());
        bool has_mark = false;
        res->set_init_state(new_state(is, has_mark));

        acc_cond::mark_t all_marks = res->acc().all_sets();

        state_set v;
        while (!todo.empty())
          {
            if (aborter && aborter->too_large(res))
              return nullptr;

            unsigned s = todo.top();
            todo.pop();

            bdd bs = bddtrue;
            for (unsigned se: s_to_ss[s])
              bs &= state_as_bdd(se);

            bdd ap = bdd_exist(bdd_support(bs), all_vars_);
            bdd all_letters = bdd_exist(bs, all_vars_);

            // First loop over all possible valuations atomic properties.
            while (all_letters != bddfalse)
              {
                bdd oneletter = bdd_satoneset(all_letters, ap, bddtrue);
                all_letters -= oneletter;

                minato_isop isop(bs & oneletter);
                bdd cube;
                while ((cube = isop.next()) != bddfalse)
                  {
                    bdd cond = bdd_exist(cube, all_vars_);
                    bdd dest = bdd_existcomp(cube, all_vars_);
                    v.clear();
                    acc_cond::mark_t m = bdd_to_state(dest, v);

                    // if there is no promise "f" between a state
                    // that does not have f, and a state that have
                    // "f", we can add one.  Doing so will help later
                    // simplifications performed by postprocessor.  An
                    // example where this is needed is the VWAA
                    // generated by ltl[23]ba for GFa.  Without the
                    // next loop, the final TGBA has 2 states instead
                    // of 1.
                    for (unsigned m1: (all_marks - m).sets())
                      {
                        if (has_reject_more_ && m1 == 0)
                          continue;
                        auto& sv = s_to_ss[s];
                        unsigned ms = mark_to_state_[m1];
                        if (std::find(v.begin(), v.end(), ms) != v.end())
                          {
                            unsigned ms = mark_to_state_[m1];
                            if (std::find(sv.begin(), sv.end(), ms) == sv.end())
                              m.set(m1);
                          }
                      }

                    unsigned d = new_state(v, has_mark);
                    if (has_mark)
                      m.set(0);
                    res->new_edge(s, d, cond, all_marks - m);
                  }
              }
          }
        res->merge_edges();
        return res;
      }
    };

  }


  twa_graph_ptr remove_alternation(const const_twa_graph_ptr& aut,
                                   bool named_states,
                                   const output_aborter* aborter)
  {
    if (aut->is_existential())
      // Nothing to do, why was this function called at all?
      return std::const_pointer_cast<twa_graph>(aut);

    alternation_remover ar(aut);
    return ar.run(named_states, aborter);
  }


  // On-the-fly alternating Büchi automaton to nondeterministic Büchi automaton
  // using the breakpoint construction.

  univ_remover_state::univ_remover_state(const std::set<unsigned>& states)
    : is_reset_(false)
  {
    is_reset_ = std::all_of(states.begin(), states.end(),
                         [] (unsigned s) { return int(s) < 0; });
    // When all states are marked (i.e. Q0 is empty), the state is in reset
    // mode, meaning that it is an accepting state.
    // The states can immediately be unmarked, since the acceptance mark is
    // held by the incoming transition.
    if (is_reset_)
      for (unsigned s : states)
        states_.insert(~s);
    else
      states_ = states;
  }

  int univ_remover_state::compare(const state* other) const
  {
    const auto o = down_cast<const univ_remover_state*>(other);
    assert(o);
    if (states_ < o->states())
      return -1;
    if (states_ > o->states())
      return 1;
    return 0;
  }

  size_t univ_remover_state::hash() const
  {
    size_t hash = 0;
    for (unsigned s : states_)
      hash ^= wang32_hash(s);
    return hash;
  }

  state* univ_remover_state::clone() const
  {
    return new univ_remover_state(*this);
  }

  const std::set<unsigned>& univ_remover_state::states() const
  {
    return states_;
  }

  bool univ_remover_state::is_reset() const
  {
    return is_reset_;
  }

  class univ_remover_succ_iterator : public twa_succ_iterator
  {
  private:
    bdd transitions_;
    bdd all_states_;
    bdd ap_;
    bdd all_letters_;
    bdd transition_;
    minato_isop isop_;
    const std::map<int, unsigned>& var_to_state_;
    univ_remover_state* dst_;

  public:
    univ_remover_succ_iterator(const_twa_graph_ptr aut,
                               const univ_remover_state* state,
                               const std::vector<int>& state_to_var,
                               const std::map<int, unsigned>& var_to_state,
                               bdd all_states)
      : transitions_(bddtrue), all_states_(all_states), transition_(bddfalse),
        isop_(bddfalse), var_to_state_(var_to_state)
    {
      // Build the bdd transitions_, from which we extract the successors.
      for (unsigned s : state->states())
        {
          bdd state_bdd = bddfalse;
          bool marked = (int)s < 0;
          if (marked)
            s = ~s;
          for (const auto& e : aut->out(s))
            {
              bdd univ_bdd = bddtrue;
              for (const auto& d : aut->univ_dests(e.dst))
                univ_bdd &= bdd_ithvar(state_to_var[d] + (marked || e.acc));
              state_bdd |= e.cond & univ_bdd;
            }
          transitions_ &= state_bdd;
        }
      ap_ = bdd_exist(bdd_support(transitions_), all_states_);
      all_letters_ = bdd_exist(transitions_, all_states_);
    }

    std::set<unsigned> bdd_to_state(bdd b) const
    {
      std::set<unsigned> state;
      while (b != bddtrue)
        {
          assert(bdd_low(b) == bddfalse);
          int v = bdd_var(b);
          b = bdd_high(b);
          auto it = var_to_state_.find(v);
          assert(it != var_to_state_.end());
          state.insert(it->second);
        }
      return state;
    }

    void one_transition()
    {
      transition_ = isop_.next();
      if (transition_ != bddfalse || all_letters_ != bddfalse)
        {
          // If it was the last transition, try the next letter.
          if (transition_ == bddfalse)
            {
              bdd oneletter = bdd_satoneset(all_letters_, ap_, bddtrue);
              all_letters_ -= oneletter;
              // Get a sum of possible transitions matching this letter.
              isop_ = minato_isop(oneletter & transitions_);
              transition_ = isop_.next();
            }
          bdd dest_bdd = bdd_exist(transition_, ap_);
          std::set<unsigned> dest = bdd_to_state(dest_bdd);
          dst_ = new univ_remover_state(dest);
        }
    }

    virtual bool first() override
    {
      one_transition();
      return transition_ != bddfalse;
    }

    virtual bool next() override
    {
      one_transition();
      return transition_ != bddfalse;
    }

    virtual bool done() const override
    {
      return transition_ == bddfalse && all_letters_ == bddfalse;
    }

    virtual const state* dst() const override
    {
      return dst_;
    }

    virtual bdd cond() const override
    {
      return bdd_exist(transition_, all_states_);
    }

    virtual acc_cond::mark_t acc() const override
    {
      assert(dst_);
      return dst_->is_reset() ?
                acc_cond::mark_t({0}) :
                acc_cond::mark_t({});
    }
  };

  twa_univ_remover::twa_univ_remover(const const_twa_graph_ptr& aut)
    : twa(aut->get_dict()), aut_(aut)
  {
    assert(aut->num_sets() == 1);
    allocate_state_vars();
  }

  void twa_univ_remover::allocate_state_vars()
  {
    auto d = aut_->get_dict();
    unsigned ns = aut_->num_states();
    state_to_var_.reserve(ns);
    all_states_ = bddtrue;
    bdd all_vars = bddtrue;
    for (unsigned s = 0; s < ns; ++s)
      {
        int v = d->register_anonymous_variables(2, this);
        // v and v + 1 respectively correspond to state s and ~s, i.e. s \in Q0
        // or s \in Q1 using the original paper's notation.
        all_states_ &= bdd_ithvar(v) & bdd_ithvar(v + 1);
        state_to_var_.push_back(v);
        var_to_state_[v] = s;
        var_to_state_[v + 1] = ~s;
      }
  }

  const state* twa_univ_remover::get_init_state() const
  {
    std::set<unsigned> state;
    for (unsigned i : aut_->univ_dests(aut_->get_init_state_number()))
      state.insert(i);
    return new univ_remover_state(state);
  }

  twa_succ_iterator* twa_univ_remover::succ_iter(const state* s) const
  {
    auto as = down_cast<const univ_remover_state*>(s);
    assert(as);
    return new univ_remover_succ_iterator(aut_, as, state_to_var_,
                                          var_to_state_, all_states_);
  }

  std::string twa_univ_remover::format_state(const state* s) const
  {
    auto as = down_cast<const univ_remover_state*>(s);
    std::ostringstream fmt;
    bool first = true;
    for (unsigned s : as->states())
      {
        if (!first)
          fmt << ',';
        else
          first = false;
        if ((int) s < 0)
          fmt << '~' << ~s;
        else
          fmt << s;
      }
    return fmt.str();
  }

  twa_univ_remover_ptr remove_univ_otf(const const_twa_graph_ptr& aut)
  {
    assert(aut->acc().is_buchi());
    auto res = std::make_shared<twa_univ_remover>(aut);
    res->copy_ap_of(aut);
    res->copy_acceptance_of(aut);
    return res;
  }
}
