// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2011-2018, 2020 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
#include <iostream>
#include <sstream>
#include <spot/twa/twa.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twaalgos/reachiter.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/isweakscc.hh>
#include <cstring>

namespace spot
{
  namespace
  {
    class stats_bfs: public twa_reachable_iterator_breadth_first
    {
    public:
      stats_bfs(const const_twa_ptr& a, twa_statistics& s)
        : twa_reachable_iterator_breadth_first(a), s_(s)
      {
      }

      void
      process_state(const state*, int, twa_succ_iterator*) override final
      {
        ++s_.states;
      }

      void
      process_link(const state*, int, const state*, int,
                   const twa_succ_iterator*) override
      {
        ++s_.edges;
      }

    private:
      twa_statistics& s_;
    };

    class sub_stats_bfs final: public stats_bfs
    {
    public:
      sub_stats_bfs(const const_twa_ptr& a, twa_sub_statistics& s)
        : stats_bfs(a, s), s_(s), apvars_(a->ap_vars())
      {
      }

      void
      process_link(const state*, int, const state*, int,
                   const twa_succ_iterator* it) override
      {
        ++s_.edges;
        s_.transitions += bdd_satcountset(it->cond(), apvars_);
      }

    private:
      twa_sub_statistics& s_;
      bdd apvars_;
    };


    template<typename SU, typename EU>
    void dfs(const const_twa_graph_ptr& ge, SU state_update, EU edge_update)
    {
      unsigned init = ge->get_init_state_number();
      unsigned num_states = ge->num_states();
      // The TODO vector serves two purposes:
      // - it is a stack of states to process,
      // - it is a set of processed states.
      // The lower 31 bits of each entry is a state on the stack. (The
      // next usable entry on the stack is indicated by todo_pos.)  The
      // 32th bit (i.e., the sign bit) of todo[x] indicates whether
      // states number x has been seen.
      std::vector<unsigned> todo(num_states, 0);
      const unsigned seen = 1 << (sizeof(unsigned)*8-1);
      const unsigned mask = seen - 1;
      unsigned todo_pos = 0;
      for (unsigned i: ge->univ_dests(init))
        {
          todo[todo_pos++] = i;
          todo[i] |= seen;
        }
      do
        {
          state_update();
          unsigned cur = todo[--todo_pos] & mask;
          todo[todo_pos] ^= cur;        // Zero the state
          for (auto& t: ge->out(cur))
            {
              edge_update(t.cond);
              for (unsigned dst: ge->univ_dests(t.dst))
                if (!(todo[dst] & seen))
                  {
                    todo[dst] |= seen;
                    todo[todo_pos++] |= dst;
                  }
            }
        }
      while (todo_pos > 0);
    }

  } // anonymous


  std::ostream& twa_statistics::dump(std::ostream& out) const
  {
    out << "edges: " << edges << '\n';
    out << "states: " << states << '\n';
    return out;
  }

  std::ostream& twa_sub_statistics::dump(std::ostream& out) const
  {
    out << "transitions: " << transitions << '\n';
    this->twa_statistics::dump(out);
    return out;
  }

  twa_statistics
  stats_reachable(const const_twa_ptr& g)
  {
    twa_statistics s;
    auto ge = std::dynamic_pointer_cast<const twa_graph>(g);
    if (!ge)
      {
        stats_bfs d(g, s);
        d.run();
      }
    else
      {
        dfs(ge,
            [&s](){ ++s.states; },
            [&s](bdd){ ++s.edges; });
      }
    return s;
  }

  twa_sub_statistics
  sub_stats_reachable(const const_twa_ptr& g)
  {
    twa_sub_statistics s;
    auto ge = std::dynamic_pointer_cast<const twa_graph>(g);
    if (!ge)
      {
        sub_stats_bfs d(g, s);
        d.run();
      }
    else
      {
        dfs(ge,
            [&s](){ ++s.states; },
            [&s, &ge](bdd cond)
            {
              ++s.edges;
              s.transitions += bdd_satcountset(cond, ge->ap_vars());
            });
      }
    return s;
  }

  void printable_formula::print(std::ostream& os, const char*) const
  {
    print_psl(os, val_);
  }

  void printable_scc_info::print(std::ostream& os, const char* pos) const
  {
    unsigned n = val_->scc_count();
    if (*pos != '[')
      {
        os << n;
        return;
      }
    bool accepting = false;
    bool rejecting = false;
    bool trivial = false;
    bool non_trivial = false;
    bool terminal = false;
    bool non_terminal = false;
    bool weak = false;
    bool non_weak = false;
    bool inherently_weak = false;
    bool non_inherently_weak = false;
    bool complete = false;
    bool non_complete = false;

    const char* beg = pos;
    auto error = [&](std::string str)
      {
        std::ostringstream tmp;
        const char* end = std::strchr(pos, ']');
        tmp << "unknown option '" << str << "' in '%"
            << std::string(beg, end + 2) << '\'';
        throw std::runtime_error(tmp.str());
      };

    do
    {
      ++pos;
      switch (*pos)
        {
        case 'a':
        case 'R':
          accepting = true;
          break;
        case 'A':
        case 'r':
          rejecting = true;
          break;
        case 'c':
          complete = true;
          break;
        case 'C':
          non_complete = true;
          break;
        case 't':
          terminal = true;
          break;
        case 'T':
          non_terminal = true;
          break;
        case 'v':
          trivial = true;
          break;
        case 'V':
          non_trivial = true;
          break;
        case 'w':
          weak = true;
          break;
        case 'W':
          non_weak = true;
          break;
        case 'i':
          if (pos[1] == 'w')
            {
              inherently_weak = true;
              ++pos;
            }
          else
            {
              error(std::string(pos, pos + 2));
            }
          break;
        case 'I':
          if (pos[1] == 'W')
            {
              non_inherently_weak = true;
              ++pos;
            }
          else
            {
              error(std::string(pos, pos + 2));
            }
          break;
        case ' ':
        case '\t':
        case '\n':
        case ',':
        case ']':
          break;
        default:
          error(std::string(pos, pos + 1));
        }
    }
    while (*pos != ']');

    val_->determine_unknown_acceptance();
    unsigned count = 0U;

    for (unsigned i = 0; i < n; ++i)
      {
        if (accepting && val_->is_rejecting_scc(i))
          continue;
        if (rejecting && val_->is_accepting_scc(i))
          continue;
        if (trivial && !val_->is_trivial(i))
          continue;
        if (non_trivial && val_->is_trivial(i))
          continue;
        if (complete && !is_complete_scc(*val_, i))
          continue;
        if (non_complete && is_complete_scc(*val_, i))
          continue;
        if (terminal && !is_terminal_scc(*val_, i))
          continue;
        if (non_terminal && is_terminal_scc(*val_, i))
          continue;
        if (weak && !is_weak_scc(*val_, i))
          continue;
        if (non_weak && is_weak_scc(*val_, i))
          continue;
        if (inherently_weak && !is_inherently_weak_scc(*val_, i))
          continue;
        if (non_inherently_weak && is_inherently_weak_scc(*val_, i))
          continue;
        ++count;
      }

    os << count;
  }

  void printable_acc_cond::print(std::ostream& os, const char* pos) const
  {
    if (*pos != '[')
      {
        os << val_.get_acceptance();
        return;
      }
    const char* beg = pos;
    ++pos;
    const char* end = strchr(pos, ']');
    try
      {
        os << val_.name(std::string(pos, end).c_str());
      }
    catch (const std::runtime_error& e)
      {
        std::ostringstream tmp;
        tmp << "while processing %"
            << std::string(beg, end + 2) << ", ";
        tmp << e.what();
        throw std::runtime_error(tmp.str());

      }
  }


  stat_printer::stat_printer(std::ostream& os, const char* format)
    : format_(format)
  {
    declare('a', &acc_);
    declare('c', &scc_);
    declare('d', &deterministic_);
    declare('e', &edges_);
    declare('f', &form_);
    declare('g', &gen_acc_);
    declare('n', &nondetstates_);
    declare('p', &complete_);
    declare('s', &states_);
    declare('S', &scc_);        // Historical.  Deprecated.  Use %c instead.
    declare('t', &trans_);
    set_output(os);
    if (format)
      prime(format);
  }

  std::ostream&
  stat_printer::print(const const_twa_graph_ptr& aut, formula f)
  {
    form_ = f;

    if (has('t'))
      {
        twa_sub_statistics s = sub_stats_reachable(aut);
        states_ = s.states;
        edges_ = s.edges;
        trans_ = s.transitions;
      }
    else if (has('s') || has('e'))
      {
        twa_statistics s = stats_reachable(aut);
        states_ = s.states;
        edges_ = s.edges;
      }

    if (has('a'))
      acc_ = aut->num_sets();

    // %S was renamed to %c in Spot 1.2, so that autfilt could use %S
    // and %s to designate the state numbers in input and output
    // automate.  We still recognize %S an obsolete and undocumented
    // alias for %c, if it is not overridden by a subclass.
    if (has('c') || has('S'))
      scc_.automaton(aut);

    if (has('n'))
      {
        nondetstates_ = count_nondet_states(aut);
        deterministic_ = (nondetstates_ == 0);
      }
    else if (has('d'))
      {
        // This is more efficient than calling count_nondet_state().
        deterministic_ = is_deterministic(aut);
      }

    if (has('p'))
      {
        complete_ = is_complete(aut);
      }

    if (has('g'))
      gen_acc_ = aut->acc();

    auto& os = format(format_);
    // Make sure we do not hold a pointer to the automaton or the
    // formula, as these may prevent atomic proposition to be freed
    // before a next job.
    scc_.reset();
    form_ = nullptr;
    return os;
  }

}
