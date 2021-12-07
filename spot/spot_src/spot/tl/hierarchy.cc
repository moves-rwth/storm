// -*- coding: utf-8 -*-
// Copyright (C) 2017-2019 Laboratoire de Recherche et Développement de
// l'Epita (LRDE)
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
#include <spot/tl/formula.hh>
#include <spot/tl/hierarchy.hh>
#include <spot/tl/nenoform.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twaalgos/minimize.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/remfin.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/strength.hh>
#include <spot/twaalgos/totgba.hh>
#include <spot/twaalgos/cobuchi.hh>

using namespace std::string_literals;

namespace spot
{
  namespace
  {
    static bool
    cobuchi_realizable(spot::formula f,
                       const const_twa_graph_ptr& aut)
    {
      // Find which algorithm must be performed between nsa_to_nca() and
      // dnf_to_nca(). Throw an exception if none of them can be performed.
      std::vector<acc_cond::rs_pair> pairs;
      bool street_or_parity = aut->acc().is_streett_like(pairs)
                              || aut->acc().is_parity();
      if (!street_or_parity && !aut->get_acceptance().is_dnf())
        throw std::runtime_error("cobuchi_realizable() only works with "
                                 "Streett-like, Parity or any "
                                 "acceptance condition in DNF");

      // If !f is a DBA, it belongs to the recurrence class, which means
      // f belongs to the persistence class (is cobuchi_realizable).
      twa_graph_ptr not_aut = ltl_to_tgba_fm(formula::Not(f), aut->get_dict());
      not_aut = scc_filter(not_aut);
      if (is_universal(not_aut))
        return true;

      // Checks if f is cobuchi_realizable.
      twa_graph_ptr cobuchi = street_or_parity ? nsa_to_nca(aut)
                                               : dnf_to_nca(aut);
      return !cobuchi->intersects(not_aut);
    }

    [[noreturn]] static void invalid_spot_pr_check()
    {
      throw std::runtime_error("invalid value for SPOT_PR_CHECK "
                               "(should be 1, 2, or 3)");
    }

    static prcheck
    algo_to_perform(bool is_persistence, bool aut_given)
    {
      static unsigned value = [&]()
        {
          int val;
          try
            {
              auto s = getenv("SPOT_PR_CHECK");
              val = s ? std::stoi(s) : 0;
            }
          catch (const std::exception& e)
            {
              invalid_spot_pr_check();
            }
          if (val < 0 || val > 3)
            invalid_spot_pr_check();
          return val;
        }();
      switch (value)
        {
        case 0:
          if (aut_given && !is_persistence)
            return prcheck::via_Parity;
          else
            return prcheck::via_CoBuchi;
        case 1:
          return prcheck::via_CoBuchi;
        case 2:
          return prcheck::via_Rabin;
        case 3:
          return prcheck::via_Parity;
        default:
          SPOT_UNREACHABLE();
        }
      SPOT_UNREACHABLE();
      return prcheck::via_Parity;
    }

    static bool
    detbuchi_realizable(const twa_graph_ptr& aut)
    {
      if (is_universal(aut))
        return true;

      bool want_old = algo_to_perform(false, true) == prcheck::via_Rabin;
      if (want_old)
        {
          // if aut is a non-deterministic TGBA, we do
          // TGBA->DPA->DRA->(D?)BA.  The conversion from DRA to
          // BA will preserve determinism if possible.
          spot::postprocessor p;
          p.set_type(spot::postprocessor::Generic);
          p.set_pref(spot::postprocessor::Deterministic);
          p.set_level(spot::postprocessor::Low);
          auto dpa = p.run(aut);
          if (dpa->acc().is_generalized_buchi())
            {
              assert(is_deterministic(dpa));
              return true;
            }
          else
            {
              auto dra = to_generalized_rabin(dpa);
              return rabin_is_buchi_realizable(dra);
            }
        }
      // Converting reduce_parity() will produce a Büchi automaton (or
      // an automaton with "t" or "f" acceptance) if the parity
      // automaton is DBA-realizable.
      spot::postprocessor p;
      p.set_type(spot::postprocessor::Parity);
      p.set_pref(spot::postprocessor::Deterministic);
      p.set_level(spot::postprocessor::Low);
      auto dpa = p.run(aut);
      return dpa->acc().is_f() || dpa->acc().is_generalized_buchi();
    }
  }

  bool
  is_persistence(formula f, twa_graph_ptr aut, prcheck algo)
  {
    if (f.is_syntactic_persistence())
      return true;

    // Perform a quick simplification of the formula taking into account the
    // following simplification's parameters: basics, synt_impl, event_univ.
    spot::tl_simplifier simpl(spot::tl_simplifier_options(true, true, true));
    f = simpl.simplify(f);
    if (f.is_syntactic_persistence())
      return true;

    if (algo == prcheck::Auto)
      algo = algo_to_perform(true, aut != nullptr);

    switch (algo)
      {
      case prcheck::via_CoBuchi:
        return cobuchi_realizable(f, aut ? aut :
                                  ltl_to_tgba_fm(f, make_bdd_dict(), true));

      case prcheck::via_Rabin:
      case prcheck::via_Parity:
        return detbuchi_realizable(ltl_to_tgba_fm(formula::Not(f),
                                                  make_bdd_dict(), true));

      case prcheck::Auto:
        SPOT_UNREACHABLE();
      }

    SPOT_UNREACHABLE();
  }

  bool
  is_recurrence(formula f, twa_graph_ptr aut, prcheck algo)
  {
    if (f.is_syntactic_recurrence())
      return true;

    // Perform a quick simplification of the formula taking into account the
    // following simplification's parameters: basics, synt_impl, event_univ.
    spot::tl_simplifier simpl(spot::tl_simplifier_options(true, true, true));
    f = simpl.simplify(f);
    if (f.is_syntactic_recurrence())
      return true;

    if (algo == prcheck::Auto)
      algo = algo_to_perform(true, aut != nullptr);

    switch (algo)
      {
      case prcheck::via_CoBuchi:
        return cobuchi_realizable(formula::Not(f),
                                  ltl_to_tgba_fm(formula::Not(f),
                                                 make_bdd_dict(), true));

      case prcheck::via_Rabin:
      case prcheck::via_Parity:
        return detbuchi_realizable(aut ? aut :
                                   ltl_to_tgba_fm(f, make_bdd_dict(), true));

      case prcheck::Auto:
        SPOT_UNREACHABLE();
      }

    SPOT_UNREACHABLE();
  }


  [[noreturn]] static void invalid_spot_o_check()
  {
    throw std::runtime_error("invalid value for SPOT_O_CHECK "
                             "(should be 1, 2, or 3)");
  }

  // This private function is defined in minimize.cc for technical
  // reasons.
  SPOT_LOCAL bool is_wdba_realizable(formula f, twa_graph_ptr aut = nullptr);

  bool
  is_obligation(formula f, twa_graph_ptr aut, ocheck algo)
  {
    if (algo == ocheck::Auto)
      {
        static ocheck env_algo = []()
          {
            int val;
            try
              {
                auto s = getenv("SPOT_O_CHECK");
                val = s ? std::stoi(s) : 0;
              }
            catch (const std::exception& e)
              {
                invalid_spot_o_check();
              }
            if (val == 0)
              return ocheck::via_WDBA;
            else if (val == 1)
              return ocheck::via_CoBuchi;
            else if (val == 2)
              return ocheck::via_Rabin;
            else if (val == 3)
              return ocheck::via_WDBA;
            else
              invalid_spot_o_check();
          }();
        algo = env_algo;
      }
    switch (algo)
      {
      case ocheck::via_WDBA:
        return is_wdba_realizable(f, aut);
      case ocheck::via_CoBuchi:
        return (is_persistence(f, aut, prcheck::via_CoBuchi)
                && is_recurrence(f, aut, prcheck::via_CoBuchi));
      case ocheck::via_Rabin:
        return (is_persistence(f, aut, prcheck::via_Rabin)
                && is_recurrence(f, aut, prcheck::via_Rabin));
      case ocheck::Auto:
        SPOT_UNREACHABLE();
      }
    SPOT_UNREACHABLE();
  }


  char mp_class(formula f)
  {
    if (f.is_syntactic_safety() && f.is_syntactic_guarantee())
      return 'B';
    auto dict = make_bdd_dict();
    auto aut = ltl_to_tgba_fm(f, dict, true);
    auto min = minimize_obligation(aut, f);
    if (aut != min) // An obligation.
      {
        scc_info si(min);
        // The minimba WDBA can have some trivial accepting SCCs
        // that we should ignore in is_terminal_automaton().
        bool g = is_terminal_automaton(min, &si, true);
        bool s = is_safety_automaton(min, &si);
        if (g)
          return s ? 'B' : 'G';
        else
          return s ? 'S' : 'O';
      }
    // Not an obligation.  Could by 'P', 'R', or 'T'.
    if (is_recurrence(f, aut))
      return 'R';
    if (is_persistence(f, aut))
      return 'P';
    return 'T';
  }

  std::string mp_class(formula f, const char* opt)
  {
    return mp_class(mp_class(f), opt);
  }

  std::string mp_class(char mpc, const char* opt)
  {
    bool verbose = false;
    bool wide = false;
    if (opt)
      for (;;)
        switch (int o = *opt++)
          {
          case 'v':
            verbose = true;
            break;
          case 'w':
            wide = true;
            break;
          case ' ':
          case '\t':
          case '\n':
          case ',':
            break;
          case '\0':
          case ']':
            goto break2;
          default:
            {
              std::ostringstream err;
              err << "unknown option '" << o << "' for mp_class()";
              throw std::runtime_error(err.str());
            }
          }
  break2:
    std::string c(1, mpc);
    if (wide)
      {
        switch (mpc)
          {
          case 'B':
            c = "GSOPRT";
            break;
          case 'G':
            c = "GOPRT";
            break;
          case 'S':
            c = "SOPRT";
            break;
          case 'O':
            c = "OPRT";
            break;
          case 'P':
            c = "PT";
            break;
          case 'R':
            c = "RT";
            break;
          case 'T':
            break;
          default:
            throw std::runtime_error("mp_class() called with unknown class");
          }
      }
    if (!verbose)
      return c;

    std::ostringstream os;
    bool first = true;
    for (char ch: c)
      {
        if (first)
          first = false;
        else
          os << ' ';
        switch (ch)
          {
          case 'B':
            os << "guarantee safety";
            break;
          case 'G':
            os << "guarantee";
            break;
          case 'S':
            os << "safety";
            break;
          case 'O':
            os << "obligation";
            break;
          case 'P':
            os << "persistence";
            break;
          case 'R':
            os << "recurrence";
            break;
          case 'T':
            os << "reactivity";
            break;
          }
      }
    return os.str();
  }

  unsigned nesting_depth(formula f, op oper)
  {
    unsigned max_depth = 0;
    for (formula child: f)
      max_depth = std::max(max_depth, nesting_depth(child, oper));
    return max_depth + f.is(oper);
  }

  unsigned nesting_depth(formula f, const op* begin, const op* end)
  {
    unsigned max_depth = 0;
    for (formula child: f)
      max_depth = std::max(max_depth, nesting_depth(child, begin, end));
    bool matched = std::find(begin, end, f.kind()) != end;
    return max_depth + matched;
  }

  unsigned nesting_depth(formula f, const char* opers)
  {
    bool want_nnf = false;
    std::vector<op> v;
    for (;;)
      switch (char c = *opers++)
        {
        case '~':
          want_nnf = true;
          break;
        case '!':
          v.push_back(op::Not);
          break;
        case '&':
          v.push_back(op::And);
          break;
        case '|':
          v.push_back(op::Or);
          break;
        case 'e':
          v.push_back(op::Equiv);
          break;
        case 'F':
          v.push_back(op::F);
          break;
        case 'G':
          v.push_back(op::G);
          break;
        case 'i':
          v.push_back(op::Implies);
          break;
        case 'M':
          v.push_back(op::M);
          break;
        case 'R':
          v.push_back(op::R);
          break;
        case 'U':
          v.push_back(op::U);
          break;
        case 'W':
          v.push_back(op::W);
          break;
        case 'X':
          v.push_back(op::X);
          break;
        case '\0':
        case ']':
          goto break2;
        default:
          throw std::runtime_error
            ("nesting_depth(): unknown operator '"s + c + '\'');
        }
  break2:
    if (want_nnf)
      f = negative_normal_form(f);
    const op* vd = v.data();
    return nesting_depth(f, vd, vd + v.size());
  }

  bool is_liveness(formula f)
  {
    return is_liveness_automaton(ltl_to_tgba_fm(f, spot::make_bdd_dict()));
  }
}
