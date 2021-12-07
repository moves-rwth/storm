// -*- coding: utf-8 -*-
// Copyright (C) 2013-2018 Laboratoire de Recherche et Développement
// de l'Epita.
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
#include <spot/twaalgos/complete.hh>

namespace spot
{
  void complete_here(twa_graph_ptr aut)
  {
    unsigned sink = -1U;
    if (aut->prop_complete().is_true())
      return;
    unsigned n = aut->num_states();

    // UM is a pair (bool, mark).  If the Boolean is false, the
    // acceptance is always satisfiable.  Otherwise, MARK is an
    // example of unsatisfiable mark.
    auto um = aut->acc().unsat_mark();
    if (!um.first)
      {
        // We cannot safely complete an automaton if its
        // acceptance is always satisfiable.
        auto acc = aut->set_buchi();
        for (auto& t: aut->edge_vector())
          t.acc = acc;
      }
    else
      {
        // Loop over the states and search a state that has only
        // self-loops labeled by the same non-accepting mark.  This
        // will be our sink state.  Note that we do not even have to
        // ensure that the state is complete as we will complete the
        // whole automaton in a second pass.
        for (unsigned i = 0; i < n; ++i)
          {
            bool sinkable = true;
            bool first = true;
            acc_cond::mark_t loopmark = um.second;
            for (auto& t: aut->out(i))
              {
                // A state with an outgoing transition that isn't a
                // self-loop is not a candidate for a sink state.
                if (t.dst != i)
                  {
                    sinkable = false;
                    break;
                  }
                // If this is the first self-loop we see, record its
                // mark.  We will check that the mark is non accepting
                // only if this is the only self-loop.
                if (first)
                  {
                    loopmark = t.acc;
                    first = false;
                  }
                // If this this not the first self loop and it has a
                // different acceptance mark, do not consider this
                // state as a sink candidate: combining loops with
                // different marks might be used to build an accepting
                // cycle.
                else if (t.acc != loopmark)
                  {
                    sinkable = false;
                    break;
                  }
              }
            // Now if sinkable==true, it means that there is either no
            // outgoing transition, or just a self-loop.  In the later
            // case we have to check that the acceptance mark of that
            // self-loop is non-accepting.  In the former case
            // "loopmark" was already set to an unsatisfiable mark, so
            // it's ok to retest it.
            if (sinkable && !aut->acc().accepting(loopmark))
              {
                // We have found a sink!
                um.second = loopmark;
                sink = i;
                break;
              }
          }
      }

    unsigned t = aut->num_edges();

    // If the automaton is empty, and the initial state is not
    // universal, pretend this is the sink.
    if (t == 0 && !aut->is_univ_dest(aut->get_init_state_number()))
      sink = aut->get_init_state_number();

    // Now complete all states (excluding any newly added sink).
    for (unsigned i = 0; i < n; ++i)
      {
        bdd missingcond = bddtrue;
        acc_cond::mark_t acc = um.second;
        unsigned edge_to_sink = 0;
        for (auto& t: aut->out(i))
          {
            missingcond -= t.cond;
            // FIXME: This is ugly.
            //
            // In case the automaton uses state-based acceptance, we
            // need to put the new edge in the same set as all
            // the other.
            //
            // In case the automaton uses edge-based acceptance,
            // it does not matter what acceptance set we put the new
            // edge into.
            // EXCEPT if there is an incomplete sinkable state: completing it
            // creates self-loops that must be rejecting.
            //
            // So in both cases, we put the edge in the same
            // acceptance sets as the last outgoing edge of the
            // state.
            acc = t.acc;
            if (t.dst == sink)
              edge_to_sink = aut->edge_number(t);
          }
        // If the state has incomplete successors, we need to add a
        // edge to some sink state.
        if (missingcond != bddfalse)
          {
            // If we haven't found any sink, simply add one.
            if (sink == -1U)
              {
                sink = aut->new_state();
                aut->new_edge(sink, sink, bddtrue, um.second);
              }
            // If we already have a brother-edge to the sink,
            // add the missing condition to that edge.
            if (edge_to_sink)
              {
                aut->edge_data(edge_to_sink).cond |= missingcond;
              }
            else                // otherwise, create the new edge.
              {
                // in case the automaton uses state-based acceptance, propagate
                // the acceptance of the first edge to the one we add.
                if (!aut->prop_state_acc().is_true() && i != sink)
                  acc = {};

                aut->new_edge(i, sink, missingcond, acc);
              }
          }
      }

    aut->prop_complete(true);
    // Get rid of any named property if the automaton changed.
    if (t < aut->num_edges())
      aut->release_named_properties();
    else
      assert(t == aut->num_edges());
  }

  twa_graph_ptr complete(const const_twa_ptr& aut)
  {
    auto res = make_twa_graph(aut, twa::prop_set::all());
    complete_here(res);
    return res;
  }

}
