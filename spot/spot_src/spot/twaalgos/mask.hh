// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2016, 2017 Laboratoire de Recherche et Développement
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
  /// \brief Clone and mask an automaton.
  ///
  /// Copy the edges of automaton \a old into the empty automaton \a
  /// cpy, creating new states as needed.  The argument \a trans
  /// should behave as a function with the following prototype:
  /// <code>
  ///   void (*trans) (unsigned src, bdd& cond, acc_cond::mark_t& acc,
  ///   unsigned dst)
  /// </code>
  ///
  /// The \a trans function may modify either the condition or the
  /// acceptance sets of the edges.  Set the condition to bddfalse to
  /// remove the edge (this will also remove the destination state and
  /// its descendants if they are not reachable by another edge).
  ///
  /// The mapping between each state of the resulting automaton
  /// and the original state of the input automaton is stored in the
  /// "original-states" named property of the produced automaton.  Call
  /// `aut->get_named_prop<std::vector<unsigned>>("original-states")`
  /// to retrieve it.
  ///
  /// If \a drop_univ_branches branch is set, universal branching is replaced
  /// by existential branching during the copy.
  ///
  /// \param init The optional new initial state.
  template<typename Trans>
  void transform_accessible(const const_twa_graph_ptr& old,
                            twa_graph_ptr& cpy,
                            Trans trans, unsigned int init,
                            bool drop_univ_branches = false)
  {
    std::vector<unsigned> todo;
    std::vector<unsigned> seen(old->num_states(), -1U);

    auto orig_states = new std::vector<unsigned>();
    orig_states->reserve(old->num_states()); // maybe less are needed.
    cpy->set_named_prop("original-states", orig_states);

    auto new_state =
      [&](unsigned old_state) -> unsigned
      {
        unsigned tmp = seen[old_state];
        if (tmp == -1U)
          {
            tmp = cpy->new_state();
            seen[old_state] = tmp;
            orig_states->emplace_back(old_state);
            todo.emplace_back(old_state);
          }
        return tmp;
      };

    // Deal with alternating automata, possibly.
    std::map<std::vector<unsigned>, unsigned> uniq;
    auto new_univ_state =
      [&](unsigned old_state) -> unsigned
      {
        if (!old->is_univ_dest(old_state))
          return new_state(old_state);

        std::vector<unsigned> tmp;
        for (auto s: old->univ_dests(old_state))
          tmp.emplace_back(new_state(s));
        std::sort(tmp.begin(), tmp.end());
        tmp.erase(std::unique(tmp.begin(), tmp.end()), tmp.end());
        auto p = uniq.emplace(tmp, 0);
        if (p.second)
          p.first->second =
            cpy->get_graph().new_univ_dests(tmp.begin(), tmp.end());
        return p.first->second;
      };

    cpy->set_init_state(new_univ_state(init));
    if (seen[init] != 0)
      throw std::runtime_error
        ("the destination automaton of transform_accessible() should be empty");

    while (!todo.empty())
      {
        unsigned old_src = todo.back();
        todo.pop_back();

        unsigned new_src = seen[old_src];
        SPOT_ASSERT(new_src != -1U);

        for (auto& t: old->out(old_src))
          {
            bdd cond = t.cond;
            acc_cond::mark_t acc = t.acc;
            trans(t.src, cond, acc, t.dst);
            if (cond == bddfalse)
              continue;
            if (drop_univ_branches)
              for (unsigned d: old->univ_dests(t.dst))
                cpy->new_edge(new_src, new_state(d), cond, acc);
            else
              cpy->new_edge(new_src, new_univ_state(t.dst), cond, acc);
          }
      }
    orig_states->shrink_to_fit();
  }

  /// \brief Copy an automaton and update each edge.
  ///
  /// Copy the states of automaton \a old, into automaton
  /// \a cpy. Each state in \a cpy will have the same id as the ones in \a old.
  /// The argument \a trans
  /// should behave as a function with the following prototype:
  /// <code>
  ///   void (*trans) (unsigned srcbdd& cond, acc_cond::mark_t& acc,
  ///   unsigned dst)
  /// </code>
  /// It can modify either the condition or the acceptance sets of
  /// the edges.  Set the condition to bddfalse to remove it.  Note that
  /// all transtions will be processed.
  /// \param init The optional new initial state.
  template<typename Trans>
  void transform_copy(const const_twa_graph_ptr& old,
                      twa_graph_ptr& cpy,
                      Trans trans, unsigned int init)
  {
    if (!old->is_existential())
      throw std::runtime_error
        ("transform_copy() does not support alternation");

    // Each state in cpy corresponds to a unique state in old.
    cpy->new_states(old->num_states());
    cpy->set_init_state(init);

    for (auto& t: old->edges())
      {
        bdd cond = t.cond;
        acc_cond::mark_t acc = t.acc;
        trans(t.src, cond, acc, t.dst);
        // Having the same number of states should assure that state ids are
        // equivilent in old and cpy.
        SPOT_ASSERT(t.src < cpy->num_states() && t.dst < cpy->num_states());
        if (cond != bddfalse)
          cpy->new_edge(t.src, t.dst, cond, acc);
      }
  }

  template<typename Trans>
  void transform_accessible(const const_twa_graph_ptr& old,
                            twa_graph_ptr& cpy,
                            Trans trans)
  {
    transform_accessible(old, cpy, trans, old->get_init_state_number());
  }

  template<typename Trans>
  void transform_copy(const const_twa_graph_ptr& old,
                      twa_graph_ptr& cpy,
                      Trans trans)
  {
    transform_copy(old, cpy, trans, old->get_init_state_number());
  }

  /// \brief Remove all edges that belong to some given acceptance sets.
  SPOT_API
  twa_graph_ptr mask_acc_sets(const const_twa_graph_ptr& in,
                              acc_cond::mark_t to_remove);

  /// \brief Keep only the states as specified by \a to_keep.
  ///
  /// Each index in the vector \a to_keep specifies wether or not to
  /// keep the transition that exit this state.  The initial state
  /// will be set to \a init.
  ///
  /// Note that the number of states in the result automaton is the
  /// same as in the input: only transitions have been removed.
  ///
  /// \see mask_keep_accessible_states
  SPOT_API
  twa_graph_ptr mask_keep_states(const const_twa_graph_ptr& in,
                                 std::vector<bool>& to_keep,
                                 unsigned int init);

  /// \brief Keep only the states specified by \a to_keep that are accessible.
  ///
  /// Each index in the vector \a to_keep specifies wether or not to
  /// keep the transition that exit this state.  The initial state
  /// will be set to \a init.  Only states that are accessible from \a
  /// init via states in \a to_keep will be preserved.
  ///
  /// If \a drop_univ_branches branch is set, universal branching is replaced
  /// by existential branching during the copy.
  ///
  /// \see mask_keep_states
  SPOT_API
  twa_graph_ptr mask_keep_accessible_states(const const_twa_graph_ptr& in,
                                            std::vector<bool>& to_keep,
                                            unsigned int init,
                                            bool drop_univ_branches = false);
}
