# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2019 Laboratoire de Recherche et Développement de l'Epita
# (LRDE).
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import spot
import buddy
bdict = spot.make_bdd_dict()
k = spot.make_kripke_graph(bdict)
p1 = buddy.bdd_ithvar(k.register_ap("p1"))
p2 = buddy.bdd_ithvar(k.register_ap("p2"))
cond1 = p1 & p2
cond2 = p1 & -p2
cond3 = -p1 & -p2
s2 = k.new_state(cond1)
s1 = k.new_state(cond2)
s3 = k.new_state(cond3)
k.new_edge(s1, s2)
k.new_edge(s2, s2)
k.new_edge(s1, s3)
k.new_edge(s3, s3)
k.new_edge(s3, s2)
k.set_init_state(s1)

hoa = """HOA: v1
States: 3
Start: 0
AP: 2 "p1" "p2"
acc-name: all
Acceptance: 0 t
properties: state-labels explicit-labels state-acc
--BODY--
State: [0&!1] 0 "1"
1 2
State: [0&1] 1 "0"
1
State: [!0&!1] 2 "2"
2 1
--END--"""
assert hoa == k.to_str('HOA')
assert k.num_states() == 3
assert k.num_edges() == 5

res = []
for e in k.out(s1):
    res.append((e.src, e.dst))
assert res == [(1, 0), (1, 2)]

res = []
for e in k.edges():
    res.append((e.src, e.dst))
assert res == [(1, 0), (0, 0), (1, 2), (2, 2), (2, 0)]

res = []
for s in k.states():
    res.append(s.cond())
assert res == [cond1, cond2, cond3]

assert k.states()[0].cond() == cond1
assert k.states()[1].cond() == cond2
assert k.states()[2].cond() == cond3
