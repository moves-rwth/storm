// -*- coding: utf-8 -*-
// Copyright (C) 2015-2018 Laboratoire de Recherche et Développement
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
#include <spot/twaalgos/mask.hh>

namespace spot
{
  twa_graph_ptr mask_acc_sets(const const_twa_graph_ptr& in,
                                 acc_cond::mark_t to_remove)
  {
    auto res = make_twa_graph(in->get_dict());
    res->copy_ap_of(in);
    res->prop_copy(in, { true, true, false, true, false, false });
    unsigned na = in->num_sets();
    unsigned tr = to_remove.count();
    assert(tr <= na);
    res->set_acceptance(na - tr,
                        in->get_acceptance().strip(to_remove, true));
    transform_accessible(in, res, [&](unsigned,
                                      bdd& cond,
                                      acc_cond::mark_t& acc,
                                      unsigned)
                         {
                           if (acc & to_remove)
                             cond = bddfalse;
                           else
                             acc = acc.strip(to_remove);
                         });
    return res;
  }

  twa_graph_ptr mask_keep_states(const const_twa_graph_ptr& in,
                                    std::vector<bool>& to_keep,
                                    unsigned int init)
  {
    if (to_keep.size() < in->num_states())
      to_keep.resize(in->num_states(), false);

    auto res = make_twa_graph(in->get_dict());
    res->copy_ap_of(in);
    res->prop_copy(in, { true, true, false, true, false, false });
    res->copy_acceptance_of(in);
    transform_copy(in, res, [&](unsigned src,
                                bdd& cond,
                                acc_cond::mark_t&,
                                unsigned dst)
                   {
                     if (!to_keep[src] || !to_keep[dst])
                       cond = bddfalse;
                   }, init);
    return res;
  }

  twa_graph_ptr mask_keep_accessible_states(const const_twa_graph_ptr& in,
                                            std::vector<bool>& to_keep,
                                            unsigned int init,
                                            bool drop_univ_branches)
  {
    if (to_keep.size() < in->num_states())
      to_keep.resize(in->num_states(), false);

    auto res = make_twa_graph(in->get_dict());
    res->copy_ap_of(in);
    res->prop_copy(in, { true, true, false, true, false, false });
    res->copy_acceptance_of(in);
    transform_accessible(in, res, [&](unsigned src,
                                      bdd& cond,
                                      acc_cond::mark_t&,
                                      unsigned dst)
                   {
                     if (!to_keep[src])
                       {
                         cond = bddfalse;
                         return;
                       }
                     bool want = false;
                     for (auto d: in->univ_dests(dst))
                       if (to_keep[d])
                         {
                           want = true;
                           break;
                         }
                     if (!want)
                       cond = bddfalse;
                   }, init, drop_univ_branches);
    return res;
  }

}
