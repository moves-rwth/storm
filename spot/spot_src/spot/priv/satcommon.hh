// -*- coding: utf-8 -*-
// Copyright (C) 2013-2016, 2018, 2019 Laboratoire de Recherche et
// Développement de l'Epita.
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

#pragma once

#include <tuple>
#include <sstream>
#include <vector>
#include <spot/misc/bddlt.hh>
#include <spot/misc/satsolver.hh>
#include <spot/misc/timer.hh>
#include <spot/twa/twagraph.hh>

#define DEBUG_CMN 0

namespace spot
{
  struct src_cond
  {
    unsigned src;
    bdd cond;

    src_cond(unsigned src, bdd cond)
      : src(src), cond(cond)
    {
    }

    bool operator<(const src_cond& other) const
    {
      if (this->src < other.src)
        return true;
      if (this->src > other.src)
        return false;
      return this->cond.id() < other.cond.id();
    }

    bool operator==(const src_cond& other) const
    {
      return (this->src == other.src
              && this->cond.id() == other.cond.id());
    }
  };

  /// \brief Interface with satsolver's litterals.
  ///
  /// This class was created to fill the need to optimize memory storage in
  /// SAT-minimization.
  ///
  /// All this relies on the fact that almost everything about the automaton
  /// candidate is known in advance and most of the time, litteral's numbers
  /// are just incremented continually (they are continuous...).
  ///
  /// This class allows to handle variables by only manipulating indices.
  class vars_helper
  {
private:
    unsigned size_src_;
    unsigned size_cond_;
    unsigned size_dst_;
    unsigned size_nacc_;
    unsigned size_path_;
    bool state_based_;
    bool dtbasat_;
    int min_t_;
    int min_ta_;
    int min_p_;
    int max_p_;

    // Vars that will be precalculated.
    unsigned sn_mult_ = 0;    // src * nacc
    unsigned cd_mult_ = 0;    // cond * dst
    unsigned dn_mult_ = 0;    // dst * nacc
    unsigned sd_mult_ = 0;    // src * dst
    unsigned dr_mult_ = 0;    // dst * 2 --> used only in get_prc(...)
    unsigned sdr_mult_ = 0;   // src * dst * 2 --> used only in get_prc(...)
    unsigned scd_mult_ = 0;   // src * cond * dst
    unsigned cdn_mult_ = 0;   // cond * dst * nacc
    unsigned psd_mult_ = 0;   // path * src * dst
    unsigned scdn_mult_ = 0;  // src * cond * dst * nacc
    unsigned scdnp_mult_ = 0;  // src * cond * dst * nacc * path

public:
    vars_helper()
      : size_src_(0), size_cond_(0), size_dst_(0), size_nacc_(0), size_path_(0),
      state_based_(false), dtbasat_(false), min_t_(0), min_ta_(0), min_p_(0),
      max_p_(0)
    {
#if DEBUG_CMN
      std::cerr << "vars_helper() constructor called\n";
#endif
    }

    /// \brief Save all different sizes and precompute some values.
    void
    init(unsigned size_src, unsigned size_cond, unsigned size_dst,
        unsigned size_nacc, unsigned size_path, bool state_based,
        bool dtbasat);

    /// \brief Compute min_t litteral as well as min_ta, min_p and max_p.
    /// After this step, all litterals are known.
    void
    declare_all_vars(int& min_t);

    /// \brief Return the transition's litteral corresponding to parameters.
    inline int
    get_t(unsigned src, unsigned cond, unsigned dst) const
    {
#if DEBUG_CMN
      if (src >= size_src_ || cond >= size_cond_ || dst >= size_dst_)
      {
        std::ostringstream buffer;
        buffer << "bad arguments get_t(" << src << ',' << cond << ',' << dst
          << ")\n";
        throw std::runtime_error(buffer.str());
      }
      std::cerr << "get_t(" << src << ',' << cond << ',' << dst << ") = "
        << min_t_ + src * cd_mult_ + cond * size_dst_ + dst << '\n';
#endif
      return min_t_ + src * cd_mult_ + cond * size_dst_ + dst;
    }

    /// \brief Return the transition_acc's litteral corresponding to parameters.
    /// If (state_based), all outgoing transitions use the same acceptance
    /// variable. Therefore, for each combination (src, nacc) there is only one
    /// litteral.
    /// Note that with Büchi automata, there is only one nacc, thus, only one
    /// litteral for each src.
    inline int
    get_ta(unsigned src, unsigned cond, unsigned dst, unsigned nacc = 0) const
    {
#if DEBUG_CMN
      if (src >= size_src_ || cond >= size_cond_ || dst >= size_dst_
          || nacc >= size_nacc_)
      {
        std::stringstream buffer;
        buffer << "bad arguments get_ta(" << src << ',' << cond << ',' << dst
          << ',' << nacc  << ")\n";
        throw std::runtime_error(buffer.str());
      }
      int res = state_based_ ? min_ta_ + src * size_nacc_ + nacc
        : min_ta_ + src * cdn_mult_ + cond * dn_mult_ + (dst * size_nacc_)
        + nacc;
      std::cerr << "get_ta(" << src << ',' << cond << ',' << dst << ") = "
        << res << '\n';
#endif
      return state_based_ ? min_ta_ + src * size_nacc_ + nacc
        : min_ta_ + src * cdn_mult_ + cond * dn_mult_ + dst * size_nacc_ + nacc;
    }

    /// \brief Return the path's litteral corresponding to parameters.
    inline int
    get_p(unsigned path, unsigned src, unsigned dst) const
    {
#if DEBUG_CMN
      if (src >= size_src_ || path >= size_path_ || dst >= size_dst_)
      {
        std::stringstream buffer;
        buffer << "bad arguments get_p(" << path << ',' << src << ',' << dst
          << ")\n";
        throw std::runtime_error(buffer.str());
      }
      std::cerr << "get_p(" << path << ',' << src << ',' << dst << ") = "
        << min_p_ + path * sd_mult_ + src * size_dst_ + dst << '\n';
#endif
      assert(!dtbasat_);
      return min_p_ + path * sd_mult_ + src * size_dst_ + dst;
    }

    /// \brief Return the path's litteral corresponding to parameters.
    /// Argument ref serves to say whether it is a candidate or a reference
    /// litteral. false -> ref | true -> cand
    inline int
    get_prc(unsigned path, unsigned src, unsigned dst, bool cand) const
    {
#if DEBUG_CMN
      if (src >= size_src_ || path >= size_path_ || dst >= size_dst_)
      {
        std::stringstream buffer;
        buffer << "bad arguments get_prc(" << path << ',' << src << ',' << dst
          << ',' << cand << ")\n";
        throw std::runtime_error(buffer.str());
      }
      std::cerr << "get_prc(" << path << ',' << src << ',' << dst << ','
        << cand << ") = " << min_p_ + path * sdr_mult_ + src * dr_mult_ +
        dst * 2 + cand << '\n';
#endif
      assert(dtbasat_);
      return min_p_ + path * sdr_mult_ + src * dr_mult_ + dst * 2 + cand;
    }

    /// \brief Use this function to get a string representation of a transition.
    std::string
    format_t(bdd_dict_ptr& debug_dict, unsigned src, bdd& cond,
             unsigned dst);

    /// \brief Use this function to get a string representation of a transition
    /// acc.
    std::string
    format_ta(bdd_dict_ptr& debug_dict, unsigned src, bdd& cond, unsigned dst,
              const acc_cond::mark_t& acc);

    /// \brief Use this function to get a string representation of a path var.
    std::string
    format_p(unsigned src_cand, unsigned src_ref,
             unsigned dst_cand, unsigned dst_ref,
             acc_cond::mark_t acc_cand, acc_cond::mark_t acc_ref);

    /// \brief Use this function to get a string representation of a path var.
    std::string
    format_p(unsigned src_cand, unsigned src_ref,
             unsigned dst_cand, unsigned dst_ref);
  };

  /// \brief Give a filename to save the log of the SAT minimization.
  ///
  /// This has priority over the SPOT_SATLOG environment variable.
  /// Pass en empty string to reset it.
  void set_satlog_filename(const std::string& filename);

  /// \brief Prints a line in the SPOT_SATLOG file.
  void print_log(timer_map& t,
                 int input_state_number,
                 int target_state_number, const twa_graph_ptr& res,
                 const satsolver& solver);

  /// \brief Returns the number of distinct values containted in a vector.
  int
  get_number_of_distinct_vals(std::vector<unsigned> v);
}
