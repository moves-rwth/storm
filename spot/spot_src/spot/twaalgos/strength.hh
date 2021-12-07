// -*- coding: utf-8 -*-
// Copyright (C) 2010-2011, 2013-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE)
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

#include <spot/twaalgos/sccinfo.hh>

namespace spot
{
  /// \brief Check whether an automaton is terminal.
  ///
  /// An automaton is terminal if it is weak, all its accepting SCCs
  /// are complete, and no accepting transitions lead to a
  /// non-accepting SCC.
  ///
  /// If ignore_trivial_scc is set, accepting transitions from trivial
  /// SCCs are ignored.
  ///
  /// This property guarantees that a word is accepted if it has some
  /// prefix that reaches an accepting transition.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  ///
  /// In addition to returning the result as a Boolean, this will set
  /// the prop_terminal() property of the automaton as a side-effect,
  /// so further calls will return in constant-time.
  SPOT_API bool
  is_terminal_automaton(const const_twa_graph_ptr& aut, scc_info* sm = nullptr,
                        bool ignore_trivial_scc = false);

  /// \brief Check whether an automaton is weak.
  ///
  /// An automaton is weak if in any given SCC, all transitions belong
  /// to the same acceptance sets.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  ///
  /// In addition to returning the result as a Boolean, this will set
  /// the prop_weak() property of the automaton as a side-effect,
  /// so further calls will return in constant-time.
  SPOT_API bool
  is_weak_automaton(const const_twa_graph_ptr& aut, scc_info* sm = nullptr);

  /// \brief Check whether an automaton is very-weak.
  ///
  /// An automaton is very-weak if in any given SCC, all transitions
  /// belong to the same acceptance sets, and the SCC has only one
  /// state.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  ///
  /// In addition to returning the result as a Boolean, this will set
  /// the prop_very_weak() and prop_weak() properties of the automaton
  /// as a side-effect, so further calls will return in constant-time.
  SPOT_API bool
  is_very_weak_automaton(const const_twa_graph_ptr& aut,
                         scc_info* sm = nullptr);

  /// \brief Check whether an automaton is inherently weak.
  ///
  /// An automaton is inherently weak if in any given SCC, there
  /// are only accepting cycles, or only rejecting cycles.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  ///
  /// In addition to returning the result as a Boolean, this will set
  /// the prop_inherently_weak() property of the automaton as a
  /// side-effect, so further calls will return in constant-time.
  SPOT_API bool
  is_inherently_weak_automaton(const const_twa_graph_ptr& aut,
                               scc_info* sm = nullptr);

  /// \brief Check whether an automaton is a safety automaton.
  ///
  /// A safety automaton has only accepting SCCs (or trivial
  /// SCCs).
  ///
  /// A minimized WDBA (as returned by a successful run of
  /// minimize_obligation()) represents safety property if it is a
  /// safety automaton.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  SPOT_API bool
  is_safety_automaton(const const_twa_graph_ptr& aut,
                      scc_info* sm = nullptr);

  /// \brief Whether the automaton represents a liveness property.
  ///
  /// An automaton represents a liveness property if after forcing the
  /// acceptance condition to true, the resulting automaton accepts
  /// all words.  In other words, there is no prefix that cannot be
  /// extended into an accepting word.
  SPOT_API bool
  is_liveness_automaton(const const_twa_graph_ptr& aut);

  /// \brief Check whether an automaton is weak or terminal.
  ///
  /// This sets the "inherently weak", "weak", "very-weak" and
  /// "terminal" properties as appropriate.
  ///
  /// \param aut the automaton to check
  ///
  /// \param sm an scc_info object for the automaton if available (it
  /// will be built otherwise).
  SPOT_API void
  check_strength(const twa_graph_ptr& aut, scc_info* sm = nullptr);


  /// \brief Extract a sub-automaton of a given strength
  ///
  /// The string \a keep should be a non-empty combination of
  /// the following letters:
  /// - 'w': keep only inherently weak SCCs (i.e., SCCs in which
  ///        all transitions belong to the same acceptance sets) that
  ///        are not terminal.
  /// - 't': keep terminal SCCs (i.e., inherently weak SCCs that are complete)
  /// - 's': keep strong SCCs (i.e., SCCs that are not inherently weak).
  /// Additionally, the string may contain comma-separated numbers representing
  /// SCC number, optionally prefixed by 'a' to denote the Nth accepting SCC.
  ///
  /// This algorithm returns a subautomaton that contains all SCCs of
  /// the requested strength (or given SCC numbers), plus any upstream
  /// SCC (but adjusted not to be accepting).  The output may be null if
  /// no SCC match a given strength.  An exception will be raised if
  /// an incorrect SCC number is supplied.
  ///
  /// The definition are basically those used in the following paper,
  /// except that we extract the "inherently weak" part instead of the
  /// weak part because we can now test for inherent weakness
  /// efficiently enough (not enumerating all cycles as suggested in
  /// the paper).   \cite renault.13.tacas
  ///
  /// \param aut the automaton to decompose
  /// \param keep a string specifying the strengths/SCCs to keep
  SPOT_API twa_graph_ptr
  decompose_scc(const const_twa_graph_ptr& aut, const char* keep);

  /// \brief Extract a sub-automaton of a given strength
  ///
  /// This works exactly like
  /// decompose_scc(const const_twa_graph_ptr&, const char*)
  /// but takes an \c scc_info as first argument.  This avoids
  /// wasting time to reconstruct that object if one is already
  /// available.
  SPOT_API twa_graph_ptr
  decompose_scc(scc_info& sm, const char* keep);

  SPOT_DEPRECATED("use decompose_scc() instead")
  SPOT_API twa_graph_ptr
  decompose_strength(const const_twa_graph_ptr& aut, const char* keep);

  /// \brief Extract a sub-automaton above an SCC
  ///
  /// This algorithm returns a subautomaton that contains the requested SCC,
  /// plus any upstream SCC (but adjusted not to be accepting).
  ///
  /// \param sm the SCC info map of the automaton
  /// \param scc_num the index in the map of the SCC to keep
  /// \param accepting if true, scc_num is interpreted as the Nth
  /// accepting SCC instead of the Nth SCC
  SPOT_API twa_graph_ptr
  decompose_scc(scc_info& sm, unsigned scc_num, bool accepting = false);
}
