// -*- coding: utf-8 -*-
// Copyright (C) 2009, 2011-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004, 2005 Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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
#include <sstream>
#include <memory>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/bfssteps.hh>
#include <spot/twaalgos/couvreurnew.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include <spot/twaalgos/gv04.hh>
#include <spot/twaalgos/magic.hh>
#include <spot/misc/hash.hh>
#include <spot/twaalgos/se05.hh>
#include <spot/twaalgos/tau03.hh>
#include <spot/twaalgos/tau03opt.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/product.hh>

namespace spot
{

  // emptiness_check_result
  //////////////////////////////////////////////////////////////////////

  twa_run_ptr
  emptiness_check_result::accepting_run()
  {
    return nullptr;
  }

  const unsigned_statistics*
  emptiness_check_result::statistics() const
  {
    return dynamic_cast<const unsigned_statistics*>(this);
  }

  const char*
  emptiness_check_result::parse_options(char* options)
  {
    option_map old(o_);
    const char* s = o_.parse_options(options);
    options_updated(old);
    return s;
  }

  void
  emptiness_check_result::options_updated(const option_map&)
  {
  }


  // emptiness_check
  //////////////////////////////////////////////////////////////////////

  emptiness_check::~emptiness_check()
  {
  }

  const unsigned_statistics*
  emptiness_check::statistics() const
  {
    return dynamic_cast<const unsigned_statistics*>(this);
  }

  const ec_statistics*
  emptiness_check::emptiness_check_statistics() const
  {
    return dynamic_cast<const ec_statistics*>(this);
  }

  const char*
  emptiness_check::parse_options(char* options)
  {
    option_map old(o_);
    const char* s = o_.parse_options(options);
    options_updated(old);
    return s;
  }

  void
  emptiness_check::options_updated(const option_map&)
  {
  }

  bool
  emptiness_check::safe() const
  {
    return true;
  }

  std::ostream&
  emptiness_check::print_stats(std::ostream& os) const
  {
    return os;
  }

  // emptiness_check_instantiator
  //////////////////////////////////////////////////////////////////////

  namespace
  {
    struct ec_algo
    {
      const char* name;
      emptiness_check_ptr(*construct)(const const_twa_ptr&,
                                      spot::option_map);
      unsigned int min_acc;
      unsigned int max_acc;
    };

    ec_algo ec_algos[] =
      {
        { "Cou99",     couvreur99,                    0, -1U },
        { "Cou99new",  get_couvreur99_new,            0, -1U },
        { "Cou99abs",  get_couvreur99_new_abstract,   0, -1U },
        { "CVWY90",    magic_search,                  0,   1 },
        { "GV04",      explicit_gv04_check,           0,   1 },
        { "SE05",      se05,                          0,   1 },
        { "Tau03",     explicit_tau03_search,         1, -1U },
        { "Tau03_opt", explicit_tau03_opt_search,     0, -1U },
      };
  }

  emptiness_check_instantiator::emptiness_check_instantiator(option_map o,
                                                             void* i)
    : o_(o), info_(i)
  {
  }

  unsigned int
  emptiness_check_instantiator::min_sets() const
  {
    return static_cast<ec_algo*>(info_)->min_acc;
  }

  unsigned int
  emptiness_check_instantiator::max_sets() const
  {
    return static_cast<ec_algo*>(info_)->max_acc;
  }

  emptiness_check_ptr
  emptiness_check_instantiator::instantiate(const const_twa_ptr& a) const
  {
    return static_cast<ec_algo*>(info_)->construct(a, o_);
  }

  emptiness_check_instantiator_ptr
  make_emptiness_check_instantiator(const char* name, const char** err)
  {
    // Skip spaces.
    while (*name && strchr(" \t\n", *name))
      ++name;

    const char* opt_str = strchr(name, '(');
    option_map o;
    if (opt_str)
      {
        const char* opt_start = opt_str + 1;
        const char* opt_end = strchr(opt_start, ')');
        if (!opt_end)
          {
            *err = opt_start;
            return nullptr;
          }
        std::string opt(opt_start, opt_end);

        const char* res = o.parse_options(opt.c_str());
        if (res)
          {
            *err  = opt.c_str() - res + opt_start;
            return nullptr;
          }
      }

    if (!opt_str)
      opt_str = name + strlen(name);

    // Ignore spaces before `(' (or trailing spaces).
    while (opt_str > name && strchr(" \t\n", *--opt_str))
      continue;
    std::string n(name, opt_str + 1);


    ec_algo* info = ec_algos;
    for (unsigned i = 0; i < sizeof(ec_algos)/sizeof(*ec_algos); ++i, ++info)
      if (n == info->name)
        {
          *err = nullptr;

          struct emptiness_check_instantiator_aux:
            public emptiness_check_instantiator
          {
            emptiness_check_instantiator_aux(option_map o, void* i):
              emptiness_check_instantiator(o, i)
            {
            }
          };
          return std::make_shared<emptiness_check_instantiator_aux>(o, info);
        }
    *err = name;
    return nullptr;
  }

  // twa_run
  //////////////////////////////////////////////////////////////////////

  twa_run::~twa_run()
  {
    for (auto i : prefix)
      i.s->destroy();
    for (auto i : cycle)
      i.s->destroy();
  }

  twa_run::twa_run(const twa_run& run)
  {
    aut = run.aut;
    for (step s : run.prefix)
      {
        s.s = s.s->clone();
        prefix.emplace_back(s);
      }
    for (step s : run.cycle)
      {
        s.s = s.s->clone();
        cycle.emplace_back(s);
      }
  }

  twa_run&
  twa_run::operator=(const twa_run& run)
  {
    if (&run != this)
      {
        this->~twa_run();
        new(this) twa_run(run);
      }
    return *this;
  }

  std::ostream&
  operator<<(std::ostream& os, const twa_run& run)
  {
    auto& a = run.aut;
    bdd_dict_ptr d = a->get_dict();

    auto pstep = [&](const twa_run::step& st)
    {
      os << "  " << a->format_state(st.s) << "\n  |  ";
      bdd_print_formula(os, d, st.label);
      if (st.acc)
        os << '\t' << st.acc;
      os << '\n';
    };

    os << "Prefix:\n";
    for (auto& s: run.prefix)
      pstep(s);
    os << "Cycle:\n";
    for (auto& s: run.cycle)
      pstep(s);
    return os;
  }

  void twa_run::ensure_non_empty_cycle(const char* where) const
  {
    if (cycle.empty())
      throw std::runtime_error(std::string(where)
                               + " expects a non-empty cycle");
  }

  namespace
  {
    class shortest_path final: public bfs_steps
    {
    public:
      shortest_path(const const_twa_ptr& a)
        : bfs_steps(a), target(nullptr)
      {
      }

      ~shortest_path()
      {
        state_set::const_iterator i = seen.begin();
        while (i != seen.end())
          (*i++)->destroy();
      }

      void
      set_target(const state_set* t)
      {
        target = t;
      }

      const state*
      search(const state* start, twa_run::steps& l)
      {
        return this->bfs_steps::search(filter(start), l);
      }

      const state*
      filter(const state* s) override
      {
        state_set::const_iterator i = seen.find(s);
        if (i == seen.end())
          seen.insert(s);
        else
          {
            s->destroy();
            s = *i;
          }
        return s;
      }

      bool
      match(twa_run::step&, const state* dest) override
      {
        return target->find(dest) != target->end();
      }

    private:
      state_set seen;
      const state_set* target;
    };
  }

  twa_run_ptr twa_run::reduce() const
  {
    ensure_non_empty_cycle("twa_run::reduce()");
    auto& a = aut;
    auto& acc = a->acc();
    auto res = std::make_shared<twa_run>(a);
    state_set ss;
    shortest_path shpath(a);
    shpath.set_target(&ss);

    // We want to find a short segment of the original cycle that
    // satisfies the acceptance condition.

    const state* segment_start; // The initial state of the segment.
    const state* segment_next; // The state immediately after the segment.


    // Start from the end of the original cycle, and rewind until all
    // acceptance sets have been seen.
    acc_cond::mark_t seen_acc = {};
    twa_run::steps::const_iterator seg = cycle.end();
    do
      {
        assert(seg != cycle.begin());
        --seg;
        seen_acc |= seg->acc;
      }
    while (!acc.accepting(seen_acc));
    segment_start = seg->s;

    // Now go forward and ends the segment as soon as we have seen all
    // acceptance sets, cloning it in our result along the way.
    seen_acc = {};
    do
      {
        assert(seg != cycle.end());
        seen_acc |= seg->acc;

        twa_run::step st = { seg->s->clone(), seg->label, seg->acc };
        res->cycle.emplace_back(st);

        ++seg;
      }
    while (!acc.accepting(seen_acc));
    segment_next = seg == cycle.end() ? cycle.front().s : seg->s;

    // Close this cycle if needed, that is, compute a cycle going
    // from the state following the segment to its starting state.
    if (segment_start != segment_next)
      {
        ss.insert(segment_start);
        const state* s = shpath.search(segment_next->clone(), res->cycle);
        ss.clear();
        assert(s->compare(segment_start) == 0);
        (void)s;

        // If the acceptance condition uses Fin, it's possible (and
        // even quite likely) that the cycle we have constructed this
        // way is now rejecting, because the closing steps might add
        // unwanted colors.  If that is the case, throw the reduced
        // cycle away and simply preserve the original one verbatim.
        if (acc.uses_fin_acceptance())
          {
            acc_cond::mark_t seen = {};
            for (auto& st: res->cycle)
              seen |= st.acc;
            if (!acc.accepting(seen))
              {
                for (auto& st: res->cycle)
                  st.s->destroy();
                res->cycle.clear();
                for (auto& st: cycle)
                  {
                    twa_run::step st2 = { st.s->clone(), st.label, st.acc };
                    res->cycle.emplace_back(st2);
                  }
              }
          }
      }

    // Compute the prefix: it's the shortest path from the initial
    // state of the automata to any state of the cycle.

    // Register all states from the cycle as target of the BFS.
    for (twa_run::steps::const_iterator i = res->cycle.begin();
         i != res->cycle.end(); ++i)
      ss.insert(i->s);

    const state* prefix_start = a->get_init_state();
    // There are two cases: either the initial state is already on
    // the cycle, or it is not.  If it is, we will have to rotate
    // the cycle so it begins on this position.  Otherwise we will shift
    // the cycle so it begins on the state that follows the prefix.
    // cycle_entry_point is that state.
    const state* cycle_entry_point;
    state_set::const_iterator ps = ss.find(prefix_start);
    if (ps != ss.end())
      {
        // The initial state is on the cycle.
        prefix_start->destroy();
        cycle_entry_point = *ps;
      }
    else
      {
        // This initial state is outside the cycle.  Compute the prefix.
        cycle_entry_point = shpath.search(prefix_start, res->prefix);
      }

    // Locate cycle_entry_point on the cycle.
    twa_run::steps::iterator cycle_ep_it;
    for (cycle_ep_it = res->cycle.begin();
         cycle_ep_it != res->cycle.end()
           && cycle_entry_point->compare(cycle_ep_it->s); ++cycle_ep_it)
      continue;
    assert(cycle_ep_it != res->cycle.end());

    // Now shift the cycle so it starts on cycle_entry_point.
    res->cycle.splice(res->cycle.end(), res->cycle,
                      res->cycle.begin(), cycle_ep_it);

    return res;
  }

  twa_run_ptr twa_run::project(const const_twa_ptr& other, bool right)
  {
    unsigned shift = 0;
    if (right)
      shift = aut->num_sets() - other->num_sets();
    auto res = std::make_shared<twa_run>(other);
    if (auto ps = aut->get_named_prop<const product_states>("product-states"))
      {
        auto a = down_cast<const_twa_graph_ptr>(aut);
        if (!a)
          throw std::runtime_error("twa_run::project() confused: "
                                   "product-states found in a non-twa_graph");
        auto oth = down_cast<const_twa_graph_ptr>(other);
        if (!oth)
          throw std::runtime_error("twa_run::project() confused: "
                                   "other ought to be a twa_graph");
        if (right)
          {
            for (auto& i: prefix)
              {
                unsigned s = (*ps)[a->state_number(i.s)].second;
                res->prefix.emplace_back(oth->state_from_number(s),
                                         i.label, i.acc >> shift);
              }
            for (auto& i: cycle)
              {
                unsigned s = (*ps)[a->state_number(i.s)].second;
                res->cycle.emplace_back(oth->state_from_number(s),
                                        i.label, i.acc >> shift);
              }
          }
        else
          {
            auto all = oth->acc().all_sets();
            for (auto& i: prefix)
              {
                unsigned s = (*ps)[a->state_number(i.s)].first;
                res->prefix.emplace_back(oth->state_from_number(s),
                                         i.label, i.acc & all);
              }
            for (auto& i: cycle)
              {
                unsigned s = (*ps)[a->state_number(i.s)].first;
                res->cycle.emplace_back(oth->state_from_number(s),
                                        i.label, i.acc & all);
              }
          }
      }
    else
      {
        auto all = other->acc().all_sets();
        for (auto& i: prefix)
          res->prefix.emplace_back(aut->project_state(i.s, other),
                                   i.label, (i.acc >> shift) & all);
        for (auto& i: cycle)
          res->cycle.emplace_back(aut->project_state(i.s, other),
                                  i.label, (i.acc >> shift) & all);
      }
    return res;
  }


  bool twa_run::replay(std::ostream& os, bool debug) const
  {
    ensure_non_empty_cycle("twa_run::replay()");
    const state* s = aut->get_init_state();
    int serial = 1;
    const twa_run::steps* l;
    std::string in;
    acc_cond::mark_t all_acc = {};
    bool all_acc_seen = false;
    state_map<std::set<int>> seen;

    if (prefix.empty())
      {
        l = &cycle;
        in = "cycle";
        if (!debug)
          os << "No prefix.\nCycle:\n";
      }
    else
      {
        l = &prefix;
        in = "prefix";
        if (!debug)
          os << "Prefix:\n";
      }

    twa_run::steps::const_iterator i = l->begin();

    if (s->compare(i->s))
      {
        if (debug)
          os << "ERROR: First state of run (in " << in << "): "
             << aut->format_state(i->s)
             << "\ndoes not match initial state of automata: "
             << aut->format_state(s) << '\n';
        s->destroy();
        return false;
      }

    for (; i != l->end(); ++serial)
      {
        if (debug)
          {
            // Keep track of the serial associated to each state so we
            // can note duplicate states and make the replay easier to read.
            auto o = seen.find(s);
            std::ostringstream msg;
            if (o != seen.end())
              {
                for (auto d: o->second)
                  msg << " == " << d;
                o->second.insert(serial);
                s->destroy();
                s = o->first;
              }
            else
              {
                seen[s].insert(serial);
              }
            os << "state " << serial << " in " << in << msg.str() << ": ";
          }
        else
          {
            os << "  ";
          }
        os << aut->format_state(s) << '\n';

        // expected outgoing transition
        bdd label = i->label;
        acc_cond::mark_t acc = i->acc;

        // compute the next expected state
        const state* next;
        ++i;
        if (i != l->end())
          {
            next = i->s;
          }
        else
          {
            if (l == &prefix)
              {
                l = &cycle;
                in = "cycle";
                i = l->begin();
                if (!debug)
                  os << "Cycle:\n";
              }
            next = l->begin()->s;
          }

        // browse the actual outgoing transitions
        twa_succ_iterator* j = aut->succ_iter(s);
        // When not debugging, S is not used as key in SEEN, so we can
        // destroy it right now.
        if (!debug)
          s->destroy();
        if (j->first())
          do
            {
              if (j->cond() != label
                  || j->acc() != acc)
                continue;

              const state* s2 = j->dst();
              if (s2->compare(next))
                {
                  s2->destroy();
                  continue;
                }
              else
                {
                  s = s2;
                  break;
                }
            }
          while (j->next());
        if (j->done())
          {
            if (debug)
              {
                os << "ERROR: no transition with label="
                   << bdd_format_formula(aut->get_dict(), label)
                   << " and acc=" << acc
                   << " leaving state " << serial
                   << " for state " << aut->format_state(next) << '\n'
                   << "The following transitions leave state " << serial
                   << ":\n";
                if (j->first())
                  do
                    {
                      const state* s2 = j->dst();
                      os << "  * label="
                         << bdd_format_formula(aut->get_dict(),
                                               j->cond())
                         << " and acc=" << j->acc()
                         << " going to " << aut->format_state(s2) << '\n';
                      s2->destroy();
                    }
                  while (j->next());
              }
            aut->release_iter(j);
            s->destroy();
            return false;
          }
        if (debug)
          {
            os << "transition with label="
               << bdd_format_formula(aut->get_dict(), label)
               << " and acc=" << acc << std::endl;
          }
        else
          {
            os << "  |  ";
            bdd_print_formula(os, aut->get_dict(), label);
            if (acc)
              os << '\t' << acc;
            os << std::endl;
          }
        aut->release_iter(j);

        // Sum acceptance marks.
        //
        // (Beware l and i designate the next step to consider.
        // Therefore if i is at the beginning of the cycle, `acc'
        // contains the acceptance conditions of the last transition
        // in the prefix; we should not account it.)
        if (l == &cycle && i != l->begin())
          {
            all_acc |= acc;
            if (!all_acc_seen && aut->acc().accepting(all_acc))
              {
                all_acc_seen = true;
                if (debug)
                  os << "all acceptance marks ("
                     << all_acc
                     << ") have been seen\n";
              }
          }
      }
    s->destroy();
    if (!aut->acc().accepting(all_acc))
      {
        if (debug)
          os << "ERROR: The cycle's acceptance marks ("
             << all_acc
             << ") do not satisfy the acceptance condition ("
             << aut->get_acceptance()
             << ")\n";
        return false;
      }

    auto o = seen.begin();
    while (o != seen.end())
      {
        // Advance the iterator before deleting the "key" pointer.
        const state* ptr = o->first;
        ++o;
        ptr->destroy();
      }

    return true;
  }

  /// Note that this works only if the automaton is a twa_graph_ptr.
  void twa_run::highlight(unsigned color)
  {
    auto a = down_cast<twa_graph_ptr>(std::const_pointer_cast<twa>(aut));
    if (!a)
      throw std::runtime_error("highlight() only work for twa_graph");

    auto h = a->get_or_set_named_prop<std::map<unsigned, unsigned>>
      ("highlight-edges");

    unsigned src = a->get_init_state_number();
    auto l = prefix.empty() ? &cycle : &prefix;
    auto e = l->end();
    for (auto i = l->begin(); i != e;)
      {
        bdd label = i->label;
        acc_cond::mark_t acc = i->acc;
        unsigned dst;
        ++i;
        if (i != e)
          {
            dst = a->state_number(i->s);
          }
        else if (l == &prefix)
          {
            l = &cycle;
            i = l->begin();
            e = l->end();
            dst = a->state_number(i->s);
          }
        else
          {
            dst = a->state_number(l->begin()->s);
          }

        for (auto& t: a->out(src))
          if (t.dst == dst && bdd_implies(label, t.cond) && t.acc == acc)
            {
              (*h)[a->get_graph().index_of_edge(t)] = color;
              break;
            }
        src = dst;
      }
  }

  twa_graph_ptr
  twa_run::as_twa(bool preserve_names) const
  {
    ensure_non_empty_cycle("twa_run::as_twa()");
    auto d = aut->get_dict();
    auto res = make_twa_graph(d);
    res->copy_ap_of(aut);
    res->copy_acceptance_of(aut);

    std::vector<std::string>* names= nullptr;
    if (preserve_names)
      {
        names = new std::vector<std::string>;
        res->set_named_prop("state-names", names);
      }

    const state* s = aut->get_init_state();
    unsigned src;
    unsigned dst;
    const twa_run::steps* l;
    acc_cond::mark_t seen_acc = {};

    state_map<unsigned> seen;

    if (prefix.empty())
        l = &cycle;
    else
        l = &prefix;

    twa_run::steps::const_iterator i = l->begin();

    assert(s->compare(i->s) == 0);
    src = res->new_state();
    seen.emplace(i->s, src);
    if (names)
      names->push_back(aut->format_state(s));

    for (; i != l->end();)
      {
        // expected outgoing transition
        bdd label = i->label;
        acc_cond::mark_t acc = i->acc;

        // compute the next expected state
        const state* next;
        ++i;
        if (i != l->end())
          {
            next = i->s;
          }
        else
          {
            if (l == &prefix)
              {
                l = &cycle;
                i = l->begin();
              }
            next = l->begin()->s;
          }

        // browse the actual outgoing transitions and
        // look for next;
        const state* the_next = nullptr;
        for (auto j: aut->succ(s))
          {
            if (j->cond() != label
                || j->acc() != acc)
              continue;

            const state* s2 = j->dst();
            if (s2->compare(next) == 0)
              {
                the_next = s2;
                break;
              }
            s2->destroy();
          }
        s->destroy();
        if (!the_next)
          throw std::runtime_error("twa_run::as_twa() unable to replay run");
        s = the_next;


        auto p = seen.emplace(next, 0);
        if (p.second)
          {
            unsigned ns = res->new_state();
            p.first->second = ns;
            if (names)
              {
                assert(ns == names->size());
                names->push_back(aut->format_state(next));
              }
          }
        dst = p.first->second;

        res->new_edge(src, dst, label, acc);
        src = dst;

        // Sum acceptance conditions.
        if (l == &cycle && i != l->begin())
          seen_acc |= acc;
      }

    s->destroy();

    assert(aut->acc().accepting(seen_acc));
    return res;
  }

}
