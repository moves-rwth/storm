// -*- coding: utf-8 -*-
// Copyright (C) 2013-2020 Laboratoire de Recherche
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
#include <spot/misc/optionmap.hh>
#include <spot/misc/satsolver.hh>
#include <spot/misc/timer.hh>
#include <spot/priv/satcommon.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/complete.hh>
#include <spot/twaalgos/dtbasat.hh>
#include <spot/twaalgos/dtwasat.hh>
#include <spot/twaalgos/isdet.hh>
#include <spot/twaalgos/langmap.hh>
#include <spot/twaalgos/postproc.hh>
#include <spot/twaalgos/sbacc.hh>
#include <spot/twaalgos/sccfilter.hh>
#include <spot/twaalgos/sccinfo.hh>

// If you set the SPOT_TMPKEEP environment variable the temporary
// file used to communicate with the sat solver will be left in
// the current directory.
//
// Additionally, if the following TRACE macro is set to 1, the CNF
// file will be output with a comment before each clause, and an
// additional output file (dtwa-sat.dbg) will be created with a list
// of all positive variables in the result and their meaning.

#define TRACE 0

#if TRACE
#define dout out << "c "
#define cnf_comment(...)  solver.comment(__VA_ARGS__)
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
    static bdd_dict_ptr debug_dict = nullptr;

    struct path
    {
      unsigned src_ref;
      unsigned dst_ref;
      acc_cond::mark_t acc_cand;
      acc_cond::mark_t acc_ref;

      path(unsigned src_ref)
        : src_ref(src_ref), dst_ref(src_ref),
          acc_cand({}), acc_ref({})
      {
      }

     path(unsigned src_ref, unsigned dst_ref,
           acc_cond::mark_t acc_cand, acc_cond::mark_t acc_ref)
        : src_ref(src_ref), dst_ref(dst_ref),
          acc_cand(acc_cand), acc_ref(acc_ref)
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
        if (this->acc_ref < other.acc_ref)
          return true;
        if (this->acc_ref > other.acc_ref)
          return false;
        if (this->acc_cand < other.acc_cand)
          return true;
        if (this->acc_cand > other.acc_cand)
          return false;
        return false;
      }

    };

    // If the DNF is
    //  Fin(1)&Fin(2)&Inf(3) | Fin(0)&Inf(3) | Fin(4)&Inf(5)&Inf(6)
    // this returns the following map:
    //  {3} => [{1,2} {0}]
    //  {5} => [{4}]
    //  {6} => [{4}]
    // We use that do detect (and disallow) what we call "silly histories",
    // i.e., transitions or histories labeled by sets such as
    // {3,1,0}, that have no way to be satisfied.  So whenever we see
    // such history in a path, we actually map it to {1,0} instead,
    // which is enough to remember that this history is not satisfiable.
    // We also forbid any transition from being labeled by {3,1,0}.
    typedef std::map<unsigned, std::vector<acc_cond::mark_t>> trimming_map;
    static trimming_map
    split_dnf_acc_by_inf(const acc_cond::acc_code& input_acc)
    {
      trimming_map res;
      auto acc = input_acc.to_dnf();
      auto pos = &acc.back();
      if (pos->sub.op == acc_cond::acc_op::Or)
        --pos;
      acc_cond::mark_t all_fin = {};
      auto start = &acc.front();
      while (pos > start)
        {
          if (pos->sub.op == acc_cond::acc_op::Fin)
            {
              // We have only a Fin term, without Inf.
              // There is nothing to do about it.
              pos -= pos->sub.size + 1;
            }
          else
            {
              // We have a conjunction of Fin and Inf sets.
              auto end = pos - pos->sub.size - 1;
              acc_cond::mark_t fin = {};
              acc_cond::mark_t inf = {};
              while (pos > end)
                {
                  switch (pos->sub.op)
                    {
                    case acc_cond::acc_op::And:
                      --pos;
                      break;
                    case acc_cond::acc_op::Fin:
                      fin |= pos[-1].mark;
                      assert(pos[-1].mark.is_singleton());
                      pos -= 2;
                      break;
                    case acc_cond::acc_op::Inf:
                      inf |= pos[-1].mark;
                      pos -= 2;
                      break;
                    case acc_cond::acc_op::FinNeg:
                    case acc_cond::acc_op::InfNeg:
                    case acc_cond::acc_op::Or:
                      SPOT_UNREACHABLE();
                      break;
                    }
                }
              assert(pos == end);

              all_fin |= fin;
              for (unsigned i: inf.sets())
                if (fin)
                  {
                    res[i].emplace_back(fin);
                  }
                else
                  {
                    // Make sure the empty set is always the first one.
                    res[i].clear();
                    res[i].emplace_back(fin);
                  }
            }
        }
      // Remove entries that are necessarily false because they
      // contain an emptyset, or entries that also appear as Fin
      // somewhere in the acceptance.
      auto i = res.begin();
      while (i != res.end())
        {
          if (all_fin.has(i->first) || !i->second[0])
            i = res.erase(i);
          else
            ++i;
        }

      return res;
    }

    struct dict
    {
      dict(const const_twa_ptr& a)
        : aut(a)
      {
      }

      const_twa_ptr aut;

      vars_helper helper;
      std::vector<bdd> alpha_vect;
      std::map<path, unsigned> path_map;
      std::map<bdd, unsigned, bdd_less_than> alpha_map;

      int nvars = 0;
      unsigned int ref_size;
      unsigned int cand_size;
      unsigned int cand_nacc;
      acc_cond::acc_code cand_acc;

      std::vector<acc_cond::mark_t> all_cand_acc;
      std::vector<acc_cond::mark_t> all_ref_acc;
      // Markings that make no sense and that we do not want to see in
      // the candidate.  See comment above split_dnf_acc_by_inf().
      std::vector<acc_cond::mark_t> all_silly_cand_acc;

      std::vector<bool> is_weak_scc;
      std::vector<acc_cond::mark_t> scc_marks;

      acc_cond cacc;
      trimming_map ref_inf_trim_map;
      trimming_map cand_inf_trim_map;

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
      transacc(unsigned src, unsigned cond, unsigned dst, unsigned nacc)
      {
        return helper.get_ta(src, cond, dst, nacc);
      }

      int
      transacc(unsigned src, bdd& cond, unsigned dst, unsigned nacc)
      {
#if TRACE
        try
        {
          return helper.get_ta(src, alpha_map.at(cond), dst, nacc);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "label of transacc " << fmt_ta(src, cond, dst, nacc)
            << " not found.\n";
          throw c;
        }
#else
        return helper.get_ta(src, alpha_map[cond], dst, nacc);
#endif
      }

      int
      pathid(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
          unsigned dst_ref, acc_cond::mark_t acc_cand = {},
          acc_cond::mark_t acc_ref = {})
      {
#if TRACE
        try
        {
          return helper.get_p(
              path_map.at(path(src_ref, dst_ref, acc_cand, acc_ref)),
              src_cand, dst_cand);
        }
        catch (const std::out_of_range& c)
        {
          std::cerr << "path(" << src_ref << ',' << dst_ref << ",...) of pathid"
            << ' '
            << fmt_p(src_cand, src_ref, dst_cand, dst_ref, acc_cand, acc_ref)
            << " not found.\n";
          throw c;
        }
#else
        return helper.get_p(
            path_map[path(src_ref, dst_ref, acc_cand, acc_ref)],
            src_cand, dst_cand);
#endif
      }

#if TRACE
      int
      pathid(unsigned path, unsigned src_cand, unsigned dst_cand)
      {
        return helper.get_p(path, src_cand, dst_cand);
      }
#endif

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
      fmt_ta(unsigned src, bdd& cond, unsigned dst, unsigned nacc)
      {
        return helper.format_ta(debug_dict, src, cond, dst, cacc.mark(nacc));
      }

      std::string
      fmt_ta(unsigned src, unsigned cond, unsigned dst, unsigned nacc)
      {
        return helper.format_ta(debug_dict, src,
                                alpha_vect[cond], dst, cacc.mark(nacc));
      }

      std::string
      fmt_p(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
          unsigned dst_ref, acc_cond::mark_t acc_cand = {},
          acc_cond::mark_t acc_ref = {})
      {
        return helper.format_p(src_cand, src_ref, dst_cand, dst_ref,
                               acc_cand, acc_ref);
      }

      acc_cond::mark_t
      inf_trim(acc_cond::mark_t m, trimming_map& tm)
      {
        for (auto& s: tm)
          {
            unsigned inf = s.first;
            if (m.has(inf))
              {
                bool remove = true;
                for (auto fin: s.second)
                  if (!(m & fin))
                    {
                      remove = false;
                      break;
                    }
                if (remove)
                  m.clear(inf);
              }
          }
        return m;
      }

      acc_cond::mark_t
      ref_inf_trim(acc_cond::mark_t m)
      {
        return inf_trim(m, ref_inf_trim_map);
      }

      acc_cond::mark_t
      cand_inf_trim(acc_cond::mark_t m)
      {
        return inf_trim(m, cand_inf_trim_map);
      }
    };


    void declare_vars(const const_twa_graph_ptr& aut,
                          dict& d, bdd ap, bool state_based, scc_info& sm)
    {
      d.is_weak_scc = sm.weak_sccs();
      unsigned scccount = sm.scc_count();
      {
        auto tmp = sm.marks();
        d.scc_marks.reserve(scccount);
        for (auto& v: tmp)
          {
            acc_cond::mark_t m = {};
            for (auto i: v)
              m |= i;
            d.scc_marks.emplace_back(m);
          }
      }

      d.cacc.add_sets(d.cand_nacc);
      d.cacc.set_acceptance(d.cand_acc);

      // If the acceptance conditions use both Fin and Inf primitives,
      // we may have some silly history configurations to ignore.
      if (aut->acc().uses_fin_acceptance())
        d.ref_inf_trim_map = split_dnf_acc_by_inf(aut->get_acceptance());
      if (d.cacc.uses_fin_acceptance())
        d.cand_inf_trim_map = split_dnf_acc_by_inf(d.cand_acc);

      bdd_dict_ptr bd = aut->get_dict();
      d.all_cand_acc.emplace_back(acc_cond::mark_t({}));
      for (unsigned n = 0; n < d.cand_nacc; ++n)
        {
          auto c = d.cacc.mark(n);

          size_t ss = d.all_silly_cand_acc.size();
          for (size_t i = 0; i < ss; ++i)
            d.all_silly_cand_acc.emplace_back(d.all_silly_cand_acc[i] | c);

          size_t s = d.all_cand_acc.size();
          for (size_t i = 0; i < s; ++i)
            {
              acc_cond::mark_t m = d.all_cand_acc[i] | c;
              if (d.cand_inf_trim(m) == m)
                d.all_cand_acc.emplace_back(m);
              else
                d.all_silly_cand_acc.emplace_back(m);
            }
        }

      d.all_ref_acc.emplace_back(acc_cond::mark_t({}));
      unsigned ref_nacc = aut->num_sets();
      for (unsigned n = 0; n < ref_nacc; ++n)
        {
          auto c = aut->acc().mark(n);
          size_t s = d.all_ref_acc.size();
          for (size_t i = 0; i < s; ++i)
            {
              acc_cond::mark_t m = d.all_ref_acc[i] | c;
              if (d.ref_inf_trim(m) != m)
                continue;
              d.all_ref_acc.emplace_back(m);
            }
        }

      d.ref_size = aut->num_states();

      if (d.cand_size == -1U)
        for (unsigned i = 0; i < d.ref_size; ++i)
          if (sm.reachable_state(i))
            ++d.cand_size;      // Note that we start from -1U the
                                // cand_size is one less than the
                                // number of reachable states.

      // In order to optimize memory usage, src_cand & dst_cand have been
      // removed from path struct (the reasons are: they were no optimization
      // on them and their values are known from the beginning).
      //
      // However, since some optimizations are based on the following i, k, f,
      // refhist, it is necessary to associate to each path constructed,
      // an ID number.
      //
      // Given this ID, src_cand, dst_cand, the corresponding litteral can be
      // retrived thanks to get_prc(...) a vars_helper's method.
      unsigned path_size = 0;
      for (unsigned i = 0; i < d.ref_size; ++i)
        {
          if (!sm.reachable_state(i))
            continue;
          unsigned i_scc = sm.scc_of(i);
          bool is_weak = d.is_weak_scc[i_scc];
          for (unsigned k = 0; k < d.ref_size; ++k)
            {
              if (!sm.reachable_state(k))
                continue;
              if (sm.scc_of(k) != i_scc)
                continue;
              size_t sfp = is_weak ? 1 : d.all_ref_acc.size();
              acc_cond::mark_t sccmarks = d.scc_marks[i_scc];
              for (size_t fp = 0; fp < sfp; ++fp)
                {
                  auto refhist = d.all_ref_acc[fp];
                  // refhist cannot have more sets than used in the SCC.
                  if (!is_weak && (sccmarks & refhist) != refhist)
                    continue;

                  size_t sf = d.all_cand_acc.size();
                  for (size_t f = 0; f < sf; ++f)
                    {
                      path p(i, k, d.all_cand_acc[f], refhist);
                      d.path_map[p] = path_size++;
                    }
                }
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
      // false: means dtwasat, i-e not dtbasat.
      d.helper.init(d.cand_size, d.alpha_vect.size(), d.cand_size,
          d.cand_nacc, path_size, state_based, false);

      // Based on all previous informations, helper knows all litterals.
      d.helper.declare_all_vars(++d.nvars);
    }

    typedef std::pair<int, int> sat_stats;

    static
    sat_stats dtwa_to_sat(satsolver& solver,
                          const_twa_graph_ptr ref,
                          dict& d,
                          bool state_based,
                          bool colored)
    {
#if TRACE
      debug_dict = ref->get_dict();
#endif
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

      scc_info sm(ref, scc_info_options::TRACK_STATES_IF_FIN_USED);
      sm.determine_unknown_acceptance();

      // Number all the SAT variables we may need.
      declare_vars(ref, d, ap, state_based, sm);

      // Store alphabet size once for all.
      unsigned alpha_size = d.alpha_vect.size();

      // Tell the satsolver the number of variables.
      solver.adjust_nvars(d.nvars);

      // Empty automaton is impossible.
      assert(d.cand_size > 0);

#if TRACE
      solver.comment("d.ref_size:", d.ref_size, '\n');
      solver.comment("d.cand_size:", d.cand_size, '\n');
#endif
      auto& racc = ref->acc();

      cnf_comment("symmetry-breaking clauses\n");
      int j = 0;
      for (unsigned l = 0; l < alpha_size; ++l, ++j)
        for (unsigned i = 0; i < d.cand_size - 1; ++i)
          for (unsigned k = i * nap + j + 2; k < d.cand_size; ++k)
            {
              cnf_comment("¬", d.fmt_t(i, l, k), '\n');
              solver.add({-d.transid(i, l, k), 0});
            }

      if (!solver.get_nb_clauses())
        cnf_comment("(none)\n");

      cnf_comment("(8) the candidate automaton is complete\n");
      for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
        for (unsigned l = 0; l < alpha_size; ++l)
          {
#if TRACE
            solver.comment("");
            for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
              {
                solver.comment_rec(d.fmt_t(q1, l, q2), "δ");
                if (q2 != d.cand_size)
                  solver.comment_rec(" ∨ ");
              }
            solver.comment_rec('\n');
#endif
            for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
              solver.add(d.transid(q1, l, q2));
            solver.add(0);
          }

      cnf_comment("(9) the initial state is reachable\n");
      {
        unsigned init = ref->get_init_state_number();
        cnf_comment(d.fmt_p(0, init, 0, init), '\n');
        solver.add({d.pathid(0, init, 0, init), 0});
      }

      if (colored)
        {
          unsigned nacc = d.cand_nacc;
          cnf_comment("transitions belong to exactly one of the", nacc,
                      "acceptance sets\n");
          for (unsigned l = 0; l < alpha_size; ++l)
            for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
              for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                {
                  for (unsigned i = 0; i < nacc; ++i)
                    for (unsigned j = 0; j < nacc; ++j)
                      if (i != j)
                        solver.add(
                            {-d.transacc(q1, l, q2, i),
                             -d.transacc(q1, l, q2, j),
                             0});
                  for (unsigned i = 0; i < nacc; ++i)
                    solver.add(d.transacc(q1, l, q2, i));
                  solver.add(0);
                }
        }

      if (!d.all_silly_cand_acc.empty())
        {
          cnf_comment("no transition with silly acceptance\n");
          for (unsigned l = 0; l < alpha_size; ++l)
            for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
              for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                for (auto& s: d.all_silly_cand_acc)
                  {
                    cnf_comment("no (", q1, ',',
                        bdd_format_formula(debug_dict, d.alpha_vect[l]),
                        ',', s, ',', q2, ")\n");
                    for (unsigned v: s.sets())
                      {
                        int tai = d.transacc(q1, l, q2, v);
                        assert(tai != 0);
                        solver.add(-tai);
                      }
                    for (unsigned v: d.cacc.comp(s).sets())
                      {
                        int tai = d.transacc(q1, l, q2, v);
                        assert(tai != 0);
                        solver.add(tai);
                      }
                    solver.add(0);
                  }
        }

      for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
        for (unsigned q1p = 0; q1p < d.ref_size; ++q1p)
          {
            if (!sm.reachable_state(q1p))
              continue;
            cnf_comment("(10) augmenting paths based on Cand[", q1,
                        "] and Ref[", q1p, "]\n");

            int p1id = d.pathid(q1, q1p, q1, q1p);
            for (auto& tr: ref->out(q1p))
              {
                unsigned dp = tr.dst;
                bdd all = tr.cond;
                while (all != bddfalse)
                  {
                    bdd s = bdd_satoneset(all, ap, bddfalse);
                    all -= s;

                    for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                      {
                        int succ = d.pathid(q2, dp, q2, dp);
                        if (p1id == succ)
                          continue;

                        cnf_comment(d.fmt_p(q1, q1p, q1, q1p), " ∧ ",
                                    d.fmt_t(q1, s, q2), "δ → ",
                                    d.fmt_p(q2, dp, q2, dp), '\n');
                        solver.add({-p1id, -d.transid(q1, s, q2), succ, 0});
                      }
                  }
              }
          }

      // construction of constraints (11,12,13)
      for (unsigned q1p = 0; q1p < d.ref_size; ++q1p)
        {
          if (!sm.reachable_state(q1p))
            continue;
          unsigned q1p_scc = sm.scc_of(q1p);
          for (unsigned q2p = 0; q2p < d.ref_size; ++q2p)
            {
              if (!sm.reachable_state(q2p))
                continue;
              // We are only interested in transition that can form a
              // cycle, so they must belong to the same SCC.
              if (sm.scc_of(q2p) != q1p_scc)
                continue;
              bool is_weak = d.is_weak_scc[q1p_scc];
              bool is_rej = sm.is_rejecting_scc(q1p_scc);

              for (unsigned q1 = 0; q1 < d.cand_size; ++q1)
                for (unsigned q2 = 0; q2 < d.cand_size; ++q2)
                  {
                    size_t sf = d.all_cand_acc.size();
                    size_t sfp = is_weak ? 1 : d.all_ref_acc.size();
                    acc_cond::mark_t sccmarks = d.scc_marks[q1p_scc];

                    for (size_t f = 0; f < sf; ++f)
                      for (size_t fp = 0; fp < sfp; ++fp)
                        {
                          auto refhist = d.all_ref_acc[fp];
                          // refhist cannot have more sets than used in the SCC
                          if (!is_weak && (sccmarks & refhist) != refhist)
                            continue;

                          acc_cond::mark_t candhist_p = d.all_cand_acc[f];
                          std::string f_p =
                              d.fmt_p(q1, q1p, q2, q2p, candhist_p, refhist);
                          cnf_comment("(11&12&13) paths from ", f_p, '\n');
                          int pid =
                            d.pathid(q1, q1p, q2, q2p, candhist_p, refhist);
                          for (auto& tr: ref->out(q2p))
                            {
                              unsigned dp = tr.dst;
                              // Skip destinations not in the SCC.
                              if (sm.scc_of(dp) != q1p_scc)
                                continue;

                              for (unsigned q3 = 0; q3 < d.cand_size; ++q3)
                                {
                                  bdd all = tr.cond;
                                  acc_cond::mark_t curacc = tr.acc;
                                  while (all != bddfalse)
                                    {
                                      bdd l = bdd_satoneset(all, ap, bddfalse);
                                      all -= l;

                                      int ti = d.transid(q2, l, q3);
                                      if (dp == q1p && q3 == q1) // (11,12) loop
                                        {
                                          bool rejloop =
                                            (is_rej ||
                                             !racc.accepting
                                             (curacc | d.all_ref_acc[fp]));

                                          auto missing = d.cand_acc
                                            .missing(candhist_p, !rejloop);

                                          for (auto& v: missing)
                                            {
#if TRACE
                                              solver.comment((rejloop ?
                                                       "(11) " : "(12) "), f_p,
                                                  " ∧ ", d.fmt_t(q2, l, q3),
                                                  "δ → (");
                                              const char* orsep = "";
                                              for (int s: v)
                                                {
                                                  if (s < 0)
                                                    solver.comment_rec(orsep,
                                                      "¬", d.fmt_ta(
                                                        q2, l, q1, -s - 1));
                                                  else
                                                    solver.comment_rec(orsep,
                                                        d.fmt_ta(q2, l, q1, s));
                                                  solver.comment_rec("FC");
                                                  orsep = " ∨ ";
                                                }
                                              solver.comment_rec(")\n");
#endif // TRACE
                                              solver.add({-pid, -ti});
                                              for (int s: v)
                                                if (s < 0)
                                                  {
                                                    int tai =
                                                      d.transacc(q2, l, q1,
                                                          -s - 1);
                                                    assert(tai != 0);
                                                    solver.add(-tai);
                                                  }
                                                else
                                                  {
                                                    int tai =
                                                      d.transacc(q2, l, q1, s);
                                                    assert(tai != 0);
                                                    solver.add(tai);
                                                  }
                                              solver.add(0);
                                            }
                                        }
                                      // (13) augmenting paths (always).
                                      {
                                        size_t sf = d.all_cand_acc.size();
                                        for (size_t f = 0; f < sf; ++f)
                                          {
                                            acc_cond::mark_t f2 =
                                              d.cand_inf_trim
                                              (candhist_p |
                                               d.all_cand_acc[f]);
                                            acc_cond::mark_t f2p = {};
                                            if (!is_weak)
                                              f2p = d.ref_inf_trim(refhist |
                                                                   curacc);
                                            int p2id = d.pathid(
                                                q1, q1p, q3, dp, f2, f2p);
                                            if (pid == p2id)
                                              continue;
#if TRACE
                                            solver.comment("(13) ", f_p, " ∧ ",
                                                 d.fmt_t(q1, l, q3), "δ ");

                                            auto biga_ = d.all_cand_acc[f];
                                            for (unsigned m = 0;
                                                 m < d.cand_nacc; ++m)
                                              {
                                                const char* not_ = "¬";
                                                if (biga_.has(m))
                                                  not_ = "";
                                                solver.comment_rec(" ∧ ", not_,
                                                    d.fmt_ta(q2, l, q3, m),
                                                    "FC");
                                              }
                                            solver.comment_rec(" → ",
                                                d.fmt_p(q1, q1p, q3, dp, f2,
                                                  f2p), '\n');
#endif
                                            solver.add({-pid, -ti});
                                            auto biga = d.all_cand_acc[f];
                                            for (unsigned m = 0;
                                                 m < d.cand_nacc; ++m)
                                              {
                                                int tai =
                                                  d.transacc(q2, l, q3, m);
                                                if (biga.has(m))
                                                  tai = -tai;
                                                solver.add(tai);
                                              }
                                            solver.add({p2id, 0});
                                          }
                                      }
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
      trace << "sat_build(..., state_based=" << state_based << ")\n";

      auto autdict = aut->get_dict();
      auto a = make_twa_graph(autdict);
      a->copy_ap_of(aut);
      if (state_based)
        a->prop_state_acc(true);
      a->prop_universal(true);
      a->set_acceptance(satdict.cand_nacc, satdict.cand_acc);
      a->new_states(satdict.cand_size);

#if TRACE
      std::fstream out("dtwa-sat.dbg",
                       std::ios_base::trunc | std::ios_base::out);
      out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
#endif
      std::map<int, acc_cond::mark_t> state_acc;
      std::set<src_cond> seen_trans;

      unsigned alpha_size = satdict.alpha_vect.size();
      for (unsigned i = 0; i < satdict.cand_size; ++i)
        for (unsigned j = 0; j < alpha_size; ++j)
          for (unsigned k = 0; k < satdict.cand_size; ++k)
          {
            if (solution[satdict.transid(i, j, k) - 1])
            {
              // Ignore unuseful transitions because of reduced cand_size.
              if (i >= satdict.cand_size)
                continue;

              // Skip (s,l,d2) if we have already seen some (s,l,d1).
              if (seen_trans.insert(src_cond(i, satdict.alpha_vect[j])).second)
              {
                acc_cond::mark_t acc = {};
                if (state_based)
                  {
                    auto tmp = state_acc.find(i);
                    if (tmp != state_acc.end())
                      acc = tmp->second;
                  }

                for (unsigned n = 0; n < satdict.cand_nacc; ++n)
                  if (solution[satdict.transacc(i, j, k, n) - 1])
                    acc |= satdict.cacc.mark(n);

                a->new_edge(i, k, satdict.alpha_vect[j], acc);
              }
            }
          }

#if TRACE
      dout << "--- transition variables ---\n";
      for (unsigned i = 0; i < satdict.cand_size; ++i)
        for (unsigned j = 0; j < alpha_size; ++j)
          for (unsigned k = 0; k < satdict.cand_size; ++k)
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
        dout << "In state_based mode, there is only 1 litteral for each "
          "combination of src and nacc, regardless of dst or cond!\n";
        for (unsigned i = 0; i < satdict.cand_size; ++i)
          for (unsigned j = 0; j < satdict.cand_nacc; ++j)
          {
            int var = satdict.transacc(i, 0, 0, j);
            std::string f_ta = satdict.fmt_ta(i, 0, 0, j);
            if (solution[var - 1])
              dout << ' ' << var << "\t " << f_ta << '\n';
            else
              dout << -var << "\t¬" << f_ta << '\n';
          }
      }
      else
        for (unsigned i = 0; i < satdict.cand_size; ++i)
          for (unsigned j = 0; j < alpha_size; ++j)
            for (unsigned k = 0; k < satdict.cand_size; ++k)
              for (unsigned l = 0; l < satdict.cand_nacc; ++l)
              {
                int var = satdict.transacc(i, j, k, l);
                std::string f_ta = satdict.fmt_ta(i, j, k, l);
                if (solution[var - 1])
                  dout << ' ' << var << "\t " << f_ta << '\n';
                else
                  dout << -var << "\t¬" << f_ta << '\n';
              }
      dout << "--- path_map variables ---\n";
      for (auto it = satdict.path_map.begin(); it != satdict.path_map.end();
          ++it)
        for (unsigned k = 0; k < satdict.cand_size; ++k)
          for (unsigned l = 0; l < satdict.cand_size; ++l)
          {
            int var = satdict.pathid(it->second, k, l);
            std::string f_p = satdict.fmt_p(k, it->first.src_ref, l,
                it->first.dst_ref, it->first.acc_cand, it->first.acc_ref);
            if (solution[var - 1])
              dout << ' ' << var << "\t " << f_p << '\n';
            else
              dout << -var << "\t¬" << f_p << '\n';
          }
#endif

      a->merge_edges();
      a->purge_unreachable_states();
      return a;
    }
  }

  twa_graph_ptr
  dtwa_sat_synthetize(const const_twa_graph_ptr& a,
                      unsigned target_acc_number,
                      const acc_cond::acc_code& target_acc,
                      int target_state_number,
                      bool state_based, bool colored)
  {
    if (!a->is_existential())
      throw std::runtime_error
        ("dtwa_sat_synthetize() does not support alternating automata");
    if (target_state_number == 0)
      return nullptr;
    trace << "dtwa_sat_synthetize(..., nacc = " << target_acc_number
          << ", acc = \"" << target_acc
          << "\", states = " << target_state_number
          << ", state_based = " << state_based << ")\n";

    dict d(a);
    d.cand_size = target_state_number;
    d.cand_nacc = target_acc_number;
    d.cand_acc = target_acc;

    satsolver solver;
    satsolver::solution_pair solution;

    timer_map t;
    t.start("encode");
    dtwa_to_sat(solver, a, d, state_based, colored);
    t.stop("encode");
    t.start("solve");
    solution = solver.get_solution();
    t.stop("solve");

    twa_graph_ptr res = nullptr;
    if (!solution.second.empty())
      res = sat_build(solution.second, d, a, state_based);

    print_log(t, a->num_states(), target_state_number, res, solver);

    trace << "dtwa_sat_synthetize(...) = " << res << '\n';
    return res;
  }


  namespace
  {
    // Chose a good reference automaton given two automata.
    //
    // The right automaton only is allowed to be null.  In that
    // case the left automaton is returned.
    //
    // The selection relies on the fact that the SAT encoding is
    // quadratic in the number of input states, and exponential in the
    // number of input sets.
    static const_twa_graph_ptr
    best_aut(const const_twa_graph_ptr& left,
             const const_twa_graph_ptr& right)
    {
      if (right == nullptr)
        return left;
      auto lstates = left->num_states();
      auto lsets = left->num_sets();
      auto rstates = right->num_states();
      auto rsets = right->num_sets();
      if (lstates <= rstates && lsets <= rsets)
        return left;
      if (lstates >= rstates && lsets >= rsets)
        return right;

      long long unsigned lw = (1ULL << lsets) * lstates * lstates;
      long long unsigned rw = (1ULL << rsets) * rstates * rstates;

      return lw <= rw ? left : right;
    }
  }

  static twa_graph_ptr
  dichotomy_dtwa_research(int max,
                          dict& d,
                          satsolver& solver,
                          const_twa_graph_ptr& prev,
                          bool state_based)
  {
    trace << "dichotomy_dtwa_research(...)\n";
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
      satsolver::solution_pair solution = solver.get_solution();
      t1.stop("solve");
      trace << (solution.second.empty() ? "UNSAT!\n" : "SAT\n");
      res = solution.second.empty() ? nullptr :
        sat_build(solution.second, d, prev, state_based);
      print_log(t1, prev->num_states(), d.cand_size - target, res, solver);
    }
    return res ? res : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtwa_sat_minimize_assume(const const_twa_graph_ptr& a,
                           unsigned target_acc_number,
                           const acc_cond::acc_code& target_acc,
                           bool state_based,
                           int max_states,
                           bool colored,
                           int sat_incr_steps)
  {
    if (sat_incr_steps < 0)
      throw std::runtime_error("with 'assume' algorithm, sat_incr_steps value "
                               "must be >= 0");

    const_twa_graph_ptr prev = a;
    dict d(prev);
    d.cand_size = (max_states < 0) ? prev->num_states() - 1 : max_states;
    d.cand_nacc = target_acc_number;
    d.cand_acc = target_acc;
    if (d.cand_size == 0)
      return nullptr;

    trace << "dtwa_sat_minimize_assume(..., nacc = " << target_acc_number
      << ", acc = \"" << target_acc << "\", states = " << d.cand_size
      << ", state_based = " << state_based << ")\n";
    trace << "sat_incr_steps: " << sat_incr_steps << '\n';

    twa_graph_ptr next = spot::make_twa_graph(spot::make_bdd_dict());
    while (next && d.cand_size > 0)
    {
      // Warns the satsolver of the number of assumptions.
      int n_assumptions = (int) d.cand_size <= sat_incr_steps ?
        d.cand_size - 1 : sat_incr_steps;
      trace << "number of assumptions: " << n_assumptions << '\n';
      satsolver solver;
      solver.set_nassumptions_vars(n_assumptions);

      // First iteration of classic solving.
      timer_map t1;
      t1.start("encode");
      dtwa_to_sat(solver, prev, d, state_based, colored);

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
        twa_graph_ptr res = dichotomy_dtwa_research(n_assumptions, d, solver,
                                                    prev, state_based);
        return res == a ? nullptr : res;
      }

      trace << "SAT, restarting from zero\n";
      next = solution.second.empty() ? nullptr :
        sat_build(solution.second, d, prev, state_based);
      print_log(t1, prev->num_states(),
                d.cand_size - n_assumptions, next, solver);

      if (next)
      {
        prev = next;
        trace << "prev has " << prev->num_states() << '\n';
        d = dict(prev);
        d.cand_size = prev->num_states() - 1;
        d.cand_nacc = target_acc_number;
        d.cand_acc = target_acc;
        if (d.cand_size == 0)
          next = nullptr;
      }

    }

    return prev == a ? nullptr : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtwa_sat_minimize_incr(const const_twa_graph_ptr& a,
                         unsigned target_acc_number,
                         const acc_cond::acc_code& target_acc,
                         bool state_based, int max_states,
                         bool colored, int sat_incr_steps)
  {
    const_twa_graph_ptr prev = a;

    dict d(prev);
    d.cand_size = (max_states < 0) ?
      prev->num_states() - 1 : max_states;
    d.cand_nacc = target_acc_number;
    d.cand_acc = target_acc;
    if (d.cand_size == 0)
      return nullptr;

    trace << "dtwa_sat_minimize_incr(..., nacc = " << target_acc_number
      << ", acc = \"" << target_acc << "\", states = " << d.cand_size
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
      dtwa_to_sat(solver, prev, d, state_based, colored);
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
        trace << "Encoding the deletion of state " << reach_states - 1 << '\n';
        cnf_comment("Next iteration:", reach_states - 1, "\n");
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
        solution = solver.get_solution();
        t2.stop("solve");
        next = solution.second.empty() ? nullptr :
          sat_build(solution.second, d, prev, state_based);
        print_log(t2, prev->num_states(), d.cand_size, next, solver);
      }

      if (next)
      {
        trace << "Starting from scratch\n";
        prev = next;
        d = dict(prev);
        d.cand_size = prev->num_states() - 1;
        d.cand_nacc = target_acc_number;
        d.cand_acc = target_acc;
        if (d.cand_size == 0)
          next = nullptr;
      }

    };

    return prev == a ? nullptr : std::const_pointer_cast<spot::twa_graph>(prev);
  }

  twa_graph_ptr
  dtwa_sat_minimize(const const_twa_graph_ptr& a,
                    unsigned target_acc_number,
                    const acc_cond::acc_code& target_acc,
                    bool state_based, int max_states,
                    bool colored)
  {
    int n_states = (max_states < 0) ? a->num_states() : max_states + 1;

    twa_graph_ptr prev = nullptr;
    for (;;)
      {
        auto src = best_aut(a, prev);
        auto next = dtwa_sat_synthetize(src, target_acc_number,
                                        target_acc, --n_states,
                                        state_based, colored);
        if (!next)
          return prev;
        else
          n_states = next->num_states();
        prev = next;
      }
    SPOT_UNREACHABLE();
  }

  twa_graph_ptr
  dtwa_sat_minimize_dichotomy(const const_twa_graph_ptr& a,
                              unsigned target_acc_number,
                              const acc_cond::acc_code& target_acc,
                              bool state_based, bool langmap,
                              int max_states, bool colored)
  {
    trace << "Dichotomy\n";
    if (max_states < 1)
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
        auto src = best_aut(a, prev);
        auto next = dtwa_sat_synthetize(src, target_acc_number,
                                        target_acc, target, state_based,
                                        colored);
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

  twa_graph_ptr
  sat_minimize(twa_graph_ptr a, const char* opt, bool state_based)
  {
    option_map om;
    auto err = om.parse_options(opt);
    if (err)
      {
        std::string e = "failed to parse option near ";
        e += err;
        throw std::runtime_error(e);
      }

    if (!is_deterministic(a))
      throw std::runtime_error
        ("SAT-based minimization only works with deterministic automata");

    int sat_incr = om.get("sat-incr", 0);
    int sat_incr_steps = om.get("sat-incr-steps", 0);
    bool sat_naive = om.get("sat-naive", 0);
    bool sat_langmap = om.get("sat-langmap", 0);
    int states = om.get("states", -1);
    int max_states = om.get("max-states", -1);
    std::string accstr = om.get_str("acc");
    bool colored = om.get("colored", 0);
    int preproc = om.get("preproc", 3);
    set_satlog_filename(om.get_str("log"));

    // Set default sat-incr-steps value if not provided and used.
    if (sat_incr == 1 && !sat_incr_steps) // Assume
      sat_incr_steps = 6;
    else if (sat_incr == 2 && !sat_incr_steps) // Incremental
      sat_incr_steps = 2;

    // No more om.get() below this.
    om.report_unused_options();

    // Assume we are going to use the input automaton acceptance...
    bool user_supplied_acc = false;
    acc_cond::acc_code target_acc = a->get_acceptance();
    int nacc = a->num_sets();

    if (accstr == "same")
      accstr.clear();
    // ... unless the user specified otherwise
    if (!accstr.empty())
      {
        user_supplied_acc = true;
        target_acc = acc_cond::acc_code(accstr.c_str());
        // Just in case we were given something like
        //  Fin(1) | Inf(3)
        // Rewrite it as
        //  Fin(0) | Inf(1)
        // without holes in the set numbers
        acc_cond::mark_t used = target_acc.used_sets();
        acc_cond a(used.max_set());
        target_acc = target_acc.strip(a.comp(used), true);
        nacc = used.count();
      }

    bool target_is_buchi = false;
    {
      acc_cond acccond(nacc);
      acccond.set_acceptance(target_acc);
      target_is_buchi = acccond.is_buchi();
    }

    if (preproc)
      {
        postprocessor post;
        auto sba = (state_based && a->prop_state_acc()) ?
          postprocessor::SBAcc : postprocessor::Any;
        post.set_pref(postprocessor::Deterministic | sba);
        post.set_type(postprocessor::Generic);
        postprocessor::optimization_level level;
        switch (preproc)
          {
          case 1:
            level = postprocessor::Low;
            break;
          case 2:
            level = postprocessor::Medium;
            break;
          case 3:
            level = postprocessor::High;
            break;
          default:
            throw
              std::runtime_error("preproc should be a value between 0 and 3.");
          }
        post.set_level(level);
        a = post.run(a, nullptr);
        // If we have WDBA, it is necessarily minimal because
        // postprocessor always run WDBA minimization in Deterministic
        // mode.  If the desired output is a Büchi automaton, or not
        // desired acceptance was specified, stop here.  There is not
        // point in minimizing a minimal automaton.
        if (a->prop_weak() && a->prop_universal()
            && (target_is_buchi || !user_supplied_acc))
          return a;
      }
    // We always complete ourself, and do not ask the above call to
    // postprocessor to do it, because that extra state would be
    // returned in the case of a WDBA.
    complete_here(a);

    if (states == -1 && max_states == -1)
      {
        if (state_based)
          max_states = sbacc(a)->num_states();
        else
          max_states = a->num_states();
        // If we have not user-supplied acceptance, the input
        // automaton is a valid one, so we start the search with one
        // less state.
        max_states -= !user_supplied_acc;
      }


    if (states == -1)
      {
        auto orig = a;
        if (!target_is_buchi || !a->acc().is_buchi() || colored)
        {
          if (sat_naive)
            a = dtwa_sat_minimize
              (a, nacc, target_acc, state_based, max_states, colored);

          else if (sat_incr == 1)
            a = dtwa_sat_minimize_assume(a, nacc, target_acc, state_based,
                max_states, colored, sat_incr_steps);

          else if (sat_incr == 2)
            a = dtwa_sat_minimize_incr(a, nacc, target_acc, state_based,
                max_states, colored, sat_incr_steps);

          else
            a = dtwa_sat_minimize_dichotomy
              (a, nacc, target_acc, state_based, sat_langmap, max_states,
               colored);
        }
        else
        {
          if (sat_naive)
            a = dtba_sat_minimize(a, state_based, max_states);

          else if (sat_incr == 1)
            a = dtba_sat_minimize_assume(a, state_based, max_states,
                                         sat_incr_steps);

          else if (sat_incr == 2)
            a = dtba_sat_minimize_incr(a, state_based, max_states,
                                       sat_incr_steps);

          else
            a = dtba_sat_minimize_dichotomy
              (a, state_based, sat_langmap, max_states);
        }

        if (!a && !user_supplied_acc)
          a = orig;
      }
    else
      {
        if (!target_is_buchi || !a->acc().is_buchi() || colored)
          a = dtwa_sat_synthetize(a, nacc, target_acc, states,
                                  state_based, colored);
        else
          a = dtba_sat_synthetize(a, states, state_based);
      }

    if (a)
      {
        if (state_based || colored)
          a = scc_filter_states(a);
        else
          a = scc_filter(a);
      }
    return a;
  }

}
