// -*- coding: utf-8 -*-
// Copyright (C) 2013-2019 Laboratoire de Recherche et Développement
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
#include <fstream>
#include <set>
#include <cassert>
#include <spot/misc/escape.hh>
#include <spot/priv/satcommon.hh>
#include <spot/twa/bddprint.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/stats.hh>


#if DEBUG_CMN
#define print_debug std::cerr
#else
#define print_debug while (0) std::cout
#endif

namespace spot
{
  void
  vars_helper::init(unsigned size_src, unsigned size_cond, unsigned size_dst,
      unsigned size_nacc, unsigned size_path, bool state_based, bool dtbasat)
  {
    print_debug << "init(" << size_src << ',' << size_cond << ',' << size_dst
      << ',' << size_nacc << ','  << size_path << ',' << state_based << ")\n";

    size_src_ = size_src;
    size_cond_ = size_cond;
    size_dst_ = size_dst;
    size_nacc_ = size_nacc;
    size_path_ = size_path;
    state_based_ = state_based;
    dtbasat_ = dtbasat;

    sn_mult_ = size_src * size_nacc;
    cd_mult_ = size_cond * size_dst;
    dn_mult_ = size_dst * size_nacc;
    sd_mult_ = size_src * size_dst;
    dr_mult_ = size_dst * 2;
    sdr_mult_ = sd_mult_ * 2;
    cdn_mult_ = size_cond * size_dst * size_nacc;
    scd_mult_ = size_src * size_cond * size_dst;
    psd_mult_ = size_path * size_src * size_dst;
    scdn_mult_ = scd_mult_ * size_nacc;
    scdnp_mult_ = scdn_mult_ * size_path;

    assert(scdnp_mult_ != 0);
  }

  void
  vars_helper::declare_all_vars(int& min_t)
  {
    min_t_ = min_t;
    min_ta_ = min_t_ + scd_mult_;
    if (!state_based_)
      min_p_ = min_ta_ + scdn_mult_;
    else
      min_p_ = min_ta_ + sn_mult_;

    if (!dtbasat_)
      max_p_ = min_p_ + psd_mult_ - 1;
    else
      max_p_ = min_p_ + psd_mult_ * 2 - 1;

    print_debug << "declare_all_trans(" << min_t << ") --> min_t_<"
      << min_t_ << ">, min_ta_<" << min_ta_ << ">, min_p<"
      << min_p_ << ">, max_p<" << max_p_ << ">\n";

    // Update satdict.nvars.
    // max_p_ - 1 was added after noticing that in some cases in dtbasat, the
    // last variable is not used and some sat solver can complain about the
    // wong number of variable in cnf mode. No worries, if it turns out to be
    // used somewhere, it will be taken into account.
    min_t = dtbasat_ ? max_p_ - 1 : max_p_;
    assert(min_t != min_t_);
  }

  std::string
  vars_helper::format_t(bdd_dict_ptr& debug_dict, unsigned src, bdd& cond,
                        unsigned dst)
  {
    std::ostringstream buffer;
    buffer << '<' << src << ',' << bdd_format_formula(debug_dict, cond)
      << ',' << dst << '>';
    return buffer.str();
  }

  std::string
  vars_helper::format_ta(bdd_dict_ptr& debug_dict,
                         unsigned src, bdd& cond, unsigned dst,
                         const acc_cond::mark_t& acc)
  {
    std::ostringstream buffer;
    buffer << '<' << src << ',' << bdd_format_formula(debug_dict, cond)
           << ',' << acc << ',' << dst << '>';
    return buffer.str();
  }

  std::string
  vars_helper::format_p(unsigned src_cand, unsigned src_ref,
                        unsigned dst_cand, unsigned dst_ref,
                        acc_cond::mark_t acc_cand, acc_cond::mark_t acc_ref)
  {
    std::ostringstream buffer;
    buffer << '<' << src_cand << ',' << src_ref << ',' << dst_cand << ','
           << dst_ref << ", " << acc_cand << ", "
           << acc_ref << '>';
    return buffer.str();
  }

  std::string
  vars_helper::format_p(unsigned src_cand, unsigned src_ref, unsigned dst_cand,
                        unsigned dst_ref)
  {
    std::ostringstream buffer;
    buffer << '<' << src_cand << ',' << src_ref;
    if (src_cand == dst_cand && src_ref == dst_ref)
      buffer << '>';
    else
      buffer << ',' << dst_cand << ',' << dst_ref << '>';
    return buffer.str();
  }

  static std::string satlog_filename;

  void
  set_satlog_filename(const std::string& filename)
  {
    satlog_filename = filename;
  }

  void
  print_log(timer_map& t,
            int input_state_number, int target_state_number,
            const twa_graph_ptr& res, const satsolver& solver)
  {
    // Always copy the environment variable into a static string,
    // so that we (1) look it up once, but (2) won't crash if the
    // environment is changed.
    static std::string envlog = []()
      {
        auto s = getenv("SPOT_SATLOG");
        return s ? s : "";
      }();
    const std::string log = satlog_filename.empty() ? envlog : satlog_filename;
    if (log.empty())
      return;

    std::ofstream out(log, std::ios_base::ate | std::ios_base::app);
    out.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    if (out.tellp() == 0)
      out <<
        ("input.states,target.states,reachable.states,edges,transitions,"
         "variables,clauses,enc.user,enc.sys,sat.user,sat.sys,automaton\n");

    const timer& te = t.timer("encode");
    const timer& ts = t.timer("solve");
    out << input_state_number << ',' << target_state_number << ',';
    if (res)
      {
        twa_sub_statistics st = sub_stats_reachable(res);
        out << st.states << ',' << st.edges << ',' << st.transitions;
      }
    else
      {
        out << ",,";
      }
    std::pair<int, int> s = solver.stats();
    out << ',' << s.first << ',' << s.second << ','
        << te.utime() + te.cutime() << ','
        << te.stime() + te.cstime() << ','
        << ts.utime() + ts.cutime() << ','
        << ts.stime() + ts.cstime() << ',';
    if (res)
      {
        std::ostringstream f;
        print_hoa(f, res, "l");
        escape_rfc4180(out << '"', f.str()) << '"';
      }
    out << std::endl;
  }

  int
  get_number_of_distinct_vals(std::vector<unsigned> v)
  {
    std::set<unsigned> distinct;
    for (auto it = v.begin(); it != v.end(); ++it)
      distinct.insert(*it);
    return distinct.size();
  }
}
