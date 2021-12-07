// -*- coding: utf-8 -*-
// Copyright (C) 2018-2020 Laboratoire de Recherche et Développement
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

#include <spot/twa/twagraph.hh>

namespace spot
{
  /// \ingroup twa_acc_transform
  /// \brief Options to control various optimizations of to_parity().
  struct to_parity_options
  {
    /// If \c search_ex is true, whenever CAR or IAR have to move
    /// several elements in a record, it tries to find an order such
    /// that the new permutation already exists.
    bool search_ex = true;
    /// If \c use_last is true and \a search_ex are true, we use the
    /// most recent state when we find multiple existing state
    /// compatible with the current move.
    bool use_last = true;
    /// If \c force_order is true, we force to use an order when CAR or IAR is
    /// applied. Given a state s and a set E ({0}, {0 1}, {2} for example) we
    /// construct an order on colors. With the given example, we ask to have
    /// a permutation that start with [0 …], [0 1 …] or [2 …] in
    ///  that order of preference.
    bool force_order = true;
    /// If \c partial_degen is true, apply a partial
    /// degeneralization to remove occurrences of acceptance
    /// subformulas such as `Fin(x) | Fin(y)` or `Inf(x) & Inf(y)`.
    bool partial_degen = true;
    /// If \c force_degen is false, to_parity will checks if we can
    /// get a better result if we don't apply partial_degeneralize.
    bool force_degen = true;
    /// If \c scc_acc_clean is true, to_parity() will ignore colors
    /// not occurring in an SCC while processing this SCC.
    bool acc_clean = true;
    /// If \c parity_equiv is true, to_parity() will check if there
    /// exists a permutations of colors such that the acceptance
    /// condition is a parity condition.
    bool parity_equiv = true;
    /// If \c parity_prefix is true, to_parity() will use a special
    /// handling for acceptance conditions of the form `Inf(m0) |
    /// (Fin(m1) & (Inf(m2) | (… β)))` that start as a parity
    /// condition (modulo a renumbering of `m0`, `m1`, `m2`, ...) but where
    /// `β` can be an arbitrary formula.  In this case, the paritization
    /// algorithm is really applied only to `β`, and the marks of the
    /// prefix are appended after a suitable renumbering.
    ///
    /// For technical reasons, activating this option (and this is the
    /// default) causes reduce_parity() to be called at the end to
    /// minimize the number of colors used.  It is therefore
    /// recommended to disable this option when one wants to follow
    /// the output CAR/IAR constructions.
    bool parity_prefix = true;
    /// If \c rabin_to_buchi is true, to_parity() tries to convert a Rabin or
    /// Streett condition to Büchi or co-Büchi with
    /// rabin_to_buchi_if_realizable().
    bool rabin_to_buchi = true;
    /// Only allow degeneralization if it reduces the number of colors in the
    /// acceptance condition.
    bool reduce_col_deg = false;
    /// Use propagate_marks_here to increase the number of marks on transition
    /// in order to move more colors (and increase the number of
    /// compatible states) when we apply LAR.
    bool propagate_col = true;
    /// If \c pretty_print is true, states of the output automaton are
    /// named to help debugging.
    bool pretty_print = false;
  };


  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return an
  /// equivalent parity automaton.
  ///
  /// If the input is already a parity automaton of any kind, it is
  /// returned unchanged.  Otherwise a new parity automaton with max
  /// odd or max even condition is created.
  ///
  /// This procedure combines many strategies in an attempt to produce
  /// the smallest possible parity automaton.  Some of the strategies
  /// include CAR (color acceptance record), IAR (index appearance
  /// record), partial degenerazation, conversion from Rabin to Büchi
  /// when possible, etc.
  ///
  /// The \a options argument can be used to selectively disable some of the
  /// optimizations.
  SPOT_API twa_graph_ptr
  to_parity(const const_twa_graph_ptr &aut,
            const to_parity_options options = to_parity_options());

  /// \ingroup twa_acc_transform
  /// \brief Take an automaton with any acceptance condition and return an
  /// equivalent parity automaton.
  ///
  /// The parity condition of the returned automaton is max even.
  ///
  /// This implements a straightforward adaptation of the LAR (latest
  /// appearance record) to automata with transition-based marks.  We
  /// call this adaptation the CAR (color apperance record), as it
  /// tracks colors (i.e., acceptance sets) instead of states.
  ///
  /// It is better to use to_parity() instead, as it will use better
  /// strategies when possible, and has additional optimizations.
  SPOT_API twa_graph_ptr
  to_parity_old(const const_twa_graph_ptr& aut, bool pretty_print = false);

  /// \ingroup twa_acc_transform
  /// \brief Turn a Rabin-like or Streett-like automaton into a parity automaton
  /// based on the index appearence record (IAR)
  ///
  /// This is an implementation of \cite kretinsky.17.tacas .
  /// If the input automaton has n states and k pairs, the output automaton has
  /// at most k!*n states and 2k+1 colors. If the input automaton is
  /// deterministic, the output automaton is deterministic as well, which is the
  /// intended use case for this function. If the input automaton is
  /// non-deterministic, the result is still correct, but way larger than an
  /// equivalent Büchi automaton.
  ///
  /// If the input automaton is Rabin-like (resp. Streett-like), the output
  /// automaton has max odd (resp. min even) acceptance condition.
  ///
  /// Throws an std::runtime_error if the input is neither Rabin-like nor
  /// Street-like.
  ///
  /// It is better to use to_parity() instead, as it will use better
  /// strategies when possible, and has additional optimizations.
  SPOT_DEPRECATED("use to_parity() instead") // deprecated since Spot 2.9
  SPOT_API twa_graph_ptr
  iar(const const_twa_graph_ptr& aut, bool pretty_print = false);

  /// \ingroup twa_acc_transform
  /// \brief Turn a Rabin-like or Streett-like automaton into a parity automaton
  /// based on the index appearence record (IAR)
  ///
  /// Returns nullptr if the input automaton is neither Rabin-like nor
  /// Streett-like, and calls spot::iar() otherwise.
  SPOT_DEPRECATED("use to_parity() and spot::acc_cond::is_rabin_like() instead")
  SPOT_API twa_graph_ptr   // deprecated since Spot 2.9
  iar_maybe(const const_twa_graph_ptr& aut, bool pretty_print = false);

} // namespace spot
