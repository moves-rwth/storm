// -*- coding: utf-8 -*-
// Copyright (C) 2013-2018 Laboratoire de Recherche et Développement
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

#pragma once

#include <spot/twaalgos/postproc.hh>
#include <spot/tl/simplify.hh>

namespace spot
{
  /// \ingroup twa_ltl
  /// \brief Translate an LTL formula into an optimized spot::tgba.
  ///
  /// This class implements a three-step translation:
  /// - syntactic simplification of the formula
  /// - translation of the formula into TGBA
  /// - postprocessing of the resulting TGBA to minimize it, or
  ///   turn it into the required form.
  ///
  /// Method set_type() may be used to specify the type of
  /// automaton produced (TGBA, BA, Monitor).  The default is TGBA.
  ///
  /// Method set_pref() may be used to specify whether small automata
  /// should be prefered over deterministic automata.
  ///
  /// Method set_level() may be used to specify the optimization level.
  ///
  /// The semantic of these three methods is inherited from the
  /// spot::postprocessor class, but the optimization level is
  /// additionally used to select which LTL simplifications to enable.
  ///
  /// Most of the techniques used to produce TGBA or BA are described
  /// in "LTL translation improvements in Spot 1.0" (Alexandre
  /// Duret-Lutz. Int. J. on Critical Computer-Based Systems, 5(1/2),
  /// pp. 31–54, March 2014).
  ///
  /// Unambiguous automata are produced using a trick described in
  /// "LTL Model Checking of Interval Markov Chains" (Michael Benedikt
  /// and Rastislav Lenhardt and James Worrell, Proceedings of
  /// TACAS'13, pp. 32–46, LNCS 7795).
  ///
  /// For reference about formula simplifications, see
  /// https://spot.lrde.epita.fr/tl.pdf (a copy of this file should be
  /// in the doc/tl/ subdirectory of the Spot sources).
  ///
  /// For reference and documentation about the post-processing step,
  /// see the documentation of the spot::postprocessor class.
  class SPOT_API translator: protected postprocessor
  {
  public:
    translator(tl_simplifier* simpl, const option_map* opt = nullptr)
      : postprocessor(opt), simpl_(simpl), simpl_owned_(nullptr)
    {
      SPOT_ASSERT(simpl);
      setup_opt(opt);
    }

    translator(const bdd_dict_ptr& dict, const option_map* opt = nullptr)
      : postprocessor(opt)
    {
      setup_opt(opt);
      build_simplifier(dict);
    }

    translator(const option_map* opt = nullptr)
      : postprocessor(opt)
    {
      setup_opt(opt);
      build_simplifier(make_bdd_dict());
    }

    ~translator()
    {
      // simpl_owned_ is 0 if simpl_ was supplied to the constructor.
      delete simpl_owned_;
    }

    using postprocessor::output_type;

    void
    set_type(output_type type)
    {
      this->postprocessor::set_type(type);
    }

    using postprocessor::output_pref;

    void
    set_pref(output_pref pref)
    {
      this->postprocessor::set_pref(pref);
    }

    using postprocessor::optimization_level;

    void
    set_level(optimization_level level)
    {
      level_ = level;
      if (simpl_owned_)
        {
          auto d = simpl_owned_->get_dict();
          delete simpl_owned_;
          build_simplifier(d);
        }
      if (!gf_guarantee_set_)
        gf_guarantee_ = level != Low;
    }

    /// \brief Convert \a f into an automaton.
    ///
    /// The formula \a f is simplified internally.
    twa_graph_ptr run(formula f);

    /// \brief Convert \a f into an automaton, and update f.
    ///
    /// The formula <code>*f</code> is replaced
    /// by the simplified version.
    twa_graph_ptr run(formula* f);

    /// \brief Clear the LTL simplification caches.
    void clear_caches();

  protected:
    void setup_opt(const option_map* opt);
    void build_simplifier(const bdd_dict_ptr& dict);
    twa_graph_ptr run_aux(formula f);

  private:
    tl_simplifier* simpl_;
    tl_simplifier* simpl_owned_;
    int comp_susp_;
    int early_susp_;
    int skel_wdba_;
    int skel_simul_;
    int relabel_bool_;
    int tls_impl_;
    bool gf_guarantee_ = true;
    bool gf_guarantee_set_ = false;
    bool ltl_split_;
    const option_map* opt_;
  };
  /// @}

}
