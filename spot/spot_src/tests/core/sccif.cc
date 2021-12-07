// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement
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
#include <iostream>
#include <spot/twa/twagraph.hh>
#include <spot/twaalgos/sccinfo.hh>
#include <spot/twaalgos/hoa.hh>

static void display(spot::scc_info& si)
{
  unsigned ns = si.scc_count();
  for (unsigned n = 0; n < ns; ++n)
    {
      std::cout << "SCC#" << n << "\n  states:";
      for (unsigned s: si.states_of(n))
        std::cout << ' ' << s;
      std::cout << '\n';
      std::cout << "  edges:";
      for (auto& e: si.inner_edges_of(n))
        std::cout << ' ' << e.src << "->" << e.dst;
      std::cout << '\n';
      std::cout << "  succs:";
      for (unsigned s: si.succ(n))
        std::cout << ' ' << s;
      std::cout << '\n';
    }
  std::cout << '\n';
}

int main()
{
  auto d = spot::make_bdd_dict();
  auto tg = make_twa_graph(d);
  bdd p1 = bdd_ithvar(tg->register_ap("p1"));
  bdd p2 = bdd_ithvar(tg->register_ap("p2"));
  tg->set_generalized_buchi(2);

  auto s1 = tg->new_state();
  auto s2 = tg->new_state();
  auto s3 = tg->new_state();
  tg->set_init_state(s1);
  tg->new_edge(s1, s1, bddfalse);
  tg->new_edge(s1, s2, p1);
  tg->new_edge(s1, s3, p2, {1});
  tg->new_edge(s2, s3, p1 & p2, {0});
  tg->new_edge(s3, s1, p1 | p2, {0, 1});
  tg->new_edge(s3, s2, p1 >> p2);
  tg->new_edge(s3, s3, bddtrue, {0, 1});

  spot::print_hoa(std::cout, tg) << '\n';

  {
    std::cout << "** default\n";
    spot::scc_info si(tg);
    display(si);
  }
  {
    std::cout << "** ignore edges to 2\n";
    auto filter = [](const spot::twa_graph::edge_storage_t&,
                     unsigned dst, void*)
      {
        if (dst == 2)
          return spot::scc_info::edge_filter_choice::ignore;
        else
          return spot::scc_info::edge_filter_choice::keep;
      };
    spot::scc_info si(tg, s1, filter, nullptr);
    display(si);
  }
  {
    std::cout << "** cut edges to 2\n";
    auto filter = [](const spot::twa_graph::edge_storage_t&,
                     unsigned dst, void* val)
      {
        if (dst == *static_cast<unsigned*>(val))
          return spot::scc_info::edge_filter_choice::cut;
        else
          return spot::scc_info::edge_filter_choice::keep;
      };
    unsigned ignore = 2;
    spot::scc_info si(tg, s1, filter, &ignore);
    display(si);
  }
}
