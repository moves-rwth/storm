// -*- coding: utf-8 -*-
// Copyright (C) 2008, 2011-2017, 2020 Laboratoire de Recherche et
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

#pragma once

#include <spot/twa/twa.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <iosfwd>
#include <spot/misc/formater.hh>

namespace spot
{

  /// \addtogroup twa_misc
  /// @{

  struct SPOT_API twa_statistics
  {
    unsigned edges;
    unsigned states;

    twa_statistics() { edges = 0; states = 0; }
    std::ostream& dump(std::ostream& out) const;
  };

  struct SPOT_API twa_sub_statistics: public twa_statistics
  {
    unsigned long long transitions;

    twa_sub_statistics() { transitions = 0; }
    std::ostream& dump(std::ostream& out) const;
  };

  /// \brief Compute statistics for an automaton.
  SPOT_API twa_statistics stats_reachable(const const_twa_ptr& g);
  /// \brief Compute sub statistics for an automaton.
  SPOT_API twa_sub_statistics sub_stats_reachable(const const_twa_ptr& g);


  class SPOT_API printable_formula: public printable_value<formula>
  {
  public:
    printable_formula&
    operator=(formula new_val)
    {
      val_ = new_val;
      return *this;
    }

    virtual void
    print(std::ostream& os, const char*) const override;
  };

  class SPOT_API printable_acc_cond final: public spot::printable
  {
    acc_cond val_;
  public:
    printable_acc_cond&
    operator=(const acc_cond& new_val)
    {
      val_ = new_val;
      return *this;
    }

    void print(std::ostream& os, const char* pos) const override;
  };

  class SPOT_API printable_scc_info final:
    public spot::printable
  {
    std::unique_ptr<scc_info> val_;
  public:
    void automaton(const const_twa_graph_ptr& aut)
    {
      val_ = std::make_unique<scc_info>(aut);
    }

    void reset()
    {
      val_ = nullptr;
    }

    void print(std::ostream& os, const char* pos) const override;
  };

  /// \brief prints various statistics about a TGBA
  ///
  /// This object can be configured to display various statistics
  /// about a TGBA.  Some %-sequence of characters are interpreted in
  /// the format string, and replaced by the corresponding statistics.
  class SPOT_API stat_printer: protected formater
  {
  public:
    stat_printer(std::ostream& os, const char* format);

    /// \brief print the configured statistics.
    ///
    /// The \a f argument is not needed if the Formula does not need
    /// to be output, and so is \a run_time).
    std::ostream&
      print(const const_twa_graph_ptr& aut, formula f = nullptr);

  private:
    const char* format_;

    printable_formula form_;
    printable_value<unsigned> states_;
    printable_value<unsigned> edges_;
    printable_value<unsigned long long> trans_;
    printable_value<unsigned> acc_;
    printable_scc_info scc_;
    printable_value<unsigned> nondetstates_;
    printable_value<unsigned> deterministic_;
    printable_value<unsigned> complete_;
    printable_acc_cond gen_acc_;
  };

  /// @}
}
