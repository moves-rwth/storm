// -*- coding: utf-8 -*-
// Copyright (C) 2013-2018 Laboratoire de Recherche
// et Développement de l'Epita.
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
#include <fstream>
#include <sstream>
#include <map>
#include <spot/misc/bddlt.hh>
#include <spot/misc/satsolver.hh>
#include <spot/misc/timer.hh>
#include <spot/priv/satcommon.hh>
#include <spot/twaalgos/dtbasat.hh>
#include <spot/twaalgos/langmap.hh>
#include <spot/twaalgos/sccinfo.hh>

// If you set the SPOT_TMPKEEP environment variable the temporary
// file used to communicate with the sat solver will be left in
// the current directory.
//
// Additionally, if the following TRACE macro is set to 1, the CNF
// file will be output with a comment before each clause, and an
// additional output file (dtba-sat.dbg) will be created with a list
// of all positive variables in the result and their meaning.

#define TRACE 0

#if TRACE
#define dout out << "c "
#define cnf_comment(...) solver.comment(__VA_ARGS__)
#define trace std::cerr
#else
#define cnf_comment(...) while (0) solver.comment(__VA_ARGS__)
#define dout while (0) std::cout
#define trace dout
#endif

namespace spot
{
  namespace
  {
    static bdd_dict_ptr debug_dict;

    struct path
    {
      int src_ref;
      int dst_ref;

      path(int src_ref, int dst_ref)
        : src_ref(src_ref), dst_ref(dst_ref)
      {
      }

      path(int src_ref)
        : src_ref(src_ref), dst_ref(src_ref)
      {
      }

      bool operator<(const path& other) const
      {
        if (this->src_ref < other.src_ref)
          return true;
        if (this->src_ref > other.src_ref)
          return false;
        if (this->dst_ref < other.dst_ref)
          return true;
        if (this->dst_ref > other.dst_ref)
          return false;
        return false;
      }

    };

    struct dict
    {
      std::vector<bdd> alpha_vect;
      std::map<path, unsigned> path_map;
      std::map<bdd, unsigned, bdd_less_than> alpha_map;
      vars_helper helper;
      int nvars = 0;
      unsigned cand_size;
      unsigned ref_size;

      int
      transid(unsigned src, unsigned cond, unsigned dst)
      {
        return helper.get_t(src, cond, dst);
      }

      int
      transid(unsigned src, bdd& cond, unsigned dst)
      {
#if TRACE
        try
        {
          return helper.get_t(src, alpha_map.at(cond), dst);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "label of transid " << fmt_t(src, cond, dst)
            << " not found.\n";
          throw c;
        }
#else
        return helper.get_t(src, alpha_map[cond], dst);
#endif
      }

      int
      transacc(unsigned src, unsigned cond, unsigned dst)
      {
        return helper.get_ta(src, cond, dst);
      }

      int
      transacc(unsigned src, bdd& cond, unsigned dst)
      {
#if TRACE
        try
        {
          return helper.get_ta(src, alpha_map.at(cond), dst);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "label of transacc " << fmt_t(src, cond, dst)
            << " not found.\n";
          throw c;
        }
#else
        return helper.get_ta(src, alpha_map[cond], dst);
#endif
      }

      int
      pathid_ref(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
          unsigned dst_ref)
      {
#if TRACE
        try
        {
          return helper.get_prc(
              path_map.at(path(src_ref, dst_ref)), src_cand, dst_cand, false);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "path(" << src_ref << ',' << dst_ref << ") of pathid_ref"
            << ' ' << fmt_p(src_cand, src_ref, dst_cand, dst_ref)
            << " not found.\n";
          throw c;
        }
#else
        return helper.get_prc(
            path_map[path(src_ref, dst_ref)], src_cand, dst_cand, false);
#endif
      }

#if TRACE
      int
      pathid_ref(unsigned path, unsigned src_cand, unsigned dst_cand)
      {
        return helper.get_prc(path, src_cand, dst_cand, false);
      }

      int
      pathid_cand(unsigned path, unsigned src_cand, unsigned dst_cand)
      {
        return helper.get_prc(path, src_cand, dst_cand, true);
      }
#endif

      int
      pathid_cand(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
          unsigned dst_ref)
      {
#if TRACE
        try
        {
          return helper.get_prc(
              path_map.at(path(src_ref, dst_ref)), src_cand, dst_cand, true);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "path(" << src_ref << ',' << dst_ref
            << ") of pathid_cand "
            << fmt_p(src_cand, src_ref, dst_cand, dst_ref) << " not found.\n";
          throw c;
        }
#else
        return helper.get_prc(
            path_map[path(src_ref, dst_ref)], src_cand, dst_cand, true);
#endif
      }

      std::string
      fmt_t(unsigned src, bdd& cond, unsigned dst)
      {
        return helper.format_t(debug_dict, src, cond, dst);
      }

      std::string
      fmt_t(unsigned src, unsigned cond, unsigned dst)
      {
        return helper.format_t(debug_dict, src, alpha_vect[cond], dst);
      }

      std::string
      fmt_p(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
          unsigned dst_ref)
      {
        return helper.format_p(src_cand, src_ref, dst_cand, dst_ref);
      }

    };

    void declare_vars(const const_twa_graph_ptr& aut,
                          dict& d,
                          bdd ap,
                          bool state_based,
                          scc_info& sm)
    {
      d.ref_size = aut->num_states();

      if (d.cand_size == -1U)
        for (unsigned i = 0; i < d.ref_size; ++i)
          if (sm.reachable_state(i))
            ++d.cand_size;        // Note that we start from -1U the
                                // cand_size is one less than the
                                // number of reachable states.

      // In order to optimize memory usage, src_cand & dst_cand have been
      // removed from path struct (the reasons are: they were no optimization
      // on them and their values are known from the beginning).
      //
      // However, since some optimizations are based on the following i and k,
      // it is necessary to associate to each path constructed, an ID number.
      //
      // Given this ID, src_cand, dst_cand and a boolean that tells we want
      // ref or cand var, the corresponding litteral can be retrieved thanks
      // to get_prc(...), a vars_helper's method.
      unsigned path_size = 0;
      for (unsigned i = 0; i < d.ref_size; ++i)
        {
          if (!sm.reachable_state(i))
            continue;
          unsigned i_scc = sm.scc_of(i);
          bool is_trivial = sm.is_trivial(i_scc);

          for (unsigned k = 0; k < d.ref_size; ++k)
            {
              if (!sm.reachable_state(k))
                continue;
              if ((sm.scc_of(k) != i_scc || is_trivial)
                    && !(i == k))
                continue;
              d.path_map[path(i, k)] = path_size++;
            }
        }

      // Fill dict's bdd vetor (alpha_vect) and save each bdd and it's
      // corresponding index in alpha_map. This is necessary beacause some
      // loops start from a precise bdd. Therefore, it's useful to know
      // it's corresponding index to deal with vars_helper.
      bdd all = bddtrue;
      for (unsigned j = 0; all != bddfalse; ++j)
      {
        bdd one = bdd_satoneset(all, ap, bddfalse);
        d.alpha_vect.push_back(one);
        d.alpha_map[d.alpha_vect[j]] = j;
        all -= one;
      }

      // Initialize vars_helper by giving it all the necessary information.
      // 1: nacc_size is 1 (with Büchi) | true: means dtbasat, i-e, not dtwasat.
      d.helper.init(d.cand_size, d.alpha_vect.size(), d.cand_size,
        1, path_size, state_based, true);

      // Based on all previous informations, helper knows all litterals.
      d.helper.declare_all_vars(++d.nvars);
    }

    typedef std::pair<int, int> sat_stats;

    static
    sat_stats dtba_to_sat(satsolver& solver,
                          const const_twa_graph_ptr& ref,
                          dict& d,
                          bool state_based)
    {
      // Compute the AP used.
      bdd ap = ref->ap_vars();

      // Count the number of atomic propositions.
      int nap = 0;
      {
        bdd cur = ap;
        while (cur != bddtrue)
          {
            ++nap;
            cur = bdd_high(cur);
          }
        nap = 1 << nap;
      }

      scc_info sm(ref, scc_info_options::NONE);

      // Number all the SAT variables we may need.
      declare_vars(ref, d, ap, state_based, sm);

      // Store alpha_vect's size once for all.
      unsigned alpha_size = d.alpha_vect.size();

      // Tell the satsolver the number of variables.
      solver.adjust_nvars(d.nvars);

      // Empty automaton is impossible.
      assert(d.cand_size > 0);

#if TRACE
      debug_dict = ref->get_dict();
      solver.comment("d.ref_size", d.ref_size, '\n');
      solver.comment("d.cand_size", d.cand_size, '\n');
#endif

      cnf_comment("symmetry-breaking clauses\n");
      unsigned j = 0;
      for (unsigned l = 0; l < alpha_size; ++l, ++j)
        for (unsigned i = 0; i < d.cand_size - 1; ++i)
          for (unsigned k = i * nap + j + 2; k < d.cand_size; ++k)
            {
              cnf_comment("¬", d.fmt_t(i, l, k), '\n');
              solver.add({-d.transid(i, l, k), 0});
            }

      if (!solver.get_nb_clauses())
         cnf_comment("(none)\n");

      cnf_comment("(1) the candidate automaton is complete\n");
      for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
        for (unsigned l = 0; l < alpha_size; ++l)
          {
#if TRACE
            solver.comment("");
            for (unsigned q2 = 0; q2 < d.cand_size; q2++)
              {
                solver.comment_rec(d.fmt_t(q1, l, q2), "δ");
                if (q2 != d.cand_size)
                  solver.comment_rec(" ∨ ");
              }
            solver.comment_rec('\n');
#endif
            for (unsigned q2 = 0; q2 < d.cand_size; q2++)
              solver.add(d.transid(q1, l, q2));
            solver.add(0);
          }

      cnf_comment("(2) the initial state is reachable\n");
      {
        unsigned init = ref->get_init_state_number();
        cnf_comment(d.fmt_p(0, init, 0, init), '\n');
        solver.add({d.pathid_ref(0, init, 0, init), 0});
      }

      for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
        {
          for (unsigned q1p = 0; q1p < d.ref_size; ++q1p)
          {
            // Added to comply with the variable declaration, i-e to avoid
            // using undeclared variables.
            if (!sm.reachable_state(q1p))
              continue;

            cnf_comment("(3) augmenting paths based on Cand[", q1, "] and Ref[",
                        q1p, "]\n");
            for (auto& tr: ref->out(q1p))
              {
                unsigned dp = tr.dst;
                bdd all = tr.cond;
                while (all != bddfalse)
                  {
                    bdd s = bdd_satoneset(all, ap, bddfalse);
                    all -= s;

                    for (unsigned q2 = 0; q2 < d.cand_size; q2++)
                      {
                        int prev = d.pathid_ref(q1, q1p, q1, q1p);
                        int succ = d.pathid_ref(q2, dp, q2, dp);
                        if (prev == succ)
                          continue;

                        cnf_comment(prev, "∧", d.fmt_t(q1, s, q2), "δ →",
                                    d.fmt_p(q2, dp, q2, dp), '\n');
                        solver.add({-prev, -d.transid(q1, s, q2), succ, 0});
                      }
                  }
              }
          }
        }

      const acc_cond& ra = ref->acc();

      // construction of contraints (4,5) : all loops in the product
      // where no accepting run is detected in the ref. automaton,
      // must also be marked as not accepting in the cand. automaton
      for (unsigned q1p = 0; q1p < d.ref_size; ++q1p)
        {
          if (!sm.reachable_state(q1p))
            continue;
          unsigned q1p_scc = sm.scc_of(q1p);
          if (sm.is_trivial(q1p_scc))
            continue;
          for (unsigned q2p = 0; q2p < d.ref_size; ++q2p)
            {
              if (!sm.reachable_state(q2p))
                continue;
              // We are only interested in transition that can form a
              // cycle, so they must belong to the same SCC.
              if (sm.scc_of(q2p) != q1p_scc)
                continue;
              for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
                for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                  {
                    std::string f_p = d.fmt_p(q1, q1p, q2, q2p);
                    cnf_comment("(4&5) matching paths from reference based on",
                                f_p, '\n');

                    int pid1 = d.pathid_ref(q1, q1p, q2, q2p);
                    for (auto& tr: ref->out(q2p))
                      {
                        unsigned dp = tr.dst;
                        // Skip destinations not in the SCC.
                        if (sm.scc_of(dp) != q1p_scc)
                          continue;

                        if (ra.accepting(tr.acc))
                          continue;
                        for (unsigned q3 = 0; q3 < d.cand_size; ++q3)
                          {
                            if (dp == q1p && q3 == q1) // (4) looping
                              {
                                bdd all = tr.cond;
                                while (all != bddfalse)
                                  {
                                    bdd s = bdd_satoneset(all, ap, bddfalse);
                                    all -= s;
#if TRACE
                                    std::string f_t = d.fmt_t(q2, s, q1);
                                    cnf_comment(f_p, "R ∧", f_t, "δ → ¬", f_t,
                                                "F\n");
#endif
                                    solver.add({-pid1,
                                                -d.transid(q2, s, q1),
                                                -d.transacc(q2, s, q1),
                                                0});
                                  }
                              }
                            else // (5) not looping
                              {
                                int pid2 = d.pathid_ref(q1, q1p, q3, dp);
                                if (pid1 == pid2)
                                  continue;

                                bdd all = tr.cond;
                                while (all != bddfalse)
                                  {
                                    bdd s = bdd_satoneset(all, ap, bddfalse);
                                    all -= s;

                                    cnf_comment(f_p, "R ∧", d.fmt_t(q2, s, q3),
                                                "δ →", d.fmt_p(q1, q1p, q3, dp),
                                                "R\n");
                                    solver.add({-pid1,
                                                -d.transid(q2, s, q3),
                                                pid2,
                                                0});
                                  }
                              }
                          }
                      }
                  }
            }
        }
      // construction of contraints (6,7): all loops in the product
      // where accepting run is detected in the ref. automaton, must
      // also be marked as accepting in the candidate.
      for (unsigned q1p = 0; q1p < d.ref_size; ++q1p)
        {
          if (!sm.reachable_state(q1p))
            continue;
          unsigned q1p_scc = sm.scc_of(q1p);
          if (sm.is_trivial(q1p_scc))
            continue;
          for (unsigned q2p = 0; q2p < d.ref_size; ++q2p)
            {
              if (!sm.reachable_state(q2p))
                continue;
              // We are only interested in transition that can form a
              // cycle, so they must belong to the same SCC.
              if (sm.scc_of(q2p) != q1p_scc)
                continue;
              for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
                for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                  {
                    std::string f_p = d.fmt_p(q1, q1p, q2, q2p);
                    cnf_comment("(6&7) matching paths from candidate based on",
                                f_p, '\n');

                    int pid1;
                    if (q1 == q2 && q1p == q2p)
                      pid1 = d.pathid_ref(q1, q1p, q2, q2p);
                    else
                      pid1 = d.pathid_cand(q1, q1p, q2, q2p);

                    for (auto& tr: ref->out(q2p))
                      {
                        unsigned dp = tr.dst;
                        // Skip destinations not in the SCC.
                        if (sm.scc_of(dp) != q1p_scc)
                          continue;
                        for (unsigned q3 = 0; q3 < d.cand_size; q3++)
                          {
                            if (dp == q1p && q3 == q1) // (6) looping
                              {
                                // We only care about the looping case if
                                // it is accepting in the reference.
                                if (!ra.accepting(tr.acc))
                                  continue;
                                bdd all = tr.cond;
                                while (all != bddfalse)
                                  {
                                    bdd s = bdd_satoneset(all, ap, bddfalse);
                                    all -= s;
#if TRACE
                                    std::string f_t = d.fmt_t(q2, s, q1);
                                    cnf_comment(f_p, "C ∧", f_t, "δ →", f_t,
                                                "F\n");
#endif
                                    solver.add({-pid1,
                                                -d.transid(q2, s, q1),
                                                d.transacc(q2, s, q1),
                                                0});
                                  }
                              }
                            else // (7) no loop
                              {
                                int pid2 = d.pathid_cand(q1, q1p, q3, dp);
                                if (pid1 == pid2)
                                  continue;

                                bdd all = tr.cond;
                                while (all != bddfalse)
                                  {
                                    bdd s = bdd_satoneset(all, ap, bddfalse);
                                    all -= s;
#if TRACE
                                    std::string f_t = d.fmt_t(q2, s, q3);
                                    cnf_comment(f_p, "C ∧", f_t, "δ ∧ ¬", f_t,
                                                "F →", d.fmt_p(q1, q1p, q3, dp),
                                                "C\n");
#endif
                                    solver.add({-pid1,
                                                -d.transid(q2, s, q3),
                                                d.transacc(q2, s, q3),
                                                pid2,
                                                0});
                                  }
                              }
                          }
                      }
                  }
            }
        }
      return solver.stats();
    }

    static twa_graph_ptr
    sat_build(const satsolver::solution& solution, dict& satdict,
              const_twa_graph_ptr aut, bool state_based)
    {
      trace << "sat_build(...)\n";

      auto autdict = aut->get_dict();
      auto a = make_twa_graph(autdict);
      a->copy_ap_of(aut);
      a->set_buchi();
      if (state_based)
        a->prop_state_acc(true);
      a->prop_universal(true);
      a->new_states(satdict.cand_size);

#if TRACE
      std::fstream out("dtba-sat.dbg",
                       std::ios_base::trunc | std::ios_base::out);
      out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
#endif
      std::set<int> acc_states;
      std::set<src_cond> seen_trans;

      unsigned alpha_size = satdict.alpha_vect.size();
      unsigned cand_size = satdict.cand_size;
      for (unsigned i = 0; i < cand_size; ++i)
        for (unsigned j = 0; j < alpha_size; ++j)
          for (unsigned k = 0; k < cand_size; ++k)
          {
            if (solution[satdict.transid(i, j, k) - 1])
            {
              // Ignore unuseful transitions because of reduced cand_size.
              if (i >= cand_size)
                continue;

              // Skip (s,l,d2) if we have already seen some (s,l,d1).
              if (seen_trans.insert(src_cond(i, satdict.alpha_vect[j])).second)
              {
                bool accept = false;
                if (state_based)
                  accept = acc_states.find(i) != acc_states.end();
                if (!accept)
                  accept = solution[satdict.transacc(i, j, k) - 1];

                a->new_acc_edge(i, k, satdict.alpha_vect[j], accept);

                if (state_based && accept)
                  acc_states.insert(i);
              }
            }
          }
#if TRACE
      dout << "--- transition variables ---\n";
      for (unsigned i = 0; i < cand_size; ++i)
        for (unsigned j = 0; j < alpha_size; ++j)
          for (unsigned k = 0; k < cand_size; ++k)
          {
            int var = satdict.transid(i, j, k);
            std::string f_t = satdict.fmt_t(i, j, k);
            if (solution[var - 1])
              dout << ' ' << var << "\t " << f_t << '\n';
            else
              dout << -var << "\t¬" << f_t << '\n';
          }
      dout << "--- transition_acc variables ---\n";
      if (state_based)
      {
        dout << "In state_based mode with Büchi automaton, there is only 1 "
          "litteral for each src, regardless of dst or cond!\n";
        for (unsigned i = 0; i < cand_size; ++i)
        {
          int var = satdict.transacc(i, 0, 0);
          std::string f_t = satdict.fmt_t(i, 0, 0);
          if (solution[var - 1])
            dout << ' ' << var << "\t " << f_t << '\n';
          else
            dout << -var << "\t¬" << f_t << '\n';
        }
      }
      else
        for (unsigned i = 0; i < cand_size; ++i)
          for (unsigned j = 0; j < alpha_size; ++j)
            for (unsigned k = 0; k < cand_size; ++k)
            {
              int var = satdict.transacc(i, j, k);
              std::string f_t = satdict.fmt_t(i, j, k);
              if (solution[var - 1])
                dout << ' ' << var << "\t " << f_t << '\n';
              else
                dout << -var << "\t¬" << f_t << '\n';
            }
      dout << "--- ref pathid variables ---\n";
      std::map<int, std::string> cand_vars;
      for (auto it = satdict.path_map.begin(); it != satdict.path_map.end();
          ++it)
        for (unsigned k = 0; k < cand_size; ++k)
          for (unsigned l = 0; l < cand_size; ++l)
          {
            // false:reference | true:cand
            int cand_v = satdict.pathid_cand(it->second, k, l);
            int ref_v = satdict.pathid_ref(it->second, k, l);
            std::string f_p = satdict.fmt_p(k, it->first.src_ref, l,
                it->first.dst_ref);

            cand_vars[cand_v] = f_p;
            if (solution[ref_v - 1])
              dout << ' ' << ref_v << "\t " << f_p << '\n';
            else
              dout << -ref_v << "\t¬" << f_p << '\n';
          }
      dout << "--- cand pathid variables ---\n";
      for (auto it = cand_vars.begin(); it != cand_vars.end(); ++it)
      {
        if (solution[it->first - 1])
          dout << ' ' << it->first << "\t " << it->second << '\n';
        else
          dout << -it->first << "\t¬" << it->second << '\n';
      }
#endif
      a->merge_edges();
      a->purge_unreachable_states();
      return a;
    }
  }

  twa_graph_ptr
  dtba_sat_synthetize(const const_twa_graph_ptr& a,
                      int target_state_number, bool state_based)
  {
    if (!a->is_existential())
      throw std::runtime_error
        ("dtba_sat_synthetize() does not support alternating automata");
    if (!a->acc().is_buchi())
      throw std::runtime_error
        ("dtba_sat_synthetize() can only work with Büchi acceptance");
    if (target_state_number == 0)
      return nullptr;
    trace << "dtba_sat_synthetize(..., states = " << target_state_number
          << ", state_based = " << state_based << ")\n";
    dict d;
    d.cand_size = target_state_number;

    satsolver solver;
    satsolver::solution_pair solution;

    timer_map t;
    t.start("encode");
    dtba_to_sat(solver, a, d, state_based);
    t.stop("encode");
    t.start("solve");
    solution = solver.get_solution();
    t.stop("solve");

    twa_graph_ptr res = nullptr;
    if (!solution.second.empty())
      res = sat_build(solution.second, d, a, state_based);

    print_log(t, a->num_states(),
              target_state_number, res, solver); // If SPOT_SATLOG is set.

    trace << "dtba_sat_synthetize(...) = " << res << '\n';
    return res;
  }

  static twa_graph_ptr
  dichotomy_dtba_research(int max,
                          dict& d,
                          satsolver& solver,
                          const_twa_graph_ptr& prev,
                          bool state_based)
  {
    trace << "dichotomy_dtba_research(...)\n";
    int min = 1;
    int target = 0;
    twa_graph_ptr res = nullptr;

    while (min < max)
    {
      target = (max + min) / 2;
      trace << "min:" << min << ", max:" << max << ", target:" << target
        << '\n';
      timer_map t1;
      t1.start("encode");
      solver.assume(d.nvars + target);
      t1.stop("encode");
      trace << "solver.assume(" << d.nvars + target << ")\n";
      t1.start("solve");
      satsolver::solution_pair solution = solver.get_solution();
      t1.stop("solve");
      if (solution.second.empty())
      {
        trace << "UNSAT\n";
        max = target;
        print_log(t1, prev->num_states(), d.cand_size - target,
                  nullptr, solver);
      }
      else
      {
        trace << "SAT\n";
        res = sat_build(solution.second, d, prev, state_based);
        min = d.cand_size - res->num_states() + 1;
        print_log(t1, prev->num_states(), d.cand_size - target,
                  res, solver);
      }
    }

    trace << "End with max:" << max << ", min:" << min << '\n';
    if (!res)
    {
      trace << "All assumptions are UNSAT, let's try without...\n";
      timer_map t1;
      t1.start("encode");
      t1.stop("encode");
      t1.start("solve");
      t1.stop("solve");
      satsolver::solution_pair solution = solver.get_solution();
      trace << (solution.second.empty() ? "UNSAT!\n" : "SAT\n");
      res = solution.second.empty() ? nullptr :
        sat_build(solution.second, d, prev, state_based);
      print_log(t1, prev->num_states(), d.cand_size - target, res, solver);
    }

    return res ? res : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtba_sat_minimize_assume(const const_twa_graph_ptr& a,
                           bool state_based,
                           int max_states,
                           int sat_incr_steps)
  {
    if (!a->acc().is_buchi())
      throw std::runtime_error
        ("dtba_sat_minimize_assume() can only work with Büchi acceptance");
    if (sat_incr_steps < 0)
      throw std::runtime_error("with 'assume' algorithm, sat_incr_steps value "
                               " must be >= 0");

    const_twa_graph_ptr prev = a;
    dict d;
    d.cand_size = (max_states < 0) ? prev->num_states() - 1 : max_states;
    if (d.cand_size == 0)
      return nullptr;

    trace << "dtba_sat_minimize_assume(..., states = " << d.cand_size
      << ", state_based = " << state_based << ")\n";
    trace << "sat_incr_steps: " << sat_incr_steps << '\n';

    twa_graph_ptr next = spot::make_twa_graph(spot::make_bdd_dict());
    while (next && d.cand_size > 0)
    {
      // Warns the satsolver of the number of assumptions.
      int n_assumptions = (int) d.cand_size <= sat_incr_steps ?
        d.cand_size - 1 : sat_incr_steps;
      trace << "number of assumptions:" << n_assumptions << '\n';
      satsolver solver;
      solver.set_nassumptions_vars(n_assumptions);

      // First iteration of classic solving.
      timer_map t1;
      t1.start("encode");
      dtba_to_sat(solver, prev, d, state_based);

      // Compute the AP used.
      bdd ap = prev->ap_vars();

      // Add all assumptions clauses.
      unsigned dst = d.cand_size - 1;
      unsigned alpha_size = d.alpha_vect.size();
      for (int i = 1; i <= n_assumptions; i++, dst--)
      {
        cnf_comment("Next iteration:", dst, "\n");
        int assume_lit = d.nvars + i;

        cnf_comment("Add clauses to forbid the dst state.\n");
        for (unsigned l = 0; l < alpha_size; ++l)
          for (unsigned j = 0; j < d.cand_size; ++j)
          {
            cnf_comment(assume_lit, "→ ¬", d.fmt_t(j, l, dst), '\n');
            solver.add({-assume_lit, -d.transid(j, l, dst), 0});
          }

        // The assumption which has just been encoded implies the preceding
        // ones.
        if (i != 1)
        {
          cnf_comment(assume_lit, "→", assume_lit - 1, '\n');
          solver.add({-assume_lit, assume_lit - 1, 0});
        }
      }
      if (n_assumptions)
      {
        trace << "solver.assume(" << d.nvars + n_assumptions << ")\n";
        solver.assume(d.nvars + n_assumptions);
      }
      t1.stop("encode");
      t1.start("solve");
      satsolver::solution_pair solution = solver.get_solution();
      t1.stop("solve");

      if (solution.second.empty() && n_assumptions) // UNSAT
      {
        print_log(t1, prev->num_states(),
                  d.cand_size - n_assumptions, nullptr, solver);
        trace << "UNSAT\n";
        return dichotomy_dtba_research(n_assumptions, d, solver,
                                       prev, state_based);
      }

      trace << "SAT, restarting from zero\n";
      next = solution.second.empty() ? nullptr :
        sat_build(solution.second, d, prev, state_based);
      print_log(t1, prev->num_states(),
                d.cand_size - n_assumptions, next, solver);

      if (next)
      {
        prev = next;
        d = dict();
        d.cand_size = prev->num_states() - 1;
        if (d.cand_size == 0)
          next = nullptr;
      }
    }

    return prev == a ? nullptr : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtba_sat_minimize_incr(const const_twa_graph_ptr& a,
      bool state_based, int max_states, int sat_incr_steps)
  {
    if (!a->acc().is_buchi())
      throw std::runtime_error
        (": dtba_sat_minimize_incr() can only work with Büchi acceptance.");
    const_twa_graph_ptr prev = a;
    dict d;
    d.cand_size = (max_states < 0) ? prev->num_states() - 1 : max_states;
    if (d.cand_size == 0)
      return nullptr;

    trace << "dtba_sat_minimize_incr(..., states = " << d.cand_size
      << ", state_based = " << state_based << ")\n";

    bool naive = sat_incr_steps < 0;
    trace << "sat_incr_steps: " << sat_incr_steps << '\n';

    twa_graph_ptr next = spot::make_twa_graph(spot::make_bdd_dict());
    while (next && d.cand_size > 0)
    {
      // First iteration of classic solving.
      satsolver solver;
      timer_map t1;
      t1.start("encode");
      dtba_to_sat(solver, prev, d, state_based);
      t1.stop("encode");
      t1.start("solve");
      satsolver::solution_pair solution = solver.get_solution();
      t1.stop("solve");
      next = solution.second.empty() ? nullptr :
        sat_build(solution.second, d, prev, state_based);
      print_log(t1, prev->num_states(), d.cand_size, next, solver);

      trace << "First iteration done\n";

      // Compute the AP used.
      bdd ap = prev->ap_vars();

      // Incremental solving loop.
      unsigned orig_cand_size = d.cand_size;
      unsigned alpha_size = d.alpha_vect.size();
      for (int k = 0; next && d.cand_size > 0 && (naive || k < sat_incr_steps);
           ++k)
      {
        prev = next;
        int reach_states = prev->num_states();
        cnf_comment("Next iteration: ", reach_states - 1, "\n");
        trace << "Encoding the deletion of state " << reach_states - 1 << '\n';

        timer_map t2;
        t2.start("encode");
        // Add new constraints.
        for (unsigned i = reach_states - 1; i < d.cand_size; ++i)
          for (unsigned l = 0; l < alpha_size; ++l)
            for (unsigned j = 0; j < orig_cand_size; ++j)
              solver.add({-d.transid(j, l, i), 0});
        t2.stop("encode");
        d.cand_size = reach_states - 1;
        t2.start("solve");
        satsolver::solution_pair solution = solver.get_solution();
        t2.stop("solve");
        next = solution.second.empty() ? nullptr :
          sat_build(solution.second, d, prev, state_based);
        print_log(t2, prev->num_states(),
                  d.cand_size, next, solver);
      }

      if (next)
      {
        trace << "Starting from scratch\n";
        prev = next;
        d = dict();
        d.cand_size = prev->num_states() - 1;
        if (d.cand_size == 0)
          next = nullptr;
      }
    }

    return prev == a ? nullptr : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtba_sat_minimize(const const_twa_graph_ptr& a,
                    bool state_based, int max_states)
  {
    int n_states = (max_states < 0) ? a->num_states() : max_states + 1;

    twa_graph_ptr prev = nullptr;
    for (;;)
      {
        auto next =
          dtba_sat_synthetize(prev ? prev : a, --n_states, state_based);
        if (!next)
          return prev;
        else
          n_states = next->num_states();
        prev = next;
      }
    SPOT_UNREACHABLE();
  }

  twa_graph_ptr
  dtba_sat_minimize_dichotomy(const const_twa_graph_ptr& a,
                              bool state_based, bool langmap, int max_states)
  {
    trace << "Dichomoty\n";
    if (max_states < 0)
      max_states = a->num_states() - 1;
    int min_states = 1;
    if (langmap)
    {
      trace << "Langmap\n";
      std::vector<unsigned> v = language_map(a);
      min_states = get_number_of_distinct_vals(v);
    }
    trace << "min_states=" << min_states << '\n';

    twa_graph_ptr prev = nullptr;
    while (min_states <= max_states)
      {
        int target = (max_states + min_states) / 2;
        auto next = dtba_sat_synthetize(prev ? prev : a, target, state_based);
        if (!next)
          {
            min_states = target + 1;
          }
        else
          {
            prev = next;
            max_states = next->num_states() - 1;
          }
      }
    return prev;
  }
}
